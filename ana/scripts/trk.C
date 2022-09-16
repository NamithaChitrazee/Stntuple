///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/scripts/global_vars.h"
#include "Stntuple/ana/scripts/modules.hh"

def_name stn_trk_001("stn_trk_ana");
def_name stn_trk_002("stn_trk_ana_05");

void stn_trk_ana(const char* TrackBlockName = "TrackBlock", int PdgCode = 11, int GeneratorCode = 56) {
//-----------------------------------------------------------------------------
// configure validation module
//-----------------------------------------------------------------------------
  stntuple::m_trk = (stntuple::TTrackAnaModule*) g.x->AddModule("stntuple::TTrackAnaModule",0);  

  stntuple::m_trk->SetTrackBlockName(TrackBlockName);
  stntuple::m_trk->SetPdgCode       (PdgCode);
  stntuple::m_trk->SetGeneratorCode (GeneratorCode);
}


void stn_trk_ana_05(const char* TrackBlockName = "TrackBlock", int PdgCode = 11, int GeneratorCode = 56) {
//-----------------------------------------------------------------------------
// configure validation module
//-----------------------------------------------------------------------------
  stntuple::m_trk = (stntuple::TTrackAnaModule*) g.x->AddModule("stntuple::TTrackAnaModule",0);  

  stntuple::m_trk->SetTrackBlockName(TrackBlockName);
  stntuple::m_trk->SetPdgCode       (PdgCode);
  stntuple::m_trk->SetGeneratorCode (GeneratorCode);

  stntuple::m_trk->SetBField (0.5);
}
