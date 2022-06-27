///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

#include "Stntuple/stat/TKinLH.hh"

ClassImp(stntuple::TKinLH)

namespace stntuple {

//-----------------------------------------------------------------------------
TKinLH::TKinLH(const char* Name, double CL, int Mode, double PMin, double PMax, int Debug) : TBelt(Name, CL) {

  fMode                 = Mode;
  fDebug.fRun           = 0;
  fDebug.fConstructBelt = 0;
  fDebug.fTestCoverage  = 0;
  fDebug.fMuMin         = 0;
  fDebug.fMuMax         = -1;
  pmin                  = PMin;
  pmax                  = PMax;
  fColor                = kBlue+2;
  
  fHist.fProb       = nullptr;
  fHist.fLlh        = nullptr;
  fHist.fInterval   = nullptr;
  fHist.fBeltLo     = nullptr;
  fHist.fBeltHi     = nullptr;
  fHist.fBeltNO     = nullptr;
  fHist.fBeltUL     = nullptr;
  fHist.fCoverage   = nullptr;

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
//-----------------------------------------------------------------------------
// read in the CE momentum distribution and form the signal and the background
// probability distributions 'fHist.prob_sig' and 'fHist.prob_bgr'
// for the moment, consider uniform distribution for the background
//-----------------------------------------------------------------------------

  TH1F* h = gh1("/projects/mu2e/hist/su2020/su2020.cele0s61b1.su2020_track_ana.1010.hist",
                "su2020_TrackAna",
                "trk_2010/p");

  TString sig_name = Form("h_%s_m%i_prob_sig",GetName(),fMode);
  TString bgr_name = Form("h_%s_m%i_prob_bgr",GetName(),fMode);
    
  fHist.prob_sig = (TH1F*) h->Clone(sig_name.Data());
  fHist.prob_bgr = (TH1F*) h->Clone(bgr_name.Data());

  fHist.prob_bgr->Reset();

  double bin = fHist.prob_sig->GetBinWidth(1);

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
  fHist.prob_sig->Scale(1./sws/bin);

  double swb = fHist.prob_bgr->Integral();
  fHist.prob_bgr->Scale(1./swb/bin);
//-----------------------------------------------------------------------------
// fSig is a fit of the signal probability distribution
// need that to avoid the binning effect
// in fact, fHist.prob_sig is not used for anything
//-----------------------------------------------------------------------------
  fSig = new TF1("f_sig",TKinLH::f_sig,pmin,pmax,1);
  fSig->SetParameter(0,1.);
  double scale = fSig->Integral(pmin,pmax);
  fSig->SetParameter(0,scale);

  fSig->SetNpx(30000);
  fSig->SetLineColor(kBlue+2);
//-----------------------------------------------------------------------------
// fBgr is a fit of the backgroundprobability distribution
// need that to avoid the binning effect
// in fact, fHist.prob_bgr is not used for anything
//-----------------------------------------------------------------------------
  fBgr = new TF1("f_bgr",TKinLH::f_bgr,pmin,pmax,1);
  fBgr->SetParameter(0,1.);
  scale = fBgr->Integral(pmin,pmax);
  fBgr->SetParameter(0,scale);

  fBgr->SetNpx(30000);
  fBgr->SetLineColor(kRed+2);
//-----------------------------------------------------------------------------
// book remaining histograms
//-----------------------------------------------------------------------------
  TString name, title;
  
  int     nbins_llh(20000), nbins_llhr(20000), nbins_llhrR(10000);
  
  double  llh_min  (-100), llh_max  (100);
  double  llhr_min (-100), llhr_max (100);
  double  llhrR_min(  -2), llhrR_max(  3);

  for (int ix=0; ix<MaxNx; ix++) {
    
    title = Form("llhs name:%s Nev=%i pmin=%5.1f",GetName(),ix,pmin);
    fHist.log_lhs[ix] = new TH1D(Form("h_%s_m%i_log_lhs_n%02i",GetName(),fMode,ix),title.Data(),nbins_llh,llh_min,llh_max);
    fHist.log_lhs[ix]->SetMarkerStyle(6);
    fHist.log_lhs[ix]->SetMarkerColor(fColor);
    fHist.log_lhs[ix]->SetLineColor  (fColor);

    title = Form("llhb name:%s Nev=%i pmin=%5.1f",GetName(),ix,pmin);
    fHist.log_lhb[ix] = new TH1D(Form("h_%s_m%i_log_lhb_n%02i",GetName(),fMode,ix),title.Data(),nbins_llh,llh_min,llh_max);
    fHist.log_lhb[ix]->SetMarkerStyle(6);
    fHist.log_lhb[ix]->SetMarkerColor(fColor);
    fHist.log_lhb[ix]->SetLineColor  (fColor);
    
    title = Form("llhr name:%s Nev=%i pmin=%5.1f",GetName(),ix,pmin);
    fHist.log_lhr[ix] = new TH1D(Form("h_%s_m%i_log_lhr_n%02i",GetName(),fMode,ix),title,nbins_llhr,llhr_min,llhr_max);
    fHist.log_lhr[ix]->SetMarkerStyle(6);
    fHist.log_lhr[ix]->SetMarkerColor(fColor);
    fHist.log_lhr[ix]->SetLineColor  (fColor);

    title   = Form("log(lhrR) name:%s Nev=%i pmin=%5.1f",GetName(),ix,pmin);
    fHist.log_lhrR[ix] = new TH1D(Form("h_%s_m%i_log_lhrR_n%02i",GetName(),fMode,ix),title,nbins_llhrR,llhrR_min,llhrR_max);
    fHist.log_lhrR[ix]->SetMarkerStyle(6);
    fHist.log_lhrR[ix]->SetMarkerColor(fColor);
    fHist.log_lhrR[ix]->SetLineColor  (fColor);
  }

  title = Form("Generated P(bgr) name:%s pmin=%5.1f",GetName(),pmin);
  fHist.gen_pbgr = new TH1D(Form("h_%s_m%i_gen_pbgr",GetName(),fMode),title,200,100,110);
    
  title = Form("Generated P(sig) name:%s pmin=%5.1f",GetName(),pmin);
  fHist.gen_psig = new TH1D(Form("h_%s_m%i_gen_psig",GetName(),fMode),title,200,100,110);
  
  for (int ix=0; ix<MaxNx; ix++) {
    name  = Form("h_%s_m%i_log_lhrR_1_n%02i",GetName(),fMode,ix);
    title = Form("%s_m%i_log_lhrR_1_n%02i",GetName(),fMode,ix);
    
    fHist.log_lhrR_1[ix] = new TH1D(name,title,nbins_llhrR,-50,50);
  }

  name  = Form("h_%s_m%i_sum_log_lhrR_1",GetName(),fMode);
  title = Form("%s_m%i_sum_log_lhrR_1",GetName(),fMode);
  
  fHist.sum_log_lhrR_1 = new TH1D(name,title,nbins_llhrR,-50,50);
//-----------------------------------------------------------------------------
// all histograms booked, determine the LLHR range (for one event),
// scan the range with a small step,
// say, 10,000+1 steps, to include the interval ends
// this step is also model-dependent
//-----------------------------------------------------------------------------
  fMinLLHR    =  1e6;
  fMaxLLHR    = -1e6;
  int nsteps  = 10000;
  double step = (pmax-pmin)/nsteps;
    
  for (int i=0; i<nsteps+1; i++) {
    double p     = pmin+i*step;
                                        // define likelihoods only within the allowed momentum range
    double llhb  = log(lh_bgr(p));
    double llhs  = log(lh_sig(p));
    double llhr  = llhb-llhs;
    if (llhr < fMinLLHR) fMinLLHR = llhr;
    if (llhr > fMaxLLHR) fMaxLLHR = llhr;
  }

  return 0;
}

//-----------------------------------------------------------------------------
double TKinLH::bgr_mom() {
  double p = fBgr->GetRandom();
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
  double p = fBgr->Eval(P);
  return p;
}

//-----------------------------------------------------------------------------
// normalized to the integral (not sum over the bins !)  = 1
//-----------------------------------------------------------------------------
double TKinLH::lh_sig(double P) {
  double p = fSig->Eval(P);
  return p;
}


//-----------------------------------------------------------------------------
// P[0] - integral of the function over [pmin,pmax]
//-----------------------------------------------------------------------------
double TKinLH::f_bgr(double* X, double * P) {
  return 1./P[0];
};


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

