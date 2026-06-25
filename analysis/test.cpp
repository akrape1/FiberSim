#include <iostream>
#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>

void test()
{
    TFile* file = TFile::Open("../ROOT/ph_1M.root", "READ");
    TTree* tree = (TTree*)file->Get("Events");

    int zstop;
    double steplen;

    tree->SetBranchAddress("reachedZStop", &zstop);
    tree->SetBranchAddress("totalStepLength_mm", &steplen);

    int nEvts = tree->GetEntries();

    TH1D* hpath = new TH1D("hpath","Path Length to z=500mm;Path length [mm];Events",100, 990, 1105);

    for(int i=0; i<nEvts; i++){
        tree->GetEntry(i);
        if(zstop > 0){
            hpath->Fill(steplen);
        }
    }

    TCanvas* c1 = new TCanvas("c1", "Nonzero paths", 1000, 800);
    c1->SetLogy();
    hpath->Draw();
    c1->SaveAs("../graphs/fullpaths1M_log.png");

}