#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;
class RunAction;

class EventAction : public G4UserEventAction
{
public:
    explicit EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEnergyDeposit(G4double edep);
    void AddStepLength(G4double stepLength);

    void MarkReachedZStop();

private:
    RunAction* fRunAction = nullptr;

    G4double fTotalEdep;
    G4double fTotalStepLength;

    G4bool fReachedZStop;
};

#endif