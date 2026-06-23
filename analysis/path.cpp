#include <iostream>
#include <string>
#include <cstring>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>

void path(const char* filename = "../ROOT/ph_5.root")
{
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
        file->Close();
        return;
    }

    int eventID;
    double stepLength_mm;
    char volumeName[100];

    steps->SetBranchAddress("eventID", &eventID);
    steps->SetBranchAddress("stepLength_mm", &stepLength_mm);
    steps->SetBranchAddress("volumeName", volumeName);

    const std::string core = "Core";
    const std::string clad = "Cladding";
    const std::string air  = "WorldPhys";

    const int nEvents = 5;

    double x[nEvents];

    double core_len[nEvents];
    double clad_len[nEvents];
    double air_len[nEvents];
    double total_len[nEvents];

    for (int evt = 0; evt < nEvents; evt++)
    {
        x[evt] = evt;

        core_len[evt] = 0.0;
        clad_len[evt] = 0.0;
        air_len[evt] = 0.0;
        total_len[evt] = 0.0;
    }

    Long64_t nEntries = steps->GetEntries();

    for (Long64_t i = 0; i < nEntries; i++)
    {
        steps->GetEntry(i);

        if (strcmp(volumeName, core.c_str()) == 0)
        {
            core_len[eventID] += stepLength_mm;
        }
        else if (strcmp(volumeName, clad.c_str()) == 0)
        {
            clad_len[eventID] += stepLength_mm;
        }
        else if (strcmp(volumeName, air.c_str()) == 0)
        {
            air_len[eventID] += stepLength_mm;
        }
    }

    for (int evt = 0; evt < nEvents; evt++)
    {
        total_len[evt] = core_len[evt] + clad_len[evt] + air_len[evt];
    }


    TCanvas* c1 = new TCanvas("c1", "path length", 900, 700);
    c1->cd();

    // Core plot
    TGraph* gcore = new TGraph(nEvents, x, core_len);
    gcore->SetTitle("Core Path Length;Photon ID;Path length [mm]");
    gcore->SetMarkerStyle(20);
    gcore->SetMarkerSize(1.3);
    gcore->SetMarkerColor(kBlue);

    gcore->Draw("AP");
    gcore->GetXaxis()->SetLimits(-0.5, 4.5);
    gcore->GetXaxis()->SetNdivisions(5);
    c1->SetGrid();
    c1->SaveAs("../graphs/core_paths.png");
    
    c1->Clear();

    // Cladding plot
    TGraph* gclad = new TGraph(nEvents, x, clad_len);
    gclad->SetTitle("Cladding Path Length;Photon ID;Path length [mm]");
    gclad->SetMarkerStyle(20);
    gclad->SetMarkerSize(1.3);
    gclad->SetMarkerColor(kRed);

    gclad->Draw("AP");
    gclad->GetXaxis()->SetLimits(-0.5, 4.5);
    gclad->GetXaxis()->SetNdivisions(5);
    c1->SetGrid();
    c1->SaveAs("../graphs/cladding_paths.png");

    c1->Clear();

    // Air plot
    TGraph* gair = new TGraph(nEvents, x, air_len);
    gair->SetTitle("Air Path Length;Photon ID;Path length [mm]");
    gair->SetMarkerStyle(20);
    gair->SetMarkerSize(1.3);
    gair->SetMarkerColor(kGreen + 2);

    gair->Draw("AP");
    gair->GetXaxis()->SetLimits(-0.5, 4.5);
    gair->GetXaxis()->SetNdivisions(5);
    c1->SetGrid();
    c1->SaveAs("../graphs/air_paths.png");

    file->Close();
}