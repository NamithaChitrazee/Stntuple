////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/dio_channel_t.hh"

namespace stntuple {

  dio_channel_t::dio_channel_t(const char* Name, double Mean, double Sigma, int Debug) : channel_t(Name, Debug) {
    fRn             = new TRandom3();
    fMean           = Mean;
    fSigma          = Sigma;
    fLumi           = nullptr;
    if (Debug) {
      fHistPDF = new TH1D(Form("h_channel_%s",Name),"par",1000,0,fMean+10*Sigma);
    }
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//-----------------------------------------------------------------------------
  double dio_channel_t::GetValue() {

    fVal = fRn->Gaus(fMean,fSigma);

    // scale with fluctuated luminosity:

    if (fLumi) {
      fVal = fVal*fLumi->GetValue()/fLumi->Mean();
    }

    if (fDebug) {
      fHistPDF->Fill(fVal);
    }

    return fVal;
  }

}
