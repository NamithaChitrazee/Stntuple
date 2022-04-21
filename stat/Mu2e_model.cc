// -*- mode:c++ -*- 
#include <iostream>

#include "TString.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TMath.h"
#include "TDirectory.h"
#include "TFile.h"

#include "Stntuple/stat/FCCalculator.hh"
#include "Stntuple/stat/Functions.hh"
#include "Stntuple/stat/Mu2e_model.hh"

using namespace stntuple;

namespace stntuple {

  //-----------------------------------------------------------------------------
  Mu2e_model::Mu2e_model(int DoConstraints, int UseLYSafetyFactor, int Verbose) {

    ///////////////////////////////////////////////////////
    // Initialize experimental parameters
    ///////////////////////////////////////////////////////
    // 2022-02-16 P.Murat: 
    //    pmin      pmax        tmin      tmax      Cosmics        DIO       PbarTOT         RPC        Total        Signal        s5         SES          Rmue
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  103.600    104.900    640.000   1650.000  4.63259e-02  3.75955e-02  9.76248e-03  1.06887e-02  1.04373e-01  4.08396e+00  4.84560e+00  2.44860e-16  1.18650e-15 <-- default
    //  103.500    104.850    630.000   1650.000  2.91161e-01  6.05779e-02  1.03715e-02  2.31683e-02  3.85279e-01  4.29211e+00  6.68101e+00  2.32986e-16  1.55658e-15 <-- LY -15%/year
    

    // 2022-01-11 P.Murat: 
    //    pmin      pmax      tmin      tmax   Cosmics         DIO         PbarTOT        RPC        Total       Signal        s5         SES          Rmue
    // ----------------------------------------------------------------------------------------------------------------------------------------------------
    //  103.600   104.900   640.000  1650.000  4.74529e-02  3.90553e-02  1.01270e-02  1.11215e-02  1.07757e-01  4.23637e+00  4.87063e+00  2.36051e-16 1.14972e-15 
  
    // 2021-07-31 (approx) optimization output
    //    pmin      pmax      tmin      tmax   Cosmics       DIO       PbarTOT        RPC       Total       Signal        s5         SES        Rmue
    // ----------------------------------------------------------------------------------------------------------------------------------------------------
    //  103.600    104.90    640.0    1650.0  4.74529e-2   3.77436e-2  1.01270e-2  1.05792e-2  1.05903e-1  4.27956e0   4.86174e0  2.33669e-16 1.13604e-15

    //lower light yield
    //  103.500    104.850   620.0    1680.0  2.43785e-1   6.55531e-2  1.15306e-2  3.85590e-2  3.59428e-1  4.74246e-1  6.54537e0  2.10861e-16 1.38016e-15
    //-----------------------------------------------------------------------------------------------------------------------------------------------------

    fUseLYSafetyFactor  = UseLYSafetyFactor;
    fDoConstraints      = DoConstraints;          // cache, currently don't need, just in case
    fVerbose            = Verbose;

    comparePDF_         = false ; // make a plot comparing full random sample to semi-random sampling
    useLogNormal_       = true  ; // use log-normal systematic uncertainty PDFs
    useCRLightYieldUnc_ = false ; // consider our uncertainty on future light-yield in cosmics estimate
    
    assumed_Rmue        = 1.e-15; // Rmue assumed in su2020/stat/mumem_sensitivity.C

    scaleLuminosity_    = 1.    ; // scale luminosity for testing purposes
    scaleUncertainty_   = 1.    ; // artificially change the systematic uncertainties
    scalePrecision_     = 1.    ; // increase or decrease numerical precision

//    cr_lo_bkg         = (UseLYSafetyFactor) ? 2.91161e-01 : 4.63259e-02; // 4.74529e-2;
//    cr_lo_frac_unc    = scaleUncertainty_*0.20;
//    cr_hi_bkg         = 0.;                                               // 0.02; //see docdb-38052 slide 20: COMBINED INTO CRV_LO FOR NOW
//    cr_hi_frac_unc    = scaleUncertainty_*0.50;
//
//    // consider our current uncertainty on the light-yield at data-taking time as a systematic on the measurement
//    cr_lo_ly          = cr_lo_bkg;
//    cr_hi_ly          = 0.5; //FIXME: increased to not be negative with high value
    
// dio_bkg           = (fUseLYSafetyFactor)  ? 6.05779e-02 : 3.75955e-02; // 3.90553e-02;  // 3.77436e-2;
// dio_frac_unc      = scaleUncertainty_*0.63;                            // using systematic upper estimate with statistical error

// rpc_bkg           = (fUseLYSafetyFactor)  ? 2.31683e-02 : 1.06887e-02; // 1.11215e-02;  // 1.057920e-2;
// rpc_frac_unc      = scaleUncertainty_*0.29;                            // using systematic upper estimate with statistical error
//    extinction        = 0.;                                                // in units of 10^-10, FIXME: Get expected value
// rpc_oot_bkg       = 14.5e-4*extinction;                                // see docdb-36503 section 11.3, scaled 13.9 by (tmax - tmin) / (1695 - 700)
// rpc_oot_frac_unc  = scaleUncertainty_*0.12;                            // using systematic upper estimate with statistical error

    pbar_bkg          = (fUseLYSafetyFactor ) ? 1.03715e-02 : 9.76248e-03; // 1.01270e-2;
    pbar_frac_unc     = scaleUncertainty_*1.;                              // 100% uncertainty quoted - careful with Gaussian mode!

    one_batch_pot     = 2.8631579e19;                    // one-batch mode POT, for separate batch uncertainties
    two_batch_pot     = 9.0285712e18;                    // two-batch mode POT, for separate batch uncertainties
    tot_pot           = one_batch_pot + two_batch_pot;

    sig               = (fUseLYSafetyFactor ) ? 4.29211 : 4.08396;    // N(events), an integral, Rmue = 1.e-15;

    deadtime_nom      = 0.;                     // deadtime already in background estimates

    sig_frac_unc      = scaleUncertainty_*0.04; // taken as the maximum from the momentum scale, but should be two-sided and correlated with DIO
    lumi_frac_unc     = scaleUncertainty_*0.1;  // muon stop normalization uncertainty
    deadtime_frac_unc = scaleUncertainty_*0.5;  // assume 10 +- 5% deadtime

    two_batch_ineff   = 0.;                     // inefficiency due to intensity cut-off already in background estimates
    two_batch_unc     = scaleUncertainty_*1.00; // assume two-batch mode loss is 6% +- 6%

    fTrigEff          = 1.00;                   // trigger efficiency included in the background estimates already

    fListOfVariables  = new TObjArray();
    
    gr_Disc           = nullptr;
    gr_DiscR          = nullptr;
  }

//-----------------------------------------------------------------------------
  void  Mu2e_model::InitVariables() { 
//-----------------------------------------------------------------------------
// All in terms of nominal and fractional uncertainty on the nominal
// normally, scaleUncertainty_ = 1
// the sensitivity optimization with the lower light yield results in a different window,
// thus different background values
// Initialize model parameters
//-----------------------------------------------------------------------------
    fSes              = 1./(scaleLuminosity_*tot_pot*1.59e-3*0.609); // for signal strength -> R_mue, SES if acceptance was 100%
    fSignalAcceptance = sig*(fSes/assumed_Rmue) ;                    // 4.23637*ses/assumed_Rmue ;  // 0.11735881;
//-----------------------------------------------------------------------------
// Deadtime variables
//-----------------------------------------------------------------------------
    deadtime_unc  = new var_t ("Deadtime_uncertainty", deadtime_frac_unc);
    deadtime_beta = new var_t ("Deadtime_beta"       , 0.               , -10., 10.);
    deadtime_var  = new var_t ("Deadtime_variation"  , 0.               ,  -1., 10.);
    deadtime      = new var_t ("Deadtime"            , deadtime_nom     ,   0.,  1.);

    fListOfVariables->Add(deadtime_unc);
    fListOfVariables->Add(deadtime_beta);
    fListOfVariables->Add(deadtime_var);
    fListOfVariables->Add(deadtime);
    
    if (useLogNormal_) {
//-----------------------------------------------------------------------------
//  Log-normal via value = nominal * (1 + uncertainty = kappa) ^ (Normal distribution = beta)
//-----------------------------------------------------------------------------
      deadtime_var->nom_ = 1.;
      deadtime_var->val_ = 1.;
      deadtime_var->set_dependencies({deadtime_unc}, {}, {deadtime_beta});
      deadtime->set_dependencies({}, {deadtime_var}, {});
    } else {
      deadtime_var->set_dependencies({deadtime_unc}, {deadtime_beta}); // add uncertainty * unit width gaussian to deadtime
      deadtime->set_dependencies({deadtime_var}, {});
    }
    
    one           = new var_t ("one"    ,  1.);                  // place-holder
    neg_one       = new var_t ("neg_one", -1.);                  // place-holder
    livetime_frac = new pow_t ("livetime_fraction");

    livetime_frac->set_dependencies({one, deadtime}, {one, neg_one}, {0, 1}); //set livetime = 1*1^0 + (-1)*deadtime^1 = 1 - deadtime

    fListOfVariables->Add(livetime_frac);
    fListOfVariables->Add(one);
    fListOfVariables->Add(neg_one);
    
    /////////////////////////////////////
    // Intensity cut-off variables
    // Two-batch mode generation had a cutoff of the intensity distribution, where 12% of the
    // log-normal distribution was not included. To account for this, assume the 2-batch
    // integrated intensity has a -6% loss with +- 6% uncertainty on this (with a max of 0)

    intensity_cutoff_2b_unc  = new var_t ("intensity_cutoff_2b_uncertainty", two_batch_unc);
    intensity_cutoff_2b_beta = new var_t ("intensity_cutoff_2b_beta"       , 0., -10., 10.);
    intensity_cutoff_2b_var  = new var_t ("intensity_cutoff_2b_variation"  , 0., -1., 10.);
    intensity_cutoff_2b      = new var_t ("intensity_cutoff_2b_effect"     , 1. - two_batch_ineff, 0., 1.);
    intensity_cutoff_1b      = new var_t ("intensity_cutoff_1b_effect"     , 1.); //no real effect for 1-batch mode

    fListOfVariables->Add(intensity_cutoff_2b_unc);
    fListOfVariables->Add(intensity_cutoff_2b_beta);
    fListOfVariables->Add(intensity_cutoff_2b_var);
    fListOfVariables->Add(intensity_cutoff_2b);
    fListOfVariables->Add(intensity_cutoff_1b);

    if (useLogNormal_) {
      intensity_cutoff_2b_var->nom_ = 1.;
      intensity_cutoff_2b_var->val_ = 1.;
      intensity_cutoff_2b_var->set_dependencies({intensity_cutoff_2b_unc}, {}, {intensity_cutoff_2b_beta});
      intensity_cutoff_2b->set_dependencies({}, {intensity_cutoff_2b_var}, {});
    } else {
      intensity_cutoff_2b_var->set_dependencies({intensity_cutoff_2b_unc}, {intensity_cutoff_2b_beta});
      intensity_cutoff_2b->set_dependencies({intensity_cutoff_2b_var}, {});
    }
    
    intensity_frac_1b = new var_t ("intensity_1b_mode", one_batch_pot/tot_pot);
    intensity_frac_2b = new var_t ("intensity_2b_mode", two_batch_pot/tot_pot);
    intensity_frac    = new pow_t ("intensity_fraction_due_to_cutoff_effect");

    fListOfVariables->Add(intensity_frac_1b);
    fListOfVariables->Add(intensity_frac_2b);
    fListOfVariables->Add(intensity_frac);
    
    // intensity = 1-batch intensity * cutoff effect + 2-batch intensity * cutoff effect
    intensity_frac->set_dependencies({intensity_cutoff_1b, intensity_cutoff_2b}, {intensity_frac_1b, intensity_frac_2b}, {1,1});

    /////////////////////////////////////
    // Trigger efficiency variables
    trigger_eff = new var_t("trig_eff", fTrigEff);
    fListOfVariables->Add(trigger_eff);

    /////////////////////////////////////
    // Luminosity variables
    lumi_unc  = new var_t ("lumi_uncertainty", lumi_frac_unc);
    lumi_beta = new var_t ("lumi_beta", 0., -10., 10.);
    lumi_var  = new var_t ("lumi_variation", 0., -1., 10.);
    lumi      = new var_t ("lumi", (scaleLuminosity_ > 0.) ? scaleLuminosity_ : 1., 0., 10.);
    
    fListOfVariables->Add(lumi_unc);
    fListOfVariables->Add(lumi_beta);
    fListOfVariables->Add(lumi_var);
    fListOfVariables->Add(lumi);

    if (useLogNormal_) {
      lumi_var->nom_ = 1.;
      lumi_var->val_ = 1.;
      lumi_var->set_dependencies({lumi_unc}, {}, {lumi_beta}); // multiply (1 + uncertainty) ^ unit width gaussian to lumi prediction
      lumi    ->set_dependencies({},
				 {lumi_var, livetime_frac, intensity_frac, trigger_eff},
				 {}); // lumi = nominal * kappa ^ beta; kappa = 1 + uncertainty, beta = normal, width 1
      // {add}, {multiply}, {powers} --> value = ((sum of adds) * (product of mul)) ^ {product of powers}
    } else {
      lumi_var->set_dependencies({lumi_unc}, {lumi_beta}); //add uncertainty * unit width gaussian to lumi prediction
      lumi    ->set_dependencies({lumi_var}, {livetime_frac, intensity_frac, trigger_eff});
    }
//-----------------------------------------------------------------------------
// DIO background
//-----------------------------------------------------------------------------
    dio      = new var_t ("dio_expectation", dio_bkg             , 0., 0.2);
    dio_beta = new var_t ("dio_beta"       , 0.                  , -10.       , 10.);
    dio_unc  = new var_t ("dio_uncertainty", dio_bkg*dio_frac_unc);
    dio_var  = new var_t ("dio_variation"  , 0.                  , -1.*dio_bkg,  5.-1.*dio_bkg);  // range - keep the background positive

    fListOfVariables->Add(dio_unc);
    fListOfVariables->Add(dio_beta);
    fListOfVariables->Add(dio_var);
    fListOfVariables->Add(dio);

    if (useLogNormal_) {
      dio_var->nom_ = 1.;
      dio_var->val_ = 1.;
      //                        add       mult              pow
      dio_var->set_dependencies({dio_unc}, {}             , {dio_beta});
      dio    ->set_dependencies({}       , {dio_var, lumi}, {}        );
    }
    else {
      dio_var->set_dependencies({dio_unc}, {dio_beta});
      dio    ->set_dependencies({dio_var}, {lumi    });
    }
//-----------------------------------------------------------------------------
// RPC background
//-----------------------------------------------------------------------------
    rpc_unc  = new var_t("rpc_uncertainty", rpc_bkg*rpc_frac_unc);
    rpc_beta = new var_t("rpc_beta", 0., -10., 10.);
    rpc_var  = new var_t("rpc_variation", 0., -1.*rpc_bkg, 5-1*rpc_bkg);
    rpc      = new var_t("rpc_expectation", rpc_bkg, 0., 5.);
    
    fListOfVariables->Add(rpc_unc);
    fListOfVariables->Add(rpc_beta);
    fListOfVariables->Add(rpc_var);
    fListOfVariables->Add(rpc);

    if(useLogNormal_) {
      rpc_var->nom_ = 1.;
      rpc_var->val_ = 1.;
      rpc_var->set_dependencies({rpc_unc}, {}      , {rpc_beta});
      rpc->    set_dependencies({}       , {rpc_var, lumi});
    } else {
      rpc_var->set_dependencies({rpc_unc}, {rpc_beta});
      rpc->    set_dependencies({rpc_var}, {lumi});
    }

    rpc_oot_unc  = new var_t ("rpc_oot_uncertainty", rpc_oot_bkg*rpc_oot_frac_unc);
    rpc_oot_beta = new var_t ("rpc_oot_beta"       , 0., -10., 10.);
    rpc_oot_var  = new var_t ("rpc_oot_variation"  , 0., -1.*rpc_oot_bkg, 5-1.*rpc_oot_bkg);
    rpc_oot      = new var_t ("rpc_oot_expectation", rpc_oot_bkg, 0., 5.);
    
    fListOfVariables->Add(rpc_oot_unc);
    fListOfVariables->Add(rpc_oot_beta);
    fListOfVariables->Add(rpc_oot_var);
    fListOfVariables->Add(rpc_oot);

    if (useLogNormal_) {
      rpc_oot_var->nom_ = 1.;
      rpc_oot_var->val_ = 1.;
      rpc_oot_var->set_dependencies({rpc_oot_unc}, {}, {rpc_oot_beta});
      rpc_oot    ->set_dependencies({}, {rpc_oot_var, lumi});
    } else {
      rpc_oot_var->set_dependencies({rpc_oot_unc}, {rpc_oot_beta});
      rpc_oot    ->set_dependencies({rpc_oot_var}, {lumi});
    }
//-----------------------------------------------------------------------------
// PBAR background
//-----------------------------------------------------------------------------
    pbar_unc  = new var_t ("pbar_uncertainty", pbar_bkg*pbar_frac_unc);
    pbar_beta = new var_t ("pbar_beta", 0., -10., 10.);
    pbar_var  = new var_t ("pbar_variation", 0., -1.*pbar_bkg, 5.-1.*pbar_bkg);
    pbar      = new var_t ("pbar_expectation", pbar_bkg, 0., 0.1);
    
    fListOfVariables->Add(pbar_unc);
    fListOfVariables->Add(pbar_beta);
    fListOfVariables->Add(pbar_var);
    fListOfVariables->Add(pbar);

    if (useLogNormal_) {
      pbar_var->nom_ = 1.;
      pbar_var->val_ = 1.;
      pbar_var->set_dependencies({pbar_unc}, {}, {pbar_beta});
      pbar    ->set_dependencies({}, {pbar_var, lumi});
    }
    else {
      pbar_var->set_dependencies({pbar_unc}, {pbar_beta});
      pbar    ->set_dependencies({pbar_var}, {lumi});
    }
//-----------------------------------------------------------------------------
// cosmic background
// consider a flat light yield uncertainty, in arbitrary units
//-----------------------------------------------------------------------------
    cr_light_yd     = new var_t ("crv_light_yield", 0., 0., 1., var_t::kFlat);
    cr_light_coeff  = new var_t ("crv_light_yield_linear_coeff", 2.*(cr_hi_ly - cr_lo_ly));
    cr_light_offset = new var_t ("crv_light_yield_linear_offset", -1.*(cr_hi_ly - cr_lo_ly));
    cr_light_var    = new pow_t ("crv_variation_due_to_ly");  // variation of the yield, linear fit to measured points
    cr_light_bkg    = new var_t ("crv_bgr_due_to_ly", cr_lo_ly, cr_lo_ly, 10.*cr_hi_ly); // delta function at min, flat above

    fListOfVariables->Add(cr_light_yd);
    fListOfVariables->Add(cr_light_coeff);
    fListOfVariables->Add(cr_light_offset);
    fListOfVariables->Add(cr_light_var);
    fListOfVariables->Add(cr_light_bkg);

    cr_light_var->set_dependencies({one, cr_light_yd}, {cr_light_offset, cr_light_coeff}, {0, 1});
    cr_light_bkg->set_dependencies({cr_light_var}, {});

    cr_lo_unc  = new var_t ("crv_lo_uncertainty", cr_lo_frac_unc);
    cr_lo_beta = new var_t ("crv_lo_beta", 0., -10., 10.);
    cr_lo_var  = new var_t ("crv_lo_variation", 0., -1.*cr_lo_bkg, 5.);
    cr_lo      = new var_t ("crv_lo_expectation", 0., 0., 0.5);
    
    fListOfVariables->Add(cr_lo_unc);
    fListOfVariables->Add(cr_lo_beta);
    fListOfVariables->Add(cr_lo_var);
    fListOfVariables->Add(cr_lo);

    if (useLogNormal_) {
      cr_lo_var->nom_ = 1.;
      cr_lo_var->val_ = 1.;
      cr_lo_var->set_dependencies({cr_lo_unc}, {}, {cr_lo_beta});
      cr_lo    ->set_dependencies({cr_light_bkg}, {cr_lo_var, livetime_frac, trigger_eff});
    } else {
      cr_lo_var->set_dependencies({cr_lo_unc}, {cr_light_bkg, cr_lo_beta});
      cr_lo    ->set_dependencies({cr_lo_var}, {livetime_frac, trigger_eff});
    }
    if (!useCRLightYieldUnc_) {
      cr_light_bkg->set_val(cr_lo_bkg);
      cr_light_bkg->set_constant();
      cr_light_yd->set_constant();
    } else {
      cr_lo_beta->set_constant();
    }

    cr_hi_unc  = new var_t ("crv_hi_uncertainty", cr_hi_bkg*cr_hi_frac_unc);
    cr_hi_beta = new var_t ("crv_hi_beta"       , 0., -10., 10.);
    cr_hi_var  = new var_t ("crv_hi_variation"  , 0., -1.*cr_hi_bkg, 5.);
    cr_hi      = new var_t ("crv_hi_expectation", cr_hi_bkg, 0., 0.5);
    
    fListOfVariables->Add(cr_hi_unc);
    fListOfVariables->Add(cr_hi_beta);
    fListOfVariables->Add(cr_hi_var);
    fListOfVariables->Add(cr_hi);

    if (useLogNormal_) {
      cr_hi_var->nom_ = 1.;
      cr_hi_var->val_ = 1.;
      cr_hi_var->set_dependencies({cr_hi_unc}, {}        , {cr_hi_beta});
      cr_hi    ->set_dependencies({}         , {cr_hi_var, livetime_frac, trigger_eff});
    } else {
      cr_hi_var->set_dependencies({cr_hi_unc}, {cr_hi_beta});
      cr_hi    ->set_dependencies({cr_hi_var}, {livetime_frac, trigger_eff});
    }
//-----------------------------------------------------------------------------
// signal
//-----------------------------------------------------------------------------
    fSig_mu   = new var_t ("nominal_signal_strength", 0., 0., 10./fSignalAcceptance);
    fSig_eff  = new var_t ("signal_efficiency"      , fSignalAcceptance);
    fSig_unc  = new var_t ("signal_uncertainty"     , sig_frac_unc);
    fSig_beta = new var_t ("signal_beta"            , 0., -10., 10.);
    fSig_var  = new var_t ("signal_variation"       , 0., -1., 5.);
    fSignal   = new var_t ("signal_expectation"     , 0., 0., 20.);

    fListOfVariables->Add(fSig_mu );
    fListOfVariables->Add(fSig_eff);
    fListOfVariables->Add(fSig_unc);
    fListOfVariables->Add(fSig_beta);
    fListOfVariables->Add(fSig_var );
    fListOfVariables->Add(fSignal);

    if (useLogNormal_) {
      fSig_var->nom_ = 1.;
      fSig_var->val_ = 1.;
      fSig_var->set_dependencies({fSig_unc}, {}, {fSig_beta});
      fSignal ->set_dependencies ({fSig_mu }, {fSig_var, fSig_eff, lumi} );
    }
    else {
      fSig_var->set_dependencies({fSig_unc}, {fSig_beta, fSig_mu});
      fSignal ->set_dependencies({fSig_mu, fSig_var}, {fSig_eff, lumi});
    }
   
    if (fVerbose    > 0 ) print();
    if (!fDoConstraints ) set_constant();
//-----------------------------------------------------------------------------
// set verbose flag, print results of initialization
//-----------------------------------------------------------------------------
    int nvar = fListOfVariables->GetEntriesFast();
    for (int i=0; i<nvar; i++) {
      var_t* var = (var_t*) fListOfVariables->UncheckedAt(i);
      var->SetVerbose(fVerbose);
    }
    print();
  }

