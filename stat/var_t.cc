////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/var_t.hh"
#include "TRandom3.h"

namespace stntuple {

  var_t::var_t(TString name, double nom) : TNamed(name,name) {
    nom_      =  nom;
    val_      =  nom;
    min_      =  nom-1.; 
    max_      =  nom+1.;
    constant_ =  false;
    verbose_  =  0;
    pdf_type_ =  kGauss;
    fHist     = new TH1F(Form("h_%s",GetName()),GetName(),1000,min_,max_);
  }

  var_t::var_t(TString name, double nom, double min, double max, int PdfType) : TNamed(name,name) {
    nom_      =  nom;
    val_      =  nom;
    min_      =  min; 
    max_      =  max;
    constant_ =  false;
    verbose_  =  0;
    pdf_type_ =  PdfType;
    fHist     = new TH1F(Form("h_%s",GetName()),GetName(),1000,min_,max_);
  }

  //set the base value to a random value following its PDF
  void var_t::set_rnd_val(TRandom3* rnd) {
    if (constant_) return;
    
    if     (pdf_type_ == kGauss) set_val(rnd->Gaus(nom_));
    else if(pdf_type_ == kFlat ) set_val(min_ + (max_-min_)*rnd->Uniform());
  }

  double var_t::get_val() {
    double val = val_;
    if (verbose_ > 2) printf("Variable %s has starting value %.3e\n", GetName(), val);
    for(var_t* var : add_) {
      val += var->get_val();
      if(verbose_ > 2) printf("Variable %s add %s = %.3e --> %.3e\n", GetName(), var->GetName(), var->get_val(), val);
    }
    for (var_t* var : mul_) {
      val *= var->get_val();
      if(verbose_ > 2) printf("Variable %s mul %s = %.3e --> %.3e\n", GetName(), var->GetName(), var->get_val(), val);
    }
    for (var_t* var : pow_) {
      val = std::pow(val, var->get_val());
      if(verbose_ > 2) printf("Variable %s pow %s = %.3e --> %.3e\n", GetName(), var->GetName(), var->get_val(), val);
    }
    if (val > max_ || val < min_) {
      if(verbose_ > 1) std::cout << "Variable " << GetName() << " value outside of bounds! Returning bound...\n";
      val = std::min(max_, std::max(min_, val));
    }

    if (verbose_ > 0) fHist->Fill(val);

    return val;
  }

  void var_t::print() {
    printf("%-30s: %.3e (%.3e) [%10.3e - %10.3e]\n",GetName(),get_val(),nom_,min_,max_);
    if(add_.size() > 0) {
      printf(" --- add = {");
      for(var_t* var : add_) {
	printf("%s = %10.3e", var->GetName(),var->get_val());
	if (var != add_[add_.size()-1]) printf(", ");
      }
      printf("}");
    }
    if(mul_.size() > 0) {
      printf(" --- mul = {");
      for(var_t* var : mul_) {
	printf("%s = %10.3e", var->GetName(),var->get_val());
	if(var != mul_[mul_.size()-1]) printf(", ");
      }
      printf("}\n");
    }
    if(pow_.size() > 0) {
      printf(" --- pow = {");
      for(var_t* var : pow_) {
	printf("%s = %10.3e", var->GetName(),var->get_val());
	if(var != pow_[pow_.size()-1]) printf(", ");
      }
      printf("}\n");
    }
  }
}
