#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
    : fEventAction(eventAction)
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto analysisManager = G4AnalysisManager::Instance();

    auto track = step->GetTrack();

    auto preStepPoint = step->GetPreStepPoint();

    const G4ThreeVector prePosition = preStepPoint->GetPosition();

    const G4double edep = step->GetTotalEnergyDeposit();
    const G4double stepLength = step->GetStepLength();

    // ------------------------------------------------------------
    // Event-level totals over all volumes
    // ------------------------------------------------------------

    fEventAction->AddEnergyDeposit(edep);
    fEventAction->AddStepLength(stepLength);

    // ------------------------------------------------------------
    // Event ID
    // ------------------------------------------------------------

    auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
    const G4int eventID = event ? event->GetEventID() : -1;

    // ------------------------------------------------------------
    // Track / particle information
    // ------------------------------------------------------------

    auto particle = track->GetParticleDefinition();

    const G4int trackID = track->GetTrackID();
    const G4int parentID = track->GetParentID();
    const G4int stepNumber = track->GetCurrentStepNumber();

    // PDG particle code:
    // gamma = 22, electron = 11, positron = -11, proton = 2212, etc.
    const G4int pdgCode = particle->GetPDGEncoding();

    const G4String particleName = particle->GetParticleName();

    // ------------------------------------------------------------
    // Creator process
    //
    // For primary particles, GetCreatorProcess() is null.
    // ------------------------------------------------------------

    G4String creatorProcessName = "Primary";

    const G4VProcess* creatorProcess = track->GetCreatorProcess();
    if (creatorProcess)
    {
        creatorProcessName = creatorProcess->GetProcessName();
    }

    // ------------------------------------------------------------
    // Volume information at the pre-step point
    //
    // We only store one volume name. No separate logical/physical volume,
    // no material name, and no copy number.
    // ------------------------------------------------------------

    G4String volumeName = "OutOfWorld";

    auto touchable = preStepPoint->GetTouchableHandle();

    if (touchable)
    {
        auto volume = touchable->GetVolume();

        if (volume)
        {
            volumeName = volume->GetName();
        }
    }

    // ------------------------------------------------------------
    // Step-level quantities
    // ------------------------------------------------------------

    const G4double globalTime = preStepPoint->GetGlobalTime();
    const G4double localTime = preStepPoint->GetLocalTime();

    const G4double kineticEnergy = preStepPoint->GetKineticEnergy();

    // ------------------------------------------------------------
    // Fill step-level ntuple
    //
    // Ntuple 1 is "Steps".
    //
    // Column layout:
    //
    //  0  eventID
    //  1  trackID
    //  2  parentID
    //  3  stepNumber
    //  4  pdgCode
    //  5  particleName
    //  6  creatorProcessName
    //  7  volumeName
    //  8  xPre [mm]
    //  9  yPre [mm]
    // 10  zPre [mm]
    // 11  globalTime [ns]
    // 12  localTime [ns]
    // 13  kineticEnergy [MeV]
    // 14  edep [MeV]
    // 15  stepLength [mm]
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

    analysisManager->FillNtupleDColumn(1, 13, kineticEnergy / MeV);
    analysisManager->FillNtupleDColumn(1, 14, edep / MeV);
    analysisManager->FillNtupleDColumn(1, 15, stepLength / mm);

    analysisManager->AddNtupleRow(1);
}