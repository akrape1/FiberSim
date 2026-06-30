#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4GenericMessenger.hh"
#include "G4Threading.hh"

G4bool RunAction::fRecordEvents = true;
G4bool RunAction::fRecordSteps  = false;

RunAction::RunAction()
{
    auto analysisManager = G4AnalysisManager::Instance();

    // Sets output with a default name.
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetFileName("output.root");

    // Needed in multithreaded mode.
    analysisManager->SetNtupleMerging(true);

    // Only make the messenger on the master thread.
    // The toggle variables are static, so workers still see the chosen values.
    if (G4Threading::IsMasterThread()) {
        fMessenger = std::make_unique<G4GenericMessenger>(
            this,
            "/output/",
            "Output control commands"
        );

        fMessenger->DeclareProperty(
            "recordEvents",
            fRecordEvents,
            "Toggle recording of the Events tree."
        );

        fMessenger->DeclareProperty(
            "recordSteps",
            fRecordSteps,
            "Toggle recording of the Steps tree."
        );
    }

    // ------------------------------------------------------------
    // Ntuple 0: one row per event
    // ------------------------------------------------------------

    // Ntuple 0: one row per event
    analysisManager->CreateNtuple("Events", "Event-level quantities");

    analysisManager->CreateNtupleIColumn("eventID");             // 0
    analysisManager->CreateNtupleIColumn("reachedZStop");        // 1

    analysisManager->CreateNtupleFColumn("totalStepLength_mm");  // 2
    analysisManager->CreateNtupleFColumn("initialRho_mm");       // 3
    analysisManager->CreateNtupleFColumn("initialPsi");          // 4
    analysisManager->CreateNtupleFColumn("finalPsi");            // 5

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

    // Step position at beginning of step
    analysisManager->CreateNtupleDColumn("X_mm");                // 8
    analysisManager->CreateNtupleDColumn("Y_mm");                // 9
    analysisManager->CreateNtupleDColumn("Z_mm");                // 10

    // Global and local time at beginning of step
    analysisManager->CreateNtupleDColumn("globalTime_ns");       // 11
    analysisManager->CreateNtupleDColumn("localTime_ns");        // 12

    // Energy / length
    analysisManager->CreateNtupleDColumn("kinE_eV");             // 13
    analysisManager->CreateNtupleDColumn("edep_eV");             // 14
    analysisManager->CreateNtupleDColumn("stepLength_mm");       // 15

    analysisManager->FinishNtuple();
}

RunAction::~RunAction() = default;

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