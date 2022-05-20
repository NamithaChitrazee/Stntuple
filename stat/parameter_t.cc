////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/parameter_t.hh"

ClassImp(stntuple::parameter_t)

namespace stntuple {

  parameter_t::parameter_t(const char* name, int Debug) : TNamed(name,name) {
    fRng   = new ROOT::Math::RandomRanLux();
    fDebug = Debug;
    fFixed = 0;
  }

  parameter_t::~parameter_t() {
    delete fRng;
  }

  void parameter_t::InitValue() {
    fValue = fMean;
    if (fDebug > 0) fHistPDF->Fill(fValue);
  }

  double parameter_t::XMin() {
    // for histogramming
    return -1.;
  }

  double parameter_t::XMax() {
    // for histogramming
    return 1;
  }

  void parameter_t::Print(const Option_t* Opt) const {
  }

}
