////////////////////////////////////////////////////////////////////////////////

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Selector.h"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"

#include "Offline/TrackerGeom/inc/Tracker.hh"

#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/RecoDataProducts/inc/HelixSeed.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/TrkStrawHitSeed.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"

using namespace std;
//-----------------------------------------------------------------------------
void TAnaDump::evalHelixInfo(const mu2e::HelixSeed* Helix,
			     int&                   NLoops,
			     int&                   NHitsLoopFailed){
  const mu2e::ComboHit*     hit(0);

  //init
  NLoops          = 0;
  NHitsLoopFailed = 0;
  
  int           nMinHitsLoop(3), nHitsLoop(0), nHitsLoopChecked(0);
  float         meanHitRadialDist(0), z_first_hit(0), z_last_hit(0), counter(0); 
  bool          isFirst(true);
  float         half_pitch  =  M_PI*fabs(Helix->_helix._lambda);
  float         dz_min_toll = 600.;
  unsigned      nhits = Helix->_hhits.size();

  for (unsigned f=0; f<nhits; ++f){
    hit = &Helix->_hhits[f];
    if (hit->_flag.hasAnyProperty(mu2e::StrawHitFlag::outlier))     continue;
      
    meanHitRadialDist += sqrtf(hit->pos().x()*hit->pos().x() + hit->pos().y()*hit->pos().y());
    ++counter;
    float z = hit->pos().z();
    if (isFirst){
      z_first_hit = z;
      z_last_hit  = z;
      nHitsLoop   = 1;
      isFirst     = false;
    }else {
      float    dz_last_hit  = z - z_last_hit;
      float    dz_first_hit = z - z_first_hit;

      if ( ( dz_first_hit < half_pitch) && ( dz_last_hit < dz_min_toll)){
	++nHitsLoop;
	z_last_hit        = z;
      } else {
	if (nHitsLoop >= nMinHitsLoop) {
	  ++NLoops;
	  nHitsLoopChecked +=  nHitsLoop;
	}
	nHitsLoop = 0;

	if ( (z - z_last_hit) >= half_pitch){
	  //reset the hits-per-loop counter
	  nHitsLoop = 1;
	      
	  //re-set the values of the first and last z of the hits within the loop
	  z_first_hit = z;
	  z_last_hit  = z;
	}
      }
    }
	
  }//end loop over the hits

  if (counter > 0) meanHitRadialDist /= counter;
  if (nHitsLoop >= nMinHitsLoop) {
    ++NLoops;
    nHitsLoopChecked +=  nHitsLoop;
  }
      
  NHitsLoopFailed   =  nHitsLoopChecked;
}

  
//-----------------------------------------------------------------------------
void TAnaDump::printHelixHit(const mu2e::HelixHit*    HelHit, const mu2e::ComboHit* Hit, 
			     const mu2e::StrawGasStep* Step, const char* Opt, int IHit, int Flags) {
  TString opt = Opt;
  opt.ToLower();

  if ((opt == "") || (opt.Index("banner") >= 0)) {
    printf("----------------------------------------------------------------------------------------");
    printf("------------------------------------------------------------------\n");
    printf("   I NSH  SHID   Flags  Pl Pn L  S      x         y         z       phi    Time    eDep ");
    printf("       PDG     PDG(M)  GenID      SimID      p       pT         pZ\n");
    printf("----------------------------------------------------------------------------------------");
    printf("------------------------------------------------------------------\n");
  }

  if (opt == "banner") return;

  mu2e::GeomHandle<mu2e::Tracker> ttHandle;
  const mu2e::Tracker* tracker = ttHandle.get();
  
  const mu2e::Straw* straw;
  
  straw = &tracker->getStraw(Hit->strawId());//ndex());
  
  const mu2e::SimParticle * sim (0);
  
  int      pdg_id(-1), mother_pdg_id(-1), generator_id(-1), sim_id(-1);
  double   mc_mom(-1.), mc_pT(-1.), mc_pZ(0.);
  double   x(0), y(0), z(0), phi(0);
  
  const XYZVectorF shPos = HelHit->pos();
  x      = shPos.x();
  y      = shPos.y();
  z      = shPos.z();    
  phi    = HelHit->helixPhi();
    
  mu2e::GenId gen_id;

  if (Step) {
    art::Ptr<mu2e::SimParticle> const& simptr = Step->simParticle(); 
    art::Ptr<mu2e::SimParticle> mother = simptr;
    
    while(mother->hasParent()) mother = mother->parent();
    
    sim = mother.operator ->();
    
    pdg_id        = simptr->pdgId();
    mother_pdg_id = sim->pdgId();
    
    if (simptr->fromGenerator()) generator_id = simptr->genParticle()->generatorId().id();
    else                         generator_id = -1;
    
    sim_id        = simptr->id().asInt();
    mc_mom        = Step->momvec().mag();
    mc_pT         = sqrt(Step->momvec().x()*Step->momvec().x() + Step->momvec().y()*Step->momvec().y());
    mc_pZ         = Step->momvec().z();
  }
  
  if ((opt == "") || (opt == "data")) {
    if (IHit  >= 0) printf("%5i " ,IHit);
    else            printf("    ");
    
    printf(" %3i ",HelHit->nStrawHits());
    
    printf("%5i",Hit->strawId().asUint16());
    
    if (Flags >= 0) printf(" %08x",Flags);
    else            printf("        ");
    printf(" %2i %2i %1i %2i  %8.3f  %8.3f %9.3f %6.3f %8.3f %6.3f %10i %10i %6i %10i %8.3f %8.3f %8.3f\n",
	   straw->id().getPlane(),
	   straw->id().getPanel(),
	   straw->id().getLayer(),
	   straw->id().getStraw(),
	   x, y, z, phi,
	   Hit->time(),
	   Hit->energyDep(),
	   pdg_id,
	   mother_pdg_id,
	   generator_id,
	   sim_id,
	   mc_mom,
	   mc_pT,
	   mc_pZ);
  }
}

