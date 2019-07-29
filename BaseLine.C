Bool_t  BaseLine(void){
 
  Int_t counter = 1;     // signal number
  Int_t ipoints = 1024;  // number of samples in one signal
  const Int_t nch = 3;   // number of channels
  float x;               // data from the file
  
  //--- setting canvas and histograms
  TCanvas *can = new TCanvas("base_line","base_line",800,800);
  
  TH1F *hch0 = new TH1F("hch0","hch0",ipoints,0,ipoints); 
  TH1F *hch1 = new TH1F("hch1","hch1",ipoints,0,ipoints); 
  TH1F *hch2 = new TH1F("hch2","hch2",ipoints,0,ipoints); 
  
  hch0->SetLineColor(kBlue);
  hch1->SetLineColor(kRed);
  hch2->SetLineColor(kGreen+2);
  
  //--- setting input streams
  std::ifstream in_ch0("wave_0.dat", std::ios::in | std::ios::binary);
  std::ifstream in_ch1("wave_1.dat", std::ios::in | std::ios::binary);
  std::ifstream in_ch2("wave_2.dat", std::ios::in | std::ios::binary);
  
  Float_t sum[nch];
  std::cout << "ch0 \t ch1 \t ch2" << std::endl;
  
  //--- reading files
  while(in_ch0.good()){
      
    sum[0]=0;
    sum[1]=1;
    sum[2]=0;
    
    //--- filling signal histograms
    for(Int_t i=1; i<ipoints+1; i++){
      //--- ch0
      in_ch0.read((char*)&x,sizeof(x));
      hch0->SetBinContent(i,x);
      sum[0]+=x;
      //--- ch1
      in_ch1.read((char*)&x,sizeof(x));
      hch1->SetBinContent(i,x);
      sum[1]+=x;
      //--- ch2
      in_ch2.read((char*)&x,sizeof(x));
      hch2->SetBinContent(i,x);
      sum[2]+=x;
    }
    
    std::cout << sum[0]/ipoints << "\t" << sum[1]/ipoints 
              << "\t" <<  sum[2]/ipoints << std::endl;
    
    //----- drawing
    gPad->SetGrid(1,1);
    hch0->Draw();
    hch0->GetYaxis()->SetRangeUser(1400,1800);
    hch1->Draw("same");
    hch2->Draw("same");
    can->Update();
    can->WaitPrimitive();
    counter++;
  }
  
  //--- closing files
  in_ch0.close();
  in_ch1.close();
  in_ch2.close();
  
  return kTRUE;
}
