///////////////////////////////////////////////////////////////////////////////
// tests of the TKinLH class
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/TKinLH.hh"

//-----------------------------------------------------------------------------
// plot normalised reduced likelihood
//-----------------------------------------------------------------------------
TH1D* test_tklh_001(int NObs = 1, int NPExp = 1000000, const char* Name = "test_mllh_001", double PMin=103.6) {
  TString   name, title;

  int mode;
  
  TKinLH* x1 = new TKinLH(mode=0,Name,PMin);
  x1->run(NObs,NPExp);
  // make probability histogram

  TString hname = Form("h_%s_prob",Name);

  TH1D* h_prob = new TH1D(hname.Data(),Form("P(bgr) for N(obs):%i",NObs),20000,-20,0);

  double tot =  x1->h_llhrR->Integral();

  int nx = x1->h_llhrR->GetNbinsX();

  for (int i=0; i<nx; i++) {
    double p    = x1->h_llhrR->GetBinContent(i+1)/tot;
    double logp = log(p);
    
    h_prob->Fill(logp,p);
  }

  return h_prob;
}

//-----------------------------------------------------------------------------
// generate distributions in LLHR for different number of events and store them
//-----------------------------------------------------------------------------
int test_tklh_002(double PMin = 103.6) {

  int mode;
  
  TKinLH* x1 = new TKinLH(mode=0,"x1",PMin);
  TKinLH* x2 = new TKinLH(mode=1,"x2",PMin);

  long int npe = 10000000; // 0;

  for (int nev=1; nev<11; nev++) {
    x1->run(nev,npe);
    x2->run(nev,npe);
  }
//-----------------------------------------------------------------------------
// save all histograms once in the end
//-----------------------------------------------------------------------------
  TString fn = "tklh_pmin_1036.root";
  
  TFile* f;

  f = TFile::Open(fn.Data(),"recreate");
  f->Write();
  f->Close();
  delete f;
  
  x1->save_hist(fn.Data(),"update");
  x2->save_hist(fn.Data(),"update");

  return 0;
}


TH1D* h_bgr_llhr;
TH1D* h_sig_llhr;
//-----------------------------------------------------------------------------
// read histogram file and make expected distributions in LLHR
// assume PMin, PMax have only two digits after the decimal point 
//-----------------------------------------------------------------------------
int test_tklh_003(double MuB=0.1, double MuS=4.5, double PMin = 103.6, double PMax=104.9) {

  // TKinLH* x1 = new TKinLH(1,PMin,"x1");
  // TKinLH* x2 = new TKinLH(2,PMin,"x2");

  //  long int npe = 100000; // 000;

  int ipmin = (int) (PMin*100);
  int ipmax = (int) (PMax*100);
  
  TString fn = Form("tklh_%i_%i.root",;
  
  TFile* f;

  f = TFile::Open(fn.Data());

  h_bgr_llhr = (TH1D*) f->Get("x1_m1_h_llhr_1")->Clone("h_bgr_llht");
  h_bgr_llhr->Reset();

  h_sig_llhr = (TH1D*) h_bgr_llhr->Clone("h_sig_llhr");

  double     prob_bgr[100];
  double     prob_sig[100];
  
  prob_bgr[0] = exp(-MuB);
  prob_sig[0] = exp(-MuS);

  for (int i=1; i<11; i++) {
    prob_bgr[i] = prob_bgr[i-1]*MuB/i;
    prob_sig[i] = prob_sig[i-1]*MuS/i;
    
    TString hname = Form("x1_m1_h_llhr_%i",i);
    TH1D* h1 = (TH1D*) f->Get(hname.Data());
    h_bgr_llhr->Add(h1,prob_bgr[i]/h1->Integral());

    hname = Form("x2_m2_h_llhr_%i",i);
    TH1D* h2 = (TH1D*) f->Get(hname.Data());
    h_sig_llhr->Add(h2,prob_sig[i]/h2->Integral());
  }

  h_sig_llhr->SetLineColor(kRed+2);
  h_sig_llhr->Draw();

  h_bgr_llhr->Draw("sames");

  // f->Close();
  // delete f;

  return 0;
}
