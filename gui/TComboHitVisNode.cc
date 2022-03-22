///////////////////////////////////////////////////////////////////////////////
// May 04 2013 P.Murat
// 
// in 'XY' mode draw calorimeter clusters as circles with different colors 
// in 'Cal' mode draw every detail...
///////////////////////////////////////////////////////////////////////////////
#include "TVirtualX.h"
#include "TPad.h"
#include "TStyle.h"
#include "TVector3.h"
#include "TLine.h"
#include "TArc.h"
#include "TArrow.h"
#include "TBox.h"
// #include "art/Framework/Principal/Event.h"
// #include "art/Framework/Principal/Handle.h"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/ConditionsService/inc/ConditionsHandle.hh"
#include "Offline/TrackerConditions/inc/StrawResponse.hh"

#include "Offline/RecoDataProducts/inc/StrawHitIndex.hh"

#include "Stntuple/gui/TComboHitVisNode.hh"
#include "Stntuple/gui/TEvdComboHit.hh"

#include "Stntuple/gui/TStnVisManager.hh"

ClassImp(TComboHitVisNode)

//-----------------------------------------------------------------------------
TComboHitVisNode::TComboHitVisNode() : TStnVisNode("TComboHitVisNode") {
  fListOfHits = new TObjArray();
  fListOfHits->SetOwner(1);

  fHitColl    = nullptr;
  fSdmcColl   = nullptr;
}

//_____________________________________________________________________________
TComboHitVisNode::TComboHitVisNode(const char* Name): TStnVisNode(Name) {
  fListOfHits = new TObjArray();
  fListOfHits->SetOwner(1);

  fHitColl    = nullptr;
  fSdmcColl   = nullptr;
}

//-----------------------------------------------------------------------------
TComboHitVisNode::~TComboHitVisNode() {
  
  delete fListOfHits;
}

//-----------------------------------------------------------------------------
int TComboHitVisNode::InitEvent() {

  fListOfHits->Delete();
//-----------------------------------------------------------------------------
// display hits corresponding to a given time peak, or all hits, 
// if the time peak is not found
//-----------------------------------------------------------------------------
  TStnVisManager* vm = TStnVisManager::Instance();

  const art::Event* event = vm->Event();

  const mu2e::ComboHit* hit0 = &(*fHitColl)->at(0);

  int nhits = (*fHitColl)->size();
  for (int ihit=0; ihit<nhits; ihit++ ) {
    const mu2e::ComboHit* hit = &(*fHitColl)->at(ihit);
    size_t ish  = hit-hit0;
    std::vector<StrawDigiIndex> shids;
    (*fHitColl)->fillStrawDigiIndices(*event,ish,shids);

    const mu2e::StrawDigiMC* mcdigi = &(*fSdmcColl)->at(shids[0]);
    const mu2e::StrawGasStep* step (nullptr);

    if (mcdigi->wireEndTime(mu2e::StrawEnd::cal) < mcdigi->wireEndTime(mu2e::StrawEnd::hv)) {
      step = mcdigi->strawGasStep(mu2e::StrawEnd::cal).get();
    }
    else {
      step = mcdigi->strawGasStep(mu2e::StrawEnd::hv ).get();
    }

    const art::Ptr<mu2e::SimParticle>& simptr = step->simParticle(); 
    const mu2e::SimParticle*           sim    = simptr.operator->();

    art::Ptr<mu2e::SimParticle>        momptr = simptr;

    while(momptr->hasParent()) momptr = momptr->parent();
    const mu2e::SimParticle* mother   = momptr.operator->();

    int mother_pdg_id = mother->pdgId();

    // if (simptr->fromGenerator()) generator_id = simptr->genParticle()->generatorId().id();
    // else                         generator_id = -1;

    float mc_mom      = step->momvec().mag();
    float mc_mom_z    = step->momvec().z();
//-----------------------------------------------------------------------------
// store TEvdComboHit
//-----------------------------------------------------------------------------
    fListOfHits->Add(new stntuple::TEvdComboHit(hit,sim,mother_pdg_id,mc_mom,mc_mom_z));
  }

  return 0;
}

//-----------------------------------------------------------------------------
// draw reconstructed tracks and straw hits
//-----------------------------------------------------------------------------
void TComboHitVisNode::PaintXY(Option_t* Option) {

  //  double        time;
  // int           station, ntrk(0);
  stntuple::TEvdComboHit  *hit;

  // const mu2e::ComboHit   *combo_hit;
  // const mu2e::Straw      *straw; 

  //  const char* view = TVisManager::Instance()->GetCurrentView();

  //  TStnVisManager* vm = TStnVisManager::Instance();

  //  int ipeak = vm->TimeCluster();

  // if (ipeak >= 0) {
  //   if ((*fTimeClusterColl) != NULL) {
  //     int ntp = (*fTimeClusterColl)->size();
  //     if (ipeak < ntp) fTimeCluster = &(*fTimeClusterColl)->at(ipeak);
  //     else             fTimeCluster = NULL;
  //   }
  // }

  // double tmin(0), tmax(2000.);

  // if (fTimeCluster) {
  //   tmin = fTimeCluster->t0().t0() - 30; // FIXME!
  //   tmax = fTimeCluster->t0().t0() + 20; // FIXME!
  // }
  
  int nhits = fListOfHits->GetEntries();
  for (int i=0; i<nhits; i++) {
    hit       = (stntuple::TEvdComboHit*) fListOfHits->At(i);
    //    time      = hit->time();

    // if ((station >= vm->MinStation()) && (station <= vm->MaxStation())) { 
    //   if ((time >= tmin) && (time <= tmax)) {
    hit->PaintXY(Option);
    //   }
    // }
  }

  gPad->Modified();
}

