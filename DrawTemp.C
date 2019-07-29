bool DrawTemp(TString log_name, bool save){

    const char *dataPath = std::getenv("SFDATA");
    TString fname = string(dataPath)+"temp_logs/"+log_name;
    
    ifstream input(fname);
    
    if(!input.is_open()){
      cout << "Input file couldn't be open!" << endl;
      cout << fname << endl;
      return false;
    }
    
    const int nSensors = 4;
    TString IDsensor[nSensors] = {"446D","044F4","2BAD","8F1F"};
    int counter[nSensors] = {0,0,0,0};
    time_t time = 0;
    time_t time_start = 0;
    double temp = 0;
    TString ID = "";
    TString C = "";
    
    TGraph *gTemp[nSensors];
    
    for(int i=0; i<nSensors; i++){
      gTemp[i] = new TGraph();
      gTemp[i]->SetName(IDsensor[i]);
      gTemp[i]->SetTitle(IDsensor[i]);
      gTemp[i]->SetMarkerStyle(20+i);
      gTemp[i]->SetMarkerColor(i+1);
    }
    
    input >> ID >> time_start >> temp >> C;
    char *time_start_char = ctime(&time_start);
    TString time_start_str(time_start_char);
    cout << "start: " << time_start_str << endl;
    
    input.clear();
    input.seekg(0,ios::beg);
    
    while(input.good()){
        input >> ID >> time >> temp >> C;
        for(int i=0; i<nSensors; i++){
          if(ID.Contains(IDsensor[i])){
              gTemp[i]->SetPoint(counter[i]+1,(time-time_start)/60.,temp);
              counter[i]++;
          }
        }
    }
    
    char *time_stop_char = ctime(&time);
    cout << "stop: " << time_stop_char << endl;
    TString time_stop_str(time_stop_char);
    
    TCanvas *can = new TCanvas("can","can",800,400);
    gPad->SetGrid(1,1);
    gTemp[0]->Draw("AP");
    gTemp[1]->Draw("P");
    gTemp[2]->Draw("P");
    gTemp[3]->Draw("P");
    
    gTemp[0]->GetYaxis()->SetRangeUser(21,30);
    gTemp[0]->GetXaxis()->SetRangeUser(0,150E3);
    gTemp[0]->GetYaxis()->SetTitle("temperature [deg C]");
    gTemp[0]->GetXaxis()->SetTitle("time since begining of measurment [min]");
    
    TLegend *leg = new TLegend(0.783,0.131,0.885,0.288);
    leg->AddEntry(gTemp[0],"Out","PEL");
    leg->AddEntry(gTemp[1],"Ch0","PEL");
    leg->AddEntry(gTemp[2],"Ch1","PEL");
    leg->AddEntry(gTemp[3],"Ref","PEL");
    leg->Draw();
    
    TLatex text;
    text.SetNDC(true);
    text.SetTextSize(0.025);
    text.DrawLatex(0.7,0.8,"start: "+time_start_str);
    text.DrawLatex(0.7,0.75,"stop: "+time_stop_str);
    
    int n = log_name.Length();
    char char_array[n-3];
    string log_name_short = strcpy(char_array,string(log_name).c_str());
    
    TString root_fname = string(dataPath)+"temp_logs/"+log_name_short+".root";
    cout << root_fname << endl;
    TFile *file = new TFile(root_fname,"RECREATE");
    
    if(save){
      gTemp[0]->Write();
      gTemp[1]->Write();
      gTemp[2]->Write();
      gTemp[3]->Write();
      can->Write();
    }
    
    can->SaveAs("temperature.png");
    file->Close();
    
    return true;
}
