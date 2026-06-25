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

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
    : fEventAction(eventAction),
      fRunAction(runAction)
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto track = step->GetTrack();
    auto particle = track->GetParticleDefinition();

    auto preStepPoint = step->GetPreStepPoint();
    auto postStepPoint = step->GetPostStepPoint();

    // ------------------------------------------------------------
    // Volume at start of step
    // ------------------------------------------------------------

    G4String volumeName = "OutOfWorld";

    auto touchable = preStepPoint->GetTouchableHandle();

    if (touchable) {
        auto volume = touchable->GetVolume();

        if (volume) {
            volumeName = volume->GetName();
        }
    }

    // ------------------------------------------------------------
    // Kill optical photons once they reach the world air volume.
    //
    // Since you do not generate photons in WorldPhys, this prevents
    // saving air steps and stops the photon as soon as it starts a
    // step in the world volume.
    // ------------------------------------------------------------

    const G4String particleName = particle->GetParticleName();

    if (particleName == "opticalphoton" && volumeName == "WorldPhys") {
        track->SetTrackStatus(fStopAndKill);
        return;
    }

    // ------------------------------------------------------------
    // Step quantities
    // ------------------------------------------------------------

    const G4ThreeVector prePosition = preStepPoint->GetPosition();
    const G4ThreeVector postPosition = postStepPoint->GetPosition();

    const G4double edep = step->GetTotalEnergyDeposit();
    const G4double stepLength = step->GetStepLength();

    // Event-level totals over recorded/non-world volumes.
    // This must stay BEFORE the recordSteps toggle.
    fEventAction->AddEnergyDeposit(edep);
    fEventAction->AddStepLength(stepLength);

    const G4double zStop = 500.0 * mm;

    const G4bool reachedZStop =
        particleName == "opticalphoton" &&
        prePosition.z() < zStop &&
        postPosition.z() >= zStop;

    // ------------------------------------------------------------
    // If Steps recording is disabled, still update event-level
    // zStop information and kill the photon if needed.
    // ------------------------------------------------------------

    if (!fRunAction->RecordSteps()) {
        if (reachedZStop) {
            fEventAction->MarkReachedZStop();
            track->SetTrackStatus(fStopAndKill);
        }

        return;
    }

    // ------------------------------------------------------------
    // Everything below here only happens when recordSteps = true.
    // ------------------------------------------------------------

    auto analysisManager = G4AnalysisManager::Instance();

    // Event ID
    auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
    const G4int eventID = event ? event->GetEventID() : -1;

    // Track / particle information
    const G4int trackID = track->GetTrackID();
    const G4int parentID = track->GetParentID();
    const G4int stepNumber = track->GetCurrentStepNumber();

    const G4int pdgCode = particle->GetPDGEncoding();

    // Creator process
    G4String creatorProcessName = "Primary";

    const G4VProcess* creatorProcess = track->GetCreatorProcess();

    if (creatorProcess) {
        creatorProcessName = creatorProcess->GetProcessName();
    }

    // Time and kinetic energy at start of step
    const G4double globalTime = preStepPoint->GetGlobalTime();
    const G4double localTime = preStepPoint->GetLocalTime();
    const G4double kinE = preStepPoint->GetKineticEnergy();

    // ------------------------------------------------------------
    // Fill Steps ntuple
    // ------------------------------------------------------------

    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, trackID);
    analysisManager->FillNtupleIColumn(1, 2, parentID);
    analysisManager->FillNtupleIColumn(1, 3, stepNumber);

    analysisManager->FillNtupleIColumn(1, 4, pdgCode);
    analysisManager->FillNtupleSColumn(1, 5, particleName);

    analysisManager->FillNtupleSColumn(1, 6, creatorProcessName);
    analysisManager->FillNtupleSColumn(1, 7, volumeName);

    analysisManager->FillNtupleDColumn(1, 8,  prePosition.x() / mm);
    analysisManager->FillNtupleDColumn(1, 9,  prePosition.y() / mm);
    analysisManager->FillNtupleDColumn(1, 10, prePosition.z() / mm);

    analysisManager->FillNtupleDColumn(1, 11, globalTime / ns);
    analysisManager->FillNtupleDColumn(1, 12, localTime / ns);

    analysisManager->FillNtupleDColumn(1, 13, kinE / eV);
    analysisManager->FillNtupleDColumn(1, 14, edep / eV);
    analysisManager->FillNtupleDColumn(1, 15, stepLength / mm);

    analysisManager->AddNtupleRow(1);

    if (reachedZStop) {
        fEventAction->MarkReachedZStop();
        track->SetTrackStatus(fStopAndKill);
    }
}