  return f/P[0];
}

//-----------------------------------------------------------------------------
int TKinLH::run(int NObs, int NPe) {

  if (fHist.log_lhs) {
    fHist.log_lhs[NObs]->Reset();
    fHist.log_lhs[NObs]->SetLineColor (fColor);
    fHist.log_lhs[NObs]->SetMarkerColor(fColor);

    fHist.log_lhb[NObs]->Reset();
    fHist.log_lhb[NObs]->SetLineColor (fColor);
    fHist.log_lhb[NObs]->SetMarkerColor(fColor);

    fHist.log_lhr[NObs]->Reset();
    fHist.log_lhr[NObs]->SetLineColor (fColor);
    fHist.log_lhr[NObs]->SetMarkerColor(fColor);

    fHist.log_lhrR[NObs]->Reset();
    fHist.log_lhrR[NObs]->SetLineColor (fColor);
    fHist.log_lhrR[NObs]->SetMarkerColor(fColor);

    fHist.gen_pbgr->Reset();
    fHist.gen_psig->Reset();
  }

  if (NObs == 0) {
//-----------------------------------------------------------------------------
// special case - no kinematic info, have to make it up in a coherent way
// need a uniform distribution in a range defined by the probability distributions
//-----------------------------------------------------------------------------
    int nb = fHist.log_lhr[NObs]->GetNbinsX();

    for (int ib=0; ib<nb; ib++) {
      double llhr = fHist.log_lhr[NObs]->GetBinCenter(ib+1);
      if ((llhr >= fMinLLHR) and (llhr <= fMaxLLHR)) {
        fHist.log_lhr[NObs]->SetBinContent(ib+1,1);
        fHist.log_lhr[NObs]->SetBinError  (ib+1,0);
      }
    }

    nb = fHist.log_lhrR[NObs]->GetNbinsX();

    for (int ib=0; ib<nb; ib++) {
      double llhr = fHist.log_lhrR[NObs]->GetBinCenter(ib+1);
      if ((llhr >= fMinLLHR) and (llhr <= fMaxLLHR)) {
        fHist.log_lhrR[NObs]->SetBinContent(ib+1,1);
        fHist.log_lhrR[NObs]->SetBinError  (ib+1,0);
      }
    }
  }
  else {
//-----------------------------------------------------------------------------
// general case: kinematic info is available
//-----------------------------------------------------------------------------
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

        if (fDebug.fRun > 0) {
                                        // fill histograms for the generated momentum
          fHist.gen_pbgr->Fill(p,1);
          fHist.gen_psig->Fill(p,1);
        }
      }

      double llhs = log(tot_lhs);
      double llhb = log(tot_lhb);
    
      double llhr = llhb-llhs;

      if (fDebug.fRun != 0) {
        printf("tot_lhs, tot_lhb, llhs, llhb, llhr: %12.5e %12.5e %12.5e  %12.5e %12.5e \n",
               tot_lhs, tot_lhb, llhs, llhb, llhr);
      }

      fHist.log_lhs[NObs]->Fill(llhs);
      fHist.log_lhb[NObs]->Fill(llhb);
    
      fHist.log_lhr[NObs]->Fill(llhr);
                                        // reduces likelihood
      double llhrR = llhr/NObs;
    
      fHist.log_lhrR[NObs]->Fill(llhrR);
    }
  }
