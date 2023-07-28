///////////////////////////////////////////////////////////////////////////////
//
// fDebuglevel.fTestCoverage = 1: print all
// fDebuglevel.fTestCoverage = 2: print missed
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "TMath.h"
#include "TCanvas.h"
#include "TROOT.h"

#include "Stntuple/stat/upper_limit.hh"

ClassImp(stntuple::upper_limit)

namespace stntuple {
//-----------------------------------------------------------------------------
upper_limit::Belt_t::Belt_t() {
  fFillColor = kRed+2;
  fFillStyle = 3005;

  fXMin      = 0;
  fXMax      = -1;
  fYMin      = 0;
  fYMax      = -1;

  fStack     = nullptr;
  fLo        = nullptr;
  fHi        = nullptr;
}

//-----------------------------------------------------------------------------
upper_limit::upper_limit(const char* Name, double CL, int Type) : TNamed(Name,Name) {

  double alpha = 1-TMath::Erf(5./sqrt(2));
  
  if (CL > 0) fCL = CL;
  else        fCL = 1-alpha/2; // always one-sided: 5.7330314e-07/2 = 2.8665157e-07

  fType = Type;

  fIxMin = MaxNx;
  fIxMax = -1;
  
  fHist.fProb       = nullptr;
  fHist.fInterval   = nullptr;
  // fHist.fBelt       = nullptr;
  // fHist.fBeltLo     = nullptr;
  // fHist.fBeltZr     = nullptr;
  // fHist.fBeltHi     = nullptr;
  fHist.fCoverage   = nullptr;

  fDebug.fConstructBelt = 0;
  fDebug.fTestCoverage  = 0;
  fDebug.fUpperLimit    = 0;
                                        // no printout by default
  fDebug.fMuMin         = 1.;
  fDebug.fMuMax         = 0.;

  fNExp  = 100000;
  fNObs  = -1;

  // fBelt.fFillColor      = kRed+2;
  // fBelt.fFillStyle      = 3005;
//-----------------------------------------------------------------------------
// calculate factorials, do that only once
// assume MaxNx to be large enough, so having N! values up to MaxNx-1 included is enough
//-----------------------------------------------------------------------------
  fFactorial[0] = 1; for (int i=1; i<MaxNx; i++) { fFactorial[i] = fFactorial[i-1]*i; }

  init_poisson_dist(0,1,fNObs);
}

//-----------------------------------------------------------------------------
// mu = MuB+MuS
// 'NObs' is the number of observed events - it constrains the background fluctuations
// NObs<0 means no prior knowledge 
// array 'Prob' should have at least MaxNx elements
// declare NObs as double to be able to scan
// 'nature': given by MuB+Mus - sampled distribution
//-----------------------------------------------------------------------------
int upper_limit::init_poisson_dist(double MuB, double MuS, int NObs) {
//-----------------------------------------------------------------------------
// the length of 'Prob' should be at least N
// CumProb[N]: probability, for a given Mean, to have rn <= N
//-----------------------------------------------------------------------------
  fMean  = MuB+MuS;
  fMuS   = MuS;
  fMuB   = MuB;
  fNObs  = NObs;
  
  if (NObs < 0) {
//-----------------------------------------------------------------------------
// standard Poisson distribution
//-----------------------------------------------------------------------------
    fProb[0] = TMath::Exp(-fMean);

    double pmax = fProb[0];
    fIPMax      = 0;

    for (int i=1; i<MaxNx; i++) {
      fProb[i] = fProb[i-1]*fMean/i;
      if (fProb[i] > pmax) {
	pmax   = fProb[i];
	fIPMax = i;
      }
    }
  }
  else {
//-----------------------------------------------------------------------------
// background probability constrained by the measurement of N events (Zech'1989)
//-----------------------------------------------------------------------------
    double pbn = 0; for (int k=0; k<=NObs; k++) { pbn += TMath::Exp(-MuB)*pow(MuB,k)/fFactorial[k]; }
    
    double pb[MaxNx];
    for (int i=0; i<MaxNx; i++) {
      if (i <= NObs) pb[i] = TMath::Exp(-MuB)*pow(MuB,i)/fFactorial[i]/pbn;
      else           pb[i] = 0;
    }
					// 'i' - bin in the constrained Poisson distribution
    double exp_mus = TMath::Exp(-MuS);

    double pmax = -1;
    fIPMax      = -1;
    for (int i=0; i<MaxNx; i++) {
					// experiment observed Nobs events, use pb[k]
      double pi = 0;
      for (int k=0; k<=i; k++) {
	double ps = exp_mus*pow(MuS,i-k)/fFactorial[i-k];
	pi        = pi + pb[k]*ps;
      }
      fProb[i] = pi;

      if (fProb[i] > pmax) {
	pmax   = fProb[i];
	fIPMax = i;
      }
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
// two parameters for uniformity of the interface
//-----------------------------------------------------------------------------
int upper_limit::construct_interval(double MuB, double MuS, int NObs) {
  int rc(0);
					// find max bin
  init_poisson_dist(MuB,MuS,NObs);

  fIxMin = 0;
  fIxMax = MaxNx-1;
  double p = 0;

  int imin = 0;
  while ((p + fProb[imin] < 1-fCL) and (imin < MaxNx)) {
    p    += fProb[imin];
    imin += 1;
  }

  fIxMin = imin;
  fSump  = 1-p;
  if (imin == MaxNx-1) {
    printf("[upper_limit::construct_interval] ERROR:  MuB:%10.3f MuS:%10.3f",MuB,MuS);
    rc = -1;
  }

  return rc;
}

//-----------------------------------------------------------------------------
// vary signal from SMin to SMax in NPoints, construct FC belt, fill belt histogram
// fBelt is the FC belt histogram
// avoid multiple useless re-initializations
//-----------------------------------------------------------------------------
  int upper_limit::construct_belt(double MuB, double SMin, double SMax, int NPoints, int NObs) {

  fMuB = MuB;
  
  Belt_t* b = belt(NObs);

  b->fSMin = SMin;
  b->fSMax = SMax;
  b->fDy   = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 1;

  for (int ix=0; ix<MaxNx; ix++) {
    b->fSign[ix][0] = 1.e6;
    b->fSign[ix][1] = -1;
  }

  for (int iy=0; iy<NPoints; iy++) {
    double mus = SMin+iy*b->fDy;

    int rc     = construct_interval(MuB,mus,NObs);
    if (rc == 0) {
      for (int ix=fIxMin; ix<=fIxMax; ix++) {
        if (mus > b->fSign[ix][1]) b->fSign[ix][1] = mus+b->fDy/2;
        if (mus < b->fSign[ix][0]) b->fSign[ix][0] = mus-b->fDy/2;

        if (fDebug.fConstructBelt == 2) {
          if ((mus >= fDebug.fMuMin) and (mus <= fDebug.fMuMax)) {
            printf("upper_limit::construct_belt: iy = %5i ix:%3i mus=%8.4f MuB=%5.3f IxMin:%3i IxMax:%3i fSign[ix][0]:%8.4f fSign[ix][1]:%8.4f\n",
                   iy,ix,mus,MuB,fIxMin,fIxMax,b->fSign[ix][0],b->fSign[ix][0]);
          }
        }
      }
    }
    else {
      if (fDebug.fConstructBelt > 0) {
	printf("TFeldmanCousinsB::Constructbelt: ERROR: mus=%10.3f MuB=%10.3f iy = %3i interval not defined\n",
	       mus,MuB,iy);
      }
    }
  }

  for (int ix=0; ix<MaxNx; ix++) {
    if (b->fSign[ix][0] == 1.e6) b->fSign[ix][0] = 0;
    if (b->fSign[ix][0] <  0   ) b->fSign[ix][0] = 0;
    if (b->fSign[ix][1] <  0   ) b->fSign[ix][1] = 0;
  }

  if (fDebug.fConstructBelt > 0) {
    for (int ix=0; ix<MaxNx; ix++) {
      printf("%32s %3i %12.5f %12.5f\n","",ix,b->fSign[ix][0],b->fSign[ix][1]);
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
int upper_limit::make_prob_hist() {

  if (fHist.fProb    ) delete fHist.fProb;
  if (fHist.fInterval) delete fHist.fInterval;
  
  // fHist.fProb     = new  TH1D("h_prob","prob"    ,MaxNx,-0.5,MaxNx-.5);
  // fHist.fInterval = new  TH1D("h_intr","interval",MaxNx,-0.5,MaxNx-.5);

  TString title = Form("prob vs N, muB:%5.2f muS:%7.4f CL:%5.3f Nobs:%3i",fMuB,fMuS,fCL,fNObs);
  fHist.fProb     = new  TH1D(Form("h_prob_%s",GetName()),title.Data(),MaxNx,-0.5,MaxNx-0.5);

  title = Form("Interval muB:%5.2f muS:%7.4f CL:%5.3f Nobs:%3i",fMuB,fMuS,fCL,fNObs);
  fHist.fInterval = new  TH1D(Form("h_intr_%s",GetName()),title.Data(),MaxNx,-0.5,MaxNx-0.5);

  title = Form("log(Prob) muB:%5.2f muS:%7.4f CL:%5.3f Nobs:%3i",fMuB,fMuS,fCL,fNObs);
  fHist.fLh       = new  TH1D(Form("h_llh_%s" ,GetName()),"log(Prob)",5000,-10,40);
  
  fHist.fInterval->SetFillStyle(3004);
  fHist.fInterval->SetFillColor(kRed+2);
  fHist.fInterval->SetLineColor(kRed+2);

  for (int i=0; i<MaxNx; i++) {
    fHist.fProb->SetBinContent(i+1,fProb[i]);
    fHist.fProb->SetBinError  (i+1,0);

    if (i < fMean) fHist.fLh->Fill( log(fProb[i]),fProb[i]);
    else           fHist.fLh->Fill(-log(fProb[i]),fProb[i]);

    if ((i >= fIxMin) and (i<= fIxMax)) {
      fHist.fInterval->SetBinContent(i+1,fProb[i]);
      fHist.fInterval->SetBinError  (i+1,0);
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
void upper_limit::make_belt_hist(int NObs) {

  Belt_t* b = belt(NObs);
  
  if (b->fStack) {
    delete b->fStack;
    delete b->fLo;
    delete b->fHi;
  }
  
  b->fLo   = new TH1D(Form("h_belt_lo_%s_%02i",GetName(),NObs),
                      Form("Upper-limit belt MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                      MaxNx,-0.5,MaxNx-0.5);

  b->fHi   = new TH1D(Form("h_belt_hi_%s_%02i",GetName(),NObs),
                      Form("Upper-limit belt MuB = %10.3f CL = %5.2f Nobs:%3i",fMuB,fCL,fNObs),
                      MaxNx,-0.5,MaxNx-0.5);

  for (int ix=0; ix<MaxNx; ix++) {
    double dx = b->fSign[ix][1]-b->fSign[ix][0];
    if (b->fSign[ix][1] > 0) {    
      b->fLo->SetBinContent(ix+1,b->fSign[ix][0]);
      b->fHi->SetBinContent(ix+1,dx);
    }
  }
  
  b->fLo->SetLineColor(b->fFillColor);

  b->fHi->SetFillStyle(b->fFillStyle);
  b->fHi->SetFillColor(b->fFillColor);
  b->fHi->SetLineColor(b->fFillColor);

  b->fStack = new THStack(Form("hs_%s",GetName()),b->fHi->GetTitle());
  b->fStack->Add(b->fLo);
  b->fStack->Add(b->fHi);
}


//-----------------------------------------------------------------------------
// assume S in 
//-----------------------------------------------------------------------------
int upper_limit::test_coverage(double MuB, double SMin, double SMax, int NPoints) {
  
  int rc(0);

  for (int nobs=0; nobs<20; nobs++) {
    rc = construct_belt(MuB,0,30,30001,nobs);
    if (rc < 0) return rc;
  }

  std::vector<float> x(NPoints+2), y(NPoints+2);

  x[0] = SMin;
  y[0] = 0;
  double dy = (SMax-SMin)/(NPoints-1);
  for (int i=0; i<NPoints; i++) {
    double s = SMin + i*dy;
    if (s == 0) s = 1.e-10;          // deal with a numerical issue around zero
                                        // now generate pseudoexperiments
    int nmissed = 0;
    for (int k=0; k<fNExp; k++) {
      int nobs = fRn.Poisson(s+MuB);
                                        // assume SMin and SMax define a confidence interval
      double mus  = nobs;
                                        // determine SMin and SMax;
      Belt_t* bb  = belt(nobs);
      double smin = bb->fSign[nobs][0];
      double smax = bb->fSign[nobs][1];

      if ((s < smin) or (s > smax)) {
        nmissed += 1;
                                        // print only missed ones
        if (fDebug.fTestCoverage == 2) {
          printf("k, s, MuB, nobs, mus, smin, smax, nmissed : %10i %10.5f %10.3f %3i %10.3f %10.3f %10.3f %10i\n",
                 k, s, MuB, nobs, mus, smin, smax, nmissed);
        }
      }
      if (fDebug.fTestCoverage == 1) {
        printf("k, s, MuB, nobs, mus, smin, smax, nmissed : %10i %10.5f %10.3f %3i %10.3f %10.3f %10.3f %10i\n",
               k, s, MuB, nobs, mus, smin, smax, nmissed);
      }
    }
    float prob = 1.-nmissed/float(fNExp);
    x[i+1] = s;
    y[i+1] = prob;
  }

  x[NPoints+1] = SMax;
  y[NPoints+1] = 0;
  
  if (fHist.fCoverage) delete fHist.fCoverage;
  fHist.fCoverage = new TGraph(NPoints+2,x.data(),y.data());
  fHist.fCoverage->SetTitle(Form("Crow-Gardner coverage test #mu_{B} = %10.3f",MuB));
  
  return rc;
}
}

