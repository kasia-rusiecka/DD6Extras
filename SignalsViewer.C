//************************************************
//*                                              *
//*               SignalsViewer.C                *
//*             Katarzyna Rusiecka               *
//*    katarzyna.rusiecka@doctoral.uj.edu.pl     * 
//*                Created in 2019               *
//*                                              *
//************************************************

#include <fstream>
#include <iostream>
#include "TString.h"

// ROOT script for fast viewing of signals recorded by the Desktop Digitizer.
// Opens binary files recorded by the Desktop Digitizer, reads them and fills
// histograms. Signals are plotted on the canvas with the WaitPrimitive() 
// method, so to see next signal double-click on the canvas is required. 
// Script should be run in the directory where data is stored. Binary data
// is sufficient for this script to run, data doesn't have to be digitized. 
// Three functions are implememnted within this macro:
// (1) SignalsViewer(Int_t ch0, Int_t ch1, Int_t ylimit, Bool_t BL)
// Draws correlated signals from two chosen channels ch0 and ch1. 
// (2) SignalsViewer(Int_t ch, Bool_t BL)
// Draws a signal from a chosen channel ch.
// (3) CutAndView(Int_t ch, TString mode, Double_t xmin, Double_t xmax, Int_t no)
// Allows to view specific signals, selected based on the cut on signal amplitude
// (fAmp), uncalibrated integral (fCharge), calibrated integral (fCal) or time T0 (fT0).
// Cut ranges are given as xmin and xmax.

// If .rootrc file is setup correctly this macro will be loaded in ROOT session
// automatically and all functions will be available in data directories.

// To run type:
//   root
//   .L SignalsViewer.C
//   Function(arguments)

//-----------------------------------------------------------------

TString path = "./";           // path to current directory, where data is stored
const Int_t ipoints = 1024;    // number of samples in 1 signal
const Float_t mV = 4.096;      // to calibrate ADC channels to mV
const Int_t iBL = 50;          // number of points for base line calculation

//-----------------------------------------------------------------

// Arguments:
// ch0 - number of the first channel for drawing
// ch1 - number of the second channel for drawing
// ylimit - upper limit for Y-axis drawing
// BL - flag for base line subtraction

Bool_t SignalsViewer(Int_t ch0, Int_t ch1, Int_t ylimit, Bool_t BL){
  
  Int_t counter = 1;             // signal number
  float x;                       // data from file ch0
  float y;                       // data from file ch1
  
  //----- Setting histograms and canvas
  TString title = Form("channel_%i_and_%i",ch0,ch1);
  TCanvas *can = new TCanvas(title,title,800,800);
  TH1F *h0 = new TH1F("h0","h0",ipoints,0,ipoints);
  TH1F *h1 = new TH1F("h1","h1",ipoints,0,ipoints);
  h0->SetLineColor(kBlack);
  h1->SetLineColor(kRed);
  
  //----- Opening data files
  TString fname0 = path+Form("wave_%i.dat",ch0);     //only binary files support
  std::ifstream input0(fname0, std::ios::binary);
  TString fname1 = path+Form("wave_%i.dat",ch1);     //only binary files support
  std::ifstream input1(fname1, std::ios::binary); 
  
  Float_t baseLine0 = 0;
  Float_t baseLine1 = 0;
  
  while(input0.good()){
      
    baseLine0 = 0;
    baseLine1 = 0;
    
    //----- Filling non-base-line-subtracted histograms
    for(Int_t i=1; i<ipoints+1; i++){
      input0.read((char*)&x,sizeof(x));
      input1.read((char*)&y,sizeof(y));
      h0->SetBinContent(i,x/mV);
      h1->SetBinContent(i,y/mV);
    }
    
    //----- Base line calculation and subtraction
    if(BL){
      for(Int_t i=1; i<iBL+1; i++){
        baseLine0+=h0->GetBinContent(i);
        baseLine1+=h1->GetBinContent(i);
      }
      
      baseLine0=baseLine0/iBL;
      baseLine1=baseLine1/iBL;
      
      for(Int_t i=1; i<ipoints+1; i++){
	      h0->SetBinContent(i,h0->GetBinContent(i)-baseLine0);
	      h1->SetBinContent(i,h1->GetBinContent(i)-baseLine1);
      }
    }
   
    //----- Setting Y-axis range
    if(ylimit!=0){
      h0->GetYaxis()->SetRangeUser(-10,ylimit);
      h1->GetYaxis()->SetRangeUser(-10,ylimit);
    }
    else{
      h0->GetYaxis()->SetRangeUser(h0->GetBinContent(20)-10,
                                   h0->GetBinContent(h0->GetMaximumBin())+20);
      h1->GetYaxis()->SetRangeUser(h1->GetBinContent(20)-10,
                                   h1->GetBinContent(h0->GetMaximumBin())+20);
    }
    
    //----- Drawing
    gPad->SetGrid(1,1);
    h0->SetTitle(Form("Signal number %i",counter));
    h0->Draw();
    h1->Draw("same");
    can->Update();
    can->WaitPrimitive();
    counter++;
  }
  
  input0.close();
  input1.close();
  
  return kTRUE;
}

