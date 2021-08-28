///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "RecoDataProducts/inc/ComboHit.hh"
#include "GeometryService/inc/GeometryService.hh"
#include "GeometryService/inc/GeomHandle.hh"

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"
//-----------------------------------------------------------------------------
// print all ComboHitCollection's in the event
//-----------------------------------------------------------------------------
void print_ch_colls() {

  printf("Available ComboHitCollections: \n");

  const art::Event* event = TAnaDump::Instance()->Event();

  art::Selector  selector(art::ProductInstanceNameSelector(""));

  // std::vector<art::Handle<mu2e::ComboHitCollection>> vcoll;
  auto vcoll = event->getMany<mu2e::ComboHitCollection>(selector);

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
