////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/channel_t.hh"

namespace stntuple {

  channel_t::channel_t(const char* Name, int Debug): TNamed(Name,Name) {
    fBgr            = nullptr;
    fHistPDF        = nullptr;
    fDebug          = Debug;
    fSignal         = 0;
  }

  channel_t::~channel_t() {
    // do not delete fBgr - it is owned by model list
    delete fHistPDF;
  }
//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//-----------------------------------------------------------------------------
  double channel_t::GetValue() {
    double val = fBgr->GetValue();

    if (fDebug > 0) fHistPDF->Fill(val);
    return val;
  }

  void channel_t::SetBgr(parameter_t* Bgr) {
    int nbins(1000);
    
    fBgr = Bgr;
    if (fDebug) {
      TString name = Form("h_channel_%s",GetName());
      fHistPDF     = new TH1D(name,Form("BGR channel %s",GetName()),nbins,fBgr->XMin(),fBgr->XMax());
    }
  }
  


}
