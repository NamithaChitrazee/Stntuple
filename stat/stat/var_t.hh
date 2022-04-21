#ifndef __su2020_var_t_hh__
#define __su2020_var_t_hh__

#include <set>
#include <iostream>
#include "Math/ProbFunc.h"
#include "Math/DistFunc.h"

#include "TString.h"
#include "TNamed.h"
#include "TH1F.h"

class TRandom3;

namespace stntuple {

  //////////////////////////////////////////////////////////////
  //a variable class:
  //knows about its:
  // bounds
  // nominal value
  // additive (applied first) variables
  // multiplicative (second) variables
  // power (third) variables
  //////////////////////////////////////////////////////////////
  class var_t : public TNamed {
  public:

    enum {
      kGauss = 1,
      kFlat  = 2
    };
    
    //Fields for the variable, all public for convenience
    double              min_;
    double              max_;
    double              nom_;
    double              val_;
    int                 pdf_type_;
    //    TString             pdf_;
    bool                constant_;

    std::vector<var_t*> add_;
    std::vector<var_t*> mul_;

    std::vector<var_t*> pow_;

    int                 verbose_;
    TH1F*               fHist;

    var_t(TString name = "Default", double nom = 0);
    var_t(TString name, double nom, double min, double max, int PdfType = kGauss);


    TH1F*  GetHist() { return fHist; }

    // To ignore calls to set_rnd_val, remains constant
    void set_constant(bool constant = true) { constant_ = constant; }

    void SetVerbose(int Verbose) { verbose_ = Verbose; }

    //initialize the variable's dependence on other variables
    void set_dependencies(std::vector<var_t*> add, std::vector<var_t*> mul, std::vector<var_t*> pow = {}) {
      add_ = add;
      mul_ = mul;
      pow_ = pow;
    }

    //set the base value of the parameter, before the evaluation of other variables
    void set_val(double val) { val_ = std::min(max_, std::max(min_, val)); }

    //set the base value to a random value following its PDF
    void set_rnd_val(TRandom3* rnd);

    //Evaluate the value by taking the base value and applying the dependent variable values
    virtual double get_val() ; 

    //print information about the variable
    void print();
  };
}

#endif
