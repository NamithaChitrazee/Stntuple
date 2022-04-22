///////////////////////////////////////////////////////////////////////////////
// init file shoudl define a function void init_mu2e_model(su2020::Mu2e_model* M)
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/model_t.hh"
#include "Stntuple/stat/pgaus_t.hh"
#include "Stntuple/stat/plogn_t.hh"
#include "Stntuple/stat/dio_channel_t.hh"
#include "Stntuple/stat/pbar_channel_t.hh"

using namespace stntuple;

stntuple::model_t* m(nullptr);
//-----------------------------------------------------------------------------
void build_model(stntuple::model_t* m, const char* Config, int Debug = 0) {

  // nuisanse parameters

  // luminosity: simple parameter with gaussian fluctuations, 10% relative uncertainty
  
  pgaus_t* lumi  = new pgaus_t("lumi"   ,1.0, 0.1 ,Debug);
  pgaus_t* dio   = new pgaus_t("dio_var",0.1 ,0.01,Debug);

  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,Debug);
  dio_channel->SetBgr(dio);
  dio_channel->fLumi = lumi;

  // channel_t* pbar    = new pbar_channel_t  ();
  // channel_t* rpci    = new rpci_channel_t  ();
  // channel_t* rpco    = new rpco_channel_t  ();
  // channel_t* cosmics = new cosmic_channel_t();
//-----------------------------------------------------------------------------
// to make it easy to loop over all channels, the signal is supposed to be the last
// channel and it should have a flag
//-----------------------------------------------------------------------------
  m->AddParameter(lumi);
  m->AddParameter(dio);
  
  m->AddChannel(dio_channel);
//-----------------------------------------------------------------------------
// fixed luminosity 
//-----------------------------------------------------------------------------
  lumi->SetFixed(1);
//-----------------------------------------------------------------------------
// at this point all initializations are done
//-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// test gaussian channel with fixed luminosity
//-----------------------------------------------------------------------------
void test_001(int SaveHist = 0) {
  TString name = Form("%s",__func__);
  int debug = 1;

  if (m) delete m;
  
  m = new stntuple::model_t(name.Data());

  stntuple::pgaus_t* lumi  = new stntuple::pgaus_t("lumi"   ,1.0, 0.1 ,debug);
  stntuple::pgaus_t* dio   = new stntuple::pgaus_t("dio_var",0.1 ,0.01,debug);

  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,debug); // 

  dio_channel->SetBgr(dio);
  dio_channel->fLumi = lumi;

  m->AddParameter(lumi);
  m->AddParameter(dio);
  
  m->AddChannel(dio_channel);
//-----------------------------------------------------------------------------
// fix luminosity
//-----------------------------------------------------------------------------
  lumi->SetFixed(1);
//-----------------------------------------------------------------------------
// done with the initializations
//-----------------------------------------------------------------------------  
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// test gaussian with luminosity fluctuating by 10%
//-----------------------------------------------------------------------------
void test_002(int SaveHist = 0) {

  TString name = Form("%s",__func__);
  int debug = 1;
  
  if (m) delete m;

  m = new stntuple::model_t(name.Data());

  stntuple::pgaus_t* lumi  = new stntuple::pgaus_t("lumi"   ,1.0, 0.1 ,debug);
  stntuple::pgaus_t* dio   = new stntuple::pgaus_t("dio_var",0.1 ,0.01,debug);

  m->AddParameter(lumi);
  m->AddParameter(dio);
  
  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,debug); // 

  dio_channel->SetBgr(dio);
  dio_channel->fLumi = lumi;

  m->AddChannel(dio_channel);
//-----------------------------------------------------------------------------
// fix luminosity
//-----------------------------------------------------------------------------
  lumi->SetFixed(0);
