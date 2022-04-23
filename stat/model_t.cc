////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "TFile.h"
#include "Stntuple/stat/model_t.hh"

namespace stntuple {

  model_t::model_t(const char* name) : TNamed(name,name) {

    fListOfChannels   = new TObjArray();
    fListOfChannels->SetOwner(kTRUE);

    fListOfParameters = new TObjArray();
    fListOfParameters->SetOwner(kTRUE);

    fRng              = new ROOT::Math::RandomRanLux();
    fHistPDF          = new TH1D(Form("h_model_%s",name),"PDF",50,0,50);
    fNPExp            = 1000000;
  }


  model_t::~model_t() {
    delete fListOfChannels;
    delete fListOfParameters;
    delete fRng;
    delete fHistPDF;
  }

  
  int model_t::GenerateNullPDF() {

    for (int i=0; i<fNPExp; i++) {
					// step 1: initalize all parameters
      InitParameters();
					// for now, val is the total expected background 
      double val = GetNullValue();
      double x   = fRng->Poisson(val);
      fHistPDF->Fill(x);
    }
    return 0;
  }


  int model_t::GeneratePDF() {

    for (int i=0; i<fNPExp; i++) {
					// step 1: initalize all parameters
      InitParameters();
					// for now, val is the total expected background 
      double val = GetValue();
      double x   = fRng->Poisson(val);
      fHistPDF->Fill(x);
    }
    return 0;
  }


  int model_t::InitParameters() {

    int np = fListOfParameters->GetEntriesFast();

    for (int i=0; i<np; i++) {
      parameter_t* p = GetParameter(i);
      p->InitValue();
    }
    return 0;
  }

//-----------------------------------------------------------------------------
// get total expected Poisson mean (background)
//-----------------------------------------------------------------------------
  double model_t::GetNullValue() {

    int nc = fListOfChannels->GetEntriesFast();

    double val = 0;
    
    for (int i=0; i<nc; i++) {
      channel_t* ch = GetChannel(i);
      if (ch->Signal() == 0) {
	val += ch->GetValue();
      }
    }
    return val;
  }

//-----------------------------------------------------------------------------
// get total expected Poisson mean, including signal
//-----------------------------------------------------------------------------
  double model_t::GetValue() {

    int nc = fListOfChannels->GetEntriesFast();

    double val = 0;
    
    for (int i=0; i<nc; i++) {
      channel_t* ch = GetChannel(i);
      val += ch->GetValue();
    }
    return val;
  }

  int model_t::SaveHist(const char* Filename) {

    TFile* f = new TFile(Filename,"recreate");

    fHistPDF->Write();

    // hUL->Write();
    // hULR->Write();

    // hDisc->Write();
    // hDiscR->Write();

    // if (gr_Disc) {
    //   gr_Disc->Write();
    //   gr_DiscR->Write();
    // }

    int np = fListOfParameters->GetEntries();
    printf("model_t::%s: np = %2i\n",__func__,np);
    for (int i=0; i<np; i++) {
      parameter_t* p = (parameter_t*) fListOfParameters->At(i);
      p->GetHistPDF()->Write();
    }

    int nch = fListOfChannels->GetEntries();
    printf("model_t::%s: nch = %2i\n",__func__,nch);
    for (int i=0; i<nch; i++) {
      channel_t* ch = (channel_t*) fListOfChannels->At(i);
      ch->GetHistPDF()->Write();
    }
    
    f->Close();
    delete f;
    return 0;
  }
}
