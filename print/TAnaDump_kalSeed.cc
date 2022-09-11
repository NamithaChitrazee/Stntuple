////////////////////////////////////////////////////////////////////////////////

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Selector.h"

#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/RecoDataProducts/inc/KalSeed.hh"
#include "Offline/RecoDataProducts/inc/HelixSeed.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/TrkStrawHitSeed.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"

using namespace std;
//-----------------------------------------------------------------------------
void TAnaDump::printKalSeed(const mu2e::KalSeed* KalSeed           ,
			    const char*          Opt               , 
			    const char*          StrawHitCollTag   ,
			    const char*          StrawDigiMCCollTag) {
  TString opt = Opt;
  
  if ((opt == "") || (opt == "banner")) {
    printf("------------------------------------------------------------------------------");
    printf("----------------------------------------------------------------------------\n");
    printf("  TrkID       Address    N      P      pT       T0     T0err    fmin      fmax");
    printf("       D0       Z0     Phi0   TanDip    radius      Ecl      chi2   FitCon  \n");
    printf("------------------------------------------------------------------------------");
    printf("----------------------------------------------------------------------------\n");
  }

  if ((opt == "") || (opt.Index("data") >= 0)) {
    int    nhits   = KalSeed->hits().size();
    
    double t0     = KalSeed->t0()._t0;
    double t0err  = KalSeed->t0()._t0err;

    for (const mu2e::KalSegment& kalSeg : KalSeed->segments()) {

      double d0     = kalSeg.helix().d0();
      double z0     = kalSeg.helix().z0();
      double phi0   = kalSeg.helix().phi0();
      double tandip = kalSeg.helix().tanDip();
      double mm2MeV = 3/10.;
      double mom    = kalSeg.mom();
      double pt     = mom*cos(atan(tandip));
      double radius = pt/mm2MeV;


      const mu2e::CaloCluster*cluster = KalSeed->caloCluster().get();
      double clusterEnergy(-1);
      if (cluster != 0) clusterEnergy = cluster->energyDep();
      printf("%5i %16p %3i %8.3f %8.5f %7.3f %6.3f %9.3f %9.3f",
	     -1,
	     KalSeed,
	     nhits,
	     mom, pt, t0, t0err, kalSeg.fmin(), kalSeg.fmax() );

      float chi2    = KalSeed->chisquared()/double(nhits - 5.);
      float fitCons = KalSeed->fitConsistency();
      
      printf(" %8.3f %8.3f %6.3f %8.4f %10.4f %8.3f %8.3f %10.3e\n",
	     d0,z0,phi0,tandip,radius,clusterEnergy,chi2,fitCons);
    }
  }

  if ((opt == "") || (opt.Index("hits") >= 0)) {
    int nsh = KalSeed->hits().size();

    const mu2e::ComboHit* hit(0), *hit_0(0);
    const mu2e::StrawGasStep* step(0);

    art::Handle<mu2e::ComboHitCollection> shcolH;
    const mu2e::ComboHitCollection*       shcol(0);
    fEvent->getByLabel<mu2e::ComboHitCollection>(art::InputTag(StrawHitCollTag),shcolH);
    if (shcolH.isValid()) shcol = shcolH.product();
    else {
      printf("ERROR in TAnaDump::printTrackSeed: no ComboHitCollection with tag=%s, BAIL OUT\n",StrawHitCollTag);
      return;
    }

    art::InputTag tag; // sdmccT(fStrawDigiMCCollTag.Data());
    art::Handle<mu2e::StrawDigiMCCollection> sdmccH;
    const mu2e::StrawDigiMCCollection* sdmcc(nullptr);

    if      (StrawDigiMCCollTag  != nullptr) tag = art::InputTag(StrawDigiMCCollTag);
    else if (fStrawDigiMCCollTag != ""     ) tag = art::InputTag(fStrawDigiMCCollTag.Data());
    else {
      printf("ERROR in TAnaDump::printTrackSeed: no StrawDigiMCCollTag specified, BAIL OUT\n");
      return;
    }
    fEvent->getByLabel(tag,sdmccH);
    if (sdmccH.isValid()) sdmcc = sdmccH.product();
    else {
      printf("ERROR in TAnaDump::printTrackSeed: no StrawDigiMCCollection with tag=%s, available collections are:\n", tag.encode().data());
 
      vector<art::Handle<mu2e::StrawDigiMCCollection>> list;
      const  art::Handle<mu2e::StrawDigiMCCollection>*  handle;
      const art::Provenance*                       prov;
      
      list  = fEvent->getMany<mu2e::StrawDigiMCCollection>();

      for (auto it = list.begin(); it != list.end(); it++) {
	handle = it.operator -> ();
	if (handle->isValid()) {
	  prov = handle->provenance();
	
	  printf("moduleLabel: %-20s, productInstanceName: %-20s, processName: %-30s nHelices: %3li\n" ,
		 prov->moduleLabel().data(),
		 prov->productInstanceName().data(),
		 prov->processName().data(),
		 handle->product()->size()
		 );
	}
      }
      return;
    }
//-----------------------------------------------------------------------------
// StrawDigiMCCollection is present
//-----------------------------------------------------------------------------
    hit_0    = &shcol->at(0);
    int      loc(-1);
    int banner_printed(0);
    for (int i=0; i<nsh; ++i){
      const mu2e::TrkStrawHitSeed* hit_seed = &KalSeed->hits().at(i);
      int  hitIndex  = int(hit_seed->index());
      hit            = &shcol->at(hitIndex);
      loc            = hit - hit_0;

      int straw_hit_flag = hit_seed->flag().hasAllProperties(mu2e::StrawHitFlagDetail::active);

      if (sdmcc) {
	const mu2e::StrawDigiMC* sdmc = &sdmcc->at(loc);
	step = nullptr;
	if (sdmc->wireEndTime(mu2e::StrawEnd::cal) < sdmc->wireEndTime(mu2e::StrawEnd::hv)) {
	  step = sdmc->strawGasStep(mu2e::StrawEnd::cal).get();
	}
	else {
	  step = sdmc->strawGasStep(mu2e::StrawEnd::hv ).get();
	}
      }

      if (banner_printed == 0){
	printComboHit(hit, step, "banner", -1, straw_hit_flag);
	banner_printed = 1;
      } 
      else {
	printComboHit(hit, step, "data"  , -1, straw_hit_flag);
      }
    }
  }
}

