#include "EventAction.hh"
#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction(RunAction* runAction)
    : fRunAction(runAction),
      fTotalEdep(0.0),
      fTotalStepLength(0.0),
      fReachedZStop(false)
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fTotalEdep = 0.0;
    fTotalStepLength = 0.0;
    fReachedZStop = false;
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    if (!fRunAction->RecordEvents()) {
        return;
    }

    auto analysisManager = G4AnalysisManager::Instance();

    const G4int eventID = event ? event->GetEventID() : -1;

    analysisManager->FillNtupleIColumn(0, 0, eventID);
    analysisManager->FillNtupleDColumn(0, 1, fTotalEdep / eV);
    analysisManager->FillNtupleDColumn(0, 2, fTotalStepLength / mm);
    analysisManager->FillNtupleIColumn(0, 3, fReachedZStop ? 1 : 0);

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