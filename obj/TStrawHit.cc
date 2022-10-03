///////////////////////////////////////////////////////////////////////////////
//  2014-01-26 P.Murat TStrawHit
///////////////////////////////////////////////////////////////////////////////
#include "TString.h"

#include "Stntuple/obj/TStrawHit.hh"

ClassImp(TStrawHit)

//-----------------------------------------------------------------------------
// V1: doesn't store the hit ID
//-----------------------------------------------------------------------------
void TStrawHit::ReadV1(TBuffer &R__b) {
  int nw_data = &fSimID-&fStrawID;

  R__b.ReadFastArray(&fStrawID,nw_data);
  R__b >> fEDep;
  if (MCFlag()) {
    int nwi_mc  = ((int*) &fEDep) - &fSimID;
    R__b.ReadFastArray(&fSimID ,nwi_mc);
    R__b >> fMcMom;
  }
  else {
    fGenID       = -1;
    fSimID       = -1;
    fPdgID       = -1;
    fMotherPdgID = -1;
    fMcMom       = -1.;
  }
}



//_____________________________________________________________________________
void TStrawHit::Streamer(TBuffer &R__b) {

  int nw_data = &fSimID-&fStrawID;
					// those are only in MC

  int nwi_mc  = ((int*) &fEDep) - &fSimID;
  
  if(R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion();
    if (R__v == 1) ReadV1(R__b);
    else if (R__v == 2) {
//-----------------------------------------------------------------------------
// curent version: V2 - read the hit ID
//-----------------------------------------------------------------------------
      TObject::Streamer(R__b);
      R__b.ReadFastArray(&fStrawID,nw_data);
      R__b >> fEDep;
      if (MCFlag()) {
	R__b.ReadFastArray(&fSimID ,nwi_mc);
	R__b >> fMcMom;
      }
      else {
	fGenID       = -1;
	fSimID       = -1;
	fPdgID       = -1;
	fMotherPdgID = -1;
	fMcMom       = -1.;
      }
    }
  }
  else {
    R__b.WriteVersion(TStrawHit::IsA());
    TObject::Streamer(R__b);
    
    R__b.WriteFastArray(&fStrawID,nw_data);
    R__b << fEDep;
    if (MCFlag()) {
      R__b.WriteFastArray(&fGenID ,nwi_mc);
      R__b << fMcMom;
    }
  } 
}

//-----------------------------------------------------------------------------
TStrawHit::TStrawHit(int ID): TObject() {
  SetUniqueID(ID);
  Clear();
}

//_____________________________________________________________________________
TStrawHit::~TStrawHit() {
}

//_____________________________________________________________________________
// straw ID has a MC flag hidden in it
//-----------------------------------------------------------------------------
void TStrawHit::Set(int StrawID, int* Time, int TOT, 
		    int GenID, int SimID, int PdgID, int MotherPdgID, 
		    float EDep, float McMom) 
{
  fStrawID     = StrawID; 
  fTime[0]     = Time[0]; 
  fTime[1]     = Time[1]; 
  fTOT         = TOT;
  fGenID       = GenID;
  fSimID       = SimID;
  fPdgID       = PdgID;
  fMotherPdgID = MotherPdgID;
  fEDep        = EDep;
  fMcMom       = McMom;
  
}

//_____________________________________________________________________________
void TStrawHit::Clear(Option_t* opt) {
  fStrawID     = 0xffffffff;
  fTime[0]     = 0xffffffff;
  fTime[1]     = 0xffffffff;
  fTOT         = 0xffffffff;
  fGenID       = -1;
  fSimID       = -1;
  fPdgID       = -1;
  fMotherPdgID = -1;
  fEDep        = -1.;
  fMcMom       = -1.;
}

//-----------------------------------------------------------------------------
// Options: "banner", "data"
//-----------------------------------------------------------------------------
void TStrawHit::Print(Option_t* Option) const {
  // print Straw hit properties
  //  printf("Superlayer: %d, Wire: %d, Cell: %d,\n",fSuperLayer,fWire,fCell);
  
  TString opt = Option;
  opt.ToLower();

  if ((opt == "") || (opt.Index("banner") >= 0)) {
    printf("----------------------------------------------------------------------------------------\n");
    printf("MC Index      Time          TOT       EDep  Flag GenID SimID      PdgID    PdgID_M      McMom\n");
    printf("----------------------------------------------------------------------------------------\n");
  }

  if (opt.Index("data") < 0) return;
  
  printf("%2i %5i %6i %6i %5i %5i %8.5f 0x%02x %5i %5i %10i %10i %10.3f\n",
	 MCFlag(), 
	 StrawID(),  
	 fTime[0], fTime[1], // ## format
	 TOT(0)  , TOT(1)  ,
	 fEDep,
	 StrawDigiFlag(),
	 GenID(),
	 fSimID,
	 fPdgID,
	 fMotherPdgID,
	 fMcMom);
  
}
