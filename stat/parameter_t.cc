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
    fRn    = new TRandom3();
    fDebug = Debug;
  }

  parameter_t::~parameter_t() {
    delete fRn;
  }

  void parameter_t::InitValue() {
  }

  void parameter_t::Print(const Option_t* Opt) const {
  }

}
