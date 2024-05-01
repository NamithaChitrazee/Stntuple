//

#include "Stntuple/val/plot_data.hh"


TString plot_data_t::fgFiguresDir = "plot_data::undefined";

void  plot_data_t::init() {
  fName          = "";
  fTitle         = "";
  fCanvasName    = "";
  fCanvasSizeX   = 1200.;
  fCanvasSizeY   =  800.;
    
  fLabel         = "";
  fLabelXMin     = 0.15;
  fLabelYMin     = 0.86;
  fLabelFontSize = 0.03;
  fLabelFont     = 52; // lower left corner

  fXMin          =  0;
  fXMax          = -1;
  fYMin          =  0;
  fYMax          = -1;

  fStats         =  1;
  fOptStat       = -1;
  fStatBoxXMin   = 0.70;
  fStatBoxXMax   = 0.90;
  fStatBoxYMin   = 0.72;
  fStatBoxYMax   = 0.90;

  fRebin         = -1;
  fXLogScale     =  0;
  fYLogScale     =  0;

  fXAxisTitle    = "";
  fYAxisTitle    = "";
  fYTitFormat    = "N / %10.3f";
    
  fLegendXMin    = 0.65;
  fLegendYMin    = 0.15;
  fLegendXMax    = 0.90;
  fLegendYMax    = 0.40;
  fLegendTextSize = -1;

  fOutputFn      = "";
}

//-----------------------------------------------------------------------------
// what ROOT does is defined by the file extention
//-----------------------------------------------------------------------------
int  plot_data_t::print(const char* Fn) {
  TString fn = Fn;
  if (fn == "") fn = fOutputFn;

  if (fn == "") {
    printf("plot_data::print ERROR: output filename is not defined. BAIL OUT\n");
    return -1;
  }
    
  if (fCanvas == nullptr) {
    printf("plot_data::print ERROR: canvas is not defined. BAIL OUT\n");
    return -1;
  }
    
  fCanvas->Modified();
  fCanvas->Update();

  printf("fOutputFn: %s",fOutputFn.Data());
  
  fCanvas->Print(fn.Data());
  return 0;
}
