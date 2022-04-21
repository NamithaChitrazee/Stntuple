////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
#include "TH1D.h"

#include "Stntuple/stat/Poisson_t.hh"

namespace stntuple {

  Poisson_t::Poisson_t(TString name, int nmax, std::vector<var_t*> mu, std::vector<var_t*> sys) :
    TNamed(name,name),mu_(mu), sys_(sys)
  {
    verbose_ = 0;
    nmax_    = nmax;
    ngen_    = 1.e5;
    fRn      = new TRandom3(90);
  }

  double Poisson_t::GetMean() {
    double mu = 0.;
    if (verbose_ > 9) printf("Poisson_t::GetMean START\n");

    for(var_t* var : mu_) {
      mu += var->get_val();
      if(verbose_ > 9) printf("%-30s : %10.4f\n",var->GetName(),var->get_val());
    }

    if (verbose_ > 9) printf("Poisson_t::GetMean END: mu = %10.4f\n",mu);
    return mu;
  }

  double Poisson_t::GetNominalMean() {
    double mu = 0.;
    for(var_t* var : sys_) {
      var->val_ = var->nom_;
    }
    for(var_t* var : mu_) {
      var->val_ = var->nom_;
      mu       += var->get_val();
    }
    if (verbose_ > 9) std::cout << __func__ << ": mu = " << mu << std::endl;
    return mu;
  }

  double Poisson_t::Eval(int n) {
    const double mu = GetMean();
    const double p = ROOT::Math::poisson_pdf(n, mu);
    if (verbose_ > 9) std::cout << __func__ << ": p = " << p << std::endl;
    return p;
  }

//-----------------------------------------------------------------------------
  void Poisson_t::RandomSys() {
    if (verbose_ > 9) printf("Poisson_t::RandomSys: START\n");
    for (var_t* var : sys_) {
      var->set_rnd_val(fRn);
      if (verbose_ > 9) { var->print();}
    }
    if (verbose_ > 9) printf("Poisson_t::RandomSys: END\n");
  }

  int Poisson_t::RandomSample() {
    //first sample the systematics
    RandomSys();
    //next sample the poisson distribution
    const double mu = GetMean();
    const int n = fRn->Poisson(mu);
    if (verbose_ > 9) std::cout << __func__ << ": n = " << n << std::endl;
    return n;
  }

  TH1D* Poisson_t::GeneratePDF() {
    // fluctuate the nuisance parameters to define a mean, then add a poisson PDF for this

    if (verbose_ > 9) printf("Poisson_t::%s nmax_: %4i\n", __func__, nmax_);
    
    int nbins = nmax_;
    TH1D* hpdf = new TH1D("hpdf", "PDF", nbins, 0., (double) nbins);
    const int nattempts = ngen_;
    for(int attempt = 0; attempt < nattempts; ++attempt) {
      RandomSys();
      const double mu = GetMean();
      for(int n = 0; n < nbins; ++n) {
	hpdf->Fill(n, ROOT::Math::poisson_pdf(n, mu));
	if (nattempts == 1) hpdf->SetBinError(n+1, 0.);
      }
    }
    hpdf->Scale(1. / nattempts);
    return hpdf;
  }

  //-----------------------------------------------------------------------------  
  void Poisson_t::Print(Option_t* Opt) const {
    printf(" %s: mu = {", GetName());
    for(var_t* var : mu_) {
      printf("%s", var->GetName());
      if(var != mu_[mu_.size()-1]) printf(", ");
    }
    printf("}\n");
    printf("sys = {");
    for(var_t* var : sys_) {
      printf("%s", var->GetName());
      if(var != sys_[sys_.size()-1]) printf(", ");
    }
    printf("}\n");
  }
}
