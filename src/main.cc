#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

#include "FTFP_BERT.hh"

int main(int argc, char** argv)
{
    auto runManager =
        G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new FTFP_BERT);

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