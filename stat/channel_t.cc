////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/channel_t.hh"

namespace stntuple {

  channel_t::channel_t(const char* Name, int Debug): TNamed(Name,Name) {
    fHistPDF        = nullptr;
    fDebug          = Debug;
    fRn             = new TRandom3();
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//-----------------------------------------------------------------------------
  double channel_t::GetValue() {
    return -1; //fVal;
  }

}
