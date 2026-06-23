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
    auto particle = track->GetParticleDefinition();

    auto preStepPoint = step->GetPreStepPoint();

    const G4ThreeVector prePosition = preStepPoint->GetPosition();

    const G4double edep = step->GetTotalEnergyDeposit();
    const G4double stepLength = step->GetStepLength();

    // Event-level totals over all volumes
    fEventAction->AddEnergyDeposit(edep);
    fEventAction->AddStepLength(stepLength);

    // Event ID
    auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
    const G4int eventID = event ? event->GetEventID() : -1;

    // Track / particle information
    const G4int trackID = track->GetTrackID();
    const G4int parentID = track->GetParentID();
    const G4int stepNumber = track->GetCurrentStepNumber();

    const G4int pdgCode = particle->GetPDGEncoding();
    const G4String particleName = particle->GetParticleName();

    // Creator process
    G4String creatorProcessName = "Primary";

    const G4VProcess* creatorProcess = track->GetCreatorProcess();
    if (creatorProcess) {
        creatorProcessName = creatorProcess->GetProcessName();
    }

    // Volume at start of step
    G4String volumeName = "OutOfWorld";

    auto touchable = preStepPoint->GetTouchableHandle();

    if (touchable) {
        auto volume = touchable->GetVolume();

        if (volume) {
            volumeName = volume->GetName();
        }
    }

    // Time and kinetic energy at start of step
    const G4double globalTime = preStepPoint->GetGlobalTime();
    const G4double localTime = preStepPoint->GetLocalTime();
    const G4double kinE = preStepPoint->GetKineticEnergy();

    // Decide whether to kill this optical photon after recording this step
    const G4bool killOpticalPhoton =
        particleName == "opticalphoton" &&
        (
            stepNumber > 10000 ||
            track->GetTrackLength() > 5000.0*mm ||
            globalTime > 100.0*ns
        );

    // Fill Steps ntuple
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

    if (killOpticalPhoton) {
        track->SetTrackStatus(fStopAndKill);
    }
}