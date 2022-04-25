////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/channel_t.hh"

namespace stntuple {

  channel_t::channel_t(const char* Name, int Debug): TNamed(Name,Name) {
    fProcess            = nullptr;
    fHistPDF        = nullptr;
    fDebug          = Debug;
    fSignal         = 0;
    fListOfPAdd    = new TObjArray();
  }

  channel_t::~channel_t() {
    // do not delete fProcess - it is owned by model list
    if (fHistPDF) delete fHistPDF;
    delete fListOfPAdd;
  }
//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//-----------------------------------------------------------------------------
  double channel_t::GetValue() {
    double val = fProcess->GetValue();

    if (fDebug > 0) fHistPDF->Fill(val);
    return val;
  }

  void channel_t::SetProcess(parameter_t* Process) {
    int nbins(1000);
    
    fProcess = Process;
    if (fDebug) {
      TString name = Form("h_channel_%s",GetName());
      fHistPDF     = new TH1D(name,Form("BGR channel %s",GetName()),nbins,fProcess->XMin(),fProcess->XMax());
    }
  }

  double channel_t::GetAddCorr() {
    double corr(1);
    
    int nadd = fListOfPAdd->GetEntriesFast();
    for (int i=0; i<nadd; i++) {
      parameter_t* p = (parameter_t*) fListOfPAdd->At(i);
      corr = corr + p->GetValue();
    }
    return corr;
  }
  


}
