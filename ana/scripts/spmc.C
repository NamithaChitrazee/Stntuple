///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
#include "Stntuple/ana/scripts/modules.hh"

def_name stn_spmc_001("stn_spmc_ana");
def_name stn_spmc_002("stn_spmc_g4s4_ana");
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
void  stn_spmc_ana(int DebugBit = -1) {
  stntuple::m_spmc = (stntuple::TSpmcAnaModule*) g.x->AddModule("stntuple::TSpmcAnaModule",0);  
  if (DebugBit >= 0) stntuple::m_spmc->SetDebugBit(DebugBit,1);
}

//-----------------------------------------------------------------------------
void  stn_spmc_g4s4_ana(int DebugBit = -1) {
  stntuple::m_spmc = (stntuple::TSpmcAnaModule*) g.x->AddModule("stntuple::TSpmcAnaModule",0);  
  stntuple::m_spmc->SetSpmcBlockName("SpmcTrackerBlock");
  stntuple::m_spmc->SetVDetBlockName("SpmcVDetBlock");
  if (DebugBit >= 0) stntuple::m_spmc->SetDebugBit(DebugBit,1);
}

