#include "SteppingAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4GenericMessenger.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
    : fEventAction(eventAction),
      fRunAction(runAction),
      fZStop(500.0 * mm),
      fMessenger(nullptr)
{
    fMessenger = new G4GenericMessenger(
        this,
        "/fiber/",
        "Fiber simulation runtime controls."
    );

    auto& zStopCmd = fMessenger->DeclarePropertyWithUnit(
        "zStop",
        "mm",
        fZStop,
        "Z position where photons are counted and killed."
    );

    zStopCmd.SetParameterName("zStop", false);
}

SteppingAction::~SteppingAction()
{
    delete fMessenger;
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto track = step->GetTrack();
    auto particle = track->GetParticleDefinition();

    const G4String particleName = particle->GetParticleName();

    if (particleName != "opticalphoton") {
        return;
    }

    auto preStepPoint = step->GetPreStepPoint();
    auto postStepPoint = step->GetPostStepPoint();

    const G4ThreeVector prePos = preStepPoint->GetPosition();
    const G4ThreeVector postPos = postStepPoint->GetPosition();

    /*
     * Store initial photon info once, on the first step.
     *
     * This records:
     *   initialRho_mm
     *   initialPsi
     *
     * using the pre-step position, direction, and polarization.
     */
    if (track->GetCurrentStepNumber() == 1) {
        fEventAction->SetInitialPhotonInfo(
            preStepPoint->GetPosition(),
            preStepPoint->GetMomentumDirection(),
            preStepPoint->GetPolarization()
        );
    }

    /*
     * Volume at start of step.
     *
     * This restores the old behavior where photons are killed
     * once they begin a step in WorldPhys.
     */
    G4String volumeName = "OutOfWorld";

    auto preTouchable = preStepPoint->GetTouchableHandle();

    if (preTouchable) {
        auto volume = preTouchable->GetVolume();

        if (volume) {
            volumeName = volume->GetName();
        }
    }

    /*
     * Kill optical photons once they reach the world air volume.
     *
     * Since photons are not generated in WorldPhys, this prevents
     * tracking and recording useless air steps.
     */
    if (volumeName == "WorldPhys") {
        track->SetTrackStatus(fStopAndKill);
        return;
    }

    /*
     * Kill pathological photons before adding event totals.
     *
     * This keeps runtime-poison photons from contaminating
     * totalStepLength_mm.
     */
    const G4int maxOpticalSteps = 10000;
    const G4double maxOpticalTrackLength = 5000.0 * mm;
    const G4double maxOpticalGlobalTime = 100.0 * ns;

    const G4bool tooManySteps =
        track->GetCurrentStepNumber() > maxOpticalSteps;

    const G4bool trackTooLong =
        track->GetTrackLength() > maxOpticalTrackLength;

    const G4bool trackTooOld =
        track->GetGlobalTime() > maxOpticalGlobalTime;

    if (tooManySteps || trackTooLong || trackTooOld) {
        track->SetTrackStatus(fStopAndKill);
        return;
    }

    /*
     * Kill photons that leave the geometry/world.
     *
     * If postStepPoint has no physical volume, the photon has left the world.
     */
    auto postTouchable = postStepPoint->GetTouchableHandle();

    if (!postTouchable || !postTouchable->GetVolume()) {
        track->SetTrackStatus(fStopAndKill);
        return;
    }

    /*
     * Add optical photon step length to the event-level total.
     *
     * This happens after the bad-track/world kills, so garbage steps
     * do not inflate the path length.
     */
    fEventAction->AddStepLength(step->GetStepLength());

    /*
     * Check whether the photon reached the z-stop plane.
     *
     * This catches crossings from below to above:
     *
     *   prePos.z() < fZStop
     *   postPos.z() >= fZStop
     */
    const G4bool reachedZStop =
        prePos.z() < fZStop &&
        postPos.z() >= fZStop;

    if (reachedZStop) {

        fEventAction->SetFinalPhotonInfo(
            postStepPoint->GetMomentumDirection(),
            postStepPoint->GetPolarization()
        );

        fEventAction->MarkReachedZStop();

        track->SetTrackStatus(fStopAndKill);
        return;
    }

    /*
     * Optional step-level output.
     *
     * This assumes your Steps ntuple is ntuple 1 and has columns matching
     * the newer polarization-recording structure.
     */
    if (fRunAction->RecordSteps()) {

        auto analysisManager = G4AnalysisManager::Instance();

        const G4Event* event =
            G4RunManager::GetRunManager()->GetCurrentEvent();

        const G4int eventID = event ? event->GetEventID() : -1;
        const G4int trackID = track->GetTrackID();
        const G4int parentID = track->GetParentID();
        const G4int stepNumber = track->GetCurrentStepNumber();

        const G4ThreeVector momentumDirection =
            postStepPoint->GetMomentumDirection();

        const G4ThreeVector polarization =
            postStepPoint->GetPolarization();

        G4String processName = "none";

        const G4VProcess* process =
            postStepPoint->GetProcessDefinedStep();

        if (process) {
            processName = process->GetProcessName();
        }

        /*
         * Ntuple 1: Steps
         */
        analysisManager->FillNtupleIColumn(1, 0, eventID);
        analysisManager->FillNtupleIColumn(1, 1, trackID);
        analysisManager->FillNtupleIColumn(1, 2, parentID);
        analysisManager->FillNtupleIColumn(1, 3, stepNumber);

        analysisManager->FillNtupleDColumn(1, 4, postPos.x() / mm);
        analysisManager->FillNtupleDColumn(1, 5, postPos.y() / mm);
        analysisManager->FillNtupleDColumn(1, 6, postPos.z() / mm);

        analysisManager->FillNtupleDColumn(1, 7, step->GetStepLength() / mm);
        analysisManager->FillNtupleDColumn(1, 8, track->GetTrackLength() / mm);
        analysisManager->FillNtupleDColumn(1, 9, track->GetGlobalTime() / ns);

        analysisManager->FillNtupleDColumn(1, 10, momentumDirection.x());
        analysisManager->FillNtupleDColumn(1, 11, momentumDirection.y());
        analysisManager->FillNtupleDColumn(1, 12, momentumDirection.z());

        analysisManager->FillNtupleDColumn(1, 13, polarization.x());
        analysisManager->FillNtupleDColumn(1, 14, polarization.y());
        analysisManager->FillNtupleDColumn(1, 15, polarization.z());

        analysisManager->FillNtupleSColumn(1, 16, processName);

        analysisManager->AddNtupleRow(1);
    }
}