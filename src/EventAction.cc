#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include <cmath>

EventAction::EventAction()
    : fTotalStepLength(0.0),
      fReachedZStop(false),
      fInitialRho(-999.0f),
      fInitialPsi(-999.0f),
      fFinalPsi(-999.0f),
      fStoredInitialPhotonInfo(false)
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fTotalStepLength = 0.0;

    fReachedZStop = false;

    fInitialRho = -999.0f;
    fInitialPsi = -999.0f;
    fFinalPsi = -999.0f;

    fStoredInitialPhotonInfo = false;
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    auto analysisManager = G4AnalysisManager::Instance();

    const G4int eventID = event->GetEventID();

    analysisManager->FillNtupleIColumn(0, 0, eventID);
    analysisManager->FillNtupleIColumn(0, 1, fReachedZStop ? 1 : 0);

    analysisManager->FillNtupleFColumn(0, 2, fTotalStepLength / mm);
    analysisManager->FillNtupleFColumn(0, 3, fInitialRho);
    analysisManager->FillNtupleFColumn(0, 4, fInitialPsi);
    analysisManager->FillNtupleFColumn(0, 5, fFinalPsi);

    analysisManager->AddNtupleRow(0);
}

void EventAction::AddStepLength(G4double stepLength)
{
    fTotalStepLength += stepLength;
}

void EventAction::MarkReachedZStop()
{
    fReachedZStop = true;
}

void EventAction::SetInitialPhotonInfo(
    const G4ThreeVector& position,
    const G4ThreeVector& direction,
    const G4ThreeVector& polarization
)
{
    if (fStoredInitialPhotonInfo) return;

    const G4double x = position.x();
    const G4double y = position.y();

    fInitialRho = static_cast<G4float>(std::sqrt(x*x + y*y) / mm);
    fInitialPsi = ComputePsi(direction, polarization);

    fStoredInitialPhotonInfo = true;
}

void EventAction::SetFinalPhotonInfo(
    const G4ThreeVector& direction,
    const G4ThreeVector& polarization
)
{
    fFinalPsi = ComputePsi(direction, polarization);
}

G4float EventAction::ComputePsi(
    const G4ThreeVector& direction,
    const G4ThreeVector& polarization
) const
{
    G4ThreeVector u = direction.unit();

    // Remove any tiny numerical component parallel to momentum.
    G4ThreeVector e = polarization - (polarization.dot(u)) * u;

    if (e.mag2() == 0.0) {
        return -999.0f;
    }

    e = e.unit();

    G4ThreeVector zhat(0.0, 0.0, 1.0);
    G4ThreeVector xhat(1.0, 0.0, 0.0);

    G4ThreeVector a = zhat.cross(u);

    // If photon is almost along z, zhat x u is tiny, so use xhat instead.
    if (a.mag2() < 1.0e-12) {
        a = xhat.cross(u);
    }

    a = a.unit();

    G4ThreeVector b = u.cross(a).unit();

    const G4double ea = e.dot(a);
    const G4double eb = e.dot(b);

    G4double psi = std::atan2(eb, ea);

    // Linear polarization: psi and psi + pi are equivalent.
    while (psi < 0.0) {
        psi += CLHEP::pi;
    }

    while (psi >= CLHEP::pi) {
        psi -= CLHEP::pi;
    }

    return static_cast<G4float>(psi);
}