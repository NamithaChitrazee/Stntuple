///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include <vector>
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Stntuple/mod/MFInterface.hh"

#include "Stntuple/mod/InitStrawHitBlock.hh"
#include "Stntuple/mod/StntupleUtilities.hh"

#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/RecoDataProducts/inc/StrawDigi.hh"

#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"
#include "Offline/MCDataProducts/inc/StrawGasStep.hh"

using std::vector ;
//-----------------------------------------------------------------------------
// in this case AbsEvent is just not used
//-----------------------------------------------------------------------------
namespace stntuple {
//-----------------------------------------------------------------------------
InitStrawHitBlock::InitStrawHitBlock() : TStnInitDataBlock() {
  fStoreCloseHitsOnly = false;     // by default, save all hits
  fMinEDep            = 0.5e-3; // 0.5 keV
}
  
int InitStrawHitBlock::InitDataBlock(TStnDataBlock* Block, AbsEvent* Event, int Mode) {

  int ev_number, rn_number, mc_flag(0), nhits(0);

  art::EventID eid = Event->id();
  ev_number        = eid.event();
  rn_number        = eid.run();

  // Mu2e: run_number < 100000: Monte Carlo
  if (rn_number < 100000) mc_flag = 1;

  if (Block->Initialized(ev_number,rn_number)) return 0;

  TStrawHitBlock* data = (TStrawHitBlock*) Block;
  data->Clear();
  
  if (fLastRun != rn_number) {
    fTpm      = &fTpmh.get(eid);
    fLastRun  = rn_number;
  }  
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  const mu2e::ComboHitCollection*    chc  (nullptr);
  const mu2e::StrawHitCollection*    shc  (nullptr);
  const mu2e::StrawDigiMCCollection* sdmcc(nullptr);
  const mu2e::StrawDigiCollection*   sdc  (nullptr);
  const mu2e::TimeClusterCollection* tcc  (nullptr);
//-----------------------------------------------------------------------------
// assume that straw hits and combo hits are created by the same module - why ?
//-----------------------------------------------------------------------------
  if (! fShCollTag.empty() != 0) {
    art::Handle<mu2e::StrawHitCollection> shch;
    bool ok = Event->getByLabel(fShCollTag,shch);
    if (ok) { 
      shc   = shch.product();
      nhits = shc->size();
    }
    else {
      mf::LogWarning("InitStrawHitBlock::InitDataBlock") << " ERROR:" << __LINE__ 
							 << " : mu2e::StrawHitCollection " 
							 << fShCollTag.encode().data() 
							 << " not found, BAIL OUT. rc = -1";
      return -1;
    }
  }
//-----------------------------------------------------------------------------
// combo hits collection is needed here
//-----------------------------------------------------------------------------
  if (! fChCollTag.empty() != 0) {
    art::Handle<mu2e::ComboHitCollection> chch;
    bool ok = Event->getByLabel(fChCollTag,chch);
    if (ok) {
      chc  = chch.product();
    }
    else {
      std::string msg = std::format("mu2e::ComboHitCollection {} not found. BAIL OUT rc=-1.\n",
                                    fChCollTag.encode().data());
      stntuple::print_(eid,e_ERROR,msg);
      return -1;
    }
  }
//-----------------------------------------------------------------------------
// straa digi collection - why is it needed here ?
//-----------------------------------------------------------------------------
  if (! fStrawDigiCollTag.empty() != 0) {
    art::Handle<mu2e::StrawDigiCollection> sdch;
    bool ok = Event->getByLabel(fStrawDigiCollTag,sdch);
    if (ok) sdc = sdch.product();

    if (sdc == nullptr) {
      mf::LogWarning("InitStrawHitBlock::InitDataBlock") << " Warning:" << __LINE__ 
							 << " : mu2e::StrawDigiCollection " 
							 << fStrawDigiCollTag.encode().data() 
							 << " not found.";
    }
  }
//-----------------------------------------------------------------------------
// use time clusters only when store close hits
//-----------------------------------------------------------------------------
  if (fStoreCloseHitsOnly) { 
    if (not fTimeClusterCollTag.empty()) {
      art::Handle<mu2e::TimeClusterCollection> tcch;
      bool ok = Event->getByLabel(fTimeClusterCollTag,tcch);
      if (ok) tcc = tcch.product();
    }
  }
  
  // don't check for MC collection in the detector data
  if (mc_flag and (not fStrawDigiMCCollTag.empty())) {
    art::Handle<mu2e::StrawDigiMCCollection> sdmcch;
    bool ok = Event->getByLabel(fStrawDigiMCCollTag,sdmcch);
    if (ok) sdmcc = sdmcch.product();

    if (sdmcc == nullptr) {
      mf::LogWarning("InitStrawHitBlock::InitDataBlock") << " Warning:" << __LINE__ 
							 << " : mu2e::StrawDigiMCCollection " 
							 << fStrawDigiMCCollTag.encode().data() 
							 << " not found.";
    }
  }
//-----------------------------------------------------------------------------
// MC data may not be present ...
//-----------------------------------------------------------------------------
  const mu2e::StrawGasStep* step (nullptr);
  const mu2e::SimParticle* sim;

  TStrawHit*           hit; 

  int   pdg_id, mother_pdg_id, sim_id, gen_id;
  float mc_mom;

  for (int ihit=0; ihit<nhits; ihit++) {
    const mu2e::StrawHit* sh = &shc->at(ihit);

    int sd_flag = 0;
    if (sdc) sd_flag = *((int*) &sdc->at(ihit).digiFlag());

    step = nullptr;
    if (sdmcc) {  
      const mu2e::StrawDigiMC* mcdigi = &sdmcc->at(ihit);
      step = mcdigi->earlyStrawGasStep().get();
    }
//-----------------------------------------------------------------------------
// if store only "close" hits, theck the timing
// max drift time is ~ 40 ns, the TC time is made out of corrected times,
// which on average are ~20 ns lower
// use CAL end for time... still a window of 80 ns should be sufficiently large
//-----------------------------------------------------------------------------
    if (fStoreCloseHitsOnly) {
      if (tcc != nullptr) {
        if (sh->energyDep() < fMinEDep) {
          // bool close_enough = stntuple::CloseEnough(sh->time()-20.,tcc,fMinDt);
          bool close_enough = stntuple::CloseEnough(sh->time()-20,tcc,fMinDt);
          if (not close_enough) {
            // accidentally, the time clustering window may be large enough
            // so hits included into the time cluster are outside the checked window
            // make sure the hit is not in the list explicitly
            int ntc = tcc->size();
            for (int itc=0; itc<ntc; itc++) {
              const mu2e::TimeCluster* tc = &tcc->at(itc);
              // look at the TC combo hits
              int nch = tc->hits().size();
              for (int j=0; j<nch; j++) {
                uint16_t ch_ind = tc->hits().at(j);
                const mu2e::ComboHit* ch = &chc->at(ch_ind); // assuming CHC is there
                // a combo hit has a list of straw hit indices
                int ch_nsh = ch->nStrawHits();
                for (int ish=0; ish<ch_nsh; ish++) {
                  if (ihit == ch->index(ish)) {
                    close_enough = true;
                    break;
                  }
                }
                if (close_enough) break;
              }
              if (close_enough) break;
            }
            
            if (not close_enough) continue;
          }
        }
      }
      else {
        std::string msg = std::format("mu2e::TimeClusterCollection {} not found, store all hits",
                                      fTimeClusterCollTag.encode().data());
        stntuple::print_(eid,e_WARNING,msg);
      }
    }
    // preserve the hit ID: its index in the original collection
    hit = data->NewHit(ihit);

    if (step) {
      art::Ptr<mu2e::SimParticle> const& simptr = step->simParticle(); 
      art::Ptr<mu2e::SimParticle> mother = simptr;
      
      while (mother->hasParent()) mother = mother->parent();

      sim = mother.get();

      pdg_id        = simptr->pdgId();
      mother_pdg_id = sim->pdgId();

      if (simptr->fromGenerator()) gen_id = simptr->genParticle()->generatorId().id();
      else                         gen_id = 0xFF;
      
      sim_id        = simptr->id().asInt();
      mc_mom        = step->momvec().mag();
    }
    else {
      pdg_id        = -1;
      mother_pdg_id = -1;
      gen_id        = 0xFF;
      sim_id        = -1;
      mc_mom        = -1.;
    }
    // define the hit MC flag
    int sid = sh->strawId().asUint16() | (mc_flag << 16);

    // straw hit time is a float

    float time [2], tot[2];

    time[0] = sh->time(mu2e::StrawEnd::cal);
    time[1] = sh->time(mu2e::StrawEnd::hv );
    
    tot[0] = sh->TOT(mu2e::StrawEnd::cal);
    tot[1] = sh->TOT(mu2e::StrawEnd::hv );
    
    gen_id  = gen_id | (sd_flag << 16) ;
    
    hit->Set(sid, time, tot, 
             gen_id, sim_id, 
             pdg_id, mother_pdg_id, 
             sh->energyDep(), mc_mom);
    
    int pln = sh->strawId().plane();
    int pnl = sh->strawId().panel();
    const mu2e::TrkPanelMap::Row* tpmd = fTpm->panel_map_by_offline_ind(pln,pnl);
    
    hit->SetMnid (tpmd->mnid ());
    hit->SetZFace(tpmd->zface());
  }

  data->f_RunNumber   = rn_number;
  data->f_EventNumber = ev_number;
  
  return 0;

}


}
