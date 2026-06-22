#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    auto analysisManager = G4AnalysisManager::Instance();

    analysisManager->SetDefaultFileType("root");

    // Default name. Can be overridden in run.mac with:
    // /analysis/setFileName some_name.root
    analysisManager->SetFileName("output.root");

    // Needed in multithreaded mode.
    analysisManager->SetNtupleMerging(true);

    // ------------------------------------------------------------
    // Ntuple 0: one row per event
    // ------------------------------------------------------------

    analysisManager->CreateNtuple("Events", "Event-level quantities");

    analysisManager->CreateNtupleIColumn("eventID");
    analysisManager->CreateNtupleDColumn("totalEdep_MeV");
    analysisManager->CreateNtupleDColumn("totalStepLength_mm");

    analysisManager->FinishNtuple();

    // ------------------------------------------------------------
    // Ntuple 1: one row per step
    // ------------------------------------------------------------

    analysisManager->CreateNtuple("Steps", "Step-level quantities");

    // Track / event identity
    analysisManager->CreateNtupleIColumn("eventID");             // 0
    analysisManager->CreateNtupleIColumn("trackID");             // 1
    analysisManager->CreateNtupleIColumn("parentID");            // 2
    analysisManager->CreateNtupleIColumn("stepNumber");          // 3

    // Particle identity
    analysisManager->CreateNtupleIColumn("pdgCode");             // 4
    analysisManager->CreateNtupleSColumn("particleName");        // 5

    // Process information
    analysisManager->CreateNtupleSColumn("creatorProcessName");  // 6

    // Volume information
    analysisManager->CreateNtupleSColumn("volumeName");          // 7

    // Pre-step position
    analysisManager->CreateNtupleDColumn("preX_mm");             // 8
    analysisManager->CreateNtupleDColumn("preY_mm");             // 9
    analysisManager->CreateNtupleDColumn("preZ_mm");             // 10

    // Time at pre-step point
    analysisManager->CreateNtupleDColumn("globalTime_ns");       // 11
    analysisManager->CreateNtupleDColumn("localTime_ns");        // 12

    // Energy / length
    analysisManager->CreateNtupleDColumn("kineticEnergy_MeV");   // 13
    analysisManager->CreateNtupleDColumn("edep_MeV");            // 14
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