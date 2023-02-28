///////////////////////////////////////////////////////////////////////////////
// stores one StepPointMCCollection in STNTUPLE data block
///////////////////////////////////////////////////////////////////////////////
#include "TLorentzVector.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "art/Framework/Principal/Handle.h"

#include "Offline/MCDataProducts/inc/StatusG4.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"

#include "Stntuple/obj/TStepPointMCBlock.hh"
#include "Stntuple/mod/InitStepPointMCBlock.hh"
//-----------------------------------------------------------------------------
int StntupleInitStepPointMCBlock::InitDataBlock(TStnDataBlock* Block, AbsEvent* AnEvent, int Mode) 
{
  // const char* oname = {"StntupleInitMu2eStepPointMCBlock"};

  art::Handle<mu2e::StepPointMCCollection> spmcch;
  const mu2e::StepPointMCCollection*       spmcc(0);

  TStepPointMCBlock* spmc_block = (TStepPointMCBlock*) Block;
  spmc_block->Clear();
//-----------------------------------------------------------------------------
// load simulation time offsets for this event: timeOffsets may not be defined yet (stages 1, 2, 3)
//-----------------------------------------------------------------------------
//  if (fTimeOffsets) fTimeOffsets->updateMap(*AnEvent);

  const mu2e::StepPointMC*                    spmc;
//-----------------------------------------------------------------------------
//  const art::Provenance*                 prov;
//  art::Selector  selector(art::ProductInstanceNameSelector(""));
//  AnEvent->getMany(selector, list_of_sp);
//
//   for (std::vector<art::Handle<mu2e::SimParticleCollection>> ::const_iterator it = list_of_sp.begin();
//        it != list_of_sp.end(); it++) {
//     handle = it.operator -> ();


  art::Handle<mu2e::StatusG4>  sg4h;
  const mu2e::StatusG4*        sg4;
  if (! fStatusG4Tag.empty() != 0) {
    bool ok = AnEvent->getByLabel(fStatusG4Tag,sg4h);
    if (ok) { 
      sg4 = sg4h.product();

      spmc_block->fG4Status         = sg4->status();
      spmc_block->fNG4Tracks        = sg4->nG4Tracks();
      spmc_block->fNOverflowSimP    = sg4->overflowSimParticles();
      spmc_block->fNKilledStepLim   = sg4->nKilledStepLimit();
      spmc_block->fNKilledFieldProp = sg4->nKilledByFieldPropagator();
      spmc_block->fG4CpuTime        = sg4->cpuTime();
      spmc_block->fG4RealTime       = sg4->realTime();
    }
  }

  AnEvent->getByLabel(fSpmcCollTag,spmcch);
  
  if (spmcch.isValid()) {
    spmcc = spmcch.product();
    //      prov = handle->provenance();
      
    //       printf("moduleLabel = %-20s, producedClassname = %-30s, productInstanceName = %-20s\n",
    // 	     prov->moduleLabel().data(),
    // 	     prov->producedClassName().data(),
    // 	     prov->productInstanceName().data());

    for (mu2e::StepPointMCCollection::const_iterator ip = spmcc->begin(); ip != spmcc->end(); ip++) {
      spmc = &(*ip);
      
      int   volume_id = spmc->volumeId();

      int   id (-1), gen_index(-1), pdg_code(-1), parent_pdg_code (-1), parent_id(-1);
      int   creation_code(-1), end_process_code(-1);
      float edep_tot (-1.), edep_nio(-1.), time(-1.), step_length(-1.);

      art::Ptr<mu2e::SimParticle> const& simptr = spmc->simParticle();
      const mu2e::SimParticle* sim              = simptr.get();
//-----------------------------------------------------------------------------
// a semi-kludge: store VD hits of e+ and e- from external photon conversions
//-----------------------------------------------------------------------------
      const mu2e::GenParticle* genp;

      if (sim->parent()) { 
	parent_id       = sim->parent()->id().asInt();
	parent_pdg_code = sim->parent()->pdgId();
	genp            = sim->parent()->genParticle().get();
      }
      else {
	genp      = sim->genParticle().get();
      }

      int generator_id(-1);
      if (genp) generator_id = genp->generatorId().id();   // ID of the MC generator
//-----------------------------------------------------------------------------
// if spmc_block->fGenProcessID is set, store only hits of the particles from 
// the signal process
//-----------------------------------------------------------------------------
      if ((spmc_block->GenProcessID() > 0) && (generator_id != spmc_block->GenProcessID())) continue;

      id            = sim->id().asInt();
      gen_index     = sim->generatorIndex();
      pdg_code      = sim->pdgId();
      creation_code = sim->creationCode();

      end_process_code = spmc->endProcessCode();

      edep_tot      = spmc->totalEDep();
      edep_nio      = spmc->nonIonizingEDep();

      //      if (fTimeOffsets) {
//-----------------------------------------------------------------------------
// time - within the microbunch
//-----------------------------------------------------------------------------
      //   double tx = fTimeOffsets->timeWithOffsetsApplied(*spmc);
      //   time = fmod(tx+fMbTime,fMbTime);
      // }
      // else 
      time =  spmc->time();

      float proper_time = spmc->properTime();
      step_length = spmc->stepLength();

      spmc_block->NewStepPointMC(volume_id, gen_index, 
				 id       , pdg_code       ,
				 parent_id, parent_pdg_code, 
				 creation_code, end_process_code,
				 edep_tot,edep_nio,
				 time, 
				 proper_time,
				 step_length,
				 spmc->position().x(),
				 spmc->position().y(),
				 spmc->position().z(),
				 spmc->momentum().x(),
				 spmc->momentum().y(),
				 spmc->momentum().z());
    }
  }

  return 0;
}


//-----------------------------------------------------------------------------
// keep this function as an example, don't really need it
//-----------------------------------------------------------------------------
int StntupleInitStepPointMCBlock::ResolveLinks(TStnDataBlock* Block, AbsEvent* Event, int Mode) {

  int ev, rn, sr;
  
  ev = Event->event();
  rn = Event->run();
  sr = Event->subRun();
  
  if (! Block->Initialized(ev,rn,sr)) return -1;

  return 0;
}
