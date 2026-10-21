///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/ana/scripts/modules.hh"

def_name stn_simp_001("stn_simp_ana");

//-----------------------------------------------------------------------------
void  stn_simp_ana(int DebugBit = -1) {
  stntuple::m_sim = (stntuple::TSimpAnaModule*) g.x->AddModule("stntuple::TSimpAnaModule",0);
  if (DebugBit >= 0) stntuple::m_sim->SetDebugBit(DebugBit,1);
}
