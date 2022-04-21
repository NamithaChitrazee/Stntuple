// non-rejected physics lists
// 3,5,6,10,11,16,17,21

void su2020_default(stntuple::Mu2e_model* M) {
  printf("---------------------- %s START\n",__func__);

  M->cr_lo_bkg      = (M->fUseLYSafetyFactor ) ? 2.91161e-01 : 4.63259e-02; // 4.74529e-2;
  M->cr_lo_frac_unc = M->scaleUncertainty_*0.20;
  M->cr_hi_bkg      = 0.;                     // 0.02; //see docdb-38052 slide 20: COMBINED INTO CRV_LO FOR NOW
  M->cr_hi_frac_unc = M->scaleUncertainty_*0.50;

  // consider our current uncertainty on the light-yield at data-taking time as a systematic on the measurement
  M->cr_lo_ly       = M->cr_lo_bkg;
  M->cr_hi_ly       = 0.5;               // FIXME: increased to not be negative with high value

  M->dio_bkg           = (M->fUseLYSafetyFactor)  ? 6.05779e-02 : 3.75955e-02; // 3.90553e-02;  // 3.77436e-2;
  M->dio_frac_unc      = M->scaleUncertainty_*0.63;        // using systematic upper estimate with statistical error
//-----------------------------------------------------------------------------
// RPC
//-----------------------------------------------------------------------------
  M->rpc_bkg           = (M->fUseLYSafetyFactor)  ? 2.31683e-02 : 1.06887e-02; // 1.11215e-02;  // 1.057920e-2;
  M->rpc_frac_unc      = M->scaleUncertainty_*0.29;        // using systematic upper estimate with statistical error

  M->extinction        = 0.;                            // in units of 10^-10, FIXME: Get expected value
  M->rpc_oot_bkg       = 14.5e-4*M->extinction;            // see docdb-36503 section 11.3, scaled 13.9 by (tmax - tmin) / (1695 - 700)
  M->rpc_oot_frac_unc  = M->scaleUncertainty_*0.12;        // using systematic upper estimate with statistical error

  printf("---------------------- %s DONE\n",__func__);
}
