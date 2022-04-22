//-----------------------------------------------------------------------------
// a model : several background channels, in the end - generate PDF
// so far, a simple counting model ?
// convention: signal is 
//-----------------------------------------------------------------------------
#ifndef __stntuple_stat_model_t_hh__
#define __stntuple_stat_model_t_hh__

#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"

#include "Math/GSLRandom.h"
#include "Math/Random.h"

#include "Stntuple/stat/parameter_t.hh"
#include "Stntuple/stat/channel_t.hh"

namespace stntuple {
  class  model_t : public TNamed {
  public:
    TObjArray*                fListOfParameters;			// should be sufficient
    TObjArray*                fListOfChannels;			// should be sufficient

    int                       fNPseudoExperiments;

    ROOT::Math::RandomRanLux* fRng;
    TH1D*                     fHistPDF;
    int                       fNPExp;             // N(pseudoexperiments) to run to generate PDF
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    model_t(const char* Name);
    ~model_t();
    
    double  GetValue();

    void    AddChannel  (channel_t* Channel) { fListOfChannels->Add(Channel); }

    void    AddParameter(parameter_t* Par  ) { fListOfParameters->Add(Par); }
    
    // set parameter values for the next event (pseudoexperiment)
    int     InitParameters();
    
    int     GeneratePDF();

    int     SaveHist(const char* Filename);

    parameter_t* GetParameter(const char* Name) {
      return (parameter_t*) fListOfParameters->FindObject(Name);
    }

    parameter_t* GetParameter(int I) { return (parameter_t*) fListOfParameters->At(I); }

    channel_t* GetChannel(int I) { return (channel_t*) fListOfChannels->At(I); }
    
};
}
#endif
