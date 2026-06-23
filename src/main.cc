#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

int main(int argc, char** argv)
{
    auto runManager =
        G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    runManager->SetUserInitialization(new DetectorConstruction());

    // ------------------------------------------------------------
    // Physics list
    // ------------------------------------------------------------
    auto physicsList = new FTFP_BERT;

    // Add optical photon processes:
    //   - G4OpBoundary: reflection/refraction/TIR at material boundaries
    //   - G4OpAbsorption: if ABSLENGTH is defined
    //   - G4OpRayleigh: if RAYLEIGH is defined
    //   - etc.
    auto opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(opticalPhysics);

    // Optional debugging controls for optical physics
    auto opticalParams = G4OpticalParameters::Instance();
    opticalParams->SetVerboseLevel(1);
    opticalParams->SetBoundaryVerboseLevel(1);

    runManager->SetUserInitialization(physicsList);

    // In multithreaded Geant4, user actions go through ActionInitialization.
    runManager->SetUserInitialization(new ActionInitialization());

    runManager->Initialize();

    auto uiManager = G4UImanager::GetUIpointer();

    if (argc > 1)
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        uiManager->ApplyCommand(command + fileName);
    }
    else
    {
        G4cout << "No macro file provided." << G4endl;
    }

    delete runManager;

    return 0;
}