#ifndef __stntuple_Mu2e_model_hh__
#define __stntuple_Mu2e_model_hh__

#include "Stntuple/stat/var_t.hh"
#include "Stntuple/stat/Functions.hh"
#include "Stntuple/stat/Poisson_t.hh"
#include "Stntuple/stat/FCCalculator.hh"

#include "TGraph.h"

namespace stntuple {
  
  class Mu2e_model {
  public: 
    Poisson_t*    fModel;
    FCCalculator* fFC;
    TH1D*         fHPdf;
    TH1D*         hUL;                  // 90% CL PDF corresponding to BGR+"90% CL signal"
    TH1D*         hULR;	                // same, but X-axis in units or Rmue
    TH1D*         hDisc;                // discovery PDF corresponding to BGR+"5 sigma signal"
    TH1D*         hDiscR;               // same, but X-axis in units of Rmue

    TGraph*       gr_Disc;
    TGraph*       gr_DiscR;
					// variables
    pow_t*        livetime_frac;
    var_t*        one;
    var_t*        neg_one;

    pow_t*        intensity_frac;
    var_t*        intensity_frac_1b;
    var_t*        intensity_frac_2b;
    
    var_t*        lumi_unc ;
    var_t*        lumi_beta;
    var_t*        lumi_var ;
    var_t*        lumi     ;

    var_t*        deadtime_unc ;
    var_t*        deadtime_beta;
    var_t*        deadtime_var ;
    var_t*        deadtime     ;

    var_t*        fSig_mu; 
    var_t*        fSig_eff;
    var_t*        fSig_unc;
    var_t*        fSig_beta;
    var_t*        fSig_var;
    var_t*        fSignal;

    var_t*        dio_unc ; 
    var_t*        dio_beta;
    var_t*        dio_var ;
    var_t*        dio     ;
   
    var_t*        pbar_unc ; 
    var_t*        pbar_beta;
    var_t*        pbar_var ;
    var_t*        pbar     ;
   
    var_t*        rpc_unc ; 
    var_t*        rpc_beta;
    var_t*        rpc_var ;
    var_t*        rpc     ;
   
    var_t*        rpc_oot_unc ; 
    var_t*        rpc_oot_beta;
    var_t*        rpc_oot_var ;
    var_t*        rpc_oot     ;

    var_t*        cr_light_yd     ;
    var_t*        cr_light_coeff  ;
    var_t*        cr_light_offset ;
    pow_t*        cr_light_var    ;
    var_t*        cr_light_bkg    ;

    var_t*        cr_lo_unc ;
    var_t*        cr_lo_beta;
    var_t*        cr_lo_var ;
    var_t*        cr_lo     ;

    var_t*        cr_hi_unc ;
    var_t*        cr_hi_beta;
    var_t*        cr_hi_var ;
    var_t*        cr_hi     ;
    
    var_t*        intensity_cutoff_2b_unc ;
    var_t*        intensity_cutoff_2b_beta;
    var_t*        intensity_cutoff_2b_var ;
    var_t*        intensity_cutoff_2b     ;
    var_t*        intensity_cutoff_1b     ;

    var_t*        trigger_eff;

    TObjArray*    fListOfVariables;

    double        fSes;
    double        fSignalAcceptance;
    double        fTrigEff;

    int           fVerbose;
    int           fDoConstraints;
//-----------------------------------------------------------------------------
    double        scaleLuminosity_ ; // scale luminosity for testing purposes
    double        scaleUncertainty_; // artificially change the systematic uncertainties
    double        scalePrecision_  ; // increase or decrease numerical precision

					//results, stored for external use
    double        upperLimit_;
    double        discovery_;
    double        nominalBkg_;
    double        nullBkg_;
    
    double        assumed_Rmue ; // Rmue assumed in su2020/stat/mumem_sensitivity.C

    bool          comparePDF_         ; // make a plot comparing full random sample to semi-random sampling
    bool          useLogNormal_       ; // use log-normal systematic uncertainty PDFs
    bool          useCRLightYieldUnc_ ; // consider our uncertainty on future light-yield in cosmics estimate

    bool          fUseLYSafetyFactor;   // use the cosmics background estimate with the lower light yield
    
    double        cr_lo_bkg      ;
    double        cr_lo_frac_unc ;
    double        cr_hi_bkg      ;
    double        cr_hi_frac_unc ;

    // consider our current uncertainty on the light-yield at data-taking time as a systematic on the measurement
    double        cr_lo_ly ;
    double        cr_hi_ly ;

    double        dio_bkg  ;   
    double        dio_frac_unc;

    double        rpc_bkg          ;
    double        rpc_frac_unc     ;
    double        extinction       ;
    double        rpc_oot_bkg      ;
    double        rpc_oot_frac_unc ;
    
    double        pbar_bkg      ;
    double        pbar_frac_unc ;
    
    double        one_batch_pot;
    double        two_batch_pot;
    double        tot_pot      ;

    double        sig;

    double        sig_frac_unc     ;
    
    double        lumi_frac_unc    ;
    double        deadtime_nom     ;
    double        deadtime_frac_unc;

    double        two_batch_ineff;
    double        two_batch_unc  ;
    
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    Mu2e_model(int DoConstraints = 1, int UseLYSafetyFactor = 0, int Verbose = 0);

    void InitVariables  ();
    void InitModel      ();
    void GenerateNullPDF();
    
    void set_constant();
    void print();
    void run(int Mode = 1);
    void SaveHist(const char* Filename);

    void Test001(int NEvents = 10);
  };
}
#endif
