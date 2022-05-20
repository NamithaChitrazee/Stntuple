////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/pfixed_t.hh"
#include "Math/Random.h"

namespace stntuple {

//-----------------------------------------------------------------------------
  pfixed_t::pfixed_t(const char* Name, double Mean, int Debug) : parameter_t(Name,Debug) {
    int nbins(1000);
    
    fMean          = Mean ;

    char name[100], title[100];
    sprintf(name ,"h_par_%s",Name);
    sprintf(title,"%s: fixed parameter",Name);
    fHistPDF = new TH1D(Form("h_par_%s",Name),"fixed parameter",nbins,fMean*0.5,fMean*1.5);
  }

//-----------------------------------------------------------------------------
// initialize the parameter value just once per pseudoexperiment
//-----------------------------------------------------------------------------
  void pfixed_t::InitValue() {

    fValue = fMean;

    if (fDebug > 0) fHistPDF->Fill(fValue);
  }

  double pfixed_t::XMin() {
    // for histogramming
    return fMean;
  }

  double pfixed_t::XMax() {
    // for histogramming
    return fMean;
  }
//-----------------------------------------------------------------------------
  void pfixed_t::Print(const Option_t* Opt) const {
  }

}
