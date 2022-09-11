#ifndef STNTUPLE_ALG_KrollWadaJoseph_hh
#define STNTUPLE_ALG_KrollWadaJoseph_hh
//Michael MacKenzie (2020)

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>

// cetlib includes
// #include "cetlib_except/exception.h"
// #include "cetlib/pow.h"

// CLHEP includes
// #include "CLHEP/Units/PhysicalConstants.h"

//ROOT includes
#include "TF1.h"
#include "TH1.h"
#include "TRandom.h"

class KrollWadaJosephInternalRadiativeCapture {

public :
  int verbose_; // Control output verbosity
  double alpha_;
  double me_;
  double mdecay_;
  double mnucleus_;
  bool  use_proton_values_;

public :
  KrollWadaJosephInternalRadiativeCapture(bool usePion) : verbose_(1) {
    // GlobalConstantsHandle<ParticleDataTable> pdt;
    me_    = 0.51099895; //pdt->particle(PDGCode::e_minus ).ref().mass().value(); //0.51099895;
    if(usePion)
      mdecay_= 139.57018; //pdt->particle(PDGCode::pi_minus).ref().mass().value(); //139.57018;
    else
      mdecay_= 105.6583755; //pdt->particle(PDGCode::mu_minus).ref().mass().value(); //105.6583755;
    alpha_ = 1./137.036; //CLHEP::fine_structure_const;
    mnucleus_ = 26.981539*931.494028;//GlobalConstantsHandle<PhysicsParams>()->getAtomicMass("Al");
    use_proton_values_ = false;
  }

  KrollWadaJosephInternalRadiativeCapture() : KrollWadaJosephInternalRadiativeCapture(false) { }
  KrollWadaJosephInternalRadiativeCapture(bool usePion, bool useProtonValues) : KrollWadaJosephInternalRadiativeCapture(usePion) {
    use_proton_values_ = useProtonValues;
  }

  void Print() {
    std::cout << "KrollWadaJosephInternalRadiativeCapture:" << std::endl
              << " M(e)       = " << me_ << std::endl
              << " M(decay)   = " << mdecay_ << std::endl
              << " M(nucleus) = " << mnucleus_ << std::endl
              << " alpha      = " << alpha_ << std::endl
              << " use_proton = " << use_proton_values_ << std::endl
              << " verbose    = " << verbose_ << std::endl;
  }

  double Probability(double x, double y, double E) {

    // mass of the recoiling system, neglecting the muon binding energy and nuclear recoil

    const double M = mnucleus_ + mdecay_ - E;

    // Set pdf to zero if x is out of bounds
    if (  x > E || x < 2*me_ ) {
      if(verbose_ > 0) std::cout << "KrollWadaJosephInternalRadiativeCapture::" << __func__
                                 << ": x out of bounds, returning 0\n";
      return 0.;
    }
    // Set pdf to zero if x or y are out of bounds
    const double eta = sqrt( 1 - (2*me_/x)*(2*me_/x) );
    if ( abs( y ) > eta ) {
      if(verbose_ > 0) std::cout << "KrollWadaJosephInternalRadiativeCapture::" << __func__
                                 << ": y out of bounds, returning 0\n";
      return 0.;
    }

    // Set parameters
    const static double rV  = 0.57;
    const static double muS = 0.064;

    const double xe = x/E;
    const double rT = (use_proton_values_) ? 1+rV*rV/3 - 2*muS*xe*xe : 1.;

    const double xl = (use_proton_values_) ? (1./(1-0.466*xe*xe)+1.88*(rV*rV/6 + muS*(1-xe*xe)))/(1+muS) : 0.;
    const double rL = 0.142*xl*xl;

    const double kRatio2 = ( (2*E*M + E*E)*(2*E*M + E*E) - 2*x*x*(2*M*M + 2*E*M + E*E ) + (x*x*x*x) )/((2*E*M + E*E)*(2*E*M + E*E));
    const double kRatio  = sqrt( kRatio2 );

    const double prefactor = ( (E+M)*(E+M) + M*M - x*x )/( (E+M)*(E+M) + M*M );

    const double trans     = rT*( ( 1+y*y )/x + 4*me_*me_/(x*x*x) );
    const double longit    = (use_proton_values_) ? rL*(1-y*y)*8*(E+M)*(E+M)*x/((2*E*M + E*E + x*x)*(2*E*M + E*E + x*x)) : 0.;

    const double prob      = alpha_/(4.*M_PI)*kRatio*prefactor*( trans + longit );

    if(verbose_ > 1) std::cout << "KrollWadaJosephInternalRadiativeCapture::" << __func__
                               << ": Probability  = " << prob << " for E = " << E
                               << " x = " << x << " y = " << y << std::endl
                               << " trans = " << trans << " longit = " << longit
                               << " prefactor = " << prefactor
                               << " kratio = " << kRatio
                               << " M = " << M << std::endl;
    if(prob < 0.) {
      //only print on verbose = 1 since > 1 already printed information
      if(verbose_ == 1) std::cout << "KrollWadaJosephInternalRadiativeCapture::" << __func__
                                  << ": Probability < 0 = " << prob << " for E = " << E
                                  << " x = " << x << " y = " << y << std::endl;
      return 0.;
    }
    return prob;
  }

};
#endif
