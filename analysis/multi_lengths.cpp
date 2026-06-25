#include <iostream>
#include <vector>
#include <string>
#include <limits>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TString.h>

#include <TLegend.h>
#include <algorithm>

struct SampleConfig {
    std::string fileName;
    std::string label;
    int diamTag;          // 4 for 1.4 mm, 8 for 1.8 mm
    double zStop_mm;
    TH1D* hist;
};

void multi_lengths()
{
    // 1.4 mm diameter
    TH1D* h4_300 = new TH1D("h4_300", "1.4 mm, z-stop = 300 mm;Path length [mm];Events", 100, 800.0, 900.0);
    TH1D* h4_500 = new TH1D("h4_500", "1.4 mm, z-stop = 500 mm;Path length [mm];Events", 100, 1000.0, 1100.0);
    TH1D* h4_700 = new TH1D("h4_700", "1.4 mm, z-stop = 700 mm;Path length [mm];Events", 100, 1200.0, 1300.0);
    TH1D* h4_900 = new TH1D("h4_900", "1.4 mm, z-stop = 900 mm;Path length [mm];Events", 100, 1400.0, 1500.0);

    // 1.8 mm diameter
    TH1D* h8_300 = new TH1D("h8_300", "1.8 mm, z-stop = 300 mm;Path length [mm];Events", 100, 800.0, 900.0);
    TH1D* h8_500 = new TH1D("h8_500", "1.8 mm, z-stop = 500 mm;Path length [mm];Events", 100, 1000.0, 1100.0);
    TH1D* h8_700 = new TH1D("h8_700", "1.8 mm, z-stop = 700 mm;Path length [mm];Events", 100, 1200.0, 1300.0);
    TH1D* h8_900 = new TH1D("h8_900", "1.8 mm, z-stop = 900 mm;Path length [mm];Events", 100, 1400.0, 1500.0);

    std::vector<TH1D*> hpaths = {
        h4_300, h4_500, h4_700, h4_900,
        h8_300, h8_500, h8_700, h8_900
    };

    for (auto* h : hpaths) {
        h->SetDirectory(nullptr);
    }

    std::vector<double> zStops_4;
    std::vector<double> zStops_8;

    std::vector<double> mean_4;
    std::vector<double> mean_8;
    std::vector<double> max_4;
    std::vector<double> max_8;

    std::vector<int> entries_4;
    std::vector<int> entries_8;


    std::vector<SampleConfig> samples = {
        {"../ROOT/1.4_mm_diam/ph_1M_300z_4.root", "4_300", 4, 300.0, h4_300},
        {"../ROOT/1.4_mm_diam/ph_1M_500z_4.root", "4_500", 4, 500.0, h4_500},
        {"../ROOT/1.4_mm_diam/ph_1M_700z_4.root", "4_700", 4, 700.0, h4_700},
        {"../ROOT/1.4_mm_diam/ph_1M_900z_4.root", "4_900", 4, 900.0, h4_900},

        {"../ROOT/1.8_mm_diam/ph_1M_300z_8.root", "8_300", 8, 300.0, h8_300},
        {"../ROOT/1.8_mm_diam/ph_1M_500z_8.root", "8_500", 8, 500.0, h8_500},
        {"../ROOT/1.8_mm_diam/ph_1M_700z_8.root", "8_700", 8, 700.0, h8_700},
        {"../ROOT/1.8_mm_diam/ph_1M_900z_8.root", "8_900", 8, 900.0, h8_900}
    };

    for (const auto& s : samples) {

        TFile* file = TFile::Open(s.fileName.c_str(), "READ");
        TTree* tree = (TTree*)file->Get("Events");

        int zstop = 0;
        double steplen = 0.0;

        tree->SetBranchAddress("reachedZStop", &zstop);
        tree->SetBranchAddress("totalStepLength_mm", &steplen);

        int nFilled = 0;
        double maxPath = -std::numeric_limits<double>::infinity();

        const Long64_t nEvts = tree->GetEntries();

        for (Long64_t i = 0; i < nEvts; i++) {
            tree->GetEntry(i);

            if (zstop > 0) {
                s.hist->Fill(steplen);

                nFilled++;

                if (steplen > maxPath) {
                    maxPath = steplen;
                }
            }
        }

        file->Close();

        double meanPath = s.hist->GetMean();

        if (nFilled == 0) {
            maxPath = 0.0;
        }

        if (s.diamTag == 4) {
            zStops_4.push_back(s.zStop_mm);
            mean_4.push_back(meanPath);
            max_4.push_back(maxPath);
            entries_4.push_back(nFilled);
        }

        if (s.diamTag == 8) {
            zStops_8.push_back(s.zStop_mm);
            mean_8.push_back(meanPath);
            max_8.push_back(maxPath);
            entries_8.push_back(nFilled);
        }
    }
/*
    h4_300->SetLineColor(kBlue);
    h4_500->SetLineColor(kBlue);
    h4_700->SetLineColor(kBlue);
    h4_900->SetLineColor(kBlue);

    h8_300->SetLineColor(kRed);
    h8_500->SetLineColor(kRed);
    h8_700->SetLineColor(kRed);
    h8_900->SetLineColor(kRed);

    h4_300->SetLineWidth(2);
    h4_500->SetLineWidth(2);
    h4_700->SetLineWidth(2);
    h4_900->SetLineWidth(2);

    h8_300->SetLineWidth(2);
    h8_500->SetLineWidth(2);
    h8_700->SetLineWidth(2);
    h8_900->SetLineWidth(2);

    TCanvas* c1 = new TCanvas("c1", "path lengths", 1250, 800);

    TPad* plotPad = new TPad("plotPad", "plots", 0.00, 0.00, 0.82, 1.00);
    plotPad->Draw();
    plotPad->Divide(2, 2);

    TPad* legendPad = new TPad("legendPad", "legend", 0.82, 0.00, 1.00, 1.00);
    legendPad->Draw();

    plotPad->cd(1);
    h4_300->SetTitle("z=300 mm stop");
    h4_300->SetStats(0);
    h8_300->SetStats(0);
    h4_300->Draw("HIST");
    h8_300->Draw("SAME HIST");

    plotPad->cd(2);
    h4_500->SetTitle("z=500 mm stop");
    h4_500->SetStats(0);
    h8_500->SetStats(0);
    h4_500->Draw("HIST");
    h8_500->Draw("SAME HIST");

    plotPad->cd(3);
    h4_700->SetTitle("z=700 mm stop");
    h4_700->SetStats(0);
    h8_700->SetStats(0);
    h4_700->Draw("HIST");
    h8_700->Draw("SAME HIST");

    plotPad->cd(4);
    h4_900->SetTitle("z=900 mm stop");
    h4_900->SetStats(0);
    h8_900->SetStats(0);
    h4_900->Draw("HIST");
    h8_900->Draw("SAME HIST");

    legendPad->cd();
    TLegend* leg = new TLegend(0.10, 0.40, 0.90, 0.60);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.10);
    leg->AddEntry(h4_300, "1.4 mm", "l");
    leg->AddEntry(h8_300, "1.8 mm", "l");
    leg->Draw();

    c1->Update();
    c1->SaveAs("../graphs/dif_diam_paths.png");
*/

    TCanvas* c2 = new TCanvas("c2", "path lengths stats", 1000, 800);
    TGraph* g4 = new TGraph(4, zStops_4.data(), mean_4.data());
    TGraph* g8 = new TGraph(4, zStops_8.data(), mean_8.data());

    g4->SetMarkerColor(kBlue);
    g4->SetMarkerStyle(20);
    g4->SetMarkerSize(1.9);

    g8->SetMarkerColor(kRed);
    g8->SetMarkerStyle(21);
    g8->SetMarkerSize(1.8);

    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle("Mean Path Length;z stop [mm];Mean path length [mm]");

    mg->Add(g4, "P");
    mg->Add(g8, "P");

    mg->Draw("A");

    TLegend* leg2 = new TLegend(0.75, 0.14, 0.91, 0.30);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->SetTextSize(0.04);

    leg2->AddEntry(g4, "1.4 mm", "p");
    leg2->AddEntry(g8, "1.8 mm", "p");

    leg2->Draw();

    c2->Update();
    c2->SaveAs("../graphs/mean_paths.png");

    c2->Clear();

    TGraph* g4m = new TGraph(4, zStops_4.data(), max_4.data());
    TGraph* g8m = new TGraph(4, zStops_8.data(), max_8.data());

    g4m->SetMarkerColor(kBlue);
    g4m->SetMarkerStyle(20);
    g4m->SetMarkerSize(1.9);

    g8m->SetMarkerColor(kRed);
    g8m->SetMarkerStyle(21);
    g8m->SetMarkerSize(1.8);

    TMultiGraph* mgm = new TMultiGraph();
    mgm->SetTitle("Max Path Length;z stop [mm];Max path length [mm]");

    mgm->Add(g4m, "P");
    mgm->Add(g8m, "P");

    mgm->Draw("A");

    // One legend off to the right side
    TLegend* leg3 = new TLegend(0.75, 0.14, 0.91, 0.30);
    leg3->SetBorderSize(0);
    leg3->SetFillStyle(0);
    leg3->SetTextSize(0.04);

    leg3->AddEntry(g4m, "1.4 mm", "p");
    leg3->AddEntry(g8m, "1.8 mm", "p");

    leg3->Draw();

    c2->Update();
    c2->SaveAs("../graphs/max_paths.png");
}