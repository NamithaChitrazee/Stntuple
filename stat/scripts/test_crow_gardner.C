///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TROOT.h"

//-----------------------------------------------------------------------------
class crow_gardner {
public:

  enum {
    MaxNx =   50,
    MaxNy = 1001
  };

  struct Belt_t {
    int    fNy;                   // use not to reinitialize
    double fSMin;
    double fSMax;
    double fDy;
    int    fCont[MaxNy][MaxNx];  // either 0 or 1 assume 10000 to be large enough for MaxNy
    double fSign[MaxNx][2];      // fBelt.Sign[ix][0] = SMin[ix], fBelt.fSign[ix][1] = SMax[ix]
    int    fIndx[MaxNx][2];      // iymin, iymax

    int    fLhCont[MaxNy][600];    // either 0 or 1 assume 10000 to be large enough for MaxNy
  } fBelt;

  struct DebugLevel_t {
    int    fAll;
    int    fConstructBelt;
    int    fUpperLimit;
  } fDebugLevel;
  
  struct {
    TH1D*   fProb;
    TH1D*   fLh;
    TH1D*   fInterval;
    TH2D*   fBelt;
    TH2D*   fLhBelt;
    TGraph* fGrCoverage;
  } fHist;

  int    fType;                      // default:0 if=1, try to force monotonic left edge
  double fCL;
  double fSump;

  double fMean;
  
  int    fIMin;
  int    fIMax;

  TRandom3 fRn;

  double fProb [100];
  double fCProb[100];

  crow_gardner(double Mean, double CL = -1, int TYpe = 0);

  int init_poisson_dist(double* Prob, double* CumProb);

  int construct_interval(double Mu);
  int construct_belt    (double SMin, double SMax, int NPoints);

  int  make_prob_hist();
  void make_belt_hist();

  int test_coverage(double SMin, double SMax, int NPoints);
  
};


crow_gardner* gcg(nullptr);

//-----------------------------------------------------------------------------
crow_gardner::crow_gardner(double Mean, double CL, int Type) {

  fMean = Mean;
  fCL   = CL;
  fType = Type;

  fIMin = MaxNx;
  fIMax = -1;
  
  fHist.fProb       = nullptr;
  fHist.fInterval   = nullptr;
  fHist.fBelt       = nullptr;
  fHist.fGrCoverage = nullptr;

  fDebugLevel.fConstructBelt = 0;
}

