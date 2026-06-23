#include <iostream>
#include <string>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TLegend.h>
#include <cmath>
#include <vector>
#include <cstring>

void distances(const char* filename = "../ROOT/ph_5.root")
{
    //open the file
    TFile* file = TFile::Open(filename, "READ");

    if (!file || file->IsZombie())
    {
        std::cout << "Could not open file: " << filename << std::endl;
        return;
    }

    //the file has two trees: Events and Steps
    TTree* steps = (TTree*)file->Get("Steps");

    if (!steps)
    {
        std::cout << "Could not find tree named Steps" << std::endl;
        file->ls();
        return;
    }


    // Branch variables
    int eventID;
    double stepLength_mm;
    char volumeName[100];

    steps->SetBranchAddress("eventID", &eventID);
    steps->SetBranchAddress("stepLength_mm", &stepLength_mm);
    steps->SetBranchAddress("volumeName", volumeName);


    // Volume names
    std::string core = "Core";
    std::string clad = "Cladding";
    std::string air  = "WorldPhys";


    //used to store the length of the path spent in each volume (per photon generated)
    TH1D* hcore = new TH1D("hcore", "Path Length in Core;Path length [mm];Events", 100, 0, 5);
    TH1D* hclad = new TH1D("hclad","Path Length in Cladding;Path length [mm];Events",100, 0, 5);
    TH1D* hair = new TH1D("hair","Path Length in Air;Path length [mm];Events",100, 0, 2500);
    TH1D* htot = new TH1D("htot","Total Path Length;Path length [mm];Events",100, 0, 2500);

 
    int nEvents = 1000;
    Long64_t nEntries = steps->GetEntries();
    for (int evt = 0; evt < nEvents; evt++)
    {
        //makes sure that the path length is 0 before looking at steps
        double core_len = 0.0;
        double clad_len = 0.0;
        double air_len  = 0.0;

        for (Long64_t i = 0; i < nEntries; i++)
        {
            steps->GetEntry(i);

            if (eventID != evt)
            {
                continue;
            }

            if (strcmp(volumeName, core.c_str()) == 0)
            {
                core_len += stepLength_mm;
            }
            else if (strcmp(volumeName, clad.c_str()) == 0)
            {
                clad_len += stepLength_mm;
            }
            else if (strcmp(volumeName, air.c_str()) == 0)
            {
                air_len += stepLength_mm;
            }
        }

        //enters the total path length in each volume for given event
        hcore->Fill(core_len);
        hclad->Fill(clad_len);
        hair->Fill(air_len);
        htot->Fill(core_len + clad_len + air_len);
    }


    //sanity check
    std::cout << "hcore entries = " << hcore->GetEntries() << std::endl;
    std::cout << "hclad entries = " << hclad->GetEntries() << std::endl;
    std::cout << "hair entries  = " << hair->GetEntries()  << std::endl;
    std::cout << "htot entries  = " << htot->GetEntries()  << std::endl;

    //plotting
    TCanvas* c1 = new TCanvas("c1", "Path lengths", 1000, 800);
    c1->Divide(2, 2);
    c1->cd(1);
    hcore->Draw();
    c1->cd(2);
    hclad->Draw();
    c1->cd(3);
    hair->Draw();
    c1->cd(4);
    htot->Draw();

    c1->SaveAs("../graphs/path_volumes2.png");

}