#ifndef __su2020_stat_FCCalculator_hh__
#define __su2020_stat_FCCalculator_hh__

#include <set>
#include "Math/ProbFunc.h"
#include "Math/DistFunc.h"

#include "Stntuple/stat/Poisson_t.hh"

namespace stntuple {

//////////////////////////////////////////////////////////////
// class to perform Feldman-Cousins limits
//////////////////////////////////////////////////////////////

  class FCCalculator {
  public:

    Poisson_t* model_;
    var_t*     fPoi;
    double     cl_;
    int        verbose_;
    TH1D*      hNull_;
    double     null_mu_;
    int        null_min_;
    int        null_max_;
    double     res_;

    FCCalculator(Poisson_t* model, var_t* poi, double cl = 0.9, int verbose = 0);
    
    //Number of events needed to be seen to be >x sigma on the right tail
    int NSigmaThreshold(TH1D* hPDF, double nsigma);
    
    //Get the median of the distribution
    int GetMedian(TH1D* hPDF, double Prob);
    
    //Find the minimum value of the POI that has a median of n.. TODO update comment
    double FindForMedianN(int n, double Prob = 0.5);
    
    //For a given PDF, construct the FC interval in N(observed)
    void CalculateIndividualInterval(TH1D* hPDF, int& nmin, int& nmax);
    
    //Get the upper or lower limit for the model given an observation
    double FindLimit(int nobs, bool upperLimit);
    
    //Calculate the interval for a given observation
    void CalculateInterval(int nobs, double& mu_min, double& mu_max);
  };
}
#endif
