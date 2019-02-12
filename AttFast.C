//************************************************
//*                                              *
//*                   AttFast.C                  *
//*              Katarzyna Rusiecka              *
//*    katarzyna.rusiecka@doctoral.uj.edu.pl     * 
//*                Created in 2019               *
//*                                              *
//************************************************

// ROOT macro for fast determination of attenuation length.
// As an argument takes name of the logfile containing 
// names of the mesurements and corresponding source 
// positions. Determines attenuation length with two methods:
// (1) Combined channels - based on charge ratio from both
// channels
// (2) Separate channels - approximated, based on position of
// the 511 keV peak, but not background-subtracted. Peaks 
// positions are determined with TSpectrum. 
// Created canvases:
// (1) All charge spectra from this measurements 
// (2) Ratio histograms
// (3) Attenuation graphs and curves, both methods. 
//
// To run type:
//   root
//   .L AttFast.C
//   AttFast("logfile.txt")

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include "TString.h"

bool AttFast(TString conf_name){
  
  const char *dataPath = std::getenv("SFDATA");  // directory where data is stored
  const double offset = 12.4;                    // source position offset [mm]
  
  //----- Reading log file  
  TString fname = string(dataPath)+"logs/"+conf_name;
  std::ifstream config(fname);
  
  if(!config.is_open()){
   std::cout << "Couldn't open config file " << conf_name << std::endl;
   return false;
  }
  
  TString dummy;
  std::string line;
  TString dir_names[20];
  double positions[20];
  int npoints = 0;
  
  while(config.good()){
    getline(config,line);
    config >> dir_names[npoints];
    config >> dummy >> dummy >> positions[npoints];
    std::cout << dir_names[npoints] << "\t" << positions[npoints] << std::endl;
    getline(config,line);
    getline(config,line);
    npoints++;
  }
  
  std::cout << "npoints: " << npoints << std::endl;
  
  //----- Acessing data 
  TFile *file;
  TTree *tree;
  std::vector <TH1D*> hch0;
  std::vector <TH1D*> hch1; 
  std::vector <TH1D*> hrat;
  
  for(int i=0; i<npoints; i++){
    file = new TFile("../"+dir_names[i]+"/results.root","READ");
    if(!file->IsOpen()){
     std::cout << "Couldn't open root file: " << std::endl;
     std::cout << dir_names[i] << "/results.root" << std::endl;
     return false;
    }
    tree = (TTree*)file->Get("tree_ft");
    tree->Draw(Form("ch_0.fCal>>htemp_ch0%i(1000,0,2E3)",i));
    tree->Draw(Form("ch_1.fCal>>htemp_ch1%i(1000,0,2E3)",i));
    tree->Draw(Form("log(sqrt(ch_1.fCal/ch_0.fCal))>>htemp_rat%i(500,-2.5,2.5)",i));
    hch0.push_back((TH1D*)gROOT->FindObjectAny(Form("htemp_ch0%i",i))->Clone(Form("hch0_%.2f",positions[i])));
    hch1.push_back((TH1D*)gROOT->FindObjectAny(Form("htemp_ch1%i",i))->Clone(Form("hch1_%.2f",positions[i])));
    hrat.push_back((TH1D*)gROOT->FindObjectAny(Form("htemp_rat%i",i))->Clone(Form("ratio_%.2f",positions[i])));
  }
  
  //----- Drawing ratio histograms
  //----- Calculating averaged attenuation length 
  TCanvas *can_rat = new TCanvas("can_rat","can_rat",1200,1200);
  can_rat->DivideSquare(npoints);
  
  TF1 *fgaus = new TF1("fgaus","gaus",-0.5,0.5);
  
  TGraphErrors *gatt_average = new TGraphErrors(npoints);
  gatt_average->SetMarkerStyle(4);
  gatt_average->SetName("gatt_average");
  gatt_average->SetTitle("attenuation curve, averaged");
  gatt_average->GetXaxis()->SetTitle("source position [mm]");
  
  for(int i=0; i<npoints; i++){
    can_rat->cd(i+1);
    gPad->SetGrid(1,1);
    hrat[i]->Draw();
    hrat[i]->Fit("fgaus","Q");
    gatt_average->SetPoint(i,positions[i]+offset,fgaus->GetParameter(1));
    gatt_average->SetPointError(i,1,fgaus->GetParError(1));
  }
  
  TCanvas *can_att = new TCanvas("can_att","can_att",1000,500);
  can_att->Divide(2,1);
  
  can_att->cd(1);
  gPad->SetGrid(1,1);
  gatt_average->Draw("AP");
  TF1 *fpol1 = new TF1("fpol1","pol1",positions[0]+offset,positions[npoints-1]+offset);
  fpol1->SetParameters(-0.1,1E-3);
  gatt_average->Fit("fpol1","RQ");
  
  std::cout << "Attenuation length (averaged channels): " << 1./fpol1->GetParameter(1) << " mm" << std::endl;
  
  //----- Drawing charge spectra 
  TCanvas *can_spectra = new TCanvas("can_spectra","can_spectra",1200,1200);
  can_spectra->DivideSquare(npoints);
  
  float max_0, max_1, max;
  
  for(int i=0; i<npoints; i++){
    can_spectra->cd(i+1);
    gPad->SetGrid(1,1);
    hch0[i]->SetTitle(Form("position %.2f mm",positions[i]+offset));
    hch0[i]->GetXaxis()->SetTitle("charge [PE]");
    hch0[i]->GetYaxis()->SetTitle("counts");
    max_0 = hch0[i]->GetMaximum();
    max_1 = hch1[i]->GetMaximum();
    max = max_0 > max_1 ? max_0 : max_1;
    hch0[i]->GetYaxis()->SetRangeUser(0,max*1.1);
    hch0[i]->SetStats(0);
    hch1[i]->SetStats(0);
    hch0[i]->SetLineColor(kBlue);
    hch1[i]->SetLineColor(kRed);
    hch0[i]->Draw();
    hch1[i]->Draw("same");
  }
  
  //----- Attenuation length - separate channels
  //----- Approximated only, no background subtraction, only peaks positions
  TSpectrum *spec= new TSpectrum(10);
  int npeaks_ch0 = 0;
  int npeaks_ch1 = 0;
  double *peaks_ch0;
  double *peaks_ch1;
  double peak_ch0;
  double peak_ch1;
  
  TGraphErrors *gatt_ch0 = new TGraphErrors(npoints);
  gatt_ch0->SetMarkerStyle(4);
  gatt_ch0->SetMarkerColor(kBlue);
  gatt_ch0->SetName("gatt_ch0");
  gatt_ch0->SetTitle("attenuation curve, ch0 and ch1");
  gatt_ch0->GetXaxis()->SetTitle("source position [mm]");
  gatt_ch0->GetYaxis()->SetTitle("511 keV peak position");
  
  TGraphErrors *gatt_ch1 = new TGraphErrors(npoints);
  gatt_ch1->SetMarkerStyle(4);
  gatt_ch1->SetMarkerColor(kRed);
  gatt_ch1->SetName("gatt_ch1");
  
  for(int i=0; i<npoints; i++){
    npeaks_ch0 = spec->Search(hch0[i],5,"goff");
    peaks_ch0 = spec->GetPositionX();
    peak_ch0 = TMath::MaxElement(npeaks_ch0,peaks_ch0);
    gatt_ch0->SetPoint(i,positions[i]+offset,peak_ch0);
    
    npeaks_ch1 = spec->Search(hch1[i],5,"goff");
    peaks_ch1 = spec->GetPositionX();
    peak_ch1 = TMath::MaxElement(npeaks_ch1,peaks_ch1);
    gatt_ch1->SetPoint(i,positions[i]+offset,peak_ch1);
  }
  
  TF1 *fexpo_ch0 = new TF1("fexpo_ch0","expo",positions[0]+offset,positions[npoints-1]+offset);
  fexpo_ch0->SetParameters(10,-0.005);
  fexpo_ch0->SetLineColor(kBlue);
  gatt_ch0->Fit(fexpo_ch0,"RQ");
  
  TF1 *fexpo_ch1 = new TF1("fexpo_ch1","expo",positions[0]+offset,positions[npoints-1]+offset);
  fexpo_ch1->SetParameters(10,0.005);
  fexpo_ch1->SetLineColor(kRed);
  gatt_ch1->Fit(fexpo_ch1,"RQ");
  
  std::cout << "Attenuation length / channel 0 = " << 1./fabs(fexpo_ch0->GetParameter(1)) << " mm" << std::endl;
  std::cout << "Attenuation length / channel 1 = " << 1./fabs(fexpo_ch1->GetParameter(1)) << " mm" << std::endl;
  
  double x0,y0,x1,y1;
  gatt_ch0->GetPoint(npoints-1,x0,y0);
  gatt_ch1->GetPoint(0,x1,y1);
  double gmin = y0 < y1 ? y0 : y1;
  gatt_ch0->GetPoint(0,x0,y0);
  gatt_ch1->GetPoint(npoints-1,x1,y1);
  double gmax = y0 > y1 ? y0 : y1;
  
  can_att->cd(2);
  gPad->SetGrid(1,1);
  gatt_ch0->GetYaxis()->SetRangeUser(0.9*gmin,1.1*gmax);
  gatt_ch0->Draw("AP");
  gatt_ch1->Draw("P");
  
  return true;
}
