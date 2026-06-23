#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

/*
Events record the total step length and total energy deposited per event (particle generated)
this allows us to not need to loop over each step and investigate
*/
void EventAction::BeginOfEventAction(const G4Event*)
{
    fTotalEnergyDeposit = 0.0;
    fTotalStepLength = 0.0;
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    auto analysisManager = G4AnalysisManager::Instance();

    const G4int eventID = event->GetEventID();

    // Ntuple 0 is "Events".
    analysisManager->FillNtupleIColumn(0, 0, eventID);
    analysisManager->FillNtupleDColumn(0, 1, fTotalEnergyDeposit / eV);
    analysisManager->FillNtupleDColumn(0, 2, fTotalStepLength / mm);

    analysisManager->AddNtupleRow(0);
}

void EventAction::AddEnergyDeposit(G4double edep)
{
    fTotalEnergyDeposit += edep;
}

void EventAction::AddStepLength(G4double length)
{
    fTotalStepLength += length;
}