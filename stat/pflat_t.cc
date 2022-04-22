////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/pflat_t.hh"
namespace stntuple {

//-----------------------------------------------------------------------------
  pflat_t::pflat_t(const char* Name, double XMin, double XMax, int Debug) : parameter_t(Name,Debug) {
    int nbins(1000);
    
    fXMin = XMin;
    fXMax = XMax;

    fMean = (XMin+XMax)/2; 
    
    TString name = Form("h_par_%s",Name);
    fHistPDF = new TH1D(name,Form("parameter %s",Name),nbins,fXMin,fXMax);
  }

//-----------------------------------------------------------------------------
// initialize the parameter value just once per pseudoexperiment
//-----------------------------------------------------------------------------
  void pflat_t::InitValue() {

    if (fFixed == 0) fValue = fXMin+fRng->Rndm()*(fXMax-fXMin);
    else             fValue = fMean;

    if (fDebug > 0) fHistPDF->Fill(fValue);
  }

  double pflat_t::XMin() {
    // for histogramming
    return fXMin;
  }

  double pflat_t::XMax() {
    // for histogramming
    return fXMax;
  }
//-----------------------------------------------------------------------------
  void pflat_t::Print(const Option_t* Opt) const {
  }

}
