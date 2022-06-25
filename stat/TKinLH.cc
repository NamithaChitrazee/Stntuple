///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

#include "Stntuple/stat/TKinLH.hh"

ClassImp(stntuple::TKinLH)

namespace stntuple {

//-----------------------------------------------------------------------------
TKinLH::TKinLH(int Mode, const char* Name, double PMin, double PMax, int Debug) : TNamed(Name, Name) {

  fMode          = Mode;
  fDebug         = Debug;
  pmin           = PMin;
  pmax           = PMax;
  
  fHist.prob_sig = nullptr;
  fHist.prob_bgr = nullptr;
  
  fHist.llhr     = nullptr;
  fHist.llhs     = nullptr;
  fHist.llhb     = nullptr;
  fHist.gen_psig     = nullptr;
  fHist.gen_pbgr     = nullptr;
  
  init();
}

//-----------------------------------------------------------------------------
TKinLH::~TKinLH() {
}

//-----------------------------------------------------------------------------
int TKinLH::generate() {
  return 0;
}

//-----------------------------------------------------------------------------
int TKinLH::init() {

  TH1F* h = gh1("/projects/mu2e/hist/su2020/su2020.cele0s61b1.su2020_track_ana.1010.hist",
                "su2020_TrackAna",
                "trk_2010/p");

  if (fHist.prob_sig) {
    delete fHist.prob_sig;
    delete fHist.prob_bgr;
  }
  
  TString sig_name = Form("%s_m%i_prob_sig",GetName(),fMode);
  TString bgr_name = Form("%s_m%i_prob_bgr",GetName(),fMode);
    
  fHist.prob_sig = (TH1F*) h->Clone(sig_name.Data());
  fHist.prob_bgr = (TH1F*) h->Clone(bgr_name.Data());

  fHist.prob_bgr->Reset();

  int nx   = fHist.prob_sig->GetNbinsX();
  for (int i=0; i<nx; i++) {
    double x = fHist.prob_sig->GetBinCenter(i+1);
    if ((x < pmin) or (x > pmax)) {
      fHist.prob_sig->SetBinContent(i+1,0);
      fHist.prob_sig->SetBinError(i+1,0);
    }
    else {
      fHist.prob_bgr->SetBinContent(i+1,1);
      fHist.prob_bgr->SetBinError  (i+1,0);
    }
  }

  double sws = fHist.prob_sig->Integral();
  fHist.prob_sig->Scale(1./sws/(pmax-pmin));

  double swb = fHist.prob_bgr->Integral();
  fHist.prob_bgr->Scale(1./swb/(pmax-pmin));

  fSig = new TF1("f_sig",TKinLH::f_sig,103.6,104.9,0);
  fSig->SetNpx(10000);
  fSig->SetLineColor(kBlue+2);

  return 0;
}

//-----------------------------------------------------------------------------
double TKinLH::bgr_mom() {
  double p = pmin + (pmax-pmin)*fRng.Rndm();
  return p;
}

//-----------------------------------------------------------------------------
double TKinLH::sig_mom() {
  double p = fSig->GetRandom();
  return p;
}

//-----------------------------------------------------------------------------
// normalized to the integral (not sum over the bins !)  = 1
//-----------------------------------------------------------------------------
double TKinLH::lh_bgr(double P) {
  int bin  = fHist.prob_bgr->FindBin(P);
  double x = fHist.prob_bgr->GetBinContent(bin);
  
  return x;
}


//-----------------------------------------------------------------------------
double TKinLH::f_sig(double* X, double * P) {
  double f(0);

  int const nranges(4);

  double prange[nranges][2] = {
    102.00, 103.50,
    103.40, 104.20,
    104.15, 104.70,
    104.65, 105.10
  };

  double pmean(105);
  
  double par[nranges][4] = {
     2.60578e-02,  2.49680e-02,  8.57067e-03,  1.01647e-03,
     2.99588e-02,  2.66528e-02,  7.15730e-03,  4.32025e-04,
    -1.81856e-02, -1.31032e-01, -1.71238e-01, -7.00615e-02,
     5.95903e-04, -5.73379e-03,  3.24206e-02, -9.19974e-02
  } ;

  double p  = X[0];
  double dp = p-pmean;

  // 1. figure out the range, assume p>prange[0]

  if ((p < prange[0][0]) or (p > prange[nranges-1][1])) {
    printf(" momentum p = %10.3f is outside the range,  BAIL OUT\n",p);
    return 1;
  }

  int ir = -1;
  for (int i=0; i<nranges; i++) {
    if (p <= prange[i][1]) {
      ir = i;
      break;
    }
  }

  if ((ir < nranges-1) and (p >= prange[ir+1][0])) {
    // overlap region
    double f1 = par[ir  ][0] + par[ir  ][1]*dp + par[ir  ][2]*dp*dp + par[ir  ][3]*dp*dp*dp;
    double f2 = par[ir+1][0] + par[ir+1][1]*dp + par[ir+1][2]*dp*dp + par[ir+1][3]*dp*dp*dp;

    double pmin = prange[ir+1][0];
    double pmax = prange[ir  ][1];
    
    double dpp = pmax-pmin;
    
    f = (f2*(p-pmin) + f1*(pmax-p))/dpp;
  }
  else {
    // no overlap
    f = par[ir][0] + par[ir][1]*dp + par[ir][2]*dp*dp + par[ir][3]*dp*dp*dp;
  }
    
  return f;
}



//-----------------------------------------------------------------------------
// normalized to the integral (not sum over the bins !)  = 1
//-----------------------------------------------------------------------------
double TKinLH::lh_sig(double P) {

  double p = fSig->Eval(P);
  return p;
}


//-----------------------------------------------------------------------------
int TKinLH::run(int NObs, int NPe) {

  if (fHist.llhs) {
    delete fHist.llhs;
    delete fHist.llhb;
    delete fHist.llhr;

    delete fHist.gen_pbgr;
    delete fHist.gen_psig;
  }

  TString name, title;
  
  title = Form("llhs name:%s Nev=%i pmin=%5.1f",GetName(),NObs,pmin);
  fHist.llhs = new TH1D(Form("%s_m%i_h_llhs_%i",GetName(),fMode,NObs),title.Data(),20000,-10,10);
  fHist.llhs->SetMarkerStyle(20);

  title = Form("llhb name:%s Nev=%i pmin=%5.1f",GetName(),NObs,pmin);
  fHist.llhb = new TH1D(Form("%s_m%i_h_llhb_%i",GetName(),fMode,NObs),title.Data(),20000,-10,10);
  fHist.llhb->SetMarkerStyle(20);

  title = Form("llhr name:%s Nev=%i pmin=%5.1f",GetName(),NObs,pmin);
  fHist.llhr = new TH1D(Form("%s_m%i_h_llhr_%i",GetName(),fMode,NObs),title,20000,-10,10);
  fHist.llhr->SetMarkerStyle(20);

  title   = Form("llhrR name:%s Nev=%i pmin=%5.1f",GetName(),NObs,pmin);
  fHist.llhrR = new TH1D(Form("%s_m%i_h_llhrR_%i",GetName(),fMode,NObs),title,20000,-10,10);
  fHist.llhrR->SetMarkerStyle(20);

  title = Form("Generated P(bgr) name:%s Nev=%i pmin=%5.1f",GetName(),NObs,pmin);
  fHist.gen_pbgr = new TH1D(Form("%s_m%i_h_pbgr",GetName(),fMode),title,200,100,110);
  
  title = Form("Generated P(sig) name:%s Nev=%i pmin=%5.1f",GetName(),NObs,pmin);
  fHist.gen_psig = new TH1D(Form("%s_m%i_h_gen_psig",GetName(),fMode),title,200,100,110);

  for (int i=0; i<NPe; i++) {
                                        // next pseudoexperiment
    double tot_lhb = 1;
    double tot_lhs = 1;

    for (int j=0; j<NObs; j++) {
                                        // generate momentum, background hypothesis
      double p = -1;
      if      (fMode == 0) p = bgr_mom();
      else if (fMode == 1) p = sig_mom();

      double lhb  = lh_bgr(p);           // 
      tot_lhb    *=lhb;

      double lhs = lh_sig(p);
      tot_lhs   *= lhs;
                                        // fill histograms for generated momentum
      fHist.gen_pbgr->Fill(p,1);
      fHist.gen_psig->Fill(p,1);
    }

    double llhs = log(tot_lhs);
    double llhb = log(tot_lhb);
    
    double llhr = llhb-llhs;

    if (fDebug != 0) {
      printf("tot_lhs, tot_lhb, llhs, llhb, llhr: %12.5e %12.5e %12.5e  %12.5e %12.5e \n",
             tot_lhs, tot_lhb, llhs, llhb, llhr);
    }

    fHist.llhs->Fill(llhs);
    fHist.llhb->Fill(llhb);
    
    fHist.llhr->Fill(llhr);

    double llhrR = llhr/NObs;
    
    fHist.llhrR->Fill(llhrR);
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
int TKinLH::save_hist(const char* Filename, const char* Option) {

  TFile* f = TFile::Open(Filename,Option);
    
  fHist.llhr->Write();
  fHist.llhrR->Write();
  fHist.llhs->Write();
  fHist.llhb->Write();
  fHist.gen_psig->Write();
  fHist.gen_pbgr->Write();

  fHist.prob_sig->Write();
  fHist.prob_bgr->Write();

  f->Write();
  f->Close();
  
  delete f;
  
  return 0;
}
}
