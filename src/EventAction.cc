#include "EventAction.hh"
#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>

EventAction::EventAction(RunAction* runAction)
    : fRunAction(runAction),
      fTotalEdep(0.0),
      fTotalStepLength(0.0),
      fReachedZStop(false),
      fHasRecordedInitialPosition(false),
      fInitialX(0.0),
      fInitialY(0.0),
      fInitialZ(0.0),
      fInitialR(0.0),
      fInitialRho(0.0)
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fTotalEdep = 0.0;
    fTotalStepLength = 0.0;

    fReachedZStop = false;

    fHasRecordedInitialPosition = false;

    fInitialX = 0.0;
    fInitialY = 0.0;
    fInitialZ = 0.0;
    fInitialR = 0.0;
    fInitialRho = 0.0;
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    if (!fRunAction->RecordEvents()) {
        return;
    }

    auto analysisManager = G4AnalysisManager::Instance();

    const G4int eventID = event->GetEventID();

    // Ntuple 0: Events
    analysisManager->FillNtupleIColumn(0, 0, eventID);
    analysisManager->FillNtupleDColumn(0, 1, fTotalEdep / eV);
    analysisManager->FillNtupleDColumn(0, 2, fTotalStepLength / mm);
    analysisManager->FillNtupleIColumn(0, 3, fReachedZStop ? 1 : 0);

    analysisManager->FillNtupleDColumn(0, 4, fInitialX / mm);
    analysisManager->FillNtupleDColumn(0, 5, fInitialY / mm);
    analysisManager->FillNtupleDColumn(0, 6, fInitialZ / mm);
    analysisManager->FillNtupleDColumn(0, 7, fInitialR / mm);
    analysisManager->FillNtupleDColumn(0, 8, fInitialRho / mm);

    analysisManager->AddNtupleRow(0);
}

void EventAction::AddEnergyDeposit(G4double edep)
{
    fTotalEdep += edep;
}

void EventAction::AddStepLength(G4double stepLength)
{
    fTotalStepLength += stepLength;
}

void EventAction::MarkReachedZStop()
{
    fReachedZStop = true;
}

void EventAction::RecordInitialPosition(const G4ThreeVector& pos)
{
    if (fHasRecordedInitialPosition) {
        return;
    }

    fInitialX = pos.x();
    fInitialY = pos.y();
    fInitialZ = pos.z();

    fInitialR = std::sqrt(
        fInitialX*fInitialX +
        fInitialY*fInitialY +
        fInitialZ*fInitialZ
    );

    fInitialRho = std::sqrt(
        fInitialX*fInitialX +
        fInitialY*fInitialY
    );

    fHasRecordedInitialPosition = true;
}

G4bool EventAction::HasRecordedInitialPosition() const
{
    return fHasRecordedInitialPosition;
}