///////////////////////////////////////////////////////////////////////////////
// init file shoudl define a function void init_mu2e_model(su2020::Mu2e_model* M)
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/model_t.hh"

#include "Stntuple/stat/pgaus_t.hh"
#include "Stntuple/stat/plogn_t.hh"
#include "Stntuple/stat/ppoi_t.hh"

#include "Stntuple/stat/dio_channel_t.hh"
#include "Stntuple/stat/pbar_channel_t.hh"
#include "Stntuple/stat/rpci_channel_t.hh"
#include "Stntuple/stat/rpco_channel_t.hh"
#include "Stntuple/stat/cosm_channel_t.hh"
#include "Stntuple/stat/mu2e_channel_t.hh"

#include "Stntuple/stat/TFeldmanCousinsB.hh"

using namespace stntuple;

stntuple::model_t*          m (nullptr);
stntuple::TFeldmanCousinsB* fc(nullptr);

//-----------------------------------------------------------------------------
void build_model_001(stntuple::model_t* m) {

  int debug(1);
  
  // nuisanse parameters
  // initialize default SU2020 configuration
  // luminosity: simple parameter with gaussian fluctuations, 10% relative uncertainty
  // definition of the relative uncertainty: sigma/mean

  double pbar_bkg_mean  = 9.76248e-03;
  double pbar_bkg_relu  = 1.;               // sigma/mean = 100%
  
  double dio_bkg_mean   = 3.75955e-02;
  double dio_bkg_relu   = 0.63;             // sigma/mean = 63% (the larger part)
  
  double rpci_bkg_mean  = 1.06887e-02;
  double rpci_bkg_relu  = 0.29;             // sigma/mean = 29% 

  double extinction     = 0.; // 1          // in units of 1.e-10;    

  double rpco_bkg_mean  = 14.5e-4;
  double rpco_bkg_relu  = 0.12;             // no low-momentum dependence 
  
  double cosm_bkg_mean  = 4.74529e-2;
  double cosm_bkg_relu  = 0.20;             // no low-momentum dependence 
  
  pgaus_t* lumi = new pgaus_t("lumi",1.0, 0.1 , debug);
//-----------------------------------------------------------------------------
// luminosity : high-luminosity cut-off in the simulation introduces additional
//-----------------------------------------------------------------------------
  double npot_1b   = 2.86e19;		               // one-batch mode POT, for separate batch uncertainties
  double npot_2b   = 9.03e18;                          // two-batch mode POT, for separate batch uncertainties

  double eff_ur_1b       = 0.994;                      // inefficiency of the upstream track rejection
  double eff_ur_2b       = 0.986;		       //
  
  float crv_deadtime_1b    (0.040094062);   // wrong weights: (1.67927e-02);
  float crv_deadtime_2b    (0.15103281) ;   // wrong weights: (7.62028e-02);

  float crv_deadtime_c_1b  (0.015486111);   // cosmic flux is independent on the beam
  float crv_deadtime_c_2b  (0.076176295);

  double lumi_cutoff_1b  = 1-0.99342;	               // simulation cutoff - didn't simulate pulse intensities 
  double lumi_cutoff_2b  = 1-0.88785;		       // above 12e7/pulse

  // these fluctuate with 
  double np_1b           = npot_1b*(1-crv_deadtime_1b)*eff_ur_1b*(1-lumi_cutoff_1b/2.);
  double np_2b           = npot_2b*(1-crv_deadtime_2b)*eff_ur_2b*(1-lumi_cutoff_2b/2.);

  double np_tot          = np_1b + np_2b;

  // uniform uncertainty on the reco efficiency above 12e7 protons/pulse - an additive parameter
  
  double x1b_max        = npot_1b*(1-crv_deadtime_1b)*eff_ur_1b/np_tot*(lumi_cutoff_1b/2);
  double x1b_min        = -x1b_max;
  
  double x2b_max        = npot_2b*(1-crv_deadtime_2b)*eff_ur_2b/np_tot*(lumi_cutoff_2b/2);
  double x2b_min        = -x2b_max;

  pflat_t* eff1b         = new pflat_t("eff1b",x1b_min,x1b_max,debug); 
  pflat_t* eff2b         = new pflat_t("eff2b",x2b_min,x2b_max,debug); 
//-----------------------------------------------------------------------------
// effect of luminosity cutoff for cosmics: 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// background means
//-----------------------------------------------------------------------------
  plogn_t* dio  = new plogn_t("dio" ,dio_bkg_mean ,dio_bkg_relu ,debug);
  plogn_t* pbar = new plogn_t("pbar",pbar_bkg_mean,pbar_bkg_relu,debug);
  plogn_t* rpci = new plogn_t("rpci",rpci_bkg_mean,rpci_bkg_relu,debug);
  plogn_t* rpco = new plogn_t("rpco",rpco_bkg_mean,rpco_bkg_relu,debug);
  plogn_t* cosm = new plogn_t("cosm",cosm_bkg_mean,cosm_bkg_relu,debug);

  dio_channel_t* dio_channel = new dio_channel_t("dio"    ,debug);
  dio_channel->SetProcess(dio);
  dio_channel->fLumi = lumi;
  dio_channel->AddPAdd(eff1b);
  dio_channel->AddPAdd(eff2b);

  pbar_channel_t* pbar_channel = new pbar_channel_t("pbar",debug);
  pbar_channel->SetProcess(pbar);
  pbar_channel->fLumi = lumi;
  pbar_channel->AddPAdd(eff1b);
  pbar_channel->AddPAdd(eff2b);

  rpci_channel_t* rpci_channel = new rpci_channel_t("rpci",debug);
  rpci_channel->SetProcess(rpci);
  rpci_channel->fLumi = lumi;
  rpci_channel->AddPAdd(eff1b);
  rpci_channel->AddPAdd(eff2b);

  rpco_channel_t* rpco_channel = new rpco_channel_t("rpco",debug);
  rpco_channel->SetProcess(rpco);
  rpco_channel->fLumi = lumi;
  rpco_channel->AddPAdd(eff1b);
  rpco_channel->AddPAdd(eff2b);
//-----------------------------------------------------------------------------
// cosmics
// no luminosity related uncertainty for cosmics
// make sure cosmics iis anti-correlated with the luminosity
//-----------------------------------------------------------------------------
  cosm_channel_t* cosm_channel = new cosm_channel_t("cosm",debug);
  cosm_channel->SetProcess(cosm);
//-----------------------------------------------------------------------------
// mu --> e signal 
//-----------------------------------------------------------------------------
  double assumed_Rmue  = 1.e-15;
  double sign_mean     = 4.08396;
  double sign_relu     = 0.04;		                // uncertainty on the signal acceptance - mom scale
  double ses           = 1./(np_tot*1.59e-3*0.609);     // for signal strength -> R_mue, SES if acceptance was 100%
  double sign_acc      = sign_mean*(ses/assumed_Rmue) ; // 4.23637*ses/assumed_Rmue ;  // 0.11735881;

  ppoi_t* mu2e = new ppoi_t("mu2e_poi",sign_mean,sign_relu,debug);

  mu2e_channel_t* mu2e_channel = new mu2e_channel_t("mu2e",debug);
  mu2e_channel->SetProcess(mu2e);
  mu2e_channel->fLumi = lumi;
  mu2e_channel->AddPAdd(eff1b);
  mu2e_channel->AddPAdd(eff2b);
  mu2e_channel->SetSignal(1);
//-----------------------------------------------------------------------------
// for cosmics it is different
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// to make it easy to loop over all channels, the signal is supposed to be the last
// channel and it should have a flag
//-----------------------------------------------------------------------------
  m->AddParameter(lumi);
  m->AddParameter(dio);
  m->AddParameter(pbar);
  m->AddParameter(rpci);
  m->AddParameter(rpco);
  m->AddParameter(cosm);
  m->AddParameter(mu2e);
  m->AddParameter(eff1b);
  m->AddParameter(eff2b);
					// add background channels
  m->AddChannel(dio_channel );
  m->AddChannel(pbar_channel);
  m->AddChannel(rpci_channel);
  m->AddChannel(rpco_channel);
  m->AddChannel(cosm_channel);
					// add signal
  m->AddChannel(mu2e_channel);        // doesn't have to be the last one 
//-----------------------------------------------------------------------------
// fixed luminosity 
//-----------------------------------------------------------------------------
  lumi->SetFixed(0);
//-----------------------------------------------------------------------------
// at this point all initializations are done
//-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// build a simple model with a signal and one background channel
// vary signal and background, but not their uncertainties
//-----------------------------------------------------------------------------
void build_model_002(stntuple::model_t* m, double MuS, double MuB) {
  int debug(1);
  // parameters
  
  pgaus_t* lumi = new pgaus_t("lumi", 0.10, 0.01,debug);  // 10% fluctuations
  plogn_t* pbar = new plogn_t("pbar", MuB , 1.00,debug);
  ppoi_t*  mu2e = new ppoi_t ("mu2e", MuS , 0.1 ,debug);

					// fix parameters
  // lumi->SetFixed(1);
  // pbar->SetFixed(1);
  // mu2e->SetFixed(1);
					// add parameters to the model
  m->AddParameter(lumi);
  m->AddParameter(pbar);
  m->AddParameter(mu2e);
//----------------------------------------------------------------------------- 
// define two processes
//----------------------------------------------------------------------------- 
  pbar_channel_t* pbar_channel = new pbar_channel_t("bgr",debug);
  pbar_channel->SetProcess(pbar);
  pbar_channel->fLumi = lumi;

  mu2e_channel_t* mu2e_channel = new mu2e_channel_t("mu2e",debug);
  mu2e_channel->SetProcess(mu2e);
  mu2e_channel->fLumi = lumi;
  mu2e_channel->SetSignal(1);
//-----------------------------------------------------------------------------
// add channels, at this point all channels need to be fuully specified and
// the signal channel flagged as such
//-----------------------------------------------------------------------------
  m->AddChannel(pbar_channel);
  m->AddChannel(mu2e_channel);		// signal

  if (debug) m->Print();
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

  dio_channel->SetProcess(dio);
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

  dio_channel->SetProcess(dio);
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

  dio_channel->SetProcess(dio);
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

  dio_channel->SetProcess(dio);
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

  pbar_channel->SetProcess(pbar);
  pbar_channel->fLumi = lumi;

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

  pbar_channel->SetProcess(pbar);
  pbar_channel->fLumi          = lumi;
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
// buld null and non-null PDFs of the Mu2e SU2020 statitical model
//-----------------------------------------------------------------------------
void test_101(int SaveHist = 0) {

  TString name = Form("%s",__func__);
  if (m) delete m;
  
  m = new stntuple::model_t(name.Data());

  build_model_001(m);

  m->GeneratePDF();

  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",name.Data());
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// validate calculation of the discovery potential, using a simple model
//-----------------------------------------------------------------------------
void test_102(int FixParameters = 1, int SaveHist = 0) {

  TString name = Form("%s",__func__);
  if (m) delete m;
//------------------------------------------------------------------------------
// build a model
//-----------------------------------------------------------------------------
  m = new stntuple::model_t(name.Data());

  double mub(0.1);
  double mus(5.0);
					// signal, then - background
  build_model_002(m,mus,mub);
					// for this test, don't need to generate a PDF
  if (FixParameters) { 
  					// fix values of all model parameters
    int np = m->NParameters();
    for (int i=0; i<np; i++) {
      parameter_t* p = m->Parameter(i);
      p->SetFixed(1);
    }
  }
//------------------------------------------------------------------------------
// create a statistical calculator
//-----------------------------------------------------------------------------
  int model_debug;
  TFeldmanCousinsB* fc = new TFeldmanCousinsB(name.Data(),-1,model_debug=0);
  fc->SetNExp(100000);

  double sig[100], nsig[100], smin, smax;
  int    npoints;

  printf(" ------------ old interface:\n");
  fc->DiscoveryProbMean(mub,smin=4.5,smax=5.5,npoints=11,sig,nsig);

  for (int i=0; i<npoints; i++) {
    printf("i, sig[i], nsig[i] : %3i %10.4f %10.4f\n",i,sig[i],nsig[i]);
  }
  
  printf(" ------------ new interface:\n");
  fc->DiscoveryProbMean(m, smin=4.5, smax=5.5, npoints=11, sig, nsig);

  for (int i=0; i<npoints; i++) {
    printf("i, sig[i], nsig[i] : %3i %10.4f %10.4f\n",i,sig[i],nsig[i]);
  }
  
  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    int par_code = FixParameters*1000;
    TString fn = Form("mu2e_sensitivity.%s.%04i.hist",name.Data(), par_code);
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// validate calculation of the discovery potential, using a full model
//-----------------------------------------------------------------------------
void test_103(int FixParameters = 1, int SaveHist = 0) {

  TString name = Form("%s",__func__);
  if (m) delete m;
//------------------------------------------------------------------------------
// build a model
//-----------------------------------------------------------------------------
  m = new stntuple::model_t(name.Data());

					// build_model_001 defines SU2020 backgrounds
  build_model_001(m);
					// for this test, don't need to generate a PDF
  if (FixParameters) { 
  					// fix values of all model parameters
    int np = m->NParameters();
    for (int i=0; i<np; i++) {
      parameter_t* p = m->Parameter(i);
      p->SetFixed(1);
    }
  }
//------------------------------------------------------------------------------
// create a statistical calculator
//-----------------------------------------------------------------------------
  int model_debug;
  TFeldmanCousinsB* fc = new TFeldmanCousinsB(name.Data(),-1,model_debug=0);
  fc->SetNExp(100000);

  double sig[100], nsig[100], smin, smax;
  int    npoints;

  double mub = m->GetBackgroundMean();
  
  printf(" ------------ old interface: mub = %10.5f\n",mub);
  fc->DiscoveryProbMean(mub,smin=4.5,smax=5.5,npoints=11,sig,nsig);

  for (int i=0; i<npoints; i++) {
    printf("i, sig[i], nsig[i] : %3i %10.4f %10.4f\n",i,sig[i],nsig[i]);
  }
  
  printf(" ------------ new interface: mub = %10.5f\n",mub);
  fc->DiscoveryProbMean(m, smin=4.5, smax=5.5, npoints=11, sig, nsig);

  for (int i=0; i<npoints; i++) {
    printf("i, sig[i], nsig[i] : %3i %10.4f %10.4f\n",i,sig[i],nsig[i]);
  }
  
  // at this point can draw PDF...and save histograms
  
  if (SaveHist > 0) {
    int par_code = FixParameters*1000;
    TString fn = Form("mu2e_sensitivity.%s.%04i.hist",name.Data(), par_code);
    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
// validate calculation of an upper limit, using a simple model
//-----------------------------------------------------------------------------
void test_111(int FixParameters = 1, int SaveHist = 0) {

  TString name = Form("%s",__func__);
  if (m) delete m;
//------------------------------------------------------------------------------
// build a model
//-----------------------------------------------------------------------------
  m = new stntuple::model_t(name.Data());

  double mub(0.1);
  double mus(5.0);
					// signal, then - background
  build_model_002(m,mus,mub);
					// for this test, don't need to generate a PDF
  if (FixParameters) { 
  					// fix values of all model parameters
    int np = m->NParameters();
    for (int i=0; i<np; i++) {
      parameter_t* p = m->Parameter(i);
      p->SetFixed(1);
    }
  }
					// create statistical calculator
  int debug_fc;

  if (fc) delete fc;
  fc = new TFeldmanCousinsB(name.Data(),-1,debug_fc=1);
  fc->SetNExp(1);

  double sig[100], prob[100], smin, smax;
  int    npoints;

  printf(" ------------ old interface:\n");
  fc->UpperLimit(mub,smin=1.,smax=5.,npoints=1,sig,prob);

  for (int i=0; i<npoints; i++) {
    printf("i, sig[i], prob[i] : %3i %10.4f %10.4f\n",i,sig[i],prob[i]);
  }
  
  printf(" ------------ new interface:\n");
  fc->UpperLimit(m, smin=1., smax=5., npoints=1, sig, prob);

  for (int i=0; i<npoints; i++) {
    printf("i, sig[i], prob[i] : %3i %10.4f %10.4f\n",i,sig[i],prob[i]);
  }
					// at this point can draw PDF...and save histograms
  if (SaveHist > 0) {
    int par_code = FixParameters*1000;
    TString fn = Form("mu2e_sensitivity.%s.%04i.hist",name.Data(), par_code);
    m->SaveHist(fn.Data());
  }
}



//-----------------------------------------------------------------------------
// validate calculation of a FC interval using a simple model
//-----------------------------------------------------------------------------
void test_121(int FixParameters = 1) {

  TString name = Form("%s",__func__);
  if (m) delete m;
//------------------------------------------------------------------------------
// build a model
//-----------------------------------------------------------------------------
  m = new stntuple::model_t(name.Data());

  double mub(3.0);
  double mus(0.5);
					// signal, then - background
  build_model_002(m,mus,mub);
					// for this test, don't need to generate a PDF
  if (FixParameters) { 
  					// fix values of all model parameters
    int np = m->NParameters();
    for (int i=0; i<np; i++) {
      parameter_t* p = m->Parameter(i);
      p->SetFixed(1);
    }
  }
					// create statistical calculator
  int debug_fc;

  if (fc) delete fc;
  fc = new TFeldmanCousinsB(name.Data(),0.9,debug_fc=11);
  fc->SetNExp(1);

  double sig[100], prob[100], smin, smax;
  int    npoints;

  fc->ConstructInterval(mub,mus);

}