//-----------------------------------------------------------------------------
// coll tag format: ModuleLabel[:CollName[:ProcessName]]
//-----------------------------------------------------------------------------
void TAnaDump::printKalSeedCollection(const char* KalSeedCollTag    ,
				      int         hitOpt            ,
				      const char* StrawHitCollTag   ,
				      const char* StrawDigiMCCollTag) {

  art::Handle<mu2e::KalSeedCollection> kseedHandle;
  
  bool ok = fEvent->getByLabel(art::InputTag(KalSeedCollTag),kseedHandle);
//-----------------------------------------------------------------------------
// make sure collection exists
//-----------------------------------------------------------------------------
  if (! ok) {
    printf("ERROR in TAnaDump::printKalSeedCollection: no KalSeedCollection tag=%s\n",KalSeedCollTag);
    print_kalseed_colls();
    return;
  }

  art::Handle<mu2e::StrawDigiMCCollection> sdmccH;
  fEvent->getByLabel(art::InputTag(StrawDigiMCCollTag),sdmccH);

  if (sdmccH.isValid()) {
    _mcdigis            = sdmccH.product();
    fStrawDigiMCCollTag = StrawDigiMCCollTag;
  }
  else {
    printf("ERROR in TAnaDump::printKalSeedCollection: no StrawDigiMCCollection tag=%s, available are\n",StrawDigiMCCollTag);
    print_sdmc_colls();
    _mcdigis = nullptr;
  }

  mu2e::KalSeedCollection*  list_of_kseeds;
  list_of_kseeds = (mu2e::KalSeedCollection*) &(*kseedHandle);

  int nks = list_of_kseeds->size();

  const mu2e::KalSeed *ks;

  int banner_printed = 0;
  for (int i=0; i<nks; i++) {
    ks = &list_of_kseeds->at(i);
    if (banner_printed == 0) {
      printKalSeed(ks,"banner");
      banner_printed = 1;
    }
    printKalSeed(ks,"data");
    if (hitOpt > 0) printKalSeed(ks,"hits",StrawHitCollTag,StrawDigiMCCollTag);

  }
}