  //-----------------------------------------------------------------------------
  void Mu2e_model::InitModel() {
///////////////////////////////////////////////////////
// Initialize the model
///////////////////////////////////////////////////////
    fModel = new Poisson_t("Counting model",20,
			   // nobs,
			   {dio          , rpc     , rpc_oot     , pbar     , cr_lo     , cr_hi     , fSignal},
			   
			   {dio_beta     , rpc_beta, rpc_oot_beta, pbar_beta, cr_lo_beta, cr_hi_beta, fSig_beta,
			    deadtime_beta, intensity_cutoff_2b_beta, lumi_beta, cr_light_yd
			   }
			   );
//-----------------------------------------------------------------------------
// without constraints, it's just a Poisson PDF, no random shifting
//-----------------------------------------------------------------------------
    fModel->ngen_ = (fDoConstraints) ? scalePrecision_*1e5 : 1; 
    fModel->Print();
  }


//-----------------------------------------------------------------------------
  void Mu2e_model::GenerateNullPDF() {
    double nexp_bkg = dio_bkg + rpc_bkg + rpc_oot_bkg + pbar_bkg + cr_lo_bkg + cr_hi_bkg;
    std::cout << "Nominal background mean = " << nexp_bkg*fTrigEff << std::endl;
    std::cout << "Generating the NULL observable PDF\n";

    if (auto o = gDirectory->Get("h_null_pdf")) delete o;

    fHPdf = fModel->GeneratePDF();
    
    fHPdf->SetName("h_null_pdf");

    std::cout << "Poisson model has a nominal mean of: " << fModel->GetNominalMean()
	      << " and NULL PDF has a mean of: " << (fHPdf->GetMean()) << std::endl;

    nominalBkg_ = fModel->GetNominalMean();
    nullBkg_    = fHPdf->GetMean();

    TCanvas* c = new TCanvas("c_pdf","Probability distributions");

    fHPdf->SetLineColor(kRed);
    fHPdf->SetLineWidth(2);
    fHPdf->SetMarkerColor(kRed);
    fHPdf->SetMarkerStyle(20);
    fHPdf->SetMarkerSize(0.8);
    fHPdf->Draw();
    fHPdf->SetAxisRange(1.e-20, 10., "Y");
    fHPdf->SetAxisRange(0, 19.9, "X");
    c->SetLogy();

    if (comparePDF_) {
      std::cout << "Randomly sampling the NULL model PDF\n";
      const int nentries = 1e6;
      TH1D* hexp = new TH1D("hexp", "", 20, 0, 20);
      for(int i = 0; i < nentries; ++i) {
	if(i % (nentries/5) == 0) {
	  std::cout << "Samping " << i << ":\n";
	  fModel->SetVerbose(10);
	} else fModel->SetVerbose(0);
	int n = fModel->RandomSample();
	if(i % (nentries/10) == 0) {
	  fSignal->print();
	  dio->print();
	  dio_var->print();
	  std::cout << std::endl;
	}

	hexp->Fill(n);
      }
      hexp->Scale(1./nentries);
      hexp->Draw("hist sames");
      hexp->SetLineWidth(2);
    }
  }


//-----------------------------------------------------------------------------
// Mode = 1 : dont make an intermediate graph
//      = 2 : make a graph
//-----------------------------------------------------------------------------
  void Mu2e_model::run(int Mode) {

    GenerateNullPDF();

//-----------------------------------------------------------------------------
// Initialize Feldman-Cousins calculator for the model
//-----------------------------------------------------------------------------
    printf("Initializing Feldman-Cousins calculator\n");
  
    fFC       = new FCCalculator(fModel, fSig_mu, 0.90/*confidence level*/, 0/*verbosity*/);

    fFC->res_ = 2.e-4/scalePrecision_; // roughly control the resolution of the calculation, as a fraction of POI range
//-----------------------------------------------------------------------------
// get median discovery information
// Mode = 2: make probability graph
//-----------------------------------------------------------------------------
    double mu_min, mu_max, prob;
    int    nseen, ndisc;

    ndisc = fFC->NSigmaThreshold(fHPdf, 5.);
    std::cout << "N(discovery) for NULL model = " << ndisc <<std:: endl;
    
    if (Mode == 2) {
      float xg1[100], xg2[100], yg[100];
      int   ng(0);

      prob = 0.05;
      for (int i=0; prob < 0.99; i++) {
	double mu = fFC->FindForMedianN(ndisc,prob);
	printf("For a median of %i, prob:%6.2f minimum R_mue is: %.3e (%.3f mean events = mu * (nominal signal eff))\n",
	       ndisc, prob, mu*fSes, mu*fSig_eff->nom_*fTrigEff*scaleLuminosity_);

	float qev = mu*fSig_eff->nom_*fTrigEff*scaleLuminosity_;
	xg1[ng]   = qev;
	xg2[ng]   = mu*fSes;
	yg [ng]   = 1-prob;

	ng     += 1;
	prob   += 0.05;
      }

      gr_Disc  = new TGraph(ng,xg1,yg);
      gr_DiscR = new TGraph(ng,xg2,yg);

      prob = 0.16;
      double mu_disc1 = fFC->FindForMedianN(ndisc,prob);
      printf("For a median of %i, prob:%6.2f minimum R_mue is: %.3e (%.3f mean events = mu * (nominal signal eff))\n",
	     ndisc, prob, mu_disc1*fSes, mu_disc1*fSig_eff->nom_*fTrigEff*scaleLuminosity_);

      prob = 0.84;
      double mu_disc2 = fFC->FindForMedianN(ndisc,prob);
      printf("For a median of %i, prob:%6.2f minimum R_mue is: %.3e (%.3f mean events = mu * (nominal signal eff))\n",
	     ndisc, prob, mu_disc2*fSes, mu_disc2*fSig_eff->nom_*fTrigEff*scaleLuminosity_);
    }
//-----------------------------------------------------------------------------
// final step: get the median expected events for the null hypothesis PDF
//-----------------------------------------------------------------------------
    printf("[Mu2e_model::run]: fHPdf nbins :%4i\n",fHPdf->GetNbinsX());
    //    printf("[Mu2e_model::run]: 1. fFC->model_->obs_.max_ :%10.3f\n",fFC->model_->obs_.max_);

    nseen = fFC->GetMedian(fHPdf,0.5);
    
    // printf("[Mu2e_model::run]: 2. fFC->model_->obs_.max_ :%10.3f\n",fFC->model_->obs_.max_);

    std::cout << "Using a single event sensitivity of: " << fSes/fSignalAcceptance << std::endl;
    std::cout << "Performing Feldman-Cousins calculation for median nseen = " << nseen << std::endl;
    
    fFC->CalculateInterval(nseen, mu_min, mu_max);
    
    printf("For %i seen, R_mue interval is: %.3e - %.3e (%.3f - %.3f mean events = mu * (nominal signal eff))\n",
	   nseen,
	   mu_min*fSes,
	   mu_max*fSes,
	   mu_min*fSig_eff->nom_*scaleLuminosity_,
	   mu_max*fSig_eff->nom_*fTrigEff*scaleLuminosity_);
    
    upperLimit_ = mu_max*fSes;

    prob = 0.50;
    double mu_disc = fFC->FindForMedianN(ndisc,0.5);
    printf("For a median of %i, prob:%6.2f minimum R_mue is: %.3e (%.3f mean events = mu * (nominal signal eff))\n",
	   ndisc, prob, mu_disc*fSes, mu_disc*fSig_eff->nom_*fTrigEff*scaleLuminosity_);
//-----------------------------------------------------------------------------
// this needs fixing ! - change fSig_mu->val_ inside the model !
//-----------------------------------------------------------------------------
    discovery_    = mu_disc*fSes;
    fSig_mu->val_ = mu_max;
    hUL           = fModel->GeneratePDF();
    hUL->SetName("hUL");

    hUL->SetLineColor(kBlue);
    hUL->SetLineWidth(2);
    hUL->Draw("same");

    fSig_mu->val_ = mu_disc;
    hDisc         = fModel->GeneratePDF();
    hDisc->SetName("hDisc");

    hDisc->SetLineColor(kGreen+2);
    hDisc->SetLineWidth(2);
    hDisc->Draw("same");

    TLegend* leg = new TLegend();
    leg->AddEntry(fHPdf, "Null PDF");
    leg->AddEntry(hUL  , "Upper limit PDF");
    leg->AddEntry(hDisc, "Discovery PDF");
    leg->Draw();
//-----------------------------------------------------------------------------
// print "5 sigma" PRD in order to plot it in a different scale
//-----------------------------------------------------------------------------
    int nx = hDisc->GetNbinsX();

    hDiscR = new TH1D("hDiskR","5 sigma probability vs Rmue",nx,0,nx*fSes/fSig_eff->nom_*fTrigEff*scaleLuminosity_);
    hULR   = new TH1D("hULR"  ,"90% CL  probability vs Rmue",nx,0,nx*fSes/fSig_eff->nom_*fTrigEff*scaleLuminosity_);
    
    for (int i=1; i<=nx; i++) {
      double y_disc = hDisc->GetBinContent(i);
      hDiscR->SetBinContent(i,y_disc);
      hDiscR->SetBinError(i,0);
      double y_90cl = hUL->GetBinContent(i);
      hULR->SetBinContent(i,y_90cl);
      hULR->SetBinError(i,0);
      //      printf(" i,x,y_disc,y_90cl: %2i %10.3f %12.5e %12.5e\n",i,(i-1)*fSes,y_disc,y_90cl);
    }
  }

//-----------------------------------------------------------------------------
  void Mu2e_model::SaveHist(const char* Filename) {

    printf("Mu2e_model::SaveHist: saving histograms to %s\n",Filename);
    
    TFile* f = new TFile(Filename,"recreate");

    fHPdf->Write();

    hUL->Write();
    hULR->Write();

    hDisc->Write();
    hDiscR->Write();

    if (gr_Disc) {
      gr_Disc->Write();
      gr_DiscR->Write();
    }

    int nv = fListOfVariables->GetEntries();
    for (int i=0; i<nv; i++) {
      var_t* var = (var_t*) fListOfVariables->At(i);
      var->GetHist()->Write();
    }
    
    f->Close();
    delete f;
  }

//-----------------------------------------------------------------------------
  void Mu2e_model::print() {
    
    printf("Nominal values:\n");
    
    lumi->print();
    lumi_unc->print();
    lumi_var->print();
    dio->print();
    dio_unc->print();
    dio_var->print();
    rpc->print();
    rpc_unc->print();
    rpc_var->print();
    rpc_oot->print();
    rpc_oot_unc->print();
    rpc_oot_var->print();
    pbar->print();
    pbar_unc->print();
    pbar_var->print();
    cr_lo->print();
    cr_lo_unc->print();
    cr_lo_var->print();
    cr_hi->print();
    cr_hi_unc->print();
    cr_hi_var->print();
    fSig_eff->print();
    fSig_unc->print();
    fSig_var->print();
    fSignal->print();
    deadtime_unc->print();
    deadtime_var->print();
    deadtime->print();
    livetime_frac->print();
    intensity_cutoff_1b->print();
    intensity_cutoff_2b->print();
    intensity_frac_1b->print();
    intensity_frac_2b->print();
    intensity_frac->print();
    trigger_eff->print();
  }

//-----------------------------------------------------------------------------
  void Mu2e_model::set_constant() {

    printf("Setting systematics to 0!\n");
    
    lumi_beta->set_constant();
    dio_beta->set_constant();
    rpc_beta->set_constant();
    rpc_oot_beta->set_constant();
    pbar_beta->set_constant();
    cr_light_yd->set_constant();
    cr_lo_beta->set_constant();
    cr_hi_beta->set_constant();
    fSig_beta->set_constant();
    deadtime_beta->set_constant();
    intensity_cutoff_2b_beta->set_constant();
  }
  
//-----------------------------------------------------------------------------
  void Mu2e_model::Test001(int NEvents) {

    InitVariables();

    fModel = new Poisson_t("Test001",20,
     			   {pbar}, {pbar_beta, deadtime_beta, intensity_cutoff_2b_beta, lumi_beta} );

    fModel->SetVerbose(100);

    printf("Mu2e_model::Test001 start event loop with %6i events\n",NEvents);

    for (int i=0; i<NEvents; i++) {
      fModel->RandomSys();
      fModel->GetMean();
    }
    
  }
  
}