//-----------------------------------------------------------------------------
void TAnaDump::printHelixSeed(const mu2e::HelixSeed* Helix          , 
			      const char*            StrawHitCollTag, 
			      const char*            StrawDigiMCCollTag,
			      const char*            Opt                ) {
  TString opt(Opt);
  
  if ((opt == "") || (opt == "banner")) {
    printf("------------------------------------------------------------------");
    printf("--------------------------------------------------------------------------------------\n");
    printf("  HelID   Address    N nL nCln     P        pT      T0     T0err  ");
    printf("    D0      FZ0      X0       Y0    Lambda    radius   ECal   chi2XY  chi2ZPhi    flag\n");
    printf("------------------------------------------------------------------");
    printf("--------------------------------------------------------------------------------------\n");
  }
 
  if ((opt == "") || (opt.Index("data") >= 0)) {

    const mu2e::RobustHelix*robustHel = &Helix->helix();

    if ((opt == "") || (opt.Index("data") >= 0)) {
      int    nhits  = Helix->hits().size();
      int    flag   = *((int*) &Helix->status()); 

      double t0     = Helix->t0()._t0;
      double t0err  = Helix->t0()._t0err;

      double fz0    = robustHel->fz0();
      //    double phi0   = robustHel->phi0();
      double radius = robustHel->radius();
      double d0     = robustHel->rcent() - radius;

      double lambda = robustHel->lambda();
      //      double tandip = lambda/radius;

      double mm2MeV = 3/10.;
      double mom    = robustHel->momentum()*mm2MeV;
      double pt     = radius*mm2MeV;

      double x0     = robustHel->centerx();
      double y0     = robustHel->centery();
    
      int    nLoops(0), nhitsLoopChecked(0);
      evalHelixInfo(Helix, nLoops, nhitsLoopChecked);

      const mu2e::CaloCluster*cluster = Helix->caloCluster().get();
      double clusterEnergy(-1);
      if (cluster != 0) clusterEnergy = cluster->energyDep();
      printf("%5i %12p %3i %2i %4i %8.3f %8.3f %7.3f %7.3f",
	     -1,
	     static_cast<void*>(Helix),
	     nhits,
	     nLoops, 
	     nhitsLoopChecked,
	     mom, pt, t0, t0err );

      float chi2xy   = robustHel->chi2dXY();
      float chi2zphi = robustHel->chi2dZPhi();

      printf(" %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %7.3f %8.3f %8.3f %08x\n",
	     d0,fz0,x0,y0,lambda,radius,clusterEnergy,chi2xy,chi2zphi, flag);
    }

    if ((opt == "") || (opt.Index("hits") >= 0) ){
      int nsh = Helix->hits().size();

      art::Handle<mu2e::ComboHitCollection> shcHandle;
      const mu2e::ComboHitCollection*       shcol;

      // const char* ProductName = "";
      // const char* ProcessName = "";
//-----------------------------------------------------------------------------
// get ComboHitCollection with single straw hits (makeSH)
//-----------------------------------------------------------------------------
      const mu2e::StrawDigiMCCollection* mcdigis(0);
      art::Handle<mu2e::StrawDigiMCCollection> mcdH;
      fEvent->getByLabel(StrawDigiMCCollTag, mcdH);
      if (mcdH.isValid()) mcdigis = mcdH.product();
      else {
	printf("ERROR in TAnaDump::printHelixSeed : no StrawDigiMCCollection tag=%s, BAIL OUT\n",StrawDigiMCCollTag);
	return;
      }

      fEvent->getByLabel(StrawHitCollTag,shcHandle);
      if (shcHandle.isValid()) shcol = shcHandle.product();
      else {
	printf("ERROR in TAnaDump::printHelixSeed : no StrawHitCollection tag=%s, BAIL OUT\n",StrawHitCollTag);
	return;
      }

      int banner_printed(0);
      for (int i=0; i<nsh; ++i){
	const mu2e::ComboHit*  helHit = &Helix->hits().at(i);
	int  hitIndex                 = helHit->index(0);     // index of the first straw hit
      
	vector<StrawDigiIndex> shids;
	Helix->hits().fillStrawDigiIndices(*(fEvent),i,shids);

	const mu2e::ComboHit* hit     = &shcol->at(hitIndex);
 
	const mu2e::StrawDigiMC* sdmc = &mcdigis->at(shids[0]);

	const mu2e::StrawGasStep* step(nullptr);

	if (sdmc->wireEndTime(mu2e::StrawEnd::cal) < sdmc->wireEndTime(mu2e::StrawEnd::hv)) {
	  step = sdmc->strawGasStep(mu2e::StrawEnd::cal).get();
	}
	else {
	  step = sdmc->strawGasStep(mu2e::StrawEnd::hv ).get();
	}
    
	if (banner_printed == 0) {
	  printHelixHit(helHit, hit, step, "banner", -1, 0);
	  banner_printed = 1;
	} 
	printHelixHit(helHit, hit, step, "data", -1, 0);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void TAnaDump::printHelixSeedCollection(const char* HelixSeedCollTag, 
					int         PrintHits       ,
					const char* StrawHitCollTag ,
					const char* StrawDigiMCCollTag) {
  
  const mu2e::HelixSeedCollection*       list_of_helixSeeds;
  art::Handle<mu2e::HelixSeedCollection> hsH;

  fEvent->getByLabel<mu2e::HelixSeedCollection>(HelixSeedCollTag,hsH);

  if (hsH.isValid()) list_of_helixSeeds = hsH.product();
  else {
    printf("ERROR: cant find HelixSeedCollection tag=%s, avalable collections are:\n",HelixSeedCollTag);
    print_helix_seed_colls();
    return;
  }

  int nhelices = list_of_helixSeeds->size();

  const mu2e::HelixSeed *helix;

  int banner_printed = 0;

  char popt[20];
  strcpy(popt,"data");
  if (PrintHits > 0) strcat(popt,"+hits");

  art::InputTag sdmc_coll_tag = StrawDigiMCCollTag;
  if (sdmc_coll_tag == nullptr) sdmc_coll_tag = fSdmcCollTag;

  for (int i=0; i<nhelices; i++) {
    helix = &list_of_helixSeeds->at(i);
    if (banner_printed == 0) {
      printHelixSeed(helix,StrawHitCollTag,sdmc_coll_tag.encode().data(),"banner"); 
      banner_printed = 1;
    }

    printHelixSeed(helix,StrawHitCollTag,sdmc_coll_tag.encode().data(),popt);
  }
}

