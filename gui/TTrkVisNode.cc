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
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

// #include "art/Framework/Principal/Event.h"
// #include "art/Framework/Principal/Handle.h"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"
#include "Offline/ConditionsService/inc/ConditionsHandle.hh"
#include "Offline/TrackerConditions/inc/StrawResponse.hh"

#include "Stntuple/gui/TEvdComboHit.hh"
#include "Stntuple/gui/TEvdTrack.hh"
#include "Stntuple/gui/TTrkVisNode.hh"
#include "Stntuple/gui/TEvdStraw.hh"
#include "Stntuple/gui/TEvdStrawHit.hh"
#include "Stntuple/gui/TEvdTrkStrawHit.hh"
#include "Stntuple/gui/TEvdStation.hh"
#include "Stntuple/gui/TEvdPanel.hh"
#include "Stntuple/gui/TEvdPlane.hh"
#include "Stntuple/gui/TEvdStrawTracker.hh"
#include "Stntuple/gui/TEvdSimParticle.hh"
#include "Stntuple/gui/TStnVisManager.hh"

#include "Stntuple/obj/TSimpBlock.hh"

#include "Offline/RecoDataProducts/inc/StrawHit.hh"

#include "Offline/DataProducts/inc/StrawId.hh"
// #include "DataProducts/inc/XYZVec.hh"

// #include "Stntuple/mod/TAnaDump.hh"

ClassImp(TTrkVisNode)

//-----------------------------------------------------------------------------
TTrkVisNode::TTrkVisNode() : TStnVisNode("") {
}

//_____________________________________________________________________________
TTrkVisNode::TTrkVisNode(const char* name, const mu2e::Tracker* Tracker, TStnTrackBlock* TrackBlock): 
  TStnVisNode(name) {
  fTracker    = new stntuple::TEvdStrawTracker(Tracker);
  fTrackBlock = TrackBlock;

  fArc        = new TArc;
  fEventTime  = 0;
  fTimeWindow = 1.e6;

  fListOfStrawHits    = new TObjArray();
  fListOfComboHits    = new TObjArray();
  fTimeCluster        = NULL;
  fUseStereoHits      = 0;

  fListOfTracks       = new TObjArray();
  fListOfSimParticles = new TObjArray();

  fComboHitColl       = nullptr;
  fStrawHitColl       = nullptr;
  fTimeClusterColl    = nullptr;
  fStrawDigiMCColl    = nullptr;
  fKalRepPtrColl      = nullptr;
  fSimpColl           = nullptr;
  fSpmcColl           = nullptr;
					// owned externally (by MuHitDisplay)
  fSimpBlock          = new TSimpBlock();
}

//-----------------------------------------------------------------------------
TTrkVisNode::~TTrkVisNode() {
  delete fArc;
  
  delete fListOfStrawHits;
  delete fListOfComboHits;

  fListOfTracks->Delete();
  delete fListOfTracks;

  fListOfSimParticles->Delete();
  delete fListOfSimParticles;
}

