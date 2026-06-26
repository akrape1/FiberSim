#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class RunAction;

class EventAction : public G4UserEventAction
{
public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEnergyDeposit(G4double edep);
    void AddStepLength(G4double stepLength);

    void MarkReachedZStop();

    void RecordInitialPosition(const G4ThreeVector& pos);
    G4bool HasRecordedInitialPosition() const;

private:
    RunAction* fRunAction;

    G4double fTotalEdep;
    G4double fTotalStepLength;

    G4bool fReachedZStop;

    G4bool fHasRecordedInitialPosition;

    G4double fInitialX;
    G4double fInitialY;
    G4double fInitialZ;
    G4double fInitialR;
    G4double fInitialRho;
};

#endif