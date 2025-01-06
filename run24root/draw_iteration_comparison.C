#include <TFile.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <iostream>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


class StFcsDb;                                                                  
StFcsDb* mFcsDb=0; 

TH2F *readFileAndProcess(const char*, const char*  );
TH2F *readFileAndProcess(const char* fileName, const char* histName) {
    char name[100];
    // Open the file
    std::ifstream file(fileName);

    //gROOT->SetBatch(1);
    //LoadSharedLibraries();
    StFcsDb* mFcsDb = new StFcsDb();
    mFcsDb->Init();    
    
    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
        return;
    }
    
    // Variables to hold the numbers from each line
    //while(fin >> ehp >> ns >> dep >> ch >> gain)
    int ehp, ns, dep, ch;
    double gain;
    int id;
    sprintf(name,"h_corrections_%s",histName);
    TH2F *h_corrections = new TH2F(name,"Corrections",50,-25.5,24.5, 40,-39.5,0.5);
    
    // Read the file line by line
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        
        // Read the numbers from the current line
        ss >> ehp >> ns >> dep >> ch >> gain;
        if (dep < 20)
          id = 32*dep + ch;
        else if (dep < 22)
          id = 32*20 + 22*(dep-20) + ch;
        else
          id = 32*20 + 22*2 + 32*(dep-22) + ch;
        if (id >= 748)
          continue;

        int row = -mFcsDb->getRowNumber(ns, id);
        int col = mFcsDb->getColumnNumber(ns, id);
        if (0 == ns)
          col *= -1;        
        // Process the numbers (you can do any processing you need here)
        //std::cout << "Read values: "  << ehp << " " << s << " " << dep << " " << y << " " << gain << std::endl;
        // Fill the histo: 
        h_corrections->Fill(col, row, gain);
        // Calculate something, for example: sum of the columns
        //double sum = ehp + s + dep + ch + corr;
        //std::cout << "Sum of the values: " << sum << std::endl;
    }
    h_corrections -> SetDirectory(0);
    // Close the file
    file.close();
    //h_corrections->Draw("colz");
    return h_corrections;

}

TH2F *get_histo(const char* fileName, const char* histName){

      // Open the first ROOT file
    TFile* file_N = TFile::Open(fileName, "READ");
    if (!file_N || file_N->IsZombie()) {
        std::cerr << "Error: Cannot open file: " << fileName << std::endl;
        return;
    }

    // Retrieve 2D histograms from both files
    TH2F* hist1 = (TH2F*)file_N->Get(histName)->Clone();
    if (!hist1) {
        std::cerr << "Error: Histogram "<< histName <<" not found in file: " << fileName << std::endl;
        file_N->Close();
        file_S->Close();
        return;
    }    
    hist1 -> SetDirectory(0);
    file_N->Close();

    return hist1;  
}  

