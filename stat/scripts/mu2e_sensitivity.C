///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
root [0] .L lib/libStntuple_stat.so 
root [1] .L Stntuple/stat/scripts/mu2e_sensitivity.C
root [2] mu2e_sensitivity("su2020_default",1,1)
*/

#include "Stntuple/stat/Mu2e_model.hh"

//-----------------------------------------------------------------------------
void mu2e_sensitivity(const char* Config = "su2020_default", int Mode = 1, int Verbose = 0) {

  stntuple::Mu2e_model* m = new stntuple::Mu2e_model(1,0,Verbose);

  // can redefine some constants

  TString init_file = Form("Stntuple/stat/scripts/%s.C",Config);
  
  gInterpreter->LoadMacro(init_file);
  gInterpreter->ProcessLine(Form("%s((stntuple::Mu2e_model*) %p);",Config,m));

  m->InitVariables();
  m->InitModel();
  m->run(Mode);

  if (Verbose > 0) {
    TString fn = Form("mu2e_sensitivity.%s.hist",Config);

    m->SaveHist(fn.Data());
  }
}

//-----------------------------------------------------------------------------
void test_001(int NEvents = 10) {

  stntuple::Mu2e_model* m = new stntuple::Mu2e_model(1,0,100);

  // can redefine some constants

  gInterpreter->LoadMacro("Stntuple/stat/scripts/su2020_default.C");
  gInterpreter->ProcessLine(Form("su2020_default((stntuple::Mu2e_model*) %p);",m));
  m->InitVariables();
  m->Test001(NEvents);
}
