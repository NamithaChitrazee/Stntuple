///////////////////////////////////////////////////////////////////////////////
// init file shoudl define a function void init_mu2e_model(su2020::Mu2e_model* M)
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/model_t.hh"
#include "Stntuple/stat/pgaus_t.hh"

using namespace stntuple;
//-----------------------------------------------------------------------------
void build_model(stntuple::model_t* m, const char* Config, int Debug = 0) {

  // nuisanse parameters

  // luminosity: simple parameter with gaussian fluctuations, 10% relative uncertainty
  
  double lum0       = 1.0;
  double sig_lum0   = 0.1;
  
  pgaus_t* lumi      = new pgaus_t("lumi",lum0,sig_lum0,Debug);

  m->fListOfParameters->Add(lumi);

  dio_channel_t* dio     = new dio_channel_t   ("dio",0.01,0.002,Debug);
  dio->fLumi = lumi;
  //dio->AddParameter(lumi);

  // channel_t* pbar    = new pbar_channel_t  ();
  // channel_t* rpci    = new rpci_channel_t  ();
  // channel_t* rpco    = new rpco_channel_t  ();
  // channel_t* cosmics = new cosmic_channel_t();
//-----------------------------------------------------------------------------
// to make it easy to loop over all channels, the signal is supposed to be the last
// channel and it should have a flag

  m->AddChannel(dio);
//-----------------------------------------------------------------------------
// at this point all initializations are done
//-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// test
//-----------------------------------------------------------------------------
void test_001(int SaveHist = 0) {

  stntuple::model_t* m = new stntuple::model_t("test_001");

  build_model(m,"test_001",1);

  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist","test_001");
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// test with the fixed luminosity
//-----------------------------------------------------------------------------
void test_002(int SaveHist = 0) {

  stntuple::model_t* m = new stntuple::model_t("test_002");

  build_model(m,"test_002",1);

  // fix lumi
  parameter_t* p = m->GetParameter("lumi");
  p->SetFixed(1);
  
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist","test_002");
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
void test_010(int NEvents = 10, int SaveHist = 0) {

  stntuple::model_t* m = new stntuple::model_t("aaa");

  build_model(m,"default");

  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist","test_001");
    m->SaveHist(fn.Data());
  }
}
