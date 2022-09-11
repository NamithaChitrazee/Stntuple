///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"

#include "Offline/TrackerGeom/inc/Tracker.hh"

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/print/Stntuple_print_functions.hh"
//--------------------------------------------------------------------------------------------------------------
void print_combo_hit_coll(const char* ComboHitCollTag, const char* StrawHitCollTag) {

  bool ok;

  const art::Event* evt = TAnaDump::Instance()->Event();
  
  mu2e::GeomHandle<mu2e::Tracker> tH;
  const mu2e::Tracker* tracker = tH.get();
  
  int iev = evt->id().event();
  
  art::Handle<mu2e::ComboHitCollection> chH;
  ok = evt->getByLabel(art::InputTag(ComboHitCollTag),chH);
  if (! ok) { printf(" no ComboHitCollection %s\n",ComboHitCollTag);  return; }
    
  art::Handle<mu2e::StrawHitCollection> shH;
  ok = evt->getByLabel(art::InputTag(StrawHitCollTag),shH);
  if (! ok) { printf(" no StrawHitCollection %s\n",StrawHitCollTag);  return; }
    
  auto chcol     = chH.product();
  auto shcol     = shH.product();

  int nch = chcol->size();

  for (int i=0; i<nch; i++) {
    const mu2e::ComboHit* ch = &(chcol->at(i));

    int nsh = ch->nStrawHits();

    int flags = *((int*) &ch->flag());
    printf("%4i %2i 0x%08x %8.3f\n",i, ch->nStrawHits(), flags, ch->time());
    
    for (int ish=0; ish<nsh; ish++) {
      int lsh = ch->index(ish);
      const mu2e::StrawHit* sh = &(shcol->at(lsh));
      
      const mu2e::Straw* straw = &tracker->getStraw(sh->strawId());
      
      printf("        %2i %3i %3i %3i %3i %8.3f %8.3f %8.3f %8.3f\n",
	     lsh, 
	     straw->id().getPlane(),
	     straw->id().getPanel(),
	     straw->id().getLayer(),
	     straw->id().getStraw(),
	     sh->time(),
	     sh->TOT(),
	     sh->dt(),
	     sh->energyDep());
    }
  }

  printf(" event: %5i   nel : %5i\n",iev,nch);
}


//--------------------------------------------------------------------------------------------------------------
void print_combo_hit_coll_2(const char* ComboHitCollTag, const char* StrawHitCollTag) {

  bool ok;

  const art::Event* evt = TAnaDump::Instance()->Event();
  
  // mu2e::GeomHandle<mu2e::Tracker> tH;
  // const mu2e::Tracker* tracker = tH.get();
  
  int iev = evt->id().event();
  
  art::Handle<mu2e::ComboHitCollection> chH;
  ok = evt->getByLabel(art::InputTag(ComboHitCollTag),chH);
  if (! ok) { printf(" no ComboHitCollection %s\n",ComboHitCollTag);  return; }
    
  art::Handle<mu2e::StrawHitCollection> shH;
  ok = evt->getByLabel(art::InputTag(StrawHitCollTag),shH);
  if (! ok) { printf(" no StrawHitCollection %s\n",StrawHitCollTag);  return; }
    
  auto chcol     = chH.product();
  auto shcol     = shH.product();

  int nch = chcol->size();

  for (int i=0; i<nch; i++) {
    const mu2e::ComboHit* ch = &(chcol->at(i));

    int nsh = ch->nStrawHits();
    
    float tm = 0;
    
    for (int ish=0; ish<nsh; ish++) {
      int lsh = ch->index(ish);
      const mu2e::StrawHit* sh = &(shcol->at(lsh));
      
      // const mu2e::Straw* straw = &tracker->getStraw(sh->strawId());
      
      // printf("        %2i %3i %3i %3i %3i %8.3f %8.3f %8.3f %8.3f\n",
      // 	     lsh, 
      // 	     straw->id().getPlane(),
      // 	     straw->id().getPanel(),
      // 	     straw->id().getLayer(),
      // 	     straw->id().getStraw(),
      // 	     sh->time(),
      // 	     sh->TOT(),
      // 	     sh->dt(),
      // 	     sh->energyDep());
      
      tm += sh->time()+sh->TOT();
    }

    tm = tm/nsh;

    printf("%4i %2i %8.3f %8.3f\n",i, ch->nStrawHits(), ch->time(), tm);
  }

  printf(" event: %5i   nel : %5i\n",iev,nch);
}

