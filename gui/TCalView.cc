///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

#include "TObjArray.h"

#include "Stntuple/base/TVisNode.hh"
//#include "TGeant/TSubdetector.hh"

#include "Stntuple/gui/TCalView.hh"
#include "Stntuple/gui/TStnVisManager.hh"
#include "Stntuple/gui/TCalVisNode.hh"

ClassImp(TCalView)

//_____________________________________________________________________________
TCalView::TCalView(int Section): TStnView(TStnVisManager::kCal,Section,"CalView","CalView") {
}

//-----------------------------------------------------------------------------
TCalView::~TCalView() {
}

//_____________________________________________________________________________
void TCalView::SetMinClusterEnergy(float MinEnergy) {
  //

  TStnVisManager* vm = TStnVisManager::Instance();

  Int_t n = vm->GetNNodes();
  for (int i=0; i<n; i++) {
    TVisNode* node = vm->GetNode(i);

    if (node->InheritsFrom("TCalVisNode")) {
      TCalVisNode* cvn = (TCalVisNode*) node;
      cvn->SetMinClusterEnergy(MinEnergy);
      cvn->InitEvent();
    }
  }
}

//_____________________________________________________________________________
void TCalView::SetMinCrystalEnergy(float MinEnergy) {
  //

  TStnVisManager* vm = TStnVisManager::Instance();

  Int_t n = vm->GetNNodes();
  for (int i=0; i<n; i++) {
    TVisNode* node = vm->GetNode(i);

    if (node->InheritsFrom("TCalVisNode")) {
      TCalVisNode* cvn = (TCalVisNode*) node;
      cvn->SetMinCrystalEnergy(MinEnergy);
      cvn->InitEvent();
    }
  }
}


//_____________________________________________________________________________
void TCalView::PrintClosestCrystal() {
  //

  TStnVisManager* vm = TStnVisManager::Instance();

  TObject* clo = vm->GetClosestObject();

  if (clo && clo->InheritsFrom("TEvdCrystal")) {
    clo->Print();
  }
  else {
    printf("TCalView::PrintClosestCrystal ERROR: closest object is not a crystal\n");
  }

}
