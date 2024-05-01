//-----------------------------------------------------------------------------
// struct to hold a plot data with multiple histograms
//-----------------------------------------------------------------------------
#ifndef __Stntuple_val_plot_data_hh__
#define __Stntuple_val_plot_data_hh__

#include "Stntuple/val/hist_data.hh"

#include "TString.h"
#include "TCanvas.h"

class plot_data_t {
public:
  int           nhist;
  hist_data_t   hd[100];                // difficult to think of having more than that

  TString       fName;
  TString       fLabel  ;		// global label to appear on a plot
  float         fLabelXMin;
  float         fLabelYMin;
  float         fLabelFontSize;
  float         fLabelFont;
					// --- limits, scales, rebinning
  float         fXMin;
  float         fXMax;
  int           fXLogScale;
  float         fYMin;
  float         fYMax;
  int           fYLogScale;
  int           fRebin;
					// --- statistics 
  int           fStats;
  int           fOptStat;		// default for all histograms on the plot
  float         fStatBoxXMin;
  float         fStatBoxYMin;
  float         fStatBoxXMax;
  float         fStatBoxYMax;
					// --- titles
  TString       fXAxisTitle;
  TString       fYAxisTitle;
  TString       fYTitFormat;		// if "", Y axis title is not printed

  float         fLegendXMin;
  float         fLegendYMin;
  float         fLegendXMax;
  float         fLegendYMax;
  float         fLegendTextSize;

  TString       fTitle;                 // if not empty, replaces the histogram title

  TString       fCanvasName;
  int           fCanvasSizeX;
  int           fCanvasSizeY;
  TCanvas*      fCanvas;

  TString       fOutputFn;

  static TString fgFiguresDir;       

  plot_data_t(int NHist = -1) {
    nhist = NHist;
    init();
  }

  void init();
  
  int  print(const char* Fn = "");
};

#endif