//-----------------------------------------------------------------------------
int TTrkVisNode::InitEvent() {

  //  const char* oname = "TTrkVisNode::InitEvent";

  mu2e::GeomHandle<mu2e::Tracker> ttHandle;
  const mu2e::Tracker* tracker = ttHandle.get();

  TStnVisManager* vm = TStnVisManager::Instance();

  const art::Event* event = vm->Event();

  // Tracker calibration object.
  //  mu2e::ConditionsHandle<mu2e::StrawResponse> srep = mu2e::ConditionsHandle<mu2e::StrawResponse>("ignored");

  const mu2e::ComboHit              *hit;
  const mu2e::StrawDigiMC           *hit_digi_mc;

  stntuple::TEvdStrawHit                      *evd_straw_hit; 
  const CLHEP::Hep3Vector           /**mid,*/ *w; 
  const mu2e::Straw                 *straw; 

  int                               n_straw_hits, color, nl, ns; // , ipeak, ihit;
  bool                              isFromConversion, intime;
  double                            sigw, /*vnorm, v,*/ sigr; 
  CLHEP::Hep3Vector                 vx0, vx1, vx2;
//-----------------------------------------------------------------------------
// first, clear the cached hit information from the previous event
//-----------------------------------------------------------------------------
  stntuple::TEvdStation*   station;
  stntuple::TEvdPlane*     plane;
  stntuple::TEvdPanel*     panel;

  int            nst, nplanes, npanels/*, isec*/; 

  nst = mu2e::StrawId::_nstations; // tracker->nStations();
  for (int ist=0; ist<nst; ist++) {
    station = fTracker->Station(ist);
    nplanes = station->NPlanes();
    for (int iplane=0; iplane<nplanes; iplane++) {
      plane = station->Plane(iplane);
      npanels = plane->NPanels();
      for (int ipanel=0; ipanel<npanels; ipanel++) {
	panel = plane->Panel(ipanel);
	nl    = panel->NLayers();
	for (int il=0; il<nl; il++) {
	  ns = panel->NStraws(il);
	  for (int is=0; is<ns; is++) {
	    panel->Straw(il,is)->Clear();
	  }
	}
      }
    }
  }
//-----------------------------------------------------------------------------
// display hits corresponding to a given time peak, or all hits, 
// if the time peak is not found
//-----------------------------------------------------------------------------
  fListOfStrawHits->Delete();

  stntuple::TEvdStraw* evd_straw;
  n_straw_hits = (*fStrawHitColl)->size();

  for (int ihit=0; ihit<n_straw_hits; ihit++ ) {

    hit         = &(*fStrawHitColl)->at(ihit);

    if ((*fStrawDigiMCColl)->size() > 0) hit_digi_mc = &(*fStrawDigiMCColl)->at(ihit);
    else                                 hit_digi_mc = NULL; // normally, should not be happening, but it does

    straw       = &tracker->getStraw(hit->strawId());
//-----------------------------------------------------------------------------
// deal with MC information - later
//-----------------------------------------------------------------------------
    const mu2e::StrawDigiMC             *mcdigi(0);
    if ((*fStrawDigiMCColl)->size() > 0) mcdigi = &(*fStrawDigiMCColl)->at(ihit); // this seems to be wrong
    // Get the straw information:

    w     = &straw->getDirection();

    isFromConversion = false;

    // printf("TTrkVisNode::InitEvent: no mcdigi->stepPointMC any more\n");

    const mu2e::StrawGasStep* step = mcdigi->earlyStrawGasStep().get();

    const mu2e::SimParticle* sim   = &(*step->simParticle());
    
    if ( sim->fromGenerator() ){
      mu2e::GenParticle* gen = (mu2e::GenParticle*) &(*sim->genParticle());
      //	    if ( gen->generatorId() == mu2e::GenId::conversionGun ){
      if ( gen->generatorId() == mu2e::GenId::StoppedParticleReactionGun ){
     	isFromConversion = true;
      }
    }
    int   pdg_id = sim->pdgId();
    float mc_mom = step->momvec().mag();
//-----------------------------------------------------------------------------
// old default, draw semi-random errors
//-----------------------------------------------------------------------------
    sigw  = hit->wireRes()/2.;      // P.Murat
    sigr  = 2.5;                    // in mm
	
    intime = fabs(hit->time()-fEventTime) < fTimeWindow;
	
    if      (pdg_id == 11) {
      if    (mc_mom > 20  ) { 
	if (intime) color = kRed;
	else        color = kBlue;
      }
      else                   { color = kRed+2;  }
    }
    else if (pdg_id ==  -11) { color = kBlue;   } 
    else if (pdg_id ==   13) { color = kGreen+2;} 
    else if (pdg_id ==  -13) { color = kGreen-2;} 
    else if (pdg_id == 2212) { color = kBlue+2; } 
    else                     { color = kBlack;  } 
//-----------------------------------------------------------------------------
// add a pointer to the hit to the straw 
//-----------------------------------------------------------------------------
    int mask = 0;
    if (intime          ) mask |= stntuple::TEvdStrawHit::kInTimeBit;
    if (isFromConversion) mask |= stntuple::TEvdStrawHit::kConversionBit;
    
    int ist, ipl, ippl, /*ifc,*/ ipn, il, is;

    ipl  = straw->id().getPlane();      // plane number here runs from 0 to 2*NStations-1
    ist  = straw->id().getStation();
    ippl = ipl % 2 ;                    // plane number within the station
    ipn  = straw->id().getPanel();
    il   = straw->id().getLayer();
    is   = straw->id().getStraw();

    evd_straw     = fTracker->Station(ist)->Plane(ippl)->Panel(ipn)->Straw(il,is/2);
    evd_straw_hit = new stntuple::TEvdStrawHit(hit,
					       evd_straw,
					       hit_digi_mc,
					       hit->pos().x(),
					       hit->pos().y(),
					       hit->pos().z(),
					       w->x(),w->y(),
					       sigw,sigr,
					       mask,color);
    evd_straw->AddHit(evd_straw_hit);
//-----------------------------------------------------------------------------
// so far, lists of straw hits and combo hits are the same, need to make them different
//-----------------------------------------------------------------------------
    fListOfStrawHits->Add(evd_straw_hit);
  }
//-----------------------------------------------------------------------------
// combo hits
//-----------------------------------------------------------------------------
  fListOfComboHits->Delete();
  int nch = (*fComboHitColl)->size();
//-----------------------------------------------------------------------------
// the rest makes sense only if nhits > 0
//-----------------------------------------------------------------------------
  if (nch > 0) { 
    const mu2e::ComboHit* hit0 = &(*fComboHitColl)->at(0);

    for (int ihit=0; ihit<nch; ihit++ ) {
      const mu2e::ComboHit* hit = &(*fComboHitColl)->at(ihit);
      size_t ish  = hit-hit0;
      std::vector<StrawDigiIndex> shids;
      (*fComboHitColl)->fillStrawDigiIndices(*event,ish,shids);

      const mu2e::StrawDigiMC* mcdigi = &(*fStrawDigiMCColl)->at(shids[0]);
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
      fListOfComboHits->Add(new stntuple::TEvdComboHit(hit,sim,step,mother_pdg_id,mc_mom,mc_mom_z));
    }
  }
//-----------------------------------------------------------------------------
// hit MC truth unformation from StepPointMC's
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// now initialize tracks
//-----------------------------------------------------------------------------
  stntuple::TEvdTrack      *trk;
  const KalRep             *krep;  
  const mu2e::TrkStrawHit  *track_hit;

  fListOfTracks->Delete();
  int ntrk = 0;

  if ((*fKalRepPtrColl) != 0) ntrk = (*fKalRepPtrColl)->size();
  
  for (int i=0; i<ntrk; i++) {
    krep = (*fKalRepPtrColl)->at(i).get();
    trk  = new stntuple::TEvdTrack(i,krep);
//-----------------------------------------------------------------------------
// add hits, skip calorimeter clusters (TrkCaloHit's)
//-----------------------------------------------------------------------------
    const TrkHitVector* hits = &krep->hitVector();
    for (auto it=hits->begin(); it!=hits->end(); it++) {
      track_hit = dynamic_cast<mu2e::TrkStrawHit*> (*it);
      if (track_hit == nullptr) continue;
      stntuple::TEvdTrkStrawHit* h = new stntuple::TEvdTrkStrawHit(track_hit);
      trk->AddHit(h);
    }

    fListOfTracks->Add(trk);
  }
//-----------------------------------------------------------------------------
// initialize SimParticles
//-----------------------------------------------------------------------------
  fListOfSimParticles->Delete();

  stntuple::TEvdSimParticle *esim;

  TDatabasePDG* pdb = TDatabasePDG::Instance();

  int np = fSimpBlock->NParticles();
  int ipp = 0;
  for (int i=0; i<np; i++) {
    TSimParticle* tsimp = fSimpBlock->Particle(i);

    TParticlePDG* p_pdg = pdb->GetParticle(tsimp->PDGCode());

    if (p_pdg == nullptr) {
      printf("Unknown SimParticle with code %i, SKIP\n",tsimp->PDGCode());
                                                            continue;
    }

    if (tsimp->NStrawHits() > 0) {
//-----------------------------------------------------------------------------
// taking the first and the last StrawGasStep's in hope that they are ordered in Z
//-----------------------------------------------------------------------------
      int first = tsimp->Shid()->front();
      int last  = tsimp->Shid()->back ();

      const mu2e::StrawGasStep* s1 = (*fStrawDigiMCColl)->at(first).earlyStrawGasStep().get();
      const mu2e::StrawGasStep* s2 = (*fStrawDigiMCColl)->at(last ).earlyStrawGasStep().get();

      esim = new stntuple::TEvdSimParticle(ipp,tsimp,s1,s2);
      fListOfSimParticles->Add(esim);
      ipp++;
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
// draw reconstructed tracks and STRAW hits, may want to display COMBO hits instead
//-----------------------------------------------------------------------------
void TTrkVisNode::PaintXY(Option_t* Option) {

  double                  time;
  int                     station, ntrk(0);

  const mu2e::Straw      *straw; 

  //  int view_type = TVisManager::Instance()->GetCurrentView()->Type();

  mu2e::GeomHandle<mu2e::Tracker> ttHandle;
  const mu2e::Tracker* tracker = ttHandle.get();

  TStnVisManager* vm = TStnVisManager::Instance();

  int ipeak = vm->TimeCluster();

  if (ipeak >= 0) {
    if ((*fTimeClusterColl) != NULL) {
      int ntp = (*fTimeClusterColl)->size();
      if (ipeak < ntp) fTimeCluster = &(*fTimeClusterColl)->at(ipeak);
      else             fTimeCluster = NULL;
    }
  }

  double tmin(0), tmax(2000.);

  if (fTimeCluster) {
    tmin = fTimeCluster->t0().t0() - 30;//FIXME!
    tmax = fTimeCluster->t0().t0() + 20;//FIXME!
  }
  
  if (vm->DisplayStrawHitsXY()) {
//-----------------------------------------------------------------------------
// display straw hits
//-----------------------------------------------------------------------------
    int nhits = fListOfStrawHits->GetEntries();
    for (int i=0; i<nhits; i++) {
      stntuple::TEvdStrawHit* evd_sh = GetEvdStrawHit(i);
      const mu2e::ComboHit*   sh     = evd_sh->StrawHit();
      straw     = &tracker->getStraw(sh->strawId());//strawIndex());
      station   = straw->id().getStation();
      time      = sh->time();

      if ((station >= vm->MinStation()) && (station <= vm->MaxStation())) { 
	if ((time >= tmin) && (time <= tmax)) {
	  evd_sh->PaintXY(Option);
	}
      }
    }
  }
  else {
//-----------------------------------------------------------------------------
// display combo hits
//-----------------------------------------------------------------------------
    int nch = fListOfComboHits->GetEntries();
    for (int i=0; i<nch; i++) {
      stntuple::TEvdComboHit* evd_ch = (stntuple::TEvdComboHit*) fListOfComboHits->At(i);
      const mu2e::ComboHit*   ch     = evd_ch->ComboHit();
      int index = ch->index(0);
      const mu2e::ComboHit* sh = &(*fStrawHitColl)->at(index);

      straw     = &tracker->getStraw(sh->strawId());           // first straw hit
      station   = straw->id().getStation();
      time      = ch->time();

      if ((station >= vm->MinStation()) && (station <= vm->MaxStation())) { 
	if ((time >= tmin) && (time <= tmax)) {
	  evd_ch->PaintXY(Option);
	}
      }
    }
  }
//-----------------------------------------------------------------------------
// now - tracks
//-----------------------------------------------------------------------------
  stntuple::TEvdTrack* evd_trk;
  //  TAnaDump::Instance()->printKalRep(0,"banner");

  if ( (fListOfTracks) != 0 )  ntrk = fListOfTracks->GetEntriesFast();

  for (int i=0; i<ntrk; i++ ) {
    evd_trk = GetEvdTrack(i);
    evd_trk->Paint(Option);
  }

//-----------------------------------------------------------------------------
// SimParticle's
//-----------------------------------------------------------------------------
  stntuple::TEvdSimParticle* esp;

  int nsim = fListOfSimParticles->GetEntriesFast();

  for (int i=0; i<nsim; i++ ) {
    esp = GetEvdSimParticle(i);
    esp->Paint(Option);
  }
//-----------------------------------------------------------------------------
// seedfit, if requested - not implemented yet
//-----------------------------------------------------------------------------
//   TAnaDump::Instance()->printKalRep(0,"banner");

//   ntrk = fListOfTracks->GetEntriesFast();
//   for (int i=0; i<ntrk; i++ ) {
//     evd_trk = (TEvdTrack*) fListOfTracks->At(i);
//     evd_trk->Paint(Option);
//   }

  gPad->Modified();
}

//-----------------------------------------------------------------------------
// in RZ view can display only straw hits, they are painted by fTracker
//-----------------------------------------------------------------------------
void TTrkVisNode::PaintRZ(Option_t* Option) {
  int             ntrk(0), nhits;
  stntuple::TEvdTrack*      evd_trk;

  //  TStnVisManager* vm = TStnVisManager::Instance();

  fTracker->PaintRZ(Option);
//-----------------------------------------------------------------------------
// do not draw all straw hits - just redraw straws in different color instead
//-----------------------------------------------------------------------------
//   int nhits = fListOfStrawHits->GetEntries();
//   for (int i=0; i<nhits; i++) {
//     hit       = (TEvdStrawHit*) fListOfStrawHits->At(i);

//     if ((station >= vm->MinStation()) && (station <= vm->MaxStation())) continue;
//     if ((time    <  vm->TMin()      ) || (time     > vm->TMax()      )) continue; 

//     hit->Paint(Option);
//   }
//-----------------------------------------------------------------------------
// display tracks and track hits
//-----------------------------------------------------------------------------
  if (fListOfTracks != 0)  ntrk = fListOfTracks->GetEntriesFast();

  for (int i=0; i<ntrk; i++ ) {
    evd_trk = (stntuple::TEvdTrack*) fListOfTracks->At(i);
    evd_trk->Paint(Option);

    nhits = evd_trk->NHits();
    for (int ih=0; ih<nhits; ih++) {
      stntuple::TEvdTrkStrawHit* hit = evd_trk->Hit(ih);
      hit->PaintRZ(Option);
    }
  }

//-----------------------------------------------------------------------------
// SimParticle's : pT at  the ST is too large, need to use parameters at the tracker entrance ?
//-----------------------------------------------------------------------------
  stntuple::TEvdSimParticle* esim;
  int nsim(0);

  if ( (fListOfSimParticles) != 0 )  nsim = fListOfSimParticles->GetEntriesFast();

  for (int i=0; i<nsim; i++ ) {
    esim = (stntuple::TEvdSimParticle*) fListOfSimParticles->At(i);
    esim->PaintRZ(Option);
  }

  gPad->Modified();
}


//-----------------------------------------------------------------------------
// TZ view is only for the pattern recognition / time cluster finding
// display reconstructed tracks and combo hits 
//-----------------------------------------------------------------------------
void TTrkVisNode::PaintTZ(Option_t* Option) {

  int nhits = fListOfComboHits->GetEntries();
  for (int i=0; i<nhits; i++) {
    stntuple::TEvdComboHit* hit = (stntuple::TEvdComboHit*) fListOfComboHits->At(i);

    // float time  = hit->Time();

    //    if ((time >= tmin) && (time <= tmax)) {
    hit->PaintTZ(Option);
    // }
  }

//-----------------------------------------------------------------------------
// SimParticle's
//-----------------------------------------------------------------------------
  stntuple::TEvdSimParticle* esim;
  int nsim(0);

  if ( (fListOfSimParticles) != 0 )  nsim = fListOfSimParticles->GetEntriesFast();

  for (int i=0; i<nsim; i++ ) {
    esim = (stntuple::TEvdSimParticle*) fListOfSimParticles->At(i);
    esim->PaintTZ(Option);
  }

  gPad->Modified();
}

//-----------------------------------------------------------------------------
int TTrkVisNode::DistancetoPrimitiveXY(Int_t px, Int_t py) {
  static TVector3 global;
  global.SetXYZ(gPad->AbsPixeltoX(px),gPad->AbsPixeltoY(py),0);

  TObject* closest(nullptr);

  int  x1, y1, dx1, dy1, min_dist(9999), dist;
  TStnVisManager* vm = TStnVisManager::Instance();

  if (vm->DisplayStrawHitsXY() == 1) {
    int nhits = fListOfStrawHits->GetEntries();
    for (int i=0; i<nhits; i++) {
      stntuple::TEvdStrawHit* hit = GetEvdStrawHit(i);

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
  }
  else {
    int nhits = fListOfComboHits->GetEntries();
    for (int i=0; i<nhits; i++) {
      stntuple::TEvdComboHit* hit = GetEvdComboHit(i);

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
  }

//-----------------------------------------------------------------------------
// tracks are represented by ellipses
//-----------------------------------------------------------------------------
  int ntracks = fListOfTracks->GetEntries();
  for (int i=0; i<ntracks; i++) {
    stntuple::TEvdTrack* trk = GetEvdTrack(i);

    dist = trk->DistancetoPrimitiveXY(px,py);

    if (dist < min_dist) {
      min_dist = dist;
      closest  = trk;
    }
  }

//-----------------------------------------------------------------------------
// simparticles are represented by ellipses
//-----------------------------------------------------------------------------
  int nsim = fListOfSimParticles->GetEntries();
  for (int i=0; i<nsim; i++) {
    stntuple::TEvdSimParticle* sim = GetEvdSimParticle(i);

    dist = sim->DistancetoPrimitiveXY(px,py);

    if (dist < min_dist) {
      min_dist = dist;
      closest  = sim;
    }
  }
  SetClosestObject(closest,min_dist);

  return min_dist;
}

//-----------------------------------------------------------------------------
Int_t TTrkVisNode::DistancetoPrimitiveRZ(Int_t px, Int_t py) {
  return 9999;
}


//-----------------------------------------------------------------------------
Int_t TTrkVisNode::DistancetoPrimitiveTZ(Int_t px, Int_t py) {

  // static TVector3 global;
  // global.SetXYZ(gPad->AbsPixeltoX(px),gPad->AbsPixeltoY(py),0);

  TObject* closest(nullptr);

  int  x1, y1, dx1, dy1, min_dist(9999), dist;

  int nhits = fListOfComboHits->GetEntries();
  for (int i=0; i<nhits; i++) {
    stntuple::TEvdComboHit* hit = (stntuple::TEvdComboHit*) fListOfComboHits->At(i);
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
//-----------------------------------------------------------------------------
// simparticles are represented by lines
//-----------------------------------------------------------------------------
  int nsim = fListOfSimParticles->GetEntries();
  for (int i=0; i<nsim; i++) {
    stntuple::TEvdSimParticle* esp = GetEvdSimParticle(i);

    dist = esp->DistancetoPrimitiveTZ(px,py);

    if (dist < min_dist) {
      min_dist = dist;
      closest  = esp;
    }
  }

  SetClosestObject(closest,min_dist);

  return min_dist;
}


//-----------------------------------------------------------------------------
void TTrkVisNode::Clear(Option_t* Opt) {
  printf(">>> name: %s TTrkVisNode::Clear is not implemented yet\n",GetName());
}

//-----------------------------------------------------------------------------
void TTrkVisNode::Print(Option_t* Opt) const {
  printf(" >>> name: %s TTrkVisNode::Print is not implemented yet\n",GetName());

//-----------------------------------------------------------------------------
// print SimParticles
//-----------------------------------------------------------------------------
  stntuple::TEvdSimParticle* sim;
  int nsim(0);

  if ( (fListOfSimParticles) != 0 )  nsim = fListOfSimParticles->GetEntriesFast();

  printf("n(sim particles) = %i\n",nsim);
  int banner_printed(0);
  for (int i=0; i<nsim; i++ ) {
    sim = (stntuple::TEvdSimParticle*) fListOfSimParticles->At(i);
    if (banner_printed == 0) {
      sim->Print("banner");
      banner_printed = 1;
    }
    sim->Print("data");
  }

//-----------------------------------------------------------------------------
// print ComboHits
//-----------------------------------------------------------------------------
  banner_printed = 0;
  int nch = fListOfComboHits->GetEntries();
  printf("n(combo hits) = %i\n",nch);

  for (int i=0; i<nch; i++) {
    stntuple::TEvdComboHit* hit = (stntuple::TEvdComboHit*) fListOfComboHits->At(i);
    if (banner_printed == 0) {
      hit->Print("banner");
      banner_printed = 1;
    }
    hit->Print("data");
  }

}