//-----------------------------------------------------------------

// Arguments:
// ch - channel number
// BL - flag for base line subtraction

Bool_t SignalsViewer(Int_t ch, Bool_t BL_flag){
  
  Int_t counter = 1;     // signal number
  float x;               // data from the file
  
  //----- Setting histogram and canvas
  TString title = Form("channel_%i",ch);
  TCanvas *can = new TCanvas(title,title,800,800);
  TH1F *h = new TH1F("h","h",ipoints,0,ipoints);
  
  //----- Opening data file
  TString fname = path+Form("wave_%i.dat",ch);   //only binary files support
  std::ifstream input(fname, std::ios::binary); 
  
  Float_t BL = 0;
  
  while(input.good()){
    
    //----- Filling non-base-line-subtracted histogram
    for(Int_t i=1; i<ipoints+1; i++){
      input.read((char*)&x,sizeof(x));
      h->SetBinContent(i,x/mV);
    }
    
    //----- Calculating and subtracting base line
    if(BL_flag){
        for(Int_t i=1; i<iBL+1; i++){
            BL+=h->GetBinContent(i);
        }
        BL = BL/iBL;
        for(Int_t i=1; i<ipoints+1; i++){
            h->SetBinContent(i,h->GetBinContent(i)-BL);
        }
    }
    
    //----- Drawing
    gPad->SetGrid(1,1);
    h->SetTitle(Form("Signal number %i",counter));
    h->Draw();
    can->Update();
    can->WaitPrimitive();
    counter++;
  }
  
  input.close();
  
  return kTRUE;
}

//-----------------------------------------------------------------

// Arguments:
// ch - channel number
// mode - selection for signals viewing; available options: fAmp, 
// fCharge, fCal, fT0.
// xmin - lower cut for signals viewing
// xmax - upper cut for signals viewing
// no - number of signals