//-----------------------------------------------------------------------------
int crow_gardner::construct_interval(double Mu) {
  // find max bin

  fMean = Mu;

  init_poisson_dist(fProb,fCProb);

  
  if (fMean > MaxNx-5) {
    printf("Mean = %12.5e is TOO MUCH. EXIT.\n",fMean);
    return -1;
  }
                                        // find bin with P(max)
  int imin = std::max(fMean-2,0.);
  int imax = std::min(fMean+2,double(MaxNx));
  
  int    imx  = -1;
  double pmax = -1;

  for (int i=imin; i<imax; i++) {
    if (fProb[i] > pmax) {
      pmax = fProb[i];
      imx  = i;
    }
  }
                                        // max bin found;
  double sp = pmax;
                                        // set both to the bin with max 
  int i1 = imx-1;
  int i2 = imx+1;

  imin   = imx;
  imax   = imx;

  if (sp < fCL) {
    while (1) {
    
      if ((i1 >= 0) and (i2 < MaxNx)) {
        if (fProb[i1] > fProb[i2]) {

          double sp1 = sp + fProb[i1];
          if (sp1 >= fCL) {
            sp   = sp1;
            imin = i1;
            break;
          }
          else {
            sp   = sp1;
            imin = i1;
            i1   = i1-1;
          }
        }
        else {
          // fProb[i2] is the largest
          double sp2 = sp + fProb[i2];
          if (sp2 >= fCL) {
            sp   = sp2;
            imax = i2;
            break;
          }
          else {
            // below the threshold
            imax = i2;
            i2   = i2+1;
            sp   = sp2;
          }
        }
      }
      else if (i1 < 0) {
        //-----------------------------------------------------------------------------
        // consider only upper part, look at i2
        //-----------------------------------------------------------------------------
        if (i2 < MaxNx) {
          // fProb[i2] is the largest
          sp   = sp + fProb[i2];
          imax = i2;
          if (sp >= fCL) {
            // done
            break;
          }
          else {
            // continue
            i2    = i2+1;
          }
        }
        else {
          // i2 = MaxNx
          printf("didnt converge, sp = %12.5e , i2 = MaxNx BREAK\n",sp);
          break;
        }
      }
      else if (i2 >= MaxNx) {
        //-----------------------------------------------------------------------------
        // consider only lower part , i1
        //-----------------------------------------------------------------------------
        if (i1 >= 0) {
          sp   = sp + fProb[i1];
          imin = i1;
          if (sp >= fCL) {
            // done
            break;
          }
          else {
            // continue
            i1 = i1-1;
          }
        }
        else {
          // i1 = 0
          printf("didnt converge, i1 = 0, sp = %12.5e , BREAK\n",sp);
          break;
        }
      }
    }
  }
//-----------------------------------------------------------------------------
// see, if can move to the left
//-----------------------------------------------------------------------------
  while (imax < MaxNx) {
    double sp1 = sp + fProb[imax+1]-fProb[imin];
    if (sp1 > fCL) {
      sp   = sp1;
      imin = imin+1;
      imax = imax+1;
    }
    else {
      break;
    }
  }
//-----------------------------------------------------------------------------
// IMin and IMax - the lower and the higher bin of the interval, could be the same
//-----------------------------------------------------------------------------
  fIMin = imin;
  fIMax = imax;
  fSump = sp;

  return 0;
}