void draw_iteration_comparison() {

  //gROOT->SetBatch(1);  
  gROOT->Macro("Load.C");
  gROOT->Macro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
  gSystem->Load("StEventMaker");
  gSystem->Load("StFcsDbMaker");  

  
  //TH2F *h_correction_it1 = (TH2F*)readFileAndProcess("fcsgaincorr_222_2.txt","it1");
  //TH2F *h_correction_it2 = (TH2F*)readFileAndProcess("fcsgaincorr_222_3.txt","it2");
  //TH2F *h_correction_it1 = (TH2F*)readFileAndProcess("fcsgaincorr_22_1.txt","it1");
  //TH2F *h_correction_it2 = (TH2F*)readFileAndProcess("fcsgaincorr_22_2.txt","it2");
  TH2F *h_correction_it1 = (TH2F*)readFileAndProcess("fcsgaincorr_170_1.txt","it1");
  TH2F *h_correction_it2 = (TH2F*)readFileAndProcess("fcsgaincorr_170_2.txt","it2");

    TH2F *h_correction_ratio = (TH2F*)h_correction_it2->Clone("h_correction_ratio");
    h_correction_ratio->Divide(h_correction_it1);
    TH1F *h_correction_ratio_1d = new TH1F("h_correction_ratio_1d","h_correction_ratio_1d",100,0,5);
    for(int i=1;i<=h_correction_ratio->GetNbinsX(); i++){
        for(int j=1;j<=h_correction_ratio->GetNbinsY(); j++){
            float binContent = h_correction_ratio->GetBinContent(i,j);
            if(binContent!=0){
              h_correction_ratio_1d->Fill(binContent);
            }
        }
    }
    TH2F* h_status_S_it1 = (TH2F*)get_histo("statusS_iteration1.root", "statusS");
    TH2F* h_status_N_it1 = (TH2F*)get_histo("statusN_iteration1.root", "statusN");
    TH2F* h_status_S_it2 = (TH2F*)get_histo("statusS_iteration2.root", "statusS");
    TH2F* h_status_N_it2 = (TH2F*)get_histo("statusN_iteration2.root", "statusN");


    // Draw histograms on a canvas
    TCanvas* canvas = new TCanvas("canvas", "2D Histograms", 1200, 900);
    gStyle->SetOptStat(0);
    canvas->Divide(2, 2);  // Create a canvas with 2 pads (side by side)
    // Histo for axis
    TH1F *h_status_axis_1 = new TH1F("h_status_axis_1","h_status_axis_1",200,-100,100);
    TH1F *h_status_axis_2 = new TH1F("h_status_axis_2","h_status_axis_2",200,-100,100);
    h_status_axis_1->GetYaxis()->SetRangeUser(-35,0);
    h_status_axis_1->GetXaxis()->SetRangeUser(-23,23);
    h_status_axis_1->SetTitle("Status");
    h_status_axis_2->GetYaxis()->SetRangeUser(-35,0);
    h_status_axis_2->GetXaxis()->SetRangeUser(-23,23);
    h_status_axis_2->SetTitle("Status");
    // Draw the first histogram in the left pad
    canvas->cd(1);
    h_status_axis_1->SetTitle("Status Iteration #1");

    h_status_axis_1 -> Draw("");
    h_status_S_it1->Draw("COLZsame");
    h_status_N_it1->Draw("COLZsame");

    h_correction_it1->SetTitle("GainCorr@Begin");
    
    h_correction_it1->GetYaxis()->SetRangeUser(-35,0);
    h_correction_it1->GetXaxis()->SetRangeUser(-23,23);
    h_correction_it1->GetZaxis()->SetRangeUser(0.1,3);
    h_correction_it2->GetYaxis()->SetRangeUser(-35,0);
    h_correction_it2->GetXaxis()->SetRangeUser(-23,23);
    h_correction_it2->GetZaxis()->SetRangeUser(0.1,3);

    h_correction_it2->SetTitle("GainCorr@End");
    
    h_correction_it1  ->GetXaxis()->SetTitle("+-Col North <-> South");
    h_correction_it2  ->GetXaxis()->SetTitle("+-Col North <-> South");

    h_correction_it1  ->GetYaxis()->SetTitle("-Row");
    h_correction_it2  ->GetYaxis()->SetTitle("-Row");

    // Draw the second histogram in the right pad
    canvas->cd(2);
    h_status_axis_2->SetTitle("Status Iteration #2");
    h_status_axis_2 -> Draw("");
    h_status_S_it2->Draw("COLZsame");
    h_status_N_it2->Draw("COLZsame");

    canvas->cd(3);
    h_correction_it1->SetTitle("Correction Iteration #1");
    h_correction_it1->Draw("COLZ");
    canvas->cd(4);
    h_correction_it2->SetTitle("Correction Iteration #2");
    h_correction_it2->Draw("COLZ");
    canvas->Print("./Plots/status_and_corr.png");

    // Draw histograms on a canvas_corr
    TCanvas* canvas_corr = new TCanvas("canvas_corr", "Iterations Comparison", 1200, 900);
    gStyle->SetOptStat(0);
    canvas_corr->Divide(2, 2);  // Create a canvas_corr with 2 pads (side by side)
    // Histo for axis
    TH1F *h_status_axis = new TH1F("h_status_axis","h_status_axis",200,-100,100);
    h_correction_it1->GetYaxis()->SetRangeUser(-35,0);
    h_correction_it1->GetXaxis()->SetRangeUser(-23,23);
    h_correction_it1->GetZaxis()->SetRangeUser(0.1,3);

    h_correction_it1->SetTitle("GainCorr@Begin");
    
    h_correction_it2->GetYaxis()->SetRangeUser(-35,0);
    h_correction_it2->GetXaxis()->SetRangeUser(-23,23);
    h_correction_it2->GetZaxis()->SetRangeUser(0.1,3);

    h_correction_it2->SetTitle("GainCorr@End");

    h_correction_ratio->GetYaxis()->SetRangeUser(-35,0);
    h_correction_ratio->GetXaxis()->SetRangeUser(-23,23);
    h_correction_ratio->GetZaxis()->SetRangeUser(0.95,1.1);
    h_correction_ratio->SetTitle("Slope=GainCorr@End/GainCorr@Begin");
    
    h_correction_it1  ->GetXaxis()->SetTitle("+-Col North <-> South");
    h_correction_it2  ->GetXaxis()->SetTitle("+-Col North <-> South");
    h_correction_ratio->GetXaxis()->SetTitle("+-Col North <-> South");

    h_correction_it1  ->GetYaxis()->SetTitle("-Row");
    h_correction_it2  ->GetYaxis()->SetTitle("-Row");
    h_correction_ratio->GetYaxis()->SetTitle("-Row");

    h_correction_ratio_1d->GetXaxis()->SetTitle("Slope=GainCorr@End/GainCorr@Begin");
    // Draw the first histogram in the left pad

    canvas_corr->cd(1);
    h_correction_it1->Draw("COLZ");
    canvas_corr->cd(2);
    h_correction_it2->Draw("COLZ");

    canvas_corr->cd(3);
    h_correction_ratio_1d->Draw("COLZ");

    canvas_corr->cd(4);
    h_correction_ratio->Draw("COLZ");

    canvas_corr->Print("./Plots/comp_170.png");
    //// Wait for user input to close the canvas
    //canvas->Update();
    //std::cout << "Press Enter to exit..." << std::endl;
    //std::cin.get();
    //
    //
    //delete canvas;
}