//-----------------------------------------------------------------------------
// fHist.log_lhrR is used to build the acceptance interval, normalize to one
//-----------------------------------------------------------------------------
  double total = fHist.log_lhrR[NObs]->Integral();
  fHist.log_lhrR[NObs]->Scale(1./total);
  return 0;
}


//-----------------------------------------------------------------------------
// three parameters to maintain uniform interface
// Nobs is used only to determine the binomial probabilities
// the step can be completed only after all log_lhrR histograms are filled
// assume the histograms are read in
//-----------------------------------------------------------------------------
int TKinLH::construct_interval(double MuB, double MuS, int NObs) {
  // defined fIxMin, fIxMax, and fSumP

  if (fHist.log_lhs) {
    fHist.sum_log_lhrR_1->Reset();
    
    for (int ix=0; ix<MaxNx; ix++) {
      fHist.log_lhrR_1[ix]->Reset();
    }
  }

  int rc = init_poisson_dist(MuB,MuS,NObs);
  
  if (rc < 0) return rc;
//-----------------------------------------------------------------------------
// next: construct 2-sided likelihood hist.. nobs=0 is a special case, keep int in mind
// find max bin, assume it corresponds to ix=1
//-----------------------------------------------------------------------------
  int nx = fHist.log_lhrR_1[0]->GetNbinsX();

  double pmax  = -1;
  
  for (int n=0; n<MaxNx; n++) {
    double p0 = fProb[n];
                                        // for n=0 all bins are filled with a constant
                                        // such that the sum would equal to 1
    TH1D*  h1 = fHist.log_lhrR[n];
    
    for (int ix=0; ix<nx; ix++) {
      double p = p0*h1->GetBinContent(ix+1);
      if (p > pmax) {
        pmax  = p;
      }
    }
  }
  printf("TKinLH::construct_interval: pmax = %12.ef\n",pmax); 
//-----------------------------------------------------------------------------
// now, create uniformly normalized distributions, 2-sided
//-----------------------------------------------------------------------------
  for (int n=0; n<MaxNx; n++) {
    double p0 = fProb[n];
//-----------------------------------------------------------------------------    
// h1 is supposed to be normalized to an integral (sum of contens of all bins) of 1
//-----------------------------------------------------------------------------
    TH1D*  h1 = fHist.log_lhrR[n];

    for (int ib=0; ib<nx; ib++) { 
      double p     = p0*(h1->GetBinContent(ib+1)/pmax);
      double log_p = -log(p);
      double wt    = p0*h1->GetBinContent(ib+1);
      if (NObs < (MuB+MuS)) log_p = -log_p;
      fHist.log_lhrR_1[n]->Fill(log_p,wt);
    }
  }
//-----------------------------------------------------------------------------
// the weight distributions do not depend on MuB abd MuS, only on kinematics
// they can be generated once and saved 
// at this point, only need to add all of them up
//-----------------------------------------------------------------------------
  for (int n=0; n<MaxNx; n++) {
    fHist.sum_log_lhrR_1->Add(fHist.log_lhrR_1[n]);
  }
//-----------------------------------------------------------------------------
// last step: define the interval in the likelihod_ratio space
//-----------------------------------------------------------------------------
  double sump = 0;
  for (int i=nx; i>0; i--) {
    double p = fHist.sum_log_lhrR_1->GetBinContent(i);
    sump += p;
    if (sump >= fCL) {
                                        // done
      fInterval.fLlhrMin = fHist.sum_log_lhrR_1->GetBinCenter(i);
      fInterval.fLlhrMax = 0;
      fInterval.fProbTot = sump;
      break;
    }
  }
  
  return 0;
}
  
