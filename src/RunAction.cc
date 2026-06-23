#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    auto analysisManager = G4AnalysisManager::Instance();

    //sets output with a default name
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetFileName("output.root");

    // Needed in multithreaded mode.
    analysisManager->SetNtupleMerging(true);

    // Ntuple 0: one row per event
    analysisManager->CreateNtuple("Events", "Event-level quantities");

    analysisManager->CreateNtupleIColumn("eventID");
    analysisManager->CreateNtupleDColumn("totalEdep_eV");
    analysisManager->CreateNtupleDColumn("totalStepLength_mm");

    analysisManager->FinishNtuple();

    // Ntuple 1: one row per step
    analysisManager->CreateNtuple("Steps", "Step-level quantities");

    // Track / event identity
    analysisManager->CreateNtupleIColumn("eventID");             // 0
    analysisManager->CreateNtupleIColumn("trackID");             // 1
    analysisManager->CreateNtupleIColumn("parentID");            // 2
    analysisManager->CreateNtupleIColumn("stepNumber");          // 3

    // Particle identity
    //pdg code is standard so don't need to search fo strings
    //particle name string is also provided
    analysisManager->CreateNtupleIColumn("pdgCode");             // 4
    analysisManager->CreateNtupleSColumn("particleName");        // 5

    // Process information - things like compton scattering
    analysisManager->CreateNtupleSColumn("creatorProcessName");  // 6

    // Volume information - where is the particle
    analysisManager->CreateNtupleSColumn("volumeName");          // 7

    //step position - when step starts
    analysisManager->CreateNtupleDColumn("X_mm");                // 8
    analysisManager->CreateNtupleDColumn("Y_mm");                // 9
    analysisManager->CreateNtupleDColumn("Z_mm");                // 10

    //global and local time when step starts
    analysisManager->CreateNtupleDColumn("globalTime_ns");       // 11
    analysisManager->CreateNtupleDColumn("localTime_ns");        // 12

    // Energy / length
    analysisManager->CreateNtupleDColumn("kinE_eV");             // 13
    analysisManager->CreateNtupleDColumn("edep_eV");             // 14
    analysisManager->CreateNtupleDColumn("stepLength_mm");       // 15

    analysisManager->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();

    // Uses filename from /analysis/setFileName if supplied.
    analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();

    analysisManager->Write();
    analysisManager->CloseFile();
}