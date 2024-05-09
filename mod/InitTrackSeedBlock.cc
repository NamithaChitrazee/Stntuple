//-----------------------------------------------------------------------------
// Apr 2016 G. Pezzullo: initialization of the MU2E STNTUPLE TrackSeed block
// Feb 2023 P.Murat: convert initializatin to a class
//-----------------------------------------------------------------------------
#include <cstdio>
#include "TROOT.h"
#include "TFolder.h"
#include "TLorentzVector.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "Stntuple/mod/InitTrackSeedBlock.hh"

#include "Stntuple/obj/TStnDataBlock.hh"
#include "Stntuple/obj/TStnEvent.hh"

#include "Stntuple/obj/TStnHelix.hh"
#include "Stntuple/obj/TStnHelixBlock.hh"

#include "Stntuple/obj/TStnTrackSeed.hh"
#include "Stntuple/obj/TStnTrackSeedBlock.hh"

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"

#include "Offline/RecoDataProducts/inc/KalSeedAssns.hh"
#include "Offline/RecoDataProducts/inc/KalSeed.hh"
#include "Offline/RecoDataProducts/inc/CaloCluster.hh"
#include "Offline/RecoDataProducts/inc/StrawHit.hh"

#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/StrawGasStep.hh"
#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"
//-----------------------------------------------------------------------------
// assume that the collection name is set, so we could grab it from the event
//-----------------------------------------------------------------------------
int  StntupleInitTrackSeedBlock::InitDataBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode) {

  int ev_number = Evt->event();
  int rn_number = Evt->run();

  if (Block->Initialized(ev_number,rn_number)) return 0;

  TStnTrackSeed*      trackSeed(0);
  TStnTrackSeedBlock* data = (TStnTrackSeedBlock*) Block;

  data->Clear();

  art::Handle<mu2e::KalSeedCollection> ksfcH;
  Evt->getByLabel(fKsfCollTag, ksfcH);
  const mu2e::KalSeedCollection*  ksfColl(0);
  int nts(0);
  if (ksfcH.isValid()){
    ksfColl = ksfcH.product();
    nts     = ksfColl->size();    
  }

  art::Handle<mu2e::ComboHitCollection> sschcH;
  Evt->getByLabel(fSschCollTag,sschcH);
  const mu2e::ComboHitCollection* sschColl = sschcH.product();
  
  const mu2e::KalSeed*     trkSeed(0);
  const mu2e::CaloCluster* cluster(nullptr);
 
  const mu2e::StrawDigiMCCollection* sdmcColl(0);
  art::Handle<mu2e::StrawDigiMCCollection> sdmccH;
  Evt->getByLabel(fSdmcCollTag,sdmccH);
  sdmcColl = sdmccH.product();
  
  TParticlePDG*        part(0);
  TDatabasePDG*        pdg_db = TDatabasePDG::Instance();
 
  for (int i=0; i<nts; i++) {
    std::vector<int>     hits_simp_id, hits_simp_index, hits_simp_z;
    
    trackSeed                  = data->NewTrackSeed();
    trkSeed                    = &ksfColl->at(i);
    cluster                    = trkSeed->caloCluster().get();

    if (cluster != 0){
      trackSeed->fClusterTime    = cluster->time();
      trackSeed->fClusterEnergy  = cluster->energyDep();
      trackSeed->fClusterX       = cluster->cog3Vector().x();
      trackSeed->fClusterY       = cluster->cog3Vector().y();
      trackSeed->fClusterZ       = cluster->cog3Vector().z();
    }else {
      trackSeed->fClusterTime    = 0; 
      trackSeed->fClusterEnergy  = -1; 
      trackSeed->fClusterX       = -1.e6; 
      trackSeed->fClusterY       = -1.e6; 
      trackSeed->fClusterZ       = -1.e6; 
    }
    
    mu2e::KalSegment     kalSeg  = trkSeed->segments().at(0);//take the KalSegment closer to the entrance of the tracker
    KinKal::CentralHelix helx    = kalSeg.centralHelix();
    trackSeed->fTrackSeed    = trkSeed;
    trackSeed->fNHits        = trkSeed->hits().size();
    trackSeed->fT0           = trkSeed->t0()._t0;
    trackSeed->fT0Err        = trkSeed->t0()._t0err;     
    trackSeed->fD0           = kalSeg.helix().d0();
    trackSeed->fFlt0         = trkSeed->flt0();     
    trackSeed->fTanDip       = kalSeg.helix().tanDip();
    trackSeed->fP            = kalSeg.mom();
    trackSeed->fPt           = trackSeed->fP*std::cos( std::atan(trackSeed->fTanDip));
    trackSeed->fZ0           = kalSeg.helix().z0();

    trackSeed->fChi2         = trkSeed->chisquared();
    trackSeed->fFitCons      = trkSeed->fitConsistency();
    trackSeed->fCharge       = helx.charge();

    // now loop over the hits to search the particle that generated the track

    int                      nsh = trkSeed->hits().size();
    const mu2e::ComboHit*    hit(0), *hit_0(0);
    hit_0     = &sschColl->at(0);

    int                   loc(-1);

    float    first_hit_z(0), last_hit_z(0);
    float    lambda = kalSeg.helix().tanDip()/kalSeg.helix().omega();

    for (int j=0; j<nsh; ++j){
      int  hitIndex  = int(trkSeed->hits().at(j).index());
      hit            = &sschColl->at(hitIndex);
      loc            = hit - hit_0;
      
      if(j==0) first_hit_z = hit->pos().z();
      else if(j==nsh-1) last_hit_z = hit->pos().z();
   
      const mu2e::StrawGasStep* step(nullptr);
      if (sdmcColl) {
 	const mu2e::StrawDigiMC* sdmc = &sdmcColl->at(loc);
        step = sdmc->earlyStrawGasStep().get();
      }

      if (step) {
	art::Ptr<mu2e::SimParticle> const& simptr = step->simParticle(); 
	int sim_id        = simptr->id().asInt();

	hits_simp_id.push_back   (sim_id); 
	hits_simp_index.push_back(loc);
	hits_simp_z.push_back(step->position().z());
     }
    }
//-----------------------------------------------------------------------------
// calculate the number of loops made
//-----------------------------------------------------------------------------
    trackSeed->fNLoops = (last_hit_z - first_hit_z)/(fabs(lambda)*2.*M_PI);
 //-----------------------------------------------------------------------------
// find the simparticle that created the majority of the hits
//-----------------------------------------------------------------------------
    float   dz_most(1e4);
    int     max(0), mostvalueindex(-1), mostvalue(-1);
    if (hits_simp_id.size() > 0) {
      mostvalue = hits_simp_id[0];
      for (int k=0; k<(int)hits_simp_id.size(); ++k){
        int co = (int)std::count(hits_simp_id.begin(), hits_simp_id.end(), hits_simp_id[k]);
        if ( (co>0) &&  (co>max)) {
          float  dz      = std::fabs(hits_simp_z[k]);
          if (dz < dz_most){
            dz_most        = dz;
            max            = co;
            mostvalue      = hits_simp_id[k];
            mostvalueindex = hits_simp_index[k];
          }
        }
      }
    }

    trackSeed->fSimpId1Hits = max;
    trackSeed->fSimpId2Hits = -1;

    const mu2e::StrawGasStep* step(nullptr);
    if (sdmcColl) {
      const mu2e::StrawDigiMC* sdmc = &sdmcColl->at(mostvalueindex);
      step = sdmc->earlyStrawGasStep().get();
    }
    const mu2e::SimParticle * sim (0);

    if (step) {
      art::Ptr<mu2e::SimParticle> const& simptr = step->simParticle(); 
      trackSeed->fSimpPDG1    = simptr->pdgId();
      art::Ptr<mu2e::SimParticle> mother = simptr;
      part   = pdg_db->GetParticle(trackSeed->fSimpPDG1);

      while(mother->hasParent()) mother = mother->parent();
      sim = mother.operator ->();

      trackSeed->fSimpPDGM1   = sim->pdgId();
      
      double   px = simptr->startMomentum().x();
      double   py = simptr->startMomentum().y();
      double   pz = simptr->startMomentum().z();
      double   mass  (-1.);
      double   energy(-1.);
      if (part) {
	mass   = part->Mass();
	energy = sqrt(px*px+py*py+pz*pz+mass*mass);
      }
      trackSeed->fMom1.SetPxPyPzE(px,py,pz,energy);

      const CLHEP::Hep3Vector sp = simptr->startPosition();
      trackSeed->fOrigin1.SetXYZT(sp.x(),sp.y(),sp.z(),simptr->startGlobalTime());
    }
//-----------------------------------------------------------------------------
// look for the second most frequent hit -what is that ?
//-----------------------------------------------------------------------------
    if (max != nsh && max > 0){
      int   secondmostvalueindex(-1);
      max     = 0;//reset max
      dz_most = 1e4;

      for (int k=0; k<(int)hits_simp_id.size(); ++k){
	int value = hits_simp_id[k];
	int co = (int)std::count(hits_simp_id.begin(), hits_simp_id.end(), value);
	if ( (co>0) && (co>max) && (value != mostvalue)) {
	  float  dz      = std::fabs(hits_simp_z[k]);
	  if (dz < dz_most){
	    max                  = co;
	    dz_most              = dz;
	    secondmostvalueindex = hits_simp_index[k];
	  }
	}
      }
      trackSeed->fSimpId2Hits = max;

      if (secondmostvalueindex >=0){
	const mu2e::StrawGasStep* step(nullptr);
	const mu2e::StrawDigiMC* sdmc = &sdmcColl->at(secondmostvalueindex);
        step = sdmc->earlyStrawGasStep().get();
		
	const mu2e::SimParticle * sim (nullptr);

	if (step) {
	  art::Ptr<mu2e::SimParticle> const& simptr = step->simParticle(); 
	  trackSeed->fSimpPDG2    = simptr->pdgId();
	  art::Ptr<mu2e::SimParticle> mother = simptr;
	  part   = pdg_db->GetParticle(trackSeed->fSimpPDG2);

	  while(mother->hasParent()) mother = mother->parent();
	  sim = mother.operator ->();

	  trackSeed->fSimpPDGM2   = sim->pdgId();
      
	  double   px = simptr->startMomentum().x();
	  double   py = simptr->startMomentum().y();
	  double   pz = simptr->startMomentum().z();
	  double   mass  (-1.);
	  double   energy(-1.);
	  if (part) {
	    mass   = part->Mass();
	    energy = sqrt(px*px+py*py+pz*pz+mass*mass);
	  }
	  trackSeed->fMom2.SetPxPyPzE(px,py,pz,energy);

	  const CLHEP::Hep3Vector sp = simptr->startPosition();
	  trackSeed->fOrigin2.SetXYZT(sp.x(),sp.y(),sp.z(),simptr->startGlobalTime());
	}      
      }
    }
  }
					// on return set event and run numbers
					// to mark block as initialized
  data->f_RunNumber   = rn_number;
  data->f_EventNumber = ev_number;

  return 0;
}