//-----------------------------------------------------------------------------
// vary signal from SMin to SMax in NPoints, construct FC belt, fill belt histogram
// fBelt is the FC belt histogram
// avoid multiple useless re-initializations
//-----------------------------------------------------------------------------
int TKinLH::construct_belt(double MuB, double SMin, double SMax, int NPoints, int NObs) {

  fMuB = MuB;
  
  fBelt.fSMin = SMin;
  fBelt.fSMax = SMax;
  fBelt.fDy   = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 1;

  if ((fBelt.fLlhInterval) and (fBelt.fLlhNPoints = NPoints)) delete fBelt.fLlhInterval;

  fBelt.fLlhNPoints  = NPoints;
  fBelt.fLlhInterval = new double[5*NPoints];

  for (int i=0; i<5*NPoints; i++) {
    fBelt.fLlhInterval[i] = 0;
  }

  for (int i=0; i<NPoints; i++) {
    double mus   = SMin+i*fBelt.fDy;

    int rc       = construct_interval(MuB,mus,NObs);
    // double lhmax = fInterval.fLlhrMax;
    // double lhmin = fInterval.fLlhrMin;

    if (rc == 0) {
      // double llh_lo = lhmin;
      // double llh_hi = lhmax;
      
      fBelt.fLlhInterval[5*i  ] = -fInterval.fLlhrMin;
      fBelt.fLlhInterval[5*i+1] = -fInterval.fLlhrMax;
      fBelt.fLlhInterval[5*i+2] = fInterval.fProbTot;
      fBelt.fLlhInterval[5*i+3] = -1;
      fBelt.fLlhInterval[5*i+4] = -1;
    }
  }

  if (fDebug.fConstructBelt > 0) {
    for (int i=0; i<NPoints; i++) {
      printf("i,fBelt.fLlhInterval[5*i  ],fBelt.fLlhInterval[5*i+1]: %12.5f %12.5f %12.5f\n",
             fBelt.fLlhInterval[5*i  ],fBelt.fLlhInterval[5*i+1], fBelt.fLlhInterval[5*i+2]);
    }
  }
  return 0;

}


