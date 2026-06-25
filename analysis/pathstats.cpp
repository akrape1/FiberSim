#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>

struct SampleConfig {
    std::string fileName;
    std::string label;
    int diamTag;          // 4 for 1.4 mm, 8 for 1.8 mm
    double zStop_mm;
};

// Finds the highest-x nonempty bin.
// Returns the upper edge of that bin as the histogram-based max estimate.
double GetHistMaxPathUpperEdge(TH1D* h)
{
    if (!h) return 0.0;

    for (int b = h->GetNbinsX(); b >= 1; --b) {
        if (h->GetBinContent(b) > 0) {
            return h->GetBinLowEdge(b + 1);
        }
    }

    return 0.0;
}

// Same idea, but returns the bin center instead of the upper edge.
double GetHistMaxPathBinCenter(TH1D* h)
{
    if (!h) return 0.0;

    for (int b = h->GetNbinsX(); b >= 1; --b) {
        if (h->GetBinContent(b) > 0) {
            return h->GetBinCenter(b);
        }
    }

    return 0.0;
}

void pathstats()
{
    std::vector<SampleConfig> samples = {
        {"../ROOT/1.4_mm_diam/ph_1M_300z_4.root", "4_300", 4, 300.0},
        {"../ROOT/1.4_mm_diam/ph_1M_500z_4.root", "4_500", 4, 500.0},
        {"../ROOT/1.4_mm_diam/ph_1M_700z_4.root", "4_700", 4, 700.0},
        {"../ROOT/1.4_mm_diam/ph_1M_900z_4.root", "4_900", 4, 900.0},

        {"../ROOT/1.8_mm_diam/ph_1M_300z_8.root", "8_300", 8, 300.0},
        {"../ROOT/1.8_mm_diam/ph_1M_500z_8.root", "8_500", 8, 500.0},
        {"../ROOT/1.8_mm_diam/ph_1M_700z_8.root", "8_700", 8, 700.0},
        {"../ROOT/1.8_mm_diam/ph_1M_900z_8.root", "8_900", 8, 900.0}
    };

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "\n"
              << std::setw(8)  << "label"
              << std::setw(8)  << "diam"
              << std::setw(10) << "zStop"
              << std::setw(12) << "entries"
              << std::setw(14) << "mean"
              << std::setw(18) << "max bin center"
              << std::setw(18) << "max upper edge"
              << std::setw(12) << "underflow"
              << std::setw(12) << "overflow"
              << "\n";

    std::cout << std::string(112, '-') << "\n";

    for (const auto& s : samples) {

        TFile* file = TFile::Open(s.fileName.c_str(), "READ");

        if (!file || file->IsZombie()) {
            std::cerr << "Could not open file: " << s.fileName << "\n";
            continue;
        }

        TTree* tree = dynamic_cast<TTree*>(file->Get("Events"));

        if (!tree) {
            std::cerr << "Could not find Events tree in file: " << s.fileName << "\n";
            file->Close();
            continue;
        }

        int zstop = 0;
        double steplen = 0.0;

        tree->SetBranchAddress("reachedZStop", &zstop);
        tree->SetBranchAddress("totalStepLength_mm", &steplen);

        // Use a full-ish range histogram, not the narrow plotting windows.
        // Adjust 10000 if your possible path lengths can exceed this.
        std::string histName = "h_" + s.label;

        TH1D* hpath = new TH1D(
            histName.c_str(),
            histName.c_str(),
            10000,
            0.0,
            10000.0
        );

        hpath->SetDirectory(nullptr);

        const Long64_t nEvts = tree->GetEntries();

        for (Long64_t i = 0; i < nEvts; ++i) {
            tree->GetEntry(i);

            if (zstop > 0) {
                hpath->Fill(steplen);
            }
        }

        const double entries = hpath->GetEntries();
        const double meanPath = hpath->GetMean();

        const double maxBinCenter = GetHistMaxPathBinCenter(hpath);
        const double maxUpperEdge = GetHistMaxPathUpperEdge(hpath);

        const double underflow = hpath->GetBinContent(0);
        const double overflow = hpath->GetBinContent(hpath->GetNbinsX() + 1);

        std::cout << std::setw(8)  << s.label
                  << std::setw(8)  << s.diamTag
                  << std::setw(10) << s.zStop_mm
                  << std::setw(12) << entries
                  << std::setw(14) << meanPath
                  << std::setw(18) << maxBinCenter
                  << std::setw(18) << maxUpperEdge
                  << std::setw(12) << underflow
                  << std::setw(12) << overflow
                  << "\n";

        if (overflow > 0) {
            std::cout << "WARNING: " << s.label
                      << " has overflow entries. Histogram max is not reliable unless you increase the histogram range.\n";
        }

        delete hpath;
        file->Close();
        delete file;
    }

    std::cout << "\nDone.\n";
}