//
#include "Stntuple/alg/TStntuple.hh"

//-----------------------------------------------------------------------------
// note different order of arguments in pbar_yield_ and pa2pbarx_
//-----------------------------------------------------------------------------
extern "C" {
  float pbar_d2n_ (float* PBeam, float* PLab , float* ThLab);
  float pa2pbarx_ (float* PLab , float* ThLab, float* PBeam);
  void  set_p2max_(float* P2Max);
}

//-----------------------------------------------------------------------------
// see mu2e-1776 for the fits
// function returns inclusive E*d^3sigma/dp^3 (pA --> pbar X) normalized to Ta
// PBeam - proton beam momentum [GeV/c]
// PLab, ThLab - momentum [GeV/c] and production angle [rad] of the outgoing antiproton
//-----------------------------------------------------------------------------
double TStntuple::PBar_Striganov_Ed3SigdP3(double PBeam, double PLab, double ThLab) {
  float pbeam(PBeam), plab(PLab), thlab(ThLab);
  double xsec = pa2pbarx_ (&plab,&thlab,&pbeam);
  return xsec;
}

//-----------------------------------------------------------------------------
// for given PBeam, PLab and ThLab, returns d^2Sigma(pA->pbarX)/dP/dTheta on Ta
// in the lab system 
//-----------------------------------------------------------------------------
double TStntuple::PBar_Striganov_d2N(double PBeam, double PLab, double ThLab) {
  float pbeam(PBeam), plab(PLab), thlab(ThLab);
  double xsec = pbar_d2n_(&pbeam,&plab,&thlab);
  return xsec;
}

//-----------------------------------------------------------------------------
void TStntuple::PBar_Striganov_SetP2Max(double P2Max) {
  float x = P2Max;
  set_p2max_(&x);
}
