///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "obj/TStnTriggerTable.hh"
#include "obj/TStnTriggerBlock.hh"
// #include "obj/TStnMcCalibrations.hh"
#include "obj/TStnEvent.hh"
#include "obj/TStnNode.hh"
#include "obj/TStnHeaderBlock.hh"
#include "TMath.h"

ClassImp(TStnTriggerBlock)

//______________________________________________________________________________
void TStnTriggerBlock::Streamer(TBuffer &R__b)
{
   // Stream an object of class TStnTriggerBlock.
  
  int   nwi = (Short_t*)&fPaths-&fNPaths;
  
  if (R__b.IsReading()) {
    /* int ver = */ R__b.ReadVersion(); 
    R__b.ReadFastArray(&fNPaths,nwi);
    fPaths.Streamer(R__b);
  } 
  else {
    R__b.WriteVersion(TStnTriggerBlock::IsA());

					// write fixed part 
    R__b.WriteFastArray(&fNPaths,nwi);
    fPaths.Streamer(R__b);
  }
}

//_____________________________________________________________________________
TStnTriggerBlock::TStnTriggerBlock() {
//  for(int i=0; i<4; i++) Beam[i] = 0.0;
}

//_____________________________________________________________________________
TStnTriggerBlock::~TStnTriggerBlock() {
}


//_____________________________________________________________________________
Int_t TStnTriggerBlock::GetListOfPassedTriggers(const TStnTriggerTable* Table,
						const char*             Name, 
						Int_t                   Level,
						TObjArray*              List) 
{
  // assume that trigger table has been already initialized
  // if Name=0 include all passed triggers

  if (List == 0) return -1;

  List->Clear();

//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
  for (int i=0; i<fNPaths; i++) {
    const TStnTrigger* trig = Table->GetTrigger(i);
    if ((Name == 0) || (strstr(trig->GetName(),Name) != 0)) {
      List->Add((TObject*)trig);
    }
    else {
      Warning("GetListOfTriggers","doesn't exist!\n");
    }
  }

  return List->GetEntriesFast();
}



//_____________________________________________________________________________
int TStnTriggerBlock::NPassedPaths() {

  int nbits = fPaths.GetNBits();
  int n_passed(0);

  for (int i=0; i<nbits; i++) {
    n_passed += fPaths.GetBit(i);
  }
  return n_passed;
}

//_____________________________________________________________________________
void TStnTriggerBlock::Clear(Option_t* opt) {
  fNPaths   = -1;
  fNModSeq  = -1;
  fNModules = -1;
  fPaths.Clear();
}

//_____________________________________________________________________________
void TStnTriggerBlock::Print(Option_t* opt) const {
  // print contents of the trigger block

  printf(" TriggerBlock: exec : %5i trigger_table : %5i calib_jobset: %5i npaths : %3i", fExecVersion, fPhysTable, fCalibJobSet, fNPaths);
					// TSID
  printf("\n");

 // if (strstr(opt,"tl1d") != 0 || strstr(opt,"TL2D") != 0) {
 //   fTl1d.Print();
 // }

 // if (strstr(opt,"tl2d") == 0 || strstr(opt,"TL2D") != 0) {
 //   fTl2d.Print();
 // }
}