//-----------------------------------------------------------------------------
// done with the initializations
//-----------------------------------------------------------------------------  
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// test gaussian with fixed luminosity
//-----------------------------------------------------------------------------
void test_003(int SaveHist = 0) {

  TString name = Form("%s",__func__);
  int debug = 1;
  
  if (m) delete m;

  m = new stntuple::model_t(name.Data());

  stntuple::pgaus_t* lumi  = new stntuple::pgaus_t("lumi"   ,1.0, 0.1 ,debug);
  stntuple::pgaus_t* dio   = new stntuple::pgaus_t("dio_var",0.1 ,0.1 ,debug);

  m->AddParameter(lumi);
  m->AddParameter(dio);
  
  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,debug); // 

  dio_channel->SetBgr(dio);
  dio_channel->fLumi = lumi;

  m->AddChannel(dio_channel);
//-----------------------------------------------------------------------------
// fix luminosity
//-----------------------------------------------------------------------------
  lumi->SetFixed(1);
//-----------------------------------------------------------------------------
// done with the initializations
//-----------------------------------------------------------------------------  
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// test gaussian with luminosity fluctuating by 10\%
//-----------------------------------------------------------------------------
void test_004(int SaveHist = 0) {

  TString name = Form("%s",__func__);
  int debug = 1;
  
  if (m) delete m;

  m = new stntuple::model_t(name.Data());

  stntuple::pgaus_t* lumi  = new stntuple::pgaus_t("lumi"   ,1.0, 0.1 ,debug);
  stntuple::pgaus_t* dio   = new stntuple::pgaus_t("dio_var",0.1 ,0.1 ,debug);

  m->AddParameter(lumi);
  m->AddParameter(dio);
  
  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,debug); // 

  dio_channel->SetBgr(dio);
  dio_channel->fLumi = lumi;

  m->AddChannel(dio_channel);
//-----------------------------------------------------------------------------
// fix luminosity
//-----------------------------------------------------------------------------
  lumi->SetFixed(0);
//-----------------------------------------------------------------------------
// done with the initializations
//-----------------------------------------------------------------------------  
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// test lognormal channel not smeared with the gaussian
//-----------------------------------------------------------------------------
void test_011(int SaveHist = 0) {
  int debug = 1;
  TString name = Form("%s",__func__);

  if (m) delete m;
  m = new stntuple::model_t(name.Data());

  // luminosity: simple parameter with gaussian fluctuations, 10% relative uncertainty
  
  pgaus_t* lumi = new pgaus_t("lumi",0.10,0.01,debug);
  plogn_t* pbar = new plogn_t("pbar",0.1 ,1.00,debug);

  m->AddParameter(lumi);
  m->AddParameter(pbar);

  pbar_channel_t* pbar_channel = new pbar_channel_t("pbar",debug);

  pbar_channel->SetBgr(pbar);
  pbar_channel->fLumi              = lumi;

  m->AddChannel(pbar_channel);
//-----------------------------------------------------------------------------
// fix luminosity
//-----------------------------------------------------------------------------
  lumi->SetFixed(1);
//-----------------------------------------------------------------------------
// end initialization
//-----------------------------------------------------------------------------
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// test lognormal channel smeared with the gaussian
//-----------------------------------------------------------------------------
void test_012(int SaveHist = 0) {
  int debug = 1;
  TString name = Form("%s",__func__);

  if (m) delete m;
  m = new stntuple::model_t(name.Data());

  // luminosity: simple parameter with gaussian fluctuations, 10% relative uncertainty
  
  pgaus_t* lumi = new pgaus_t("lumi",0.10,0.01,debug);
  plogn_t* pbar = new plogn_t("pbar",0.1 ,1.00,debug);

  m->AddParameter(lumi);
  m->AddParameter(pbar);

  pbar_channel_t* pbar_channel = new pbar_channel_t("pbar",debug);

  pbar_channel->SetBgr(pbar);
  pbar_channel->fLumi              = lumi;
//-----------------------------------------------------------------------------
// luminosity
//-----------------------------------------------------------------------------
  lumi->SetFixed(0);

  m->AddChannel(pbar_channel);
//-----------------------------------------------------------------------------
// end initialization
//-----------------------------------------------------------------------------
  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
void test_010(int NEvents = 10, int SaveHist = 0) {

  if (m) delete m;
  
  m = new stntuple::model_t("aaa");

  build_model(m,"default");

  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist","test_001");
    m->SaveHist(fn.Data());
  }
}
