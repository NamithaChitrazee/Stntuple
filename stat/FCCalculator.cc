#include <set>
#include "Math/ProbFunc.h"
#include "Math/DistFunc.h"

#include "TH1D.h"
#include "Stntuple/stat/FCCalculator.hh"

namespace stntuple {

  //////////////////////////////////////////////////////////////
  // class to perform Feldman-Cousins limits
  //////////////////////////////////////////////////////////////

  FCCalculator::FCCalculator(Poisson_t* model, var_t* Poi, double cl, int verbose) :
    model_(model), fPoi(Poi), cl_(cl), verbose_(verbose), res_(1.e-3) {

    double oldval = fPoi->val_;
    fPoi->val_    = 0.;

    hNull_        = model->GeneratePDF();
    hNull_->SetName("FC_NULL");
    null_mu_      = hNull_->GetMean();
    
    if(verbose > 0) printf("%s: Null mean is %.3e events\n", __func__, null_mu_);
    fPoi->val_      = oldval;
    CalculateIndividualInterval(hNull_, null_min_, null_max_);
    if(null_min_ > 0) {
      printf("!!! %s: N(obs) = 0 is not contained within the NULL hypothesis!\n", __func__);
    }
    if(verbose_ > 0) printf("%s: Null interval at %.3f CL: %i - %i\n", __func__, cl, null_min_, null_max_);
  }

  //Number of events needed to be seen to be >x sigma on the right tail
  int FCCalculator::NSigmaThreshold(TH1D* hPDF, double nsigma) {
    if(nsigma < 0.) return 0; //ignore this region
    //for numerical reasons, consider 1 - p(nsigma) --> 0 instead of p(nsigma) --> 1
    const double psigma = ROOT::Math::gaussian_cdf(-nsigma);
    double p = 1.;
    int nbins = hPDF->GetNbinsX();
    int n = -1;
    if(verbose_ > 1) printf("%s: Printing threshold calculation:\n", __func__);
    while(p > psigma) {
      ++n;
      p = hPDF->Integral(n + 1, nbins);
      if(verbose_ > 1) printf(" n = %2i P(n' >= n) = %.3e\n", n, p);
    }
    return n;
  }

  //Get the median of the distribution
  int FCCalculator::GetMedian(TH1D* hPDF, double Prob) {
    double p = 0.;
    int n = -1;
    while(p < Prob) {
      ++n;
      p += hPDF->GetBinContent(n+1);
    }
    return n;
  }

  //Find the minimum value of the POI that has a median of n
  double FCCalculator::FindForMedianN(int n, double Prob) {
    double mu_max = fPoi->max_;
    double mu_min = fPoi->min_;
    while(abs(mu_max - mu_min)/(mu_max+mu_min) > res_) {
      const double mu = (mu_max + mu_min) / 2.;
      fPoi->val_ = mu;
      TH1D* h = model_->GeneratePDF();
      int median = GetMedian(h,Prob);
      if(median >= n) mu_max = mu; //mu satisfies criteria, so set as maximum
      else            mu_min = mu; //mu fails, so must be larger
      delete h;
    }
    return (mu_min + mu_max) / 2.;
  }

  //For a given PDF, construct the FC interval in N(observed)
  void FCCalculator::CalculateIndividualInterval(TH1D* hPDF, int& nmin, int& nmax) {

    //    printf("FCCalculator::%s model->obs_.max_: %10.3f\n",__func__,model_->obs_.max_);
    
    
    nmin = hPDF->GetNbinsX();
    nmax = 0;
    double p = 0.;
    //mu_bkg for the denominator of the likelihood ratio ordering parameter
    double mu = null_mu_; //hNull_->GetMean() - 0.5; //bins are centered at n + 0.5

    double pdf_integral = hPDF->Integral();

    printf("[FCCalculator::%s] cl_ : %12.5e integral: %12.5e nbinsx: %4i\n",__func__,
	   cl_,pdf_integral,hPDF->GetNbinsX());
	   
    if (pdf_integral < cl_) {
      printf("!!! [FCCalculator::%s: PDF doesn't have the range to calculate the confidence level precision!\n", __func__);
      nmax = nmin;
      nmin = 0;
      return;
    }
    std::set<int> ns;
    if(verbose_ > 2) printf("[FCCalculator::%s: Printing interval construction:\n", __func__);
    while(p < cl_) {
      int nbest = -1;
      double rbest = -1.;
      for(int n = 0; n < hPDF->GetNbinsX(); ++n) {
	if(ns.count(n)) continue;
	double r = hPDF->GetBinContent(n+1) / ROOT::Math::poisson_pdf(n, std::max(mu, (double) n));
	if(r > rbest) {
	  rbest = r;
	  nbest = n;
	}
      }
      p += hPDF->GetBinContent(nbest+1);
      nmin = std::min(nbest, nmin);
      nmax = std::max(nbest, nmax);
      ns.insert(nbest);
      if(verbose_ > 9) printf(" n = %i, r = %.3e, p = %.3f\n", nbest, rbest, p);
    }
    if(verbose_ > 2) printf(" Final interval: %i - %i\n", nmin, nmax);
  }

  //Get the upper or lower limit for the model given an observation
  double FCCalculator::FindLimit(int nobs, bool upperLimit) {

    //    printf("FCCalculator::%s model->obs_.max_: %10.3f\n",__func__,model_->obs_.max_);

    int attempts = 0;
    const int maxAttempts = 100;
    double mu_min   = 0.;
    double mu_max   = fPoi->max_;
    double mu_range = fPoi->max_ - fPoi->min_;

    while(abs((mu_max - mu_min) / mu_range) > res_ && attempts < maxAttempts) {
      ++attempts;
      double mu = (mu_max + mu_min) / 2.;
      fPoi->val_ = mu;
      if (verbose_ > 2) model_->Print();
      TH1D* h = model_->GeneratePDF();
      int nmin, nmax;
      CalculateIndividualInterval(h, nmin, nmax);
      if(upperLimit) {
	if(nmin > nobs) mu_max = mu; //gone past allowed
	else mu_min = mu; //still allowed
      } else {
	if(nmax < nobs) mu_min = mu; //gone past allowed
	else mu_max = mu; //still allowed
      }
      delete h;
      if(verbose_ > 1) printf("%s: Attempt %i has bounds %.3f - %.3f\n", __func__, attempts, mu_min, mu_max);
    }
    if (attempts == maxAttempts) {
      printf("!!! %s: Hit maximum limit finding attempts for N(obs) = %i, mu in %.3f - %.3f (upper = %o)\n",
	     __func__, nobs, mu_min, mu_max, upperLimit);
    }
    return (mu_max + mu_min) / 2.;
  }

  //Calculate the interval for a given observation
  void FCCalculator::CalculateInterval(int nobs, double& mu_min, double& mu_max) {
    //    printf("FCCalculator::%s model->obs_.max_: %10.3f\n",__func__,model_->obs_.max_);
    
    //first test if mu = 0 is included in the interval
    if(nobs > null_max_) {
      mu_min = FindLimit(nobs, false);
    } else {
      mu_min = 0.;
    }
    mu_max = FindLimit(nobs, true);
  }
}

