///////////////////////////////////////////////////////////////////////////////
//  2014-01-26 P.Murat TStrWaveform
///////////////////////////////////////////////////////////////////////////////
#include "TString.h"

#include "Stntuple/obj/TStrWaveform.hh"

ClassImp(TStrWaveform)

//_____________________________________________________________________________
void TStrWaveform::Streamer(TBuffer &R__b) {

  if(R__b.IsReading()) {
    // Version_t R__v = R__b.ReadVersion();
    R__b.ReadVersion();
//-----------------------------------------------------------------------------
// curent version: V1
//-----------------------------------------------------------------------------
    int nw;
    R__b >> nw;
    if (fData == nullptr) {
      fNWords = nw;
      fData = new ushort[fNWords];
    }
    else if (nw != fNWords) {
      delete fData;
      fNWords = nw;
      fData = new ushort[fNWords];
    }
    R__b.ReadFastArray(fData,fNWords);
  }
  else {
    R__b.WriteVersion(TStrWaveform::IsA());
    R__b << fNWords;
    R__b.WriteFastArray(fData,fNWords);
  } 
}

//_____________________________________________________________________________
TStrWaveform::TStrWaveform(int ID): TObject() {
  
  SetUniqueID(ID);
  fNWords = 0;
  fData   = nullptr;
}

//_____________________________________________________________________________
TStrWaveform::~TStrWaveform() {
  if (fData) delete fData;
}

//_____________________________________________________________________________
// init
//-----------------------------------------------------------------------------
void TStrWaveform::Set(int NWords, const ushort* Data) {
  if (NWords != fNWords) {
    if (fData) delete fData;
    fNWords = NWords;
    fData   = new ushort[fNWords];
  }
  memcpy(fData,Data,fNWords*sizeof(ushort));
}

//_____________________________________________________________________________
void TStrWaveform::Clear(Option_t* opt) {
}

//-----------------------------------------------------------------------------
// Options: "banner", "data"
//-----------------------------------------------------------------------------
void TStrWaveform::Print(Option_t* Option) const {
  // print straw ADC waveform
  
  TString opt = Option;
  opt.ToLower();

  if ((opt == "") || (opt.Index("banner") >= 0)) {
    printf("------------------------------------------------------------------------------\n");
    printf(" nw  ----------------------------- samples -------------------------------------\n");
    printf("------------------------------------------------------------------------------\n");
  }

  if ((opt != "") and (opt.Index("data") < 0)) return;

  printf("%3i",fNWords);

  for (int i=0; i<fNWords; i++) {
    printf(" %5i",fData[i]);
  }
  
  printf("\n");
}
