///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "TROOT.h"
#include "TFolder.h"
#include "TNamed.h"

#include <Stntuple/obj/TStnEvent.hh>
#include <Stntuple/obj/TStnHeaderBlock.hh>
#include <Stntuple/mod/InitStntupleDataBlocks.hh>
#include <Stntuple/mod/StntupleUtilities.hh>

#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/MCDataProducts/inc/ProtonBunchIntensity.hh"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art/Framework/Principal/Selector.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Provenance.h"

void stntuple_get_version(char* ver, char* test);

//_____________________________________________________________________________
Int_t StntupleInitMu2eHeaderBlock(TStnDataBlock* block, AbsEvent* AnEvent, int mode) 
{
  
  static TFolder*         fol     = NULL;
  static const char       oname[] = "StntupleInitMu2eHeaderBlock";

  if (! fol) {
//-----------------------------------------------------------------------------
//  initialize local static variables
//-----------------------------------------------------------------------------
    fol     = (TFolder*) gROOT->GetRootFolder()->FindObject("Stntuple");
  }

  TStnHeaderBlock* data = (TStnHeaderBlock*) block;

  data->fEventNumber   = AnEvent->event();
  data->fRunNumber     = AnEvent->run();
  data->fSectionNumber = AnEvent->subRun();
  if (data->fRunNumber < 100000) data->fMcFlag = 1.; // gblEnv->monteFlag();
  else                           data->fMcFlag = 0;

  data->fNTracks       = -1;
//-----------------------------------------------------------------------------
// instantaneous luminosity
//-----------------------------------------------------------------------------
  data->fInstLum       = -1.;
  data->fMeanLum       = -1.;

  art::Handle<mu2e::ProtonBunchIntensity> pbiHandle;
  AnEvent->getByLabel("protonBunchIntensity", pbiHandle);

  if (!pbiHandle.isValid()) {
    mf::LogWarning(oname) << " WARNING: no ProtonBunchIntensity objects found\n";
  }
  else {
//-----------------------------------------------------------------------------
// Loop over ProtonBunchIntensity objects
//-----------------------------------------------------------------------------
    const mu2e::ProtonBunchIntensity* pbi= pbiHandle.product();

    data->fInstLum = pbi->intensity();
    data->fMeanLum = -1. ; // pbi->meanIntensity();
  }
//-----------------------------------------------------------------------------
//  STNTUPLE version
//-----------------------------------------------------------------------------
  char  ver[200], text[200];
  stntuple_get_version(ver,text);
  data->fStnVersion = ver;
//-----------------------------------------------------------------------------
// number of straw hits
//-----------------------------------------------------------------------------
  char   strh_module_label[100], strh_description[100];

  data->GetModuleLabel("mu2e::StrawHitCollection",strh_module_label);
  data->GetDescription("mu2e::StrawHitCollection",strh_description );

  art::Handle<mu2e::StrawHitCollection> strh_handle;
  const mu2e::StrawHitCollection*       list_of_hits(0);

  if (strh_module_label[0] != 0) {
    if (strh_description[0] == 0) AnEvent->getByLabel(strh_module_label,strh_handle);
    else                          AnEvent->getByLabel(strh_module_label,strh_description,strh_handle);

    if (strh_handle.isValid()) {
      list_of_hits = strh_handle.product();
      data->fNStrawHits = list_of_hits->size();
    }
  }

  return 0;
}

//_____________________________________________________________________________
Int_t StntupleInitMu2eHeaderBlockLinks(TStnDataBlock* Block, AbsEvent* AnEvent, int Mode) 
{
  // Mu2e version, section number is defined

  Int_t  ev_number, rn_number;

  ev_number = AnEvent->event();
  rn_number = AnEvent->run();

  if (! Block->Initialized(ev_number,rn_number)) return -1;

					// do not do initialize links 2nd time

  if (Block->LinksInitialized()) return 0;

  TStnHeaderBlock* header = (TStnHeaderBlock*) Block;
//-----------------------------------------------------------------------------
// mark links as initialized
//-----------------------------------------------------------------------------
  header->fLinksInitialized = 1;

  return 0;
}


