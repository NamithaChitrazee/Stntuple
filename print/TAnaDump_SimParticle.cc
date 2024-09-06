//
#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"
#include "Stntuple/geom/TCrvNumerology.hh"

#include "TROOT.h"
#include "TVector2.h"

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Selector.h"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"

#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/CalorimeterGeom/inc/DiskCalorimeter.hh"
#include "Offline/CalorimeterGeom/inc/Calorimeter.hh"

#include "Offline/MCDataProducts/inc/GenParticle.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/StrawGasStep.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"
#include "Offline/MCDataProducts/inc/PhysicalVolumeInfo.hh"
#include "Offline/MCDataProducts/inc/PhysicalVolumeInfoMultiCollection.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME         "TAnaDump_SimParticle"

using namespace std;

//-----------------------------------------------------------------------------------------------------
// if PrintData non-null, it is a pointer to the index ..
//-----------------------------------------------------------------------------------------------------
void TAnaDump::printSimParticle(const mu2e::SimParticle* P, const char* Opt, const void* PrintData) {

    TString opt = Opt;

    if ((opt == "") || (opt.Index("banner") >= 0)) {
      printf("-----------------------------------------------------------------------------------------");
      printf("-----------------------------------------");
      printf("---------------------------------------------------------------------------------------\n");
      printf("Index Primary     ID Parent     GenpID        PDG      X0          Y0         Z0         ");
      printf("T0       Px0       Py0      Pz0        E0 ");
      printf("        X1         Y1           Z1        T1         Px1       Py1      Pz1        E1  \n");
      printf("-----------------------------------------------------------------------------------------");
      printf("------------------------------------------");
      printf("---------------------------------------------------------------------------------------\n");
    }
 
    if ((opt == "") || (opt.Index("data") >= 0)) {
      int  id        = P->id().asInt();
      
      int  parent_id (-1);

      if (P->parent()) {
	parent_id = P->parent()->id().asInt();
      }
      int  pdg_id    = P->pdgId();
      int  primary   = P->isPrimary();

      int index (-1.);
      if (PrintData) index = *((int*) PrintData);

      printf("%5i %7i %6i %6i %10i %10i",
	     index, primary, id, parent_id, 
	     P->generatorIndex(), pdg_id);

      printf(" %10.3f %10.3f %10.3f %9.3f %9.3f %9.3f %9.3f %9.3f",
	     P->startPosition().x(),
	     P->startPosition().y(),
	     P->startPosition().z(),
	     P->startGlobalTime(),
	     P->startMomentum().x(),
	     P->startMomentum().y(),
	     P->startMomentum().z(),
	     P->startMomentum().e());

      printf(" %10.3f %10.3f %10.3f %10.3f %9.3f %9.3f %9.3f %9.3f\n",
	     P->endPosition().x(),
	     P->endPosition().y(),
	     P->endPosition().z(),
	     P->endGlobalTime(),
	     P->endMomentum().x(),
	     P->endMomentum().y(),
	     P->endMomentum().z(),
	     P->endMomentum().e());
    }
  }



//-----------------------------------------------------------------------------
void TAnaDump::printSimParticleCollection(const char* ModuleLabel, 
					  const char* ProductName, 
					  const char* ProcessName) {

  art::InputTag tag(ModuleLabel,ProductName,ProcessName);
  printSimParticleCollection(tag);                             
}

//-----------------------------------------------------------------------------
void TAnaDump::printSimParticleCollection(const art::InputTag& CollTag) { 

  art::Handle<mu2e::SimParticleCollection> handle;
  const mu2e::SimParticleCollection*       coll(0);
  const mu2e::SimParticle*                 simp(0);

  try { coll = fEvent->getValidHandle<mu2e::SimParticleCollection>(CollTag).product(); }
  catch (...) {
    TLOG(TLVL_ERROR) << "failed to locate SimParticleCollection " << CollTag << ". BAIL OUT";
    return;
  }

  int banner_printed(0);

  int np = coll->size();

  int i = 0;
  for ( mu2e::SimParticleCollection::const_iterator j=coll->begin(); j != coll->end(); ++j) {
    simp = &j->second;

    if (banner_printed == 0) {
      printSimParticle(simp,"banner",&i);
      banner_printed = 1;
    }
    printSimParticle(simp,"data",&i);
    i++;
  }

  if (i != np) {
    printf(" inconsistency in TAnaDump::printSimParticleCollection\n");
  }
}
