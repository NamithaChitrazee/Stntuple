////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/ppoi_t.hh"
namespace stntuple {

//-----------------------------------------------------------------------------
  ppoi_t::ppoi_t(const char* Name, double Mean, double Sigma, int Debug) : parameter_t(Name,Debug) {
    int nbins(1000);
    
    fMean  = Mean ;
    fSigma = Sigma;			// doesn't make sense here
    
    // double bin = (Mean+10*Sigma)/nbins;
    fHistPDF = new TH1D(Form("h_par_%s",Name),"parameter",nbins,0,Mean+10*Sigma);
  }

//-----------------------------------------------------------------------------
// initialize the parameter value just once per pseudoexperiment
//-----------------------------------------------------------------------------
  void ppoi_t::InitValue() {

    if (fFixed == 0) {
      fValue = fRng->Poisson(fMean);
    }
    else             fValue = fMean;

    if (fDebug > 0) fHistPDF->Fill(fValue);
  }

  double ppoi_t::XMin() {
    // for histogramming
    return 0;
  }

  double ppoi_t::XMax() {
    // for histogramming
    return fMean + 10*fSigma;
  }
//-----------------------------------------------------------------------------
  void ppoi_t::Print(const Option_t* Opt) const {
  }

}