//-----------------------------------------------------------------------------
void TKinLH::make_belt_hist() {

  if (fHist.fBeltLo) {
    delete fHist.fBeltLo;
    delete fHist.fBeltHi;
    delete fHist.fBeltNO;
    delete fHist.fBeltUL;
  }
  
  fHist.fBeltLo   = new TH1D(Form("h_belt_lo_%s",GetName()),
                             Form("TBeltLH LO MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                             fBelt.fLlhNPoints,fBelt.fSMin,fBelt.fSMax);

  fHist.fBeltHi   = new TH1D(Form("h_belt_hi_%s",GetName()),
                             Form("TBeltLH HI MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                             fBelt.fLlhNPoints,fBelt.fSMin,fBelt.fSMax);

  fHist.fBeltNO   = new TH1D(Form("h_belt_no_%s",GetName()),
                             Form("TBeltLH NO MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                             fBelt.fLlhNPoints,fBelt.fSMin,fBelt.fSMax);

  fHist.fBeltNO1   = new TH1D(Form("h_belt_no1_%s",GetName()),
                             Form("TBeltLH NO1 MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                             fBelt.fLlhNPoints,fBelt.fSMin,fBelt.fSMax);

  fHist.fBeltUL   = new TH1D(Form("h_belt_ul_%s",GetName()),
                             Form("TBeltLH UL MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                             fBelt.fLlhNPoints,fBelt.fSMin,fBelt.fSMax);

  for (int ix=0; ix<fBelt.fLlhNPoints; ix++) {
    fHist.fBeltLo ->SetBinContent(ix+1,fBelt.fLlhInterval[5*ix+1]);
    fHist.fBeltHi ->SetBinContent(ix+1,fBelt.fLlhInterval[5*ix  ]);
    fHist.fBeltNO ->SetBinContent(ix+1,fBelt.fLlhInterval[5*ix+2]);
    fHist.fBeltUL ->SetBinContent(ix+1,fBelt.fLlhInterval[5*ix+3]);
    fHist.fBeltNO1->SetBinContent(ix+1,fBelt.fLlhInterval[5*ix+4]);
  }

  fHist.fBeltLo->GetXaxis()->SetTitle("#mu_{S}");
  fHist.fBeltLo->GetYaxis()->SetTitle("LLH");
  
  fHist.fBeltLo->SetFillStyle(fBelt.fFillStyle);
  fHist.fBeltLo->SetFillColor(fBelt.fFillColor);
  fHist.fBeltLo->SetLineColor(fBelt.fFillColor);

  fHist.fBeltHi->SetFillStyle(fBelt.fFillStyle);
  fHist.fBeltHi->SetFillColor(fBelt.fFillColor);
  fHist.fBeltHi->SetLineColor(fBelt.fFillColor);

  fHist.fBeltUL->SetFillStyle(3002);
  fHist.fBeltUL->SetFillColor(fBelt.fFillColor);
  fHist.fBeltUL->SetLineColor(fBelt.fFillColor);

  fHist.fBeltNO->SetMarkerStyle(20);
}

//-----------------------------------------------------------------------------
// assume S in 
//-----------------------------------------------------------------------------
int TKinLH::test_coverage(double MuB, double SMin, double SMax, int NPoints) {
  return 0;
}

//-----------------------------------------------------------------------------
int TKinLH::read_hist(const char* Filename) {

  TFile* f = TFile::Open(Filename);

  for (int ix=0; ix<MaxNx; ix++) {
    fHist.log_lhr [ix] = (TH1D*) f->Get(Form("h_%s_m%i_log_lhr_n%02i",GetName(),fMode,ix))->Clone();
    fHist.log_lhs [ix] = (TH1D*) f->Get(Form("h_%s_m%i_log_lhs_n%02i",GetName(),fMode,ix))->Clone();
    fHist.log_lhb [ix] = (TH1D*) f->Get(Form("h_%s_m%i_log_lhb_n%02i",GetName(),fMode,ix))->Clone();

    fHist.log_lhrR  [ix] = (TH1D*) f->Get(Form("h_%s_m%i_log_lhrR_n%02i"  ,GetName(),fMode,ix))->Clone();
    //    fHist.log_lhrR_1[ix] = (TH1D*) f->Get(Form("h_%s_m%i_log_lhrR_1_n%02i",GetName(),fMode,ix))->Clone();
    
  }

  fHist.prob_sig = (TH1F*) f->Get(Form("h_%s_m%i_prob_sig",GetName(),fMode))->Clone();
  fHist.prob_bgr = (TH1F*) f->Get(Form("h_%s_m%i_prob_bgr",GetName(),fMode))->Clone();

  //  fHist.sum_log_lhrR_1 = (TH1D*) f->Get(Form("h_%s_m%i_sum_log_lhrR_1",GetName(),fMode))->Clone();

  // f->Close();
  
  // delete f;
  
  return 0;
}
  

//-----------------------------------------------------------------------------
int TKinLH::save_hist(const char* Filename, const char* Option) {

  TFile* f = TFile::Open(Filename,Option);

  for (int ix=0; ix<MaxNx; ix++) {
    fHist.log_lhr [ix]->Write();
    fHist.log_lhs [ix]->Write();
    fHist.log_lhb [ix]->Write();

    fHist.log_lhrR  [ix]->Write();
    fHist.log_lhrR_1[ix]->Write();
    
  }

  fHist.prob_sig->Write();
  fHist.prob_bgr->Write();

  fHist.sum_log_lhrR_1->Write();

  f->Write();
  f->Close();
  
  delete f;
  
  return 0;
}
  
//-----------------------------------------------------------------------------
void TKinLH::Print(const char* Option) const {
  printf("%-20s: mode:%i PMin: %10.3f PMax:%10.3f\n",GetName(), fMode,pmin, pmax);
}

}
