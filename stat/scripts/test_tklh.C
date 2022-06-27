///////////////////////////////////////////////////////////////////////////////
// tests of the TKinLH class
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/TKinLH.hh"
using namespace stntuple;
//-----------------------------------------------------------------------------
// plot normalised reduced likelihood
//-----------------------------------------------------------------------------
TH1D* test_tklh_001(const char* Name = "test_mllh_001", double CL=0.9, int Mode = 0,
                    int NObs = 1, int NPExp = 1000000, 
                    double PMin=103.6, double PMax=104.9) {
  
  TString   title;

  TKinLH* x1 = new TKinLH(Name,CL,Mode=0,PMin,PMax);
  x1->run(NObs,NPExp);
  
  // make probability histogram

  TString hname = Form("h_%s_prob",Name);

  TH1D* h_prob = new TH1D(hname.Data(),Form("P(bgr) for N(obs):%i",NObs),20000,-20,0);

  double tot =  x1->fHist.log_lhrR[NObs]->Integral();

  int nx = x1->fHist.log_lhrR[NObs]->GetNbinsX();

  for (int i=0; i<nx; i++) {
    double p    = x1->fHist.log_lhrR[NObs]->GetBinContent(i+1)/tot;
    double logp = log(p);
    
    h_prob->Fill(logp,p);
  }

  return h_prob;
}

//-----------------------------------------------------------------------------
// generate distributions in LLHR for a range of Nobs and store them
//-----------------------------------------------------------------------------
int test_tklh_002(const char* Name = "test_thlh_002", double CL=0.9, int Mode = 0, 
                  double PMin = 102.0, double PMax = 105.0, 
                  int NMax = TKinLH::MaxNx, long int NPe = 100000) {

  TKinLH* x1 = new TKinLH(Name,CL,Mode,PMin,PMax);
  //  TKinLH* x2 = new TKinLH(mode=1,"x2",PMin);

  for (int nobs=0; nobs<NMax; nobs++) {
    x1->run(nobs,NPe);
    //    x2->run(nobs,npe);
  }
//-----------------------------------------------------------------------------
// save all histograms once in the end
//-----------------------------------------------------------------------------
  TString fn = Form("tklh_%0.f_%.0f.root",PMin*10,PMax*10);
  
  TFile* f;

  x1->save_hist(fn.Data(),"recreate");
  //  x2->save_hist(fn.Data(),"update");

  return 0;
}


TH1D* h_bgr_llhr;
TH1D* h_sig_llhr;
//-----------------------------------------------------------------------------
// read histogram file and construct a confidence interval
// kinematic histograms do not know anything about CL, but do know about the object name...
//-----------------------------------------------------------------------------
int test_tklh_003(const char* Name, double CL, int Mode = 0,
                  double MuB=0.1, double MuS=4.5,
                  double PMin = 103.6, double PMax=104.9) {

  TKinLH* x1 = new TKinLH(Name,CL,Mode,PMin,PMax);


  TString fn = Form("tklh_%.0f_%.0f.root",PMin*10,PMax*10);

  x1->read_hist(fn.Data());

  x1->construct_interval(MuB,MuS,CL);

  x1->fHist.sum_log_lhrR_1->Draw("");
    
  return 0;
}

//-----------------------------------------------------------------------------
// read histogram file and construct a confidence interval
// plot belt
//-----------------------------------------------------------------------------
int test_tklh_004(const char* Name, double CL, int Mode = 0,
                  double PMin = 103.6, double PMax=104.9,
                  double MuB=0.1, double SMin=1, double SMax=5,
                  int NPoints = 10, int NObs = 0)
{
  TKinLH* x1 = new TKinLH(Name,CL,Mode,PMin,PMax);

  TString fn = Form("tklh_%.0f_%.0f.root",PMin*10,PMax*10);

  x1->read_hist(fn.Data());

  x1->construct_belt(MuB,SMin,SMax,NPoints,NObs);
  x1->make_belt_hist();

  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) c = new TCanvas(cname,Name,1200,800);
  else              c->cd();

  x1->fHist.fBeltHi->GetYaxis()->SetRangeUser(0,5);
  x1->fHist.fBeltHi->Draw();
  // llh->fHist.fBeltHi->Draw("same");
  // x1->fHist.fBeltNO1->Draw("same");
    
  return 0;
}
