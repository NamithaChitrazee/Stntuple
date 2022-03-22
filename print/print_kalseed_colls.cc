///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "Offline/RecoDataProducts/inc/KalSeed.hh"

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"
//-----------------------------------------------------------------------------
// print all HelixSeed collections in the event
//-----------------------------------------------------------------------------
void print_kalseed_colls() {

  printf("--------------------------- Available mu2e::KalSeedCollection\'s: \n");

  const art::Event* event = TAnaDump::Instance()->Event();

  art::Selector  selector(art::ProductInstanceNameSelector(""));

  // std::vector<art::Handle<mu2e::KalSeedCollection>> vcoll;
  auto vcoll = event->getMany<mu2e::KalSeedCollection>(selector);

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
