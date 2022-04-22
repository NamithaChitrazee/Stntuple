////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/plogn_t.hh"
#include "Math/Random.h"

namespace stntuple {

//-----------------------------------------------------------------------------
  plogn_t::plogn_t(const char* Name, double Mean, double SigmaOverMean, int Debug) : parameter_t(Name,Debug) {
    int nbins(1000);
    
    fMean          = Mean ;
    fSigmaOverMean = SigmaOverMean;

    fSigma = sqrt(log(1+SigmaOverMean*SigmaOverMean));
    fMu    = log(Mean)-fSigma*fSigma/2;
    
    // double bin = (Mean+10*Sigma)/nbins;
    fHistPDF = new TH1D(Form("h_par_%s",Name),"lognormal parameter",nbins,0,fMean+10*fSigma);
  }

//-----------------------------------------------------------------------------
// initialize the parameter value just once per pseudoexperiment
//-----------------------------------------------------------------------------
  void plogn_t::InitValue() {

    if (fFixed == 0) fValue = fRng->LogNormal(fMu,fSigma);
    else             fValue = fMean;

    if (fDebug > 0) fHistPDF->Fill(fValue);
  }

  double plogn_t::XMin() {
    // for histogramming
    return 0;
  }

  double plogn_t::XMax() {
    // for histogramming
    return fMean + 10*fSigma;
  }
//-----------------------------------------------------------------------------
  void plogn_t::Print(const Option_t* Opt) const {
  }

}
