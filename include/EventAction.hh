#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddStepLength(G4double stepLength);

    void MarkReachedZStop();

    void SetInitialPhotonInfo(
        const G4ThreeVector& position,
        const G4ThreeVector& direction,
        const G4ThreeVector& polarization
    );

    void SetFinalPhotonInfo(
        const G4ThreeVector& direction,
        const G4ThreeVector& polarization
    );

private:
    G4double fTotalStepLength;

    G4bool fReachedZStop;

    G4float fInitialRho;
    G4float fInitialPsi;
    G4float fFinalPsi;

    G4bool fStoredInitialPhotonInfo;

    G4float ComputePsi(
        const G4ThreeVector& direction,
        const G4ThreeVector& polarization
    ) const;
};

#endif