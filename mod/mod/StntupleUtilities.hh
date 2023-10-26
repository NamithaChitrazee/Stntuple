//-----------------------------------------------------------------------------
//  Jan 11 2001 P.Murat: declarations for a set of STNTUPLE utility routines
//-----------------------------------------------------------------------------
#ifndef StntupleUtilities_hh
#define StntupleUtilities_hh

#include "Offline/MCDataProducts/inc/StepPointMC.hh"

#include <vector>

namespace stntuple {

  double get_proper_time(const mu2e::SimParticle* Simp);

}
#endif
