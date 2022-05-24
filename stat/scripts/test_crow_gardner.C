///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "TMath.h"
#include "TCanvas.h"
#include "TROOT.h"

#include "Stntuple/stat/crow_gardner.hh"

using namespace stntuple ;

//-----------------------------------------------------------------------------
// tests
//-----------------------------------------------------------------------------
crow_gardner* test_cg_001(double MuB, double MuS = 1, double CL = 0.9, const char* Name = "test_cg_001") {

  crow_gardner* cg = new crow_gardner(Name,MuS,CL);

  cg->construct_interval(MuB,MuS);
                                        // make histograms
  cg->make_prob_hist();

  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) c = new TCanvas(cname,Name,1200,800);
  else              c->cd();

  cg->fHist.fProb->Draw("text");
  cg->fHist.fInterval->Draw("sames");

  printf("IxMin, IxMax, Sum(P) : %3i %3i %12.5e\n",cg->fIxMin,cg->fIxMax,cg->fSump);

  return cg;
}

//-----------------------------------------------------------------------------
// construct belt
//-----------------------------------------------------------------------------
crow_gardner* test_cg_002(double MuB = 0, double CL = 0.9, const char* Name = "test_cg_002") {

  crow_gardner* cg = new crow_gardner(Name,1,CL);

                                        // make histograms
  cg->construct_belt(MuB,0,35,35001);
  cg->make_belt_hist();

  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) c = new TCanvas(cname,Name,1200,800);
  else              c->cd();

  cg->fHist.fBelt->Draw();

  return cg;
}

//-----------------------------------------------------------------------------
// test_cg_003: test 90% coverage
//-----------------------------------------------------------------------------
crow_gardner* test_cg_003(double MuB, double SMin, double SMax, int NPoints, double CL = 0.9, const char* Name = "cg_test_003") {

  crow_gardner* cg = new crow_gardner(Name,SMin,CL);

  cg->SetNExp(1000);
  cg->fDebugLevel.fTestCoverage = 1;
                                        // test itself
  
  cg->test_coverage(MuB,SMin,SMax,NPoints);
  
                                        // make histograms
  char cname[100];
  sprintf(cname,"c_%s",Name);
  
  TCanvas* c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cname);
  if (c == nullptr) {
    c = new TCanvas(cname,Name,1200,800);
  }

  cg->fHist.fCoverage->Draw("alp");
  
  return cg;
}

