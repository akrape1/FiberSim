#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>

void countpaths()
{
    const char* filename = "../ROOT/ph_1000.root";
    TFile* file = TFile::Open(filename, "READ");

    if (!file || file->IsZombie())
    {
        std::cout << "Could not open file: " << filename << std::endl;
        return;
    }

    TTree* steps = (TTree*)file->Get("Steps");

    if (!steps)
    {
        std::cout << "Could not find tree named Steps" << std::endl;
        file->ls();
        return;
    }

    // Only read the branches we actually need.
    steps->SetBranchStatus("*", 0);
    steps->SetBranchStatus("eventID", 1);
    steps->SetBranchStatus("stepLength_mm", 1);
    steps->SetBranchStatus("volumeName", 1);

    int eventID;
    double stepLength_mm;
    char volumeName[100];

    steps->SetBranchAddress("eventID", &eventID);
    steps->SetBranchAddress("stepLength_mm", &stepLength_mm);
    steps->SetBranchAddress("volumeName", volumeName);

    // Optional ROOT cache help.
    steps->SetCacheSize(50 * 1024 * 1024);
    steps->AddBranchToCache("eventID", true);
    steps->AddBranchToCache("stepLength_mm", true);
    steps->AddBranchToCache("volumeName", true);

    TTree* events = (TTree*)file->Get("Events");
    double nEvents = events->GetEntries();

    std::vector<double> core_len(nEvents, 0.0);
    std::vector<double> clad_len(nEvents, 0.0);
    std::vector<double> air_len(nEvents, 0.0);
    std::vector<double> total_len(nEvents, 0.0);

    Long64_t nEntries = steps->GetEntries();

    for (Long64_t i = 0; i < nEntries; i++)
    {
        steps->GetEntry(i);

        if (eventID < 0)
            continue;

        if (eventID >= (int)core_len.size())
        {
            int newSize = eventID + 1;
            core_len.resize(newSize, 0.0);
            clad_len.resize(newSize, 0.0);
            air_len.resize(newSize, 0.0);
            total_len.resize(newSize, 0.0);
        }

        total_len[eventID] += stepLength_mm;

        if (strcmp(volumeName, "Core") == 0)
        {
            core_len[eventID] += stepLength_mm;
        }
        else if (strcmp(volumeName, "Cladding") == 0)
        {
            clad_len[eventID] += stepLength_mm;
        }
        else if (strcmp(volumeName, "WorldPhys") == 0)
        {
            air_len[eventID] += stepLength_mm;
        }
    }

    nEvents = core_len.size();

    auto max_or_one = [](const std::vector<double>& v)
    {
        double m = *std::max_element(v.begin(), v.end());
        if (m <= 0.0) return 1.0;
        return 1.05 * m;
    };

    //raw data
    TH1D* hcore = new TH1D("hcore", "Path Length in Core;Path length [mm];Events",100, 0, max_or_one(core_len));
    TH1D* hclad = new TH1D("hclad", "Path Length in Cladding;Path length [mm];Events",100, 0, max_or_one(clad_len));
    TH1D* hair = new TH1D("hair", "Path Length in Air;Path length [mm];Events",100, 0, max_or_one(air_len));
    TH1D* htot = new TH1D("htot", "Total Path Length;Path length [mm];Events",100, 0, max_or_one(total_len));

    for (int evt = 0; evt < nEvents; evt++)
    {
        hcore->Fill(core_len[evt]);
        hclad->Fill(clad_len[evt]);
        hair->Fill(air_len[evt]);
        htot->Fill(total_len[evt]);
    }

    //Saving processed data for easier/faster analysis
    TFile* outFile = TFile::Open("../ROOT/path1000.root", "RECREATE");

    if (!outFile || outFile->IsZombie())
    {
        std::cout << "Could not create output summary file" << std::endl;
        return;
    }

    TTree* outTree = new TTree("PathSummary", "Per-event path length summary");

    int out_eventID;
    double out_core_mm;
    double out_clad_mm;
    double out_air_mm;
    double out_total_mm;

    outTree->Branch("eventID", &out_eventID);
    outTree->Branch("path_Core_mm", &out_core_mm);
    outTree->Branch("path_Cladding_mm", &out_clad_mm);
    outTree->Branch("path_Air_mm", &out_air_mm);
    outTree->Branch("path_Total_mm", &out_total_mm);

    for (int evt = 0; evt < nEvents; evt++)
    {
        out_eventID = evt;

        out_core_mm  = core_len[evt];
        out_clad_mm  = clad_len[evt];
        out_air_mm   = air_len[evt];
        out_total_mm = total_len[evt];

        outTree->Fill();
    }

    outTree->Write();
    outFile->Close();

}