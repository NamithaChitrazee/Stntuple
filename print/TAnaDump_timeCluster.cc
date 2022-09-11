///////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/print/TAnaDump.hh"

#include "Stntuple/print/Stntuple_print_functions.hh"

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Selector.h"

#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"

using namespace std;
//-----------------------------------------------------------------------------
void TAnaDump::printTimeCluster(const mu2e::TimeCluster*        TimeCluster, 
				const char*                     Opt, 
				const mu2e::ComboHitCollection* ChColl, 
				const char*                     SdmcCollTag) {

  const mu2e::ComboHit*      hit;
  int                        flags;

  // MC collection could be absent, missing, or incorrectly specified

  const mu2e::StrawDigiMCCollection* mcdigis(nullptr);
  art::Handle<mu2e::StrawDigiMCCollection> sdmccH;
  fEvent->getByLabel(SdmcCollTag, sdmccH);
  if (sdmccH.isValid()) mcdigis = sdmccH.product();
  
  TString opt = Opt;
  opt.ToLower();

  if ((opt == "") || (opt.Index("banner") >= 0)) {
    printf("-------------------------------------------------------------------\n");
    printf("    Energy       X          Y          Z        T0       NCH   NSH \n");
    printf("-------------------------------------------------------------------\n");
  }
  double caloClusterEnergy(-1);
  if (TimeCluster->caloCluster().get()!=0)  caloClusterEnergy = TimeCluster->caloCluster()->energyDep();

  if ((opt == "") || (opt.Index("data") >= 0)) {

    int nsh(0); 
    int nhits = TimeCluster->nhits();
					// loop over the combo hits and count the number of straw hits
    for (int i=0; i<nhits; i++) {
      int index = TimeCluster->hits().at(i);
					// pointers could be screwed up
      if (opt.Index("debug") < 0) {
	hit  = &(ChColl->at(index));
	nsh += hit->nStrawHits();
      }
    }

    printf("%10.3f %10.3f %10.3f %10.3f %10.3f %5i %5i\n",
	   caloClusterEnergy, 
	   TimeCluster->position().x(),
	   TimeCluster->position().y(),
	   TimeCluster->position().z(),
	   TimeCluster->t0().t0(),
	   nhits           ,
	   nsh             );

    if (opt.Index("hits") >= 0) {
//-----------------------------------------------------------------------------
// print straw hits in the list
//-----------------------------------------------------------------------------
      if (opt.Index("debug") < 0) printComboHit(0,nullptr,"banner",0,0);
      else                        printf("i  index(in SH)\n--------------\n");

      int  nhits = TimeCluster->nhits();

      const mu2e::StrawGasStep* step(nullptr);

      for (int i=0; i<nhits; i++) {
	int index = int(TimeCluster->hits().at(i));

	if (opt.Index("debug") < 0) {
	  hit         = &(ChColl->at(index));
	  vector<StrawDigiIndex> shids;
	  ChColl->fillStrawDigiIndices(*(fEvent),index,shids);
	  flags = 0;                  // *((int*) &TimeCluster->_shfcol->at(index));
	  step  = nullptr;
	  if (mcdigis) {
	    const mu2e::StrawDigiMC* sdmc = &mcdigis->at(shids[0]);
	    step  = sdmc->earlyStrawGasStep().get();
	  }
	  printComboHit(hit,step,"data",i,flags);
	}
	else {
//-----------------------------------------------------------------------------
// debug mode: print only an index and a location of the hit
//-----------------------------------------------------------------------------
	  printf("%5i %5i\n",i,index);
	}
      }
    }
  }
}

//-----------------------------------------------------------------------------
void TAnaDump::printTimeClusterCollection(const char* TcCollTag  , 
					  const char* ChCollTag  ,
					  int         hitOpt     ,
					  const char* SdmcCollTag) {

  art::Handle<mu2e::TimeClusterCollection>  tccH;
  const mu2e::TimeClusterCollection*        tcc(0);
  const mu2e::TimeCluster*                  tc (0);

  art::Handle<mu2e::ComboHitCollection>     chcH;
  const mu2e::ComboHitCollection*           chc(0);
//-----------------------------------------------------------------------------
// locate collections to be used
//-----------------------------------------------------------------------------
  fEvent->getByLabel(TcCollTag,tccH);
  if (tccH.isValid()) tcc = (mu2e::TimeClusterCollection* ) tccH.product();
  else {
    print_tc_colls();
    return;
  }

  fEvent->getByLabel(ChCollTag,chcH);
  if (chcH.isValid()) chc = (mu2e::ComboHitCollection* )chcH.product();
  else {
    print_ch_colls();
    return;
  }

  int banner_printed(0);

  for (auto j=tcc->begin(); j != tcc->end(); ++j) {
    tc = &(*j); // pointer to TimeCluster

    if (banner_printed == 0) {
      printTimeCluster(tc,"banner");
      banner_printed = 1;
    }

    if      (hitOpt == 0) printTimeCluster(tc,"data",chc,SdmcCollTag);
    else if (hitOpt == 1) {
//-----------------------------------------------------------------------------
// if hit printout has been requested, print banner in front of each cluster
//-----------------------------------------------------------------------------
      printTimeCluster(tc,"data+hits",chc,SdmcCollTag);
      banner_printed = 0;
    }
    else if (hitOpt == 2) {
//-----------------------------------------------------------------------------
// debug mode
//-----------------------------------------------------------------------------
      printTimeCluster(tc,"data+hits+debug",chc,SdmcCollTag);
      banner_printed = 0;
    }
  }
}

