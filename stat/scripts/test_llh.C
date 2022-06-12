///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "TMath.h"
#include "TCanvas.h"
#include "TROOT.h"

#include "Stntuple/stat/TBeltLH.hh"

using namespace stntuple ;

//-----------------------------------------------------------------------------
// tests
//-----------------------------------------------------------------------------
TBeltLH* test_llh_001(double MuB, double MuS = 1, double CL = 0.9, int NObs = -1, const char* Name = "test_llh_001") {

  TBeltLH* llh = new TBeltLH(Name,CL);

  llh->construct_interval(MuB,MuS,NObs);
                                                // make histograms
  llh->make_prob_hist();

  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) {
    c = new TCanvas(cname,Name,1200,900);
    c->Divide(2,1);
  }

  c->cd(1);
 
  gStyle->SetPaintTextFormat("6.4f");
  llh->fHist.fProb->Draw("text00");
  llh->fHist.fInterval->Draw("sames");

  printf("IxMin, IxMax, Sum(P) : %3i %3i %12.5e\n",llh->fIxMin,llh->fIxMax,llh->fSump);

  c->cd(2);

  llh->fHist.fLlh->SetMarkerStyle(20);

  llh->fHist.fLlhInterval->SetMarkerStyle(20);
  llh->fHist.fLlhInterval->SetLineColor(kRed+2);
  llh->fHist.fLlhInterval->SetLineWidth(2);

  llh->fHist.fLlh->Draw("p");
  llh->fHist.fLlhInterval->Draw("sames,p");

  return llh;
}

//-----------------------------------------------------------------------------
// construct belt
//-----------------------------------------------------------------------------
TBeltLH* test_llh_002(double MuB = 0, double CL = 0.9, int NObs = -1, const char* Name = "test_cg_002") {

  TBeltLH* llh = new TBeltLH(Name,CL);

                                        // make histograms
  llh->construct_belt(MuB,0,35,35001,NObs);
  llh->make_belt_hist();

  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) c = new TCanvas(cname,Name,1200,800);
  else              c->cd();

  llh->fHist.fBeltUL->GetYaxis()->SetRangeUser(0,5);
  llh->fHist.fBeltUL->Draw();
  // llh->fHist.fBeltHi->Draw("same");
  llh->fHist.fBeltNO1->Draw("same");

  return llh;
}

//-----------------------------------------------------------------------------
// test_cg_003: test 90% coverage
//-----------------------------------------------------------------------------
TBeltLH* test_llh_003(double MuB, double SMin, double SMax, int NPoints, double CL = 0.9, const char* Name = "test_cg_003") {

  TBeltLH* belt = new TBeltLH(Name,CL);

  belt->SetNExp(1000);
  belt->fDebug.fTestCoverage = 1;
                                        // test itself
  
  belt->test_coverage(MuB,SMin,SMax,NPoints);
  
                                        // make histograms
  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) {
    c = new TCanvas(cname,Name,1200,800);
  }

  belt->fHist.fCoverage->Draw("alp");
  
  return belt;
}

