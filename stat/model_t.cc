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
    fHistNullPDF      = new TH1D(Form("h_model_%s_null_pdf",name),"Null PDF"       ,50,0,50);
    fHistS0BPDF       = new TH1D(Form("h_model_%s_s0b_pdf"  ,name),"S+B  PDF"      ,50,0,50);
    fHistS1BPDF       = new TH1D(Form("h_model_%s_s1b_pdf"  ,name),"S(mean)+B  PDF",50,0,50);
    fNPExp            = 1000000;
  }


  model_t::~model_t() {
    delete fListOfChannels;
    delete fListOfParameters;
    delete fRng;
    delete fHistNullPDF;
    delete fHistS0BPDF;
    delete fHistS1BPDF;
  }

//-----------------------------------------------------------------------------
// get total expected fluctuated (!) Poisson mean (background only)
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
// get total expected Poisson mean (background, no fluctuations of the nuisance parameters)
//-----------------------------------------------------------------------------
  double model_t::GetBackgroundMean() {

    int nc = fListOfChannels->GetEntriesFast();

    double val = 0;
    
    for (int i=0; i<nc; i++) {
      channel_t* ch = GetChannel(i);
      if (ch->Signal() == 0) {
	val += ch->Process()->Mean();
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

  
  int model_t::GeneratePDF() {

    fMuB  = 0;
    fMuBx = 0;
    fMuS  = 0;
    fMuSx = 0;
    
    for (int i=0; i<fNPExp; i++) {
					// step 1: initalize all parameters
      InitParameters();
					// for now, val is the total expected background 
      double null_val = GetNullValue(); // this returns the fluctuated background mean
      double x_null   = fRng->Poisson(null_val);
      fHistNullPDF->Fill(x_null);

      fMuB  += null_val;
      fMuBx += x_null;
					// account for correlations

      channel_t* sig   = SignalChannel();

      double s0    = sig->fProcess->Mean(); // this is a fixed (not fluctuated) mean value. 
      double x_s0  = fRng->Poisson(s0);
      double x_s0b = x_null+x_s0;
      fHistS0BPDF->Fill(x_s0b);

      // in the calculation of the s1b value, sig and background are fluctuated in GetValue()
      // with parameters defining the signal also fluctuated
      double s1    = sig->GetValue();       // this is a fluctuated mean value
      double x_s1  = fRng->Poisson(s1);     // use that mean for sampling a Poisson distribution
      double x_s1b = x_null+x_s1;
      fHistS1BPDF->Fill(x_s1b);

      fMuS  += s1;
      fMuSx += x_s1;
    }

    fMuB  = fMuB /fNPExp;
    fMuBx = fMuBx/fNPExp;
    fMuS  = fMuS /fNPExp;
    fMuSx = fMuSx/fNPExp;

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

  int model_t::SaveHist(const char* Filename) {

    TFile* f = new TFile(Filename,"recreate");

    fHistNullPDF->Write();
    fHistS0BPDF->Write();
    fHistS1BPDF->Write();

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

  void model_t::Print(const Option_t* Opt) const {
    printf("fMuB  = %10.4f\n",fMuB );
    printf("fMuBx = %10.4f\n",fMuBx);
    printf("fMuS  = %10.4f\n",fMuS );
    printf("fMuSx = %10.4f\n",fMuSx);

    model_t* m = (model_t*) this;

    printf("<MuB> : %8.5f\n",m->GetBackgroundMean());
  }
}
