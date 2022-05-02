// my implementation of the Feldman-Cousins algorithm
// 
#include "Stntuple/stat/TFeldmanCousinsB.hh"
#include "TCanvas.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TROOT.h"
#include "Math/QuantFuncMathCore.h"
#include "Math/DistFunc.h"

ClassImp(stntuple::TFeldmanCousinsB)
//-----------------------------------------------------------------------------
// CL > 0: CL has the meaning of a probability - 0.9, 0.95 .. .0.99 etc
//    < 0: CL is the "discovery probability", probability corresponding
//         to 5 gaussian sigma level, 
//-----------------------------------------------------------------------------
namespace stntuple {
TFeldmanCousinsB::TFeldmanCousinsB(const char* Name, double CL, int DebugLevel):
  TNamed(Name,Name),
  fRn()
{
  SetCL(CL);

  fDebugLevel = DebugLevel;
//-----------------------------------------------------------------------------
// calculate factorials - just once
//-----------------------------------------------------------------------------
  fFactorial[0] = 1;
  for (int i=1; i<MaxNx; i++) {
    fFactorial[i] = fFactorial[i-1]*i;
  }
//-----------------------------------------------------------------------------
// book histograms
//-----------------------------------------------------------------------------
  fHist.fBgProb    = new TH1D(Form("h_bg_prob_%s"   ,GetName()),"h_bg_prob",MaxNx,0,MaxNx);
  fHist.fBsProb    = new TH1D(Form("h_bs_prob_%s"   ,GetName()),"h_bs_prob",MaxNx,0,MaxNx);
  fHist.fCumBgProb = new TH1D(Form("h_cumbg_prob_%s",GetName()),"h_cumbg_prob",MaxNx,0,MaxNx);
  fHist.fCumBsProb = new TH1D(Form("h_cumbs_prob_%s",GetName()),"h_cumbs_prob",MaxNx,0,MaxNx);

  fHist.fProb      = new TH1D(Form("h_prob_2D_%s"    ,GetName()),"h prob 2D" ,MaxNx,-0.5,MaxNx-0.5);
  fHist.fBelt      = nullptr;

  fNExp            = 10000000;
					// make sure uninitialzied values don't make sense
  fBelt.fNy        = -1;
  fBelt.fSMin      = 1e6;
  fBelt.fSMax      = -1e6;
}

TFeldmanCousinsB::~TFeldmanCousinsB() {
  delete fHist.fBgProb;
  delete fHist.fBsProb;
  delete fHist.fCumBgProb;
  delete fHist.fCumBsProb;
  delete fHist.fProb;
  if (fHist.fBelt) delete fHist.fBelt;
}

void  TFeldmanCousinsB::SetCL(double CL) {
  double alpha = 1-TMath::Erf(5./sqrt(2));
  
  if (CL > 0) fCL = CL;
  else        fCL = 1-alpha/2; // always two-sided: 1-5.7330314e-07

  fLog1mCL        = log(1-fCL);
}

void TFeldmanCousinsB::InitPoissonDist(double Mean, double* Prob, double* CumProb, int N) {
//-----------------------------------------------------------------------------
// the length of 'Prob' should be at least N
// CumProb[N]: probability, for a given Mean, to have rn <= N
//-----------------------------------------------------------------------------

  Prob[0] = TMath::Exp(-Mean);
  for (int i=1; i<N; i++) {
    Prob[i] = Prob[i-1]*Mean/i;
  }
					// integral
  CumProb[0] = 1;
  for (int i=1; i<N;i++) {
    CumProb[i] = 0;
    for (int k=i; k<N; k++) {
      CumProb[i] += TMath::Exp(-Mean)*pow(Mean,k)/fFactorial[k];
    }
  }
}

//-----------------------------------------------------------------------------
void TFeldmanCousinsB::Init(double Bgr, double Sig) {
  //  fSigMean = SigMean;

  fMeanBgr = Bgr;
  fMeanSig = Sig;
  
  InitPoissonDist(fMeanBgr         , fBgProb, fCumBgProb, MaxNx);
  InitPoissonDist(fMeanBgr+fMeanSig, fBsProb, fCumBsProb, MaxNx);
//-----------------------------------------------------------------------------
// [re]-initialize 1D histograms with the probabilities and integral probabilities
//-----------------------------------------------------------------------------
  for (int i=0; i<MaxNx; i++) {
    fHist.fBgProb->SetBinContent(i+1,fBgProb[i]);

    fHist.fBsProb->SetBinContent(i+1,fBsProb[i]);
    fHist.fProb  ->SetBinContent(i+1,fBsProb[i]);
    
    fHist.fCumBgProb->SetBinContent(i+1,fCumBgProb[i]);
    fHist.fCumBsProb->SetBinContent(i+1,fCumBsProb[i]);
  }
}

int TFeldmanCousinsB::ConstructInterval(double MuB, double MuS) {
//-----------------------------------------------------------------------------
// output: [fIMin,fIMax] : a CL interval constructed using FC ordering for given 
//         MuB and MuS
//-----------------------------------------------------------------------------
  int rc(0);				// return code

  if (fDebugLevel > 10) {
    printf("TFeldmanCousinsB::ConstructInterval: MuB = %10.3f MuS = %10.3f\n",MuB,MuS);
  }
  
  Init(MuB,MuS);
  
  for (int ix=0; ix<MaxNx; ix++) {
    double sbest = ix-MuB;

    if (sbest <= 0) sbest = 0;

    double sb = sbest+MuB;

    fBestProb[ix] = TMath::Power(sb,ix)*TMath::Exp(-sb)/fFactorial[ix];
    fBestSig [ix] = sbest;
    fLhRatio [ix] = fBsProb[ix]/fBestProb[ix];
  }
  
  if (fDebugLevel > 10) {
    PrintData("BestProb" ,'d',fBestProb     ,18);
    PrintData("BestSig"  ,'d',fBestSig      ,18);
    PrintData("LhRatio"  ,'d',fLhRatio      ,18);
  }
//-----------------------------------------------------------------------------
// sort ranks
//-----------------------------------------------------------------------------
  double rmax;
    
  for (int i=0; i<MaxNx; i++) fRank[i] = i;

  for (int i1=0; i1<MaxNx-1; i1++) {
    rmax = fLhRatio[fRank[i1]];
    for (int i2=i1+1; i2<MaxNx; i2++) {
      double r2 = fLhRatio[fRank[i2]];
      if (r2 > rmax) {
	rmax      = r2;
	int i     = fRank[i1];
	fRank[i1] = fRank[i2];
	fRank[i2] = i;
      }
    }
  }
  if (fDebugLevel > 10) {
    PrintData("Rank   " ,'i',fRank     ,18);
  }
//-----------------------------------------------------------------------------
// build confidence interval corresponding to the probability fCL - 0.9 , 0.95, etc
//-----------------------------------------------------------------------------
  fIMin       = MaxNx;
  fIMax       = -1;

  int covered = 0;
  fProb       = 0;
  
  if (fDebugLevel > 10) {
    printf(" ix fRank[ix] fBsProb[ind] fProb      1-fProb fIMin fIMax\n");
    printf(" --------------------------------------------------------\n");
  }
  for (int ix=0; ix<MaxNx; ix++) {
    int ind = fRank[ix];
    fProb  += fBsProb[ind];
    if (ind < fIMin) fIMin = ind;
    if (ind > fIMax) fIMax = ind;

    if (fDebugLevel > 10) {
      printf("%3i %8i %10.3e %10.3e %10.3e %5i %5i\n",ix,ind,fBsProb[ind],fProb,1-fProb,fIMin,fIMax);
    }
    
    if (fProb > fCL) {
      covered = 1;
      break;
    }
  }

  if (covered == 0) {
    printf("trouble ! interval not covered : prob = %12.5e , 1-CL = %12.5e\n",fProb,1-fCL);
  }
  else {
    printf("TFeldmanCousinsB::ConstructInterval:success: prob = %12.5e fIMin:%3i, fIMax:%3i, 1-CL = %12.5e\n",
	   fProb,fIMin,fIMax,1-fCL);
  }

  return rc;
}

int TFeldmanCousinsB::ConstructInterval(model_t* Model) {
//-----------------------------------------------------------------------------
// Model already comes with initialized probability distributions and cumulative probability
// distribution
//
// output: [fIMin,fIMax] : a CL interval constructed using FC ordering for given 
//         MuB and MuS
//-----------------------------------------------------------------------------
  int rc(0);				// return code

  // Init(MuB,MuS);

  double mub = Model->MuB();
  
  for (int ix=0; ix<MaxNx; ix++) {
    double sbest = ix-mub;

    if (sbest <= 0) sbest = 0;

    double sb = sbest+mub;

    fBestProb[ix] = TMath::Power(sb,ix)*TMath::Exp(-sb)/fFactorial[ix];
    fBestSig [ix] = sbest;
    fLhRatio [ix] = fBsProb[ix]/fBestProb[ix];
  }
//-----------------------------------------------------------------------------
// sort ranks
//-----------------------------------------------------------------------------
  double rmax;
    
  for (int i=0; i<MaxNx; i++) fRank[i] = i;

  for (int i1=0; i1<MaxNx-1; i1++) {
    rmax = fLhRatio[fRank[i1]];
    for (int i2=i1+1; i2<MaxNx; i2++) {
      double r2 = fLhRatio[fRank[i2]];
      if (r2 > rmax) {
	rmax      = r2;
	int i     = fRank[i1];
	fRank[i1] = fRank[i2];
	fRank[i2] = i;
      }
    }

    if (fDebugLevel > 0) {
      PrintData("Rank   " ,'i',fRank     ,18);
    }
  }
//-----------------------------------------------------------------------------
// build confidence interval corresponding to the probability fCL - 0.9 , 0.95, etc
//-----------------------------------------------------------------------------
  fIMin       = MaxNx;
  fIMax       = -1;

  int covered = 0;
  fProb       = 0;
  
  for (int ix=0; ix<MaxNx; ix++) {
    int ind = fRank[ix];
    fProb  += fBsProb[ind];
    if (ind < fIMin) fIMin = ind;
    if (ind > fIMax) fIMax = ind;

    if (fDebugLevel > 0) {
      printf(" ix ind=fRank[ix] fBsProb[ind] fProb, 1-fProb fIMin fIMax :%3i %3i %10.3e %10.3e %10.3e %3i %3i\n",
	     ix,ind,fBsProb[ind],fProb,1-fProb,fIMin,fIMax);
    }
    
    if (fProb > fCL) {
      covered = 1;
      break;
    }
  }

  if (covered == 0) {
    printf("trouble ! interval not covered : prob = %12.5e , 1-CL = %12.5e\n",fProb,1-fCL);
  }

  return rc;
}

//-----------------------------------------------------------------------------
// vary signal from SMin to SMax in NPoints, construct FC belt, fill belt histogram
// fBelt is the FC belt histogram
// avoid multiple useless re-initializations
//-----------------------------------------------------------------------------
int TFeldmanCousinsB::ConstructBelt(double Bgr, double SMin, double SMax, int NPoints) {

  if ((fBelt.fBgr == Bgr) and (fBelt.fNy == NPoints) and (fBelt.fSMin == SMin) and (fBelt.fSMax == SMax)) return 0;

  fBelt.fBgr  = Bgr;
  fBelt.fSMin = SMin;
  fBelt.fSMax = SMax;
  fBelt.fNy   = NPoints;
  
  fBelt.fDy   = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 1;

  // if (fHist.fBelt) delete fHist.fBelt;

  // fHist.fBelt = new TH2D(Form("h_belt_%s",GetName()),"FC belt",TFeldmanCousinsB::MaxNx,0,TFeldmanCousinsB::MaxNx,
  // 			 NPoints,SMin-step/2,SMax-step/2);
  
  for (int iy=0; iy<NPoints; iy++) {
    double mus = SMin+(iy+0.5)*fBelt.fDy;
    ConstructInterval(Bgr,mus);
    for (int ix=0; ix<MaxNx; ix++) {
      if ((ix < fIMin) || (ix > fIMax)) fBelt.fCont[iy][ix] = 0;
      else                              fBelt.fCont[iy][ix] = 1;
    }
  }
//-----------------------------------------------------------------------------
// for convenience, for each N, number of measured events, define the belt boundaries - fSBelt
//-----------------------------------------------------------------------------
  for (int ix=0; ix<MaxNx; ix++) {
    int iymin     = 0;
    int iymax     = 0;
    int inside    = 0;
    for (int iy=0; iy<NPoints; iy++) {
      if (fBelt.fCont[iy][ix] > 0) {
	if (inside == 0) {
	  iymin   = iy;
	  inside  = 1;
	}
      }
      else {
	if (inside == 1) {
	  iymax   = iy;
	  inside  = 0;
	  break;
	}
      }
    }
					// probably want step = 10^-3
    fBelt.fSign[ix][0] = SMin+(iymin+0.5)*fBelt.fDy;
    fBelt.fSign[ix][1] = SMin+(iymax+0.5)*fBelt.fDy;
    fBelt.fIndx[ix][0] = iymin;
    fBelt.fIndx[ix][1] = iymax;

    if (fDebugLevel > 0) {
      printf("ix, smin, smax : %3i %12.5f %12.5f\n",ix,fBelt.fSign[ix][0],fBelt.fSign[ix][1]);
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
// in general, need to scan a range of signals, call this function multiple times
//-----------------------------------------------------------------------------
void TFeldmanCousinsB::DiscoveryProb(double MuB, double SMin, double SMax, int NPoints, double* MuS, double* Prob) {
//-----------------------------------------------------------------------------
// construct FC CL confidence interval (covering fCL) assuming Signal=0
//-----------------------------------------------------------------------------
  ConstructInterval(MuB,0);

  double step = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 0;

  for (int ix=0; ix<NPoints; ix++) {
    MuS[ix]   = SMin+ix*step;
    double tot = MuB+MuS[ix];
//-----------------------------------------------------------------------------
// ndisc: number of "discovery experiments", pseudoexperiments in which NULL
// hypothesis is excluded at (1-fCL) level
//-----------------------------------------------------------------------------
    long int ndisc = 0;			
    for (int i=0; i<fNExp; i++) {
      int rn = fRn.Poisson(tot);
//-----------------------------------------------------------------------------
// definition of the discovery:
// rn > fIMax, i.e  the  probability to observe'rn' is less than 1-fCL
//-----------------------------------------------------------------------------
      if (rn > fIMax) ndisc ++;
    }
    Prob[ix] = double(ndisc)/double(fNExp);
  }
}

void TFeldmanCousinsB::DiscoveryProbMean(double MuB, double SMin, double SMax, int NPoints, double* MuS, double* Prob) {
//-----------------------------------------------------------------------------
// in general, need to scan a range of signals, call this function multiple times
// watch for 5 
// construct FC CL confidence interval (covering fCL) for MuS=0
//-----------------------------------------------------------------------------
  double mus;
  ConstructInterval(MuB,mus=0);

  double step = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 0;

  for (int ix=0; ix<NPoints; ix++) {
    MuS[ix]    = SMin+ix*step;
    double tot = MuB+MuS[ix];
//-----------------------------------------------------------------------------
// ndisc: number of "discovery experiments", pseudoexperiments in which NULL
// hypothesis is excluded at (1-fCL) level
//-----------------------------------------------------------------------------
    double sum  = 0;
    double sumn = 0;
    for (int i=0; i<fNExp; i++) {
      int rn = fRn.Poisson(tot);
//-----------------------------------------------------------------------------
// define probability for the background to fluctuate above rn
//-----------------------------------------------------------------------------
      double p;
      if (rn > 0) p   = ROOT::Math::poisson_cdf_c(rn-1,MuB);
      else        p   = ROOT::Math::poisson_cdf_c(0   ,MuB);

      double      sig = ROOT::Math::gaussian_quantile_c(p,1);

      sum      += sig;
      sumn     += 1;
      if (fDebugLevel > 0) {
	printf("i, tot, rn, p, sig, sum : %3i %10.4f %3i %15.8e %12.5e %12.5e\n",i,tot,rn, p,sig, sum);
      }
    }
    
    Prob[ix]  = sum/sumn; // ROOT::Math::gaussian_quantile(pp,1);

    if (fDebugLevel > 0) {
      printf("ix, sum, sumn, MuS[ix], Prob[ix] : %3i %12.5e %10.3e %12.5e %12.5e\n",
	     ix,sum,sumn,MuS[ix],Prob[ix]);
    }
  }
}


void TFeldmanCousinsB::DiscoveryProbMean(model_t* Model, double SMin, double SMax, int NPoints, double* MuS, double* NSig) {
//-----------------------------------------------------------------------------
// in general, need to scan a range of signals, call this function multiple times
// watch for 5 
// construct FC CL confidence interval (covering fCL) for MuS=0
//-----------------------------------------------------------------------------
  double step = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 0;

  parameter_t* signal_process = Model->SignalChannel()->Process();

  for (int ix=0; ix<NPoints; ix++) {
					// scan a range of signal strengths
    MuS[ix]    = SMin+ix*step;
    signal_process->SetMean(MuS[ix]);	// should be fixed
//-----------------------------------------------------------------------------
// ndisc: number of "discovery experiments", pseudoexperiments in which NULL
// hypothesis is excluded at (1-fCL) level
//-----------------------------------------------------------------------------
    double sum  = 0;
    for (long int i=0; i<fNExp; i++) {
					// next pseudoexperiment: fluctuate nuisanse parameters
      Model->InitParameters();
					// fluctuated background mean for this pseudoexperiment
      
      double mub = Model->GetNullValue();
					// why do I need to construct an interval here? - probably, I don't
      //      ConstructInterval(Model);
					// signal strength, fluctuated in a correlated way with the background

      double mus = signal_process->GetValue();

      // model->GetValue() should do the same, but need to test

      // the total
      double tot = mub+mus;
      
      int    nobs = fRn.Poisson(tot);
//-----------------------------------------------------------------------------
// define probability for the background to fluctuate and result in an observatioin
// of 'nobs' or more events
//-----------------------------------------------------------------------------
      double p;
      if (nobs > 0) p = ROOT::Math::poisson_cdf_c(nobs-1,mub);
      else          p = ROOT::Math::poisson_cdf_c(0     ,mub);
//-----------------------------------------------------------------------------
// 'sig' is the deviation, in units of a 'gaussian sigma' corresponding
// to the probability 'p' of the background to fluctuate and give 'nobs' or more events
// such that p = Integral(sig,infinity)
//-----------------------------------------------------------------------------
      double sig = ROOT::Math::gaussian_quantile_c(p,1);

      sum      += sig;
      if (fDebugLevel > 0) {
	printf("i, tot, nobs, p, sig, sum : %3li %10.4f %3i %15.8e %12.5e %12.5e\n",i,tot,nobs, p,sig, sum);
      }
    }
    
    NSig[ix]  = sum/fNExp; // ROOT::Math::gaussian_quantile(pp,1);

    if (fDebugLevel > 0) {
      printf("ix, sum, fNExp, MuS[ix], NSig[ix] : %3i %12.5e %12li %12.5e %12.5e\n",
	     ix,sum,fNExp,MuS[ix],NSig[ix]);
    }
  }
}

void TFeldmanCousinsB::PlotDiscoveryProbMean(double MuB, double MuS) {
  // double mus;
  // ConstructInterval(MuB,mus=0);

  TH1F* h_sigm = new TH1F("h_sigm","h1 sigm"     ,2000,    0,20);
  TH1F* h_prob = new TH1F("h_prob","h1 log(prob)",2000, - 99, 1);
//-----------------------------------------------------------------------------
// ndisc: number of "discovery experiments", pseudoexperiments in which NULL
// hypothesis is excluded at (1-fCL) level
//-----------------------------------------------------------------------------
  if (fDebugLevel > 0) {
    printf("  i  rn cum_prob[rn]        p          sig          sum       \n");
    printf("--------------------------------------------------------------\n");
  }
  for (int i=0; i<fNExp; i++) {
    int rn = fRn.Poisson(MuB+MuS);
    //    int rn = fRn.Poisson(MuS);
//-----------------------------------------------------------------------------
// define probability for the background to fluctuate above rn
//-----------------------------------------------------------------------------
    double p;
    if (rn > 0) p   = ROOT::Math::poisson_cdf_c(rn-1,MuB);
    else        p   = ROOT::Math::poisson_cdf_c(0   ,MuB);
    
    double      sig = ROOT::Math::gaussian_quantile_c(p,1);
      
    h_sigm->Fill(sig);
    h_prob->Fill(log(p));
  }

  const char* name = "c_fc_PlotDiscoveryProbMean";
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(name);
  if (c == nullptr) {
    c = new TCanvas("c_fc_PlotDiscoveryProbMean","c",1500,500);
    c->Divide(2,1);
  }
  c->cd(1);
  h_sigm->Draw();
  c->cd(2);
  h_prob->Draw();
}


void TFeldmanCousinsB::PrintData(const char* Title, char DataType, void* Data, int MaxInd) {

  int k      = 0;
  int findex = 0;
  int n_per_line(20);

  double*   pd = (double*) Data;
  int*      pi = (int*   ) Data;
  
  printf("-- %-10s: ",Title);
  for (int i=0; i<MaxInd; i++) {
    if (k == n_per_line) {
      printf("\n%03i",n_per_line*findex);
      k = 0;
    }
    if      (DataType == 'd') printf(" %9.2e",pd[i]);
    else if (DataType == 'i') printf(" %9i"  ,pi[i]);
    k++;
  }

  if (k > 0) printf("\n");
}

void TFeldmanCousinsB::PrintProbs(int N) {
//-----------------------------------------------------------------------------
// print results
//-----------------------------------------------------------------------------
  printf(" <bgr> = %10.4f <sig> = %10.4f\n",fMeanBgr,fMeanSig);
  printf(" IMin, IMax, Prob = %3i %3i %12.5f\n",fIMin,fIMax,fProb);

  PrintData("BgProb"  ,'d',fBgProb   ,N);
  PrintData("BestSig" ,'d',fBestSig  ,N);
  PrintData("BsProb"  ,'d',fBsProb   ,N);
  PrintData("CuBsProb",'d',fCumBsProb,N);
  PrintData("BestProb",'d',fBestProb ,N);
  PrintData("LhRatio" ,'d',fLhRatio  ,N);
  PrintData("Rank   " ,'i',fRank     ,N);
}


int TFeldmanCousinsB::SolveFor(double Val, const double* X, const double* Y, int NPoints, double* XVal) {
//-----------------------------------------------------------------------------
// fiven NPoints (X,Y) representing a smooth curve f(X), find XVal,
// such that f(XVal) = Val
//-----------------------------------------------------------------------------
  int i0 = -1;
  for (int i=0; i< NPoints; i++) {
    if (fDebugLevel > 0) {
      printf("i, X[i], Y[i] : %2i %10.3e %12.5e\n",i,X[i],Y[i]);
    }
    if (Y[i] < Val) continue;
    i0 = i-1;
    break;
  }

  if (i0 == -1) {
    printf("TFeldmanCousinsB::solve_for in trouble: wrong range, bail out\n");
    return -1;
  }
  
  int i1 = i0+1;
  int i2 = i0+2;
  
  TMatrixD m(3,3);

  m(0,0) = X[i0]*X[i0]; m(0,1) = X[i0]; m(0,2) = 1;
  m(1,0) = X[i1]*X[i1]; m(1,1) = X[i1]; m(1,2) = 1;
  m(2,0) = X[i2]*X[i2]; m(2,1) = X[i2]; m(2,2) = 1;

  TMatrixD minv(TMatrixD::kInverted,m);

  TVectorD vy(3), vp(3);
  vy(0) = Y[i0];
  vy(1) = Y[i1];
  vy(2) = Y[i2];

  vp = minv*vy;

  double a = vp(0);
  double b = vp(1);
  double c = vp(2);
//-----------------------------------------------------------------------------
// finally, the solution for XVal
//-----------------------------------------------------------------------------
  if (a < 0) *XVal = -b/(2*a) - sqrt(b*b/(4*a*a)-(c-Val)/a);
  else       *XVal = -b/(2*a) + sqrt(b*b/(4*a*a)-(c-Val)/a);

  if (fDebugLevel > 0) {
    printf ("a,b,c,x = %12.5e %12.5e %12.5e %12.5e \n",a,b,c,*XVal);
  }
  return 0;
}



void TFeldmanCousinsB::UpperLimit(double MuB, double SMin, double SMax, int NPoints, double* S, double* Prob) {
//-----------------------------------------------------------------------------
// construct FC CL confidence belt
// constructing the FC belt assumes dividing the signal interval into (NPoints-1) steps,
// use 1000 points, assume signal in the range [0,10]
//-----------------------------------------------------------------------------
  ConstructBelt(MuB,0,10,1001);
//-----------------------------------------------------------------------------
// ndisc: number of "discovery experiments", pseudoexperiments in which NULL
// hypothesis is excluded at (1-fCL) level
//-----------------------------------------------------------------------------
  int nsteps = (NPoints > 1) ? NPoints -1 : 1;
  double step = (SMax-SMin)/nsteps;
  
  for (int is=0; is<NPoints; is++) {
    S[is] = SMin+is*step;
    long int nexcl = 0;			
    for (int i=0; i<fNExp; i++) {
      int nobs = fRn.Poisson(MuB);
//-----------------------------------------------------------------------------
// definition of the discovery:
// rn > fIMax, i.e  the  probability to observe 'rn' is less than 1-fCL
//-----------------------------------------------------------------------------
      if (S[is] > fBelt.fSign[nobs][1]) nexcl++;
      if (fDebugLevel > 0) {
	printf(" MuB, nobs S[is] fBelt.fSign[nobs][1] : %12.5e %3i %12.5e %12.5e\n",
	       MuB, nobs, S[is],fBelt.fSign[nobs][1]);
      }
    }
    Prob[is] = double(nexcl)/double(fNExp);
  }
}

  
void TFeldmanCousinsB::UpperLimit(model_t* Model, double SMin, double SMax, int NPoints, double* S, double* Prob) {
//-----------------------------------------------------------------------------
// nexcl: number of "discovery experiments", pseudoexperiments in which NULL
//        hypothesis is excluded at (1-fCL) level
//-----------------------------------------------------------------------------
  int    nsteps = (NPoints > 1) ? NPoints -1 : 1;
  double step   = (SMax-SMin)/nsteps;
					// save initial mean

  parameter_t* signal_process = Model->SignalChannel()->Process();

  double old_mean = signal_process->Mean();
  
  for (int is=0; is<NPoints; is++) {
					// new signal mean
    S[is] = SMin+is*step;

    signal_process->SetMean(S[is]);
    
    long int nexcl = 0;			
    for (long int i=0; i<fNExp; i++) {
					// next pseudoexperiment: fluctuate nuisanse parameters
      Model->InitParameters();
					// retrieve fluctuated background and signal means for
					// this pseudoexperiment
      double mub = Model->GetNullValue();
      double mus = signal_process->GetValue();
//-----------------------------------------------------------------------------
// construct FC CL confidence belt
// constructing the FC belt assumes dividing the signal interval into (NPoints-1) steps,
// use 1000 points, assume signal in the range [0,10]
//-----------------------------------------------------------------------------
      ConstructBelt(mub,0,10,1001);
      
      int nobs = fRn.Poisson(mub);
//-----------------------------------------------------------------------------
// definition of the NULL hypothesis exclusion: S > 
// rn > fIMax, i.e  the  probability to observe'rn' is less than 1-fCL
//-----------------------------------------------------------------------------
      if (mus > fBelt.fSign[nobs][1]) nexcl++;
      if (fDebugLevel > 0) {
	printf(" mub, nobs S[is] mus fBelt.fSign[nobs][1] : %12.5e %3i %12.5e %12.5e %12.5e\n",
	       mub, nobs, S[is], mus, fBelt.fSign[nobs][1]);
      }
    }
    Prob[is] = double(nexcl)/double(fNExp);
  }
					// finally, restore the signal mean
					// in principle, better to copy the model
  signal_process->SetMean(old_mean);
}
};
