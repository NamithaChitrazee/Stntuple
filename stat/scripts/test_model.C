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
  // initialize default SU2020 configuration
  // luminosity: simple parameter with gaussian fluctuations, 10% relative uncertainty
  // definition of the relative uncertainty: sigma/mean

  double pbar_bkg_mean = 9.76248e-03;
  double pbar_bkg_relu = 1.;               // sigma/mean = 100%
  
  double dio_bkg_mean  = 3.75955e-02;
  double dio_bkg_relu  = 0.63;             // sigma/mean = 63% (the larger part)
  
  double rpci_bkg_mean  = 1.06887e-02;
  double rpci_rel_unc   = 0.29;             // sigma/mean = 29% 

  double extinction     = 0.; // 1          // in units of 1.e-10;    

  double rpco_bkg_mean  = 14.5e-4;
  double rpco_rel_unc   = 0.12;             // no low-momentum dependence 
  
  double cosm_bkg_mean  = 4.74529e-2;
  double cosm_rel_unc   = 0.20;             // no low-momentum dependence 
  
  pgaus_t* lumi = new pgaus_t("lumi",1.0, 0.1 , debug);

  pgaus_t* dio  = new plogn_t("dio" ,dio_bkg_mean ,dio_bkg_relu ,debug);
  plogn_t* pbar = new plogn_t("pbar",pbar_bkg_mean,pbar_bkg_relu,debug);
  plogn_t* rpci = new plogn_t("rpci",rpci_bkg_mean,rpci_bkg_relu,debug);
  plogn_t* rpco = new plogn_t("rpco",rpco_bkg_mean,rpco_bkg_relu,debug);
  plogn_t* cosm = new plogn_t("cosm",cosm_bkg_mean,cosm_bkg_relu,debug);

  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,Debug);
  dio_channel->SetBgr(dio);
  dio_channel->fLumi = lumi;

  pbar_channel_t* pbar_channel = new pbar_channel_t("pbar",debug);
  pbar_channel->SetBgr(pbar);
  pbar_channel->fLumi = lumi;

  rpci_channel_t* rpci_channel = new rpci_channel_t("rpci",debug);
  rpci_channel->SetBgr(rpci);
  rpci_channel->fLumi = lumi;

  rpco_channel_t* rpco_channel = new rpco_channel_t("rpco",debug);
  rpco_channel->SetBgr(rpco);
  rpco_channel->fLumi = lumi;
//-----------------------------------------------------------------------------
// no luminosity related uncertainty for cosmics
// make sure cosmics iis anti-correlated with the luminosity
//-----------------------------------------------------------------------------
  cosm_channel_t* cosm_channel = new cosm_channel_t("cosm",debug);
  cosm_channel->SetBgr(cosm);
//-----------------------------------------------------------------------------
// now - mu --> e signal 
//-----------------------------------------------------------------------------
  double assumed_Rmue  = 1.e-15;
  double sign_mean     = 4.08396;
  double sign_relu     = 0.04;		                // uncertainty on the signal acceptance - mom scale
  double ses           = 1./(*tot_pot*1.59e-3*0.609);   // for signal strength -> R_mue, SES if acceptance was 100%
  double sign_acc      = sign_mean*(ses/assumed_Rmue) ; // 4.23637*ses/assumed_Rmue ;  // 0.11735881;

  mu2e_channel_t* mu2e_channel = new mu2e_channel_t("mu2e",debug);
  mu2e_channel->SetBgr(mu2e);
  mu2e_channel->fLumi = mu2e;
//-----------------------------------------------------------------------------
// luminosity : high-luminosity cut-off in the simulation introduces additional
// uncertainty: 
//-----------------------------------------------------------------------------
  double one_batch_pot   = 2.86e19;
  double two_batch_pot   = 9.03e18;                    // two-batch mode POT, for separate batch uncertainties
  double eff_ur_1b       = 0.994;
  double eff_ur_1b       = 0.;
  double tot_pot         = one_batch_pot + two_batch_pot;
  double two_batch_ineff = 0.;           // inefficiency due to intensity cut-off already in background estimates
  double two_batch_unc   = scaleUncertainty_*1.00; // assume two-batch mode loss is 6% +- 6%
//-----------------------------------------------------------------------------
// to make it easy to loop over all channels, the signal is supposed to be the last
// channel and it should have a flag
//-----------------------------------------------------------------------------
  m->AddParameter(lumi);
  m->AddParameter(dio);
  m->AddParameter(pbar);
  m->AddParameter(rpci);
  m->AddParameter(rpco);
  m->AddParameter(mu2e);
					// add background channels
  m->AddChannel(dio_channel ,0);
  m->AddChannel(pbar_channel,0);
  m->AddChannel(rpci_channel,0);
  m->AddChannel(rpco_channel,0);
  m->AddChannel(cosm_channel,0);
					// add signal
  m->AddChannel(mu2e_channel,1);        // doesn't have to be the last one 
//-----------------------------------------------------------------------------
// fixed luminosity 
//-----------------------------------------------------------------------------
  lumi->SetFixed(0);
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
