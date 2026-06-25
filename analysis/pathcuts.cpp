#include <iostream>
#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>

void pathcuts()
{
    const char* filename = "../ROOT/path1000.root";
    TFile* file = TFile::Open(filename, "READ");

    if (!file || file->IsZombie())
    {
        std::cout << "Could not open file: " << filename << std::endl;
        return;
    }

    TTree* tree = (TTree*)file->Get("PathSummary");

    if (!tree)
    {
        std::cout << "Could not find tree named PathSummary" << std::endl;
        file->ls();
        return;
    }

    int eventID;
    double core;
    double clad;
    double air;
    double total;

    tree->SetBranchAddress("eventID", &eventID);
    tree->SetBranchAddress("path_Core_mm", &core);
    tree->SetBranchAddress("path_Cladding_mm", &clad);
    tree->SetBranchAddress("path_Air_mm", &air);
    tree->SetBranchAddress("path_Total_mm", &total);

    Long64_t nEvents = tree->GetEntries();

    // First pass: find maxima and count true zeros
    const double eps = 1.0e-12;

    int nZeroCore = 0;
    int nZeroClad = 0;
    int nZeroAir  = 0;
    int nZeroTot  = 0;

    double maxCore = 0.0;
    double maxClad = 0.0;
    double maxAir  = 0.0;
    double maxTot  = 0.0;

    for (Long64_t i = 0; i < nEvents; i++)
    {
        tree->GetEntry(i);

        if (std::abs(core) <= eps) nZeroCore++;
        else if (core > maxCore) maxCore = core;

        if (std::abs(clad) <= eps) nZeroClad++;
        else if (clad > maxClad) maxClad = clad;

        if (std::abs(air) <= eps) nZeroAir++;
        else if (air > maxAir) maxAir = air;

        if (std::abs(total) <= eps) nZeroTot++;
        else if (total > maxTot) maxTot = total;
    }

    auto xmax = [](double x)
    {
        if (x <= 0.0) return 1.0;
        return 1.05 * x;
    };

    TH1D* hcore = new TH1D("hcore","Non-zero Path Length in Core;Path length [mm];Events",100, 0, xmax(maxCore));
    TH1D* hclad = new TH1D("hclad","Non-zero Path Length in Cladding;Path length [mm];Events",100, 0, xmax(maxClad));
    TH1D* hair = new TH1D("hair","Non-zero Path Length in Air;Path length [mm];Events",100, 0, xmax(maxAir));
    TH1D* htot = new TH1D("htot","Non-zero Total Path Length;Path length [mm];Events",100, 0, xmax(maxTot));

    // Second pass: fill only nonzero values
    for (Long64_t i = 0; i < nEvents; i++)
    {
        tree->GetEntry(i);
        if (std::abs(core) > eps)
            hcore->Fill(core);

        if (std::abs(clad) > eps)
            hclad->Fill(clad);

        if (std::abs(air) > eps)
            hair->Fill(air);

        if (std::abs(total) > eps)
            htot->Fill(total);
    }

    TH1D* hcore1 = (TH1D*)hcore->Clone("hcore1");
    hcore1->SetTitle("Core Path Length;Path length [mm];Events");
    hcore1->GetXaxis()->SetRangeUser(500.0, 5000.0);

    TH1D* hclad1 = (TH1D*)hclad->Clone("hclad1");
    hclad1->SetTitle("Cladding Path Length;Path length [mm];Events");
    hclad1->GetXaxis()->SetRangeUser(10.0, 170.0);

/*    TCanvas* c1 = new TCanvas("c1", "Nonzero path lengths", 1000, 800);
    c1->Divide(2, 2);

    c1->cd(1);
    hcore->Draw();

    c1->cd(2);
    hclad->Draw();

    c1->cd(3);
    hair->Draw();

    c1->cd(4);
    htot->Draw();

    c1->SaveAs("../graphs/paths_nonzero.png");
*/

    TCanvas* c2 = new TCanvas("c2", "Nonzero paths", 1000, 800);
    hcore1->Draw();
    c2->SaveAs("../graphs/core_paths.png");
    
    c2->Clear();

    hclad1->Draw();
    c2->SaveAs("../graphs/clad_paths.png");

}