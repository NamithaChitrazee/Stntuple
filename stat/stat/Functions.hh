#ifndef FCSYS_FUNCTIONS
#define FCSYS_FUNCTIONS

#include "Stntuple/stat/var_t.hh"
namespace stntuple {

  //Exponential function wrapper
  class exp_t : public var_t {
  public:
    exp_t(TString name = "Default", double nom = 0) : var_t(name,nom) {}

    virtual double get_val() override; 

    void set_dependencies(std::vector<var_t*> add, std::vector<var_t*> mul = {}, std::vector<var_t*> pow = {}) {
      var_t::set_dependencies(add, mul, pow);
    }

    void print();
  };

  //Polynomial function
  class pow_t : public var_t {
  public:

    pow_t(TString name = "Default", double nom = 0) : var_t(name,nom) {}

    virtual double get_val() override ; 

    void   set_dependencies(std::vector<var_t*> vars, std::vector<var_t*> coeff, std::vector<int> powers) ;
    void   print();

    std::vector<var_t*> vars_;
    std::vector<var_t*> coeff_;
    std::vector<int> powers_;
  };
}
#endif
