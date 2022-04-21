//////////////////////////////////////////////////////////////////////////////

#include "Stntuple/stat/Functions.hh"

namespace stntuple {
    
  // Exponential function wrapper

  double exp_t::get_val() {
    double val = var_t::get_val();
    if (verbose_ > 0) fHist->Fill(val);
    return std::exp(val);
  }

  void exp_t::print() {
    printf(" %s: %.3e = e^{((%.3e", GetName(), get_val(), val_);
    for(unsigned index = 0; index < add_.size(); ++index) {
      printf(" + %s", add_[index]->GetName());
    }
    printf(")");
    for(unsigned index = 0; index < mul_.size(); ++index) {
      printf(" * %s", mul_[index]->GetName());
    }
    printf(")");
    if(pow_.size() > 0) {
      printf("^{");
      for(unsigned index = 0; index < pow_.size(); ++index) {
	printf("%s", pow_[index]->GetName());
	if(index < pow_.size() - 1) printf(" * ");
      }
      printf("}");
    }
    printf("}\n");
  }

//-----------------------------------------------------------------------------
// Polynomial function
//-----------------------------------------------------------------------------
  double pow_t::get_val() {
    double val(0.);
    for(unsigned index = 0; index < powers_.size(); ++index) {
      val += coeff_[index]->get_val()*std::pow(vars_[index]->get_val(), powers_[index]);
    }

    if (verbose_ > 0) fHist->Fill(val);
    return val;
  }

  void pow_t::set_dependencies(std::vector<var_t*> vars, std::vector<var_t*> coeff, std::vector<int> powers) {
      if(vars.size() != coeff.size() || vars.size() != powers.size()) {
        std::cout << "!!! Error: pow::" << __func__ << " polynomial parameter lists not the same length!\n";
        throw 10;
      }
      vars_   = vars;
      coeff_  = coeff;
      powers_ = powers;
    }

  void pow_t::print() {
    printf("%s: %.3e = ", GetName(), get_val());
    for(unsigned index = 0; index < vars_.size(); ++index) {
      printf(" (%s) * (%s)^{%i}", coeff_[index]->GetName(), vars_[index]->GetName(), powers_[index]);
      if(index < vars_.size() - 1) printf(" + ");
    }
    printf("\n");
  }
}
