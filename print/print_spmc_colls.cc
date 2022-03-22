///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "Offline/MCDataProducts/inc/StepPointMC.hh"

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"
//-----------------------------------------------------------------------------
// print all StepPointMCCollection's in the event
//-----------------------------------------------------------------------------
void print_spmc_colls() {
  printf("Available StepPointMCCollections: \n");

  const art::Event* event = TAnaDump::Instance()->Event();

  art::Selector  selector(art::ProductInstanceNameSelector(""));
  auto vcoll = event->getMany<mu2e::StepPointMCCollection>(selector);

  for (auto handle = vcoll.begin(); handle != vcoll.end(); handle++) {
    if (handle->isValid()) {
      const art::Provenance* prov = handle->provenance();
      
      printf("moduleLabel: %-20s, productInstanceName: %-20s, processName:= %-30s\n" ,
	     prov->moduleLabel().data(),
	     prov->productInstanceName().data(),
	     prov->processName().data()
	     );
    }
  }
  return;
}