Bool_t CutAndView(Int_t ch, TString mode, Double_t xmin, Double_t xmax, Int_t no){
  
  float x;             // data from the file
 
  TFile *file = new TFile("results.root","READ");    // opening ROOT file
  TTree *tree = (TTree*)file->Get("tree_ft");        // accessing tree
  
  //----- setting up canvas
  TString title = Form("channel_%i",ch);
  TCanvas *can = new TCanvas(title,title,1200,1200);
  can->DivideSquare(no);
  
  //----- setting up histograms
  std::vector <TH1F*> h(no);
  
  for(Int_t i=0; i<no; i++){  
    h[i] = new TH1F(Form("h_%i",i),Form("h_%i",i),ipoints,0,ipoints);
  }
  
  //----- opening data file
  TString fname = Form("wave_%i.dat",ch);     //only binary files support
  ifstream input(fname, ios::binary); 
  
  //----- setting branch address
  DDSignal *sig = new DDSignal();
  TString bname = Form("ch_%i",ch);
  tree->SetBranchAddress(bname,&sig);
  Int_t nentries = tree->GetEntries();
  
  Int_t counter = 0;
  Double_t BL = 0;     //base line
 
  for(Long64_t i=0; i<nentries; i++) {
     tree->GetEntry(i);
     BL = 0;
     
     if(mode == "fAmp"){
       if(sig->GetAmplitude()>xmin && sig->GetAmplitude()<xmax){   // selection of signals of specific amplitude                          
         input.clear();                                            // setting position in the file
         input.seekg(0,std::ios::beg);
         input.seekg(sizeof(x)*ipoints*i);
           for(Int_t ii=1; ii<ipoints+1; ii++){                    // filling non-bas-line-subtracted histogram and calculating BL
             input.read((char*)&x, sizeof(x));
             h[counter]->SetBinContent(ii,x/mV);
             if(ii<iBL) 
               BL+=x/mV;
           }
           BL = BL/iBL;
           for(Int_t ii=1; ii<ipoints+1; ii++){                    // base line subtraction
             h[counter]->SetBinContent(ii,h[counter]->GetBinContent(ii)-BL);
           }
         can->cd(counter+1);     // drawing
         gPad->SetGrid(1,1);
         h[counter]->Draw();
         if(counter< no-1)     // checking number of signals
           counter++;
         else 
           break;
       }
     }
     else if(mode == "fCharge"){                                  // selection of signals of specific charge (uncalibrated)
       if(sig->GetCharge()>xmin && sig->GetCharge()<xmax){
         input.clear();
         input.seekg(0,std::ios::beg);
         input.seekg(sizeof(x)*ipoints*i);
         for(Int_t ii=1; ii<ipoints+1; ii++){
           input.read((char*)&x,sizeof(x));
           h[counter]->SetBinContent(ii,x/mV);
           if(ii<iBL)
               BL+=x/mV;
         }
         BL = BL/iBL;
         for(Int_t ii=1; ii<ipoints+1; ii++){
           h[counter]->SetBinContent(ii,h[counter]->GetBinContent(ii)-BL);
         }
         can->cd(counter+1);
         gPad->SetGrid(1,1);
         h[counter]->Draw();
         if(counter< no-1)
           counter++;
         else 
           break;
       }
     }
     else if(mode == "fCal"){                                  // selection of signals of specific charge (calibrated)
       if(sig->GetCalibrated()>xmin && sig->GetCalibrated()<xmax){
         input.clear();
         input.seekg(0,std::ios::beg);
         input.seekg(sizeof(x)*ipoints*i);
         for(Int_t ii=1; ii<ipoints+1; ii++){
           input.read((char*)&x,sizeof(x));
           h[counter]->SetBinContent(ii,x/mV);
           if(ii<iBL)
               BL+=x/mV;
         }
         BL = BL/iBL;
         for(Int_t ii=1; ii<ipoints+1; ii++){
           h[counter]->SetBinContent(ii,h[counter]->GetBinContent(ii)-BL);
         }
         can->cd(counter+1);
         gPad->SetGrid(1,1);
         h[counter]->Draw();
         if(counter< no-1)
           counter++;
         else 
           break;
       }
     }
     else if(mode == "fT0"){                                  // selection of signals of specific time T0
       if(sig->GetT0()>xmin && sig->GetT0()<xmax){
         input.clear();
         input.seekg(0,std::ios::beg);
         input.seekg(sizeof(x)*ipoints*i);
         for(Int_t ii=1; ii<ipoints+1; ii++){
           input.read((char*)&x,sizeof(x));
           h[counter]->SetBinContent(ii,x/mV);
           if(ii<iBL)
               BL+=x/mV;
         }
         BL = BL/iBL;
         for(Int_t ii=1; ii<ipoints+1; ii++){
           h[counter]->SetBinContent(ii,h[counter]->GetBinContent(ii)-BL);
         }
         can->cd(counter+1);
         gPad->SetGrid(1,1);
         h[counter]->Draw();
         if(counter< no-1)
           counter++;
         else 
           break;
       }
     }
     else{
       std::cout << "##### Unknown mode! Possible versions are: fAmp, fCharge, fCal, fT0" << std::endl;  
       return kFALSE;
     }
  }
     
  input.close();
  file->Close();
  
  return kTRUE;
}