//-----------------------------------------------------------------------------
// draw reconstructed tracks and straw hits
//-----------------------------------------------------------------------------
void TComboHitVisNode::PaintTZ(Option_t* Option) {

  int nhits = fListOfHits->GetEntries();
  for (int i=0; i<nhits; i++) {
    stntuple::TEvdComboHit* hit = (stntuple::TEvdComboHit*) fListOfHits->At(i);

    // float time  = hit->Time();

    //    if ((time >= tmin) && (time <= tmax)) {
    hit->PaintTZ(Option);
    // }
  }

  gPad->Modified();
}

// //-----------------------------------------------------------------------------
// void TComboHitVisNode::PaintRZ(Option_t* Option) {
//   int                       ntrk(0), nhits;
//   stntuple::TEvdTrack*      evd_trk;

//   //  TStnVisManager* vm = TStnVisManager::Instance();

//   fTracker->PaintRZ(Option);
// //-----------------------------------------------------------------------------
// // do not draw all straw hits - just redraw straws in different color instead
// //-----------------------------------------------------------------------------
// //   int nhits = fListOfStrawHits->GetEntries();
// //   for (int i=0; i<nhits; i++) {
// //     hit       = (TEvdStrawHit*) fListOfStrawHits->At(i);

// //     if ((station >= vm->MinStation()) && (station <= vm->MaxStation())) continue;
// //     if ((time    <  vm->TMin()      ) || (time     > vm->TMax()      )) continue; 

// //     hit->Paint(Option);
// //   }
// //-----------------------------------------------------------------------------
// // display tracks and track hits
// //-----------------------------------------------------------------------------

//   gPad->Modified();
// }

//-----------------------------------------------------------------------------
int TComboHitVisNode::DistancetoPrimitiveXY(Int_t px, Int_t py) {
  static TVector3 global;
  global.SetXYZ(gPad->AbsPixeltoX(px),gPad->AbsPixeltoY(py),0);

  TObject* closest(nullptr);

  int  x1, y1, dx1, dy1, min_dist(9999), dist;

  int nhits = fListOfHits->GetEntries();
  for (int i=0; i<nhits; i++) {
    stntuple::TEvdComboHit* hit = (stntuple::TEvdComboHit*) fListOfHits->At(i);

    x1  = gPad->XtoAbsPixel(hit->Pos()->X());
    y1  = gPad->YtoAbsPixel(hit->Pos()->Y());
    dx1 = px-x1;
    dy1 = py-y1;

    dist  = (int) sqrt(dx1*dx1+dy1*dy1);
    if (dist < min_dist) {
      min_dist = dist;
      closest  = hit;
    }
  }

  SetClosestObject(closest,min_dist);

  return min_dist;
}

//-----------------------------------------------------------------------------
Int_t TComboHitVisNode::DistancetoPrimitiveTZ(Int_t px, Int_t py) {

  // static TVector3 global;
  // global.SetXYZ(gPad->AbsPixeltoX(px),gPad->AbsPixeltoY(py),0);

  TObject* closest(nullptr);

  int  x1, y1, dx1, dy1, min_dist(9999), dist;

  int nhits = fListOfHits->GetEntries();
  for (int i=0; i<nhits; i++) {
    stntuple::TEvdComboHit* hit = (stntuple::TEvdComboHit*) fListOfHits->At(i);
    x1  = gPad->XtoAbsPixel(hit->Z());
    y1  = gPad->YtoAbsPixel(hit->T());
    dx1 = px-x1;
    dy1 = py-y1;

    dist  = (int) sqrt(dx1*dx1+dy1*dy1);
    if (dist < min_dist) {
      min_dist = dist;
      closest  = hit;
    }
  }

  SetClosestObject(closest,min_dist);

  return min_dist;
}

// //-----------------------------------------------------------------------------
// void TComboHitVisNode::Print(Option_t* Option) const {
//   //TComboHitVisNode* node = (TComboHitVisNode*) this;

//   printf("TComboHitVisNode name: %s\n",GetName());
//   printf("nhits: %5i\n",fListOfHits->GetEntriesFast());
//   for (int i=0; i<18; i++) {
//     printf(" %3i",fHitData->fStation[i].fHits.GetEntries());
//   }
//   printf("\n");
// }

