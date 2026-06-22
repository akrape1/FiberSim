#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;

class EventAction : public G4UserEventAction
{
public:
    EventAction() = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEnergyDeposit(G4double edep);
    void AddStepLength(G4double length);

private:
    G4double fTotalEnergyDeposit = 0.0;
    G4double fTotalStepLength = 0.0;
};

#endif