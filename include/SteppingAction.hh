#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;
class RunAction;
class G4GenericMessenger;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(EventAction* eventAction, RunAction* runAction);
    ~SteppingAction() override;

    void UserSteppingAction(const G4Step* step) override;

private:
    EventAction* fEventAction;
    RunAction* fRunAction;

    G4double fZStop;
    G4GenericMessenger* fMessenger;
};

#endif