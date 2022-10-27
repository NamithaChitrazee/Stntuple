///////////////////////////////////////////////////////////////////////////////
// 2022-09-25 P.Murat
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
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

// #include "art/Framework/Principal/Event.h"
// #include "art/Framework/Principal/Handle.h"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/ConditionsService/inc/ConditionsHandle.hh"
#include "Offline/TrackerConditions/inc/StrawResponse.hh"

#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/DataProducts/inc/StrawId.hh"

#include "Stntuple/print/TAnaDump.hh"

#include "Stntuple/gui/TEvdComboHit.hh"
#include "Stntuple/gui/TEvdTimeCluster.hh"
#include "Stntuple/gui/TEvdTimeClusterVisNode.hh"
#include "Stntuple/gui/TEvdStraw.hh"
#include "Stntuple/gui/TEvdStrawHit.hh"
#include "Stntuple/gui/TEvdTrkStrawHit.hh"
#include "Stntuple/gui/TEvdStation.hh"
#include "Stntuple/gui/TEvdPanel.hh"
#include "Stntuple/gui/TEvdPlane.hh"
#include "Stntuple/gui/TEvdStrawTracker.hh"
#include "Stntuple/gui/TEvdSimParticle.hh"
#include "Stntuple/gui/TStnVisManager.hh"

#include "Stntuple/obj/TStnTimeClusterBlock.hh"
#include "Stntuple/obj/TSimpBlock.hh"

ClassImp(TEvdTimeClusterVisNode)

using stntuple::TEvdTimeCluster;

//-----------------------------------------------------------------------------
TEvdTimeClusterVisNode::TEvdTimeClusterVisNode() : TStnVisNode("") {
}

//_____________________________________________________________________________
TEvdTimeClusterVisNode::TEvdTimeClusterVisNode(const char* name, TStnTimeClusterBlock* TcBlock): 
  TStnVisNode(name) {
  fTcBlock            = TcBlock;
	            
  fListOfTimeClusters = new TObjArray();
  fListOfTimeClusters->SetOwner(kTRUE);

  fTcColl             = nullptr;
  fTcColl             = nullptr;
}

//-----------------------------------------------------------------------------
TEvdTimeClusterVisNode::~TEvdTimeClusterVisNode() {
  delete fListOfTimeClusters;
}

//-----------------------------------------------------------------------------
int TEvdTimeClusterVisNode::InitEvent() {

  TStnVisManager* vm = TStnVisManager::Instance();
  const art::Event* event = vm->Event();

//-----------------------------------------------------------------------------
// pointers to collections of interest
//-----------------------------------------------------------------------------
  art::Handle<mu2e::TimeClusterCollection> tccH;
  event->getByLabel(art::InputTag(fTcCollTag), tccH);
  if (tccH.isValid()) fTcColl = tccH.product();
  else {
    mf::LogWarning("TEvdTimeClusterVisNode::InitEvent") << " WARNING:" << __LINE__ 
							<< " : mu2e::TimeClusterCollection " 
							<< fTcCollTag << " not found";
    fTcColl = nullptr;
  }

  art::Handle<mu2e::ComboHitCollection> chcH;
  event->getByLabel(art::InputTag(fChCollTag), chcH);
  if (chcH.isValid()) fChColl = chcH.product();
  else {
    mf::LogWarning("TEvdTimeClusterVisNode::InitEvent") << " WARNING:" << __LINE__ 
							<< " : mu2e::ComboHitCollection " 
							<< fChCollTag << " not found";
    fChColl = nullptr;
  }
//-----------------------------------------------------------------------------
// initialize helices
//-----------------------------------------------------------------------------
  stntuple::TEvdTimeCluster  *tcl;
  const mu2e::TimeCluster    *tc;  

  fListOfTimeClusters->Delete();
  int ntc = 0;

  if (fTcColl != nullptr) ntc = fTcColl->size();
  
  for (int i=0; i<ntc; i++) {
    tc    = &fTcColl->at(i);
//-----------------------------------------------------------------------------
// time clusters are made out of combo hits, define tmax, tmin
//-----------------------------------------------------------------------------
    int nch = tc->nhits();
    
    double tmin(1.e6), tmax(-1), zmin(1.e6), zmax(-1.);

    if (fChColl != nullptr) {
      for (int ih=0; ih<nch; ih++) {
	int ind = tc->hits().at(ih);
	const mu2e::ComboHit* hit = &fChColl->at(ind);
	float time = hit->correctedTime();
	if (time < tmin) tmin = time;
	if (time > tmax) tmax = time;

	float z = hit->pos().z();
	if (z < zmin) zmin = z;
	if (z > zmax) zmax = z;
      }
    }
    else {
      tmin = 0;
      tmax = 1700;
      zmin = -1600;
      zmax = 1600;
    }
      
    tcl   = new stntuple::TEvdTimeCluster(i,tc,tmin,tmax,zmin,zmax);

    fListOfTimeClusters->Add(tcl);
  }

  return 0;
}

//-----------------------------------------------------------------------------
// TZ view is only for the pattern recognition / time cluster finding
// display reconstructed tracks and combo hits 
//-----------------------------------------------------------------------------
void TEvdTimeClusterVisNode::PaintTZ(Option_t* Option) {

  int ncl = fListOfTimeClusters->GetEntries();
  for (int i=0; i<ncl; i++) {
    TEvdTimeCluster* tcl = (TEvdTimeCluster*) fListOfTimeClusters->At(i);
    tcl->PaintTZ(Option);
  }
}

//-----------------------------------------------------------------------------
// sets closest object among objects associated with the node 
// returns min_dist to the view
//-----------------------------------------------------------------------------
Int_t TEvdTimeClusterVisNode::DistancetoPrimitiveTZ(Int_t px, Int_t py) {
  TObject* closest(nullptr);
  int      min_dist(9999);

  int ntc = fListOfTimeClusters->GetEntriesFast();
  for (int i=0; i<ntc; i++) {
    TEvdTimeCluster* tcl = EvdTimeCluster(i);
    int dist = tcl->DistancetoPrimitiveTZ(px,py);
    if (dist < min_dist) {
      min_dist = dist;
      closest  = tcl;
    }
  }

  SetClosestObject(closest,min_dist);

  return min_dist;
}


//-----------------------------------------------------------------------------
void TEvdTimeClusterVisNode::Clear(Option_t* Opt) {
  printf(">>> name: %s TEvdTimeClusterVisNode::Clear is not implemented yet\n",GetName());
}

//-----------------------------------------------------------------------------
void TEvdTimeClusterVisNode::Print(Option_t* Opt) const {
  printf(" >>> name: %s TEvdTimeClusterVisNode::Print is not implemented yet\n",GetName());

  TAnaDump* ad = TAnaDump::Instance();
  ad->printTimeClusterCollection(fTcCollTag.data(),fChCollTag.data(),1,fSdmcCollTag.data());
}