//-----------------------------------------------------------------------------
int crow_gardner::init_poisson_dist(double* Prob, double* CumProb) {
//-----------------------------------------------------------------------------
// the length of 'Prob' should be at least N
// CumProb[N]: probability, for a given Mean, to have rn <= N
//-----------------------------------------------------------------------------
  Prob[0] = TMath::Exp(-fMean);
  for (int i=1; i<MaxNx; i++) {
    Prob[i] = Prob[i-1]*fMean/i;
  }
					// integral from i to imax, add redundancy for a cross check
  for (int i=0; i<MaxNx;i++) {
    CumProb[i] = 0;
    for (int k=i; k<MaxNx; k++) {
      CumProb[i] += Prob[k];
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
// vary signal from SMin to SMax in NPoints, construct FC belt, fill belt histogram
// fBelt is the FC belt histogram
// avoid multiple useless re-initializations
//-----------------------------------------------------------------------------
int crow_gardner::construct_belt(double SMin, double SMax, int NPoints) {

  if (NPoints > MaxNy) {
    NPoints = MaxNy;
    printf("crow_gardner::construct_belt WARNING: NPOints > MaxNy, truncating, NPoints=%i\n",MaxNy);
  }
  
  fBelt.fSMin = SMin;
  fBelt.fSMax = SMax;
  fBelt.fNy   = MaxNx;
  fBelt.fNy   = NPoints;
  
  fBelt.fDy   = (NPoints > 1) ? (SMax-SMin)/(NPoints-1) : 1;

  for (int iy=0; iy<NPoints; iy++) {
    double mus = SMin+(iy+0.5)*fBelt.fDy;
    int rc     = construct_interval(mus);
    if (rc == 0) {
      for (int ix=0; ix<MaxNx; ix++) {
	if ((ix < fIMin) || (ix > fIMax)) fBelt.fCont[iy][ix] = 0;
	else                              fBelt.fCont[iy][ix] = 1;
      }

      // int nb     = 600;
      // double bin = 0.1;
      // for (int ix=0; ix<600; ix++) {
      //   double x = -10+(ix+0.5)*bin;
      //   if ((x < log(fProb[fIMin])) || (x > -log(fProb[fIMax]))) fBelt.fLhCont[iy][ix] = 0;
      //   else                                                     fBelt.fLhCont[iy][ix] = 1;
      // }
    }
    else {
      if (fDebugLevel.fConstructBelt > 0) {
	printf("TFeldmanCousinsB::ConstructBelt: ERROR: mus=%10.4f IY = %3i interval not defined\n",
	       mus,iy);
      }
      for (int ix=0; ix<MaxNx; ix++) {
	fBelt.fCont[iy][ix] = 0;
      }
    }
  }
//-----------------------------------------------------------------------------
// for convenience, for each N, number of measured events, define the belt boundaries - fSBelt
//-----------------------------------------------------------------------------
  if (fDebugLevel.fConstructBelt > 0) {
    printf("[crow_gardner::construct_belt  Nobs    smin       smax\n");
    printf("------------------------------------------------------\n");
  }
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
	// got outside, the last inside cell is the previous one
	if (inside == 1) {
	  iymax   = iy-1;
	  inside  = 0;
	  break;
	}
      }
    }
					// probably, want step in Y ~ 10^-3
    fBelt.fSign[ix][0] = SMin+(iymin-0.5)*fBelt.fDy;  // lower edge of the iymin cell
    fBelt.fSign[ix][1] = SMin+(iymax+0.5)*fBelt.fDy;  // upper edge of the iymax cell
    fBelt.fIndx[ix][0] = iymin;
    fBelt.fIndx[ix][1] = iymax;

    if (fDebugLevel.fConstructBelt > 0) {
      printf("%32s %3i %12.5f %12.5f\n","",ix,fBelt.fSign[ix][0],fBelt.fSign[ix][1]);
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
int crow_gardner::make_prob_hist() {

  if (fHist.fProb    ) delete fHist.fProb;
  if (fHist.fInterval) delete fHist.fInterval;
  
  // fHist.fProb     = new  TH1D("h_prob","prob"    ,MaxNx,-0.5,MaxNx-.5);
  // fHist.fInterval = new  TH1D("h_intr","interval",MaxNx,-0.5,MaxNx-.5);

  fHist.fProb     = new  TH1D("h_prob","prob vs N",MaxNx,0.,MaxNx);
  fHist.fInterval = new  TH1D("h_intr","interval" ,MaxNx,0.,MaxNx);
  fHist.fLh       = new  TH1D("h_llh" ,"log(Prob)",5000,-10,40);
  
  fHist.fInterval->SetFillStyle(3004);
  fHist.fInterval->SetFillColor(kRed+2);
  fHist.fInterval->SetLineColor(kRed+2);

  for (int i=0; i<MaxNx; i++) {
    fHist.fProb->SetBinContent(i+1,fProb[i]);
    fHist.fProb->SetBinError  (i+1,0);

    if (i < fMean) fHist.fLh->Fill( log(fProb[i]),fProb[i]);
    else           fHist.fLh->Fill(-log(fProb[i]),fProb[i]);

    if ((i >= fIMin) and (i<= fIMax)) {
      fHist.fInterval->SetBinContent(i+1,fProb[i]);
      fHist.fInterval->SetBinError  (i+1,0);
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
void crow_gardner::make_belt_hist() {
  if (fHist.fBelt) delete fHist.fBelt;

  // fHist.fBelt = new TH2D("h_belt","Crow-Gardner belt",MaxNx,-0.5,MaxNx-0.5,
  // 			 fBelt.fNy,fBelt.fSMin-fBelt.fDy/2,fBelt.fSMax+fBelt.fDy/2);

  fHist.fBelt   = new TH2D("h_belt","Crow-Gardner belt",MaxNx,0.,MaxNx,
  			 fBelt.fNy,fBelt.fSMin-fBelt.fDy/2,fBelt.fSMax+fBelt.fDy/2);

  // fHist.fLhBelt = new TH2D("h_lhbelt","Crow-Gardner LH belt",600,-10,50,
  // 			 fBelt.fNy,fBelt.fSMin-fBelt.fDy/2,fBelt.fSMax+fBelt.fDy/2);

  for (int ix=0; ix<MaxNx; ix++) {
    for (int iy=0; iy<fBelt.fNy; iy++) {
      fHist.fBelt->SetBinContent(ix+1,iy+1,fBelt.fCont[iy][ix]);
    }
  }

  // for (int ix=0; ix<600; ix++) {
  //   for (int iy=0; iy<fBelt.fNy; iy++) {
  //     fHist.fLhBelt->SetBinContent(ix+1,iy+1,fBelt.fLhCont[iy][ix]);
  //   }
  // }
}


//-----------------------------------------------------------------------------
// assume S in 
int crow_gardner::test_coverage(double SMin, double SMax, int NPoints) {
  
  int rc(0);
  rc = construct_belt(0,44,MaxNy);
  if (rc < 0) return rc;

  int nev     = 100000;

  float x[10000], y[10000];

  double dy = (SMax-SMin)/(NPoints-1);
  for (int i=0; i<NPoints; i++) {
    double s = SMin + i*dy;
                                        // now generate pseudoexperiments
    int nmissed = 0;
    for (int k=0; k<nev; k++) {
      int nobs = fRn.Poisson(s);
                                        // assume SMin and SMax define a confidence interval
      double sig = nobs;
                                        // determine SMin and SMax;
      double smin = fBelt.fSign[nobs][0];
      double smax = fBelt.fSign[nobs][1];

      if ((s < smin) or (s > smax)) {
        nmissed += 1;
      }
    }
    float prob = 1.-nmissed/float(nev);
    x[i] = s;
    y[i] = prob;
  }

  if (fHist.fGrCoverage) delete fHist.fGrCoverage;
  fHist.fGrCoverage = new TGraph(NPoints,x,y);
  
  return rc;
}

//-----------------------------------------------------------------------------
// tests
//-----------------------------------------------------------------------------
int test_cg_001(double Mean = 1, double CL = 0.9) {

  crow_gardner* cg = new crow_gardner(Mean,0.9);
  gcg = cg;

  cg->init_poisson_dist(cg->fProb, cg->fCProb);

  cg->construct_interval(Mean);
                                        // make histograms
  cg->make_prob_hist();

  cg->fHist.fProb->Draw("text");
  cg->fHist.fInterval->Draw("sames");

  // build an interval

  return 0;
}

//-----------------------------------------------------------------------------
// construct belt
//-----------------------------------------------------------------------------
crow_gardber* cg_test_002(double Mean = 1, double CL = 0.9) {
  TString name = "cg_test_003";

  crow_gardner* cg = new crow_gardner(Mean,0.9);
  gcg = cg;

                                        // make histograms
  cg->construct_belt(0,20,1001);
  cg->make_belt_hist();

  char cname[100];
  sprintf(cname,"c_%s",name.Data());
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) {
    c = new TCanvas(cname,cname,1200,800);
  }

  cg->fHist.fBelt->SetFillColor(kBlue+2);
  cg->fHist.fBelt->SetFillStyle(3001);
  cg->fHist.fBelt->SetStats(0);
  cg->fHist.fBelt->Draw("box");

  return cg;
}

//-----------------------------------------------------------------------------
// test_cg_003: test 90% coverage
//-----------------------------------------------------------------------------
crow_gardner* cg_test_003(double SMin, double SMax, int NPoints, double CL = 0.9) {
  TString name = "cg_test_003";

  crow_gardner* cg = new crow_gardner(SMin,CL);
  gcg = cg;
                                        // test itself
  cg->test_coverage(SMin,SMax,NPoints);
                                        // make histograms
  char cname[100];
  sprintf(cname,"c_%s",name.Data());
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) {
    c = new TCanvas(cname,cname,1200,800);
  }

  cg->fHist.fGrCoverage->Draw("alp");
  
  return cg;
}