//-----------------------------------------------------------------------------
int StntupleInitTrackSeedBlock::ResolveLinks(TStnDataBlock* Block, AbsEvent* AnEvent, int Mode) {
//-----------------------------------------------------------------------------
// do not do initialize links the 2nd time
//-----------------------------------------------------------------------------
  if (Block->LinksInitialized()) return 0;

  art::Handle<mu2e::KalHelixAssns> ksfhaH;
  const mu2e::KalHelixAssns* ksfha(0);
  AnEvent->getByLabel(fKsfCollTag, ksfhaH);
  if (ksfhaH.isValid()) ksfha = ksfhaH.product();

  TStnTrackSeedBlock* tsb = (TStnTrackSeedBlock*) Block;
  TStnEvent*          ev  = Block->GetEvent();
  TStnHelixBlock*     hb  = (TStnHelixBlock*) ev->GetDataBlock(fHsBlockName.encode().data());
  
  int nts(0);
  if (tsb!=nullptr){
    nts = tsb->NTrackSeeds();
  }
  int nhel(0);
  if (hb!=nullptr){
    nhel = hb->NHelices();
  }
  
  for (int i=0; i<nts; i++) {
    TStnTrackSeed* tseed = tsb->TrackSeed(i);
    const mu2e::KalSeed* ksf   = tseed->fTrackSeed;
//-----------------------------------------------------------------------------
// looking for the seed in associations
//-----------------------------------------------------------------------------
    const mu2e::HelixSeed* hs(nullptr);
    if (ksfha){
      for (auto ass: *ksfha) {
        const mu2e::KalSeed* qsf = ass.first.get();
        if (qsf == ksf) {
          hs = ass.second.get();
        break;
        }
      }
    }

    int  hindex(-1);

    if (hs == nullptr) { 
      printf("ERROR in StntupleInitTrackSeedBlock::ResolveLinks: kseed->helix() is gone. FIXIT\n");
    }
    else {
//-----------------------------------------------------------------------------
// search for a helix in helix block
//-----------------------------------------------------------------------------
      const mu2e::HelixSeed* hs2(nullptr);
      for (int j=0; j<nhel; ++j){
        TStnHelix* hel  = hb->Helix(j);
        hs2 = hel->fHelix;
        if (hs2 == hs){
          hindex = j;
          break;
        }    
      }
    }
    
    if (hindex < 0) {
      printf("ERROR in InitTrackSeedBlock::ResolveLinks: trackseed %s:%i has no HelixSeed associated\n", 
             fKsfCollTag.encode().data(),i);
    }
    tseed->SetHelixIndex(hindex);
  }
//-----------------------------------------------------------------------------
// mark links as initialized
//-----------------------------------------------------------------------------
  Block->SetLinksInitialized();

  return 0;
}
