#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

#include <memory>

class G4Run;
class G4GenericMessenger;

class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    G4bool RecordEvents() const { return fRecordEvents; }
    G4bool RecordSteps()  const { return fRecordSteps;  }

private:
    static G4bool fRecordEvents;
    static G4bool fRecordSteps;

    std::unique_ptr<G4GenericMessenger> fMessenger;
};

#endif