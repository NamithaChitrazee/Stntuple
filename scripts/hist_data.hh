//-----------------------------------------------------------------------------
// a 'dataset' here is a histogram file plus data fields for plotting attributes
// hist_data_t::fHist is supposed to be defined on exit
// need libStntuple_val.so
//-----------------------------------------------------------------------------
#ifndef __Stntuple_scripts_hist_data_hh__
#define __Stntuple_scripts_hist_data_hh__

#include "Stntuple/val/stntuple_val_functions.hh"

#include "stn_book.hh"
#include "hist_file.hh"

class hist_data_t {
public:
  TH1*            fHist;                  // if non-null, fFile is not needed
  hist_file_t*    fFile;                  // it contains a pointer to the dataset
  const stn_book* fBook;

  TString       fLabel  ;		// label to appear on a plot
  float         fLabelXMin;
  float         fLabelYMin;
  float         fLabelFontSize;
  float         fLabelFont;

  TString       fModule;	        // module name
  TString       fName;	                // histogram name, as saved in a file
  TString       fNewName;	        // redefines histogram name at plotting time
  int           fLineColor;             // these are utility fields to be used as needed
  int           fLineWidth;
  int           fFillColor;             // these are utility fields to be used as needed
  int           fFillStyle;             // these are utility fields to be used as needed
  int           fMarkerStyle;
  float         fMarkerSize;
  int           fMarkerColor;
  float         fXMin;
  float         fXMax;
  float         fYMin;
  float         fYMax;
  int           fStats;
  int           fOptStat;
  float         fStatBoxXMin;
  float         fStatBoxYMin;
  float         fStatBoxXMax;
  float         fStatBoxYMax;
  int           fRebin;
  int           fYLogScale;
  TString       fDrawOpt;
  float         fScale;			// 1:normalize to NGenEvents, 2:normalize to integral
  float         fLumiSF;                // alternative to scale: just scale by this number
  TString       fPlotName;
  TString       fPlotLabel;
  TString       fXAxisTitle;
  TString       fYAxisTitle;
  float         fAxisFontSize;
  TString       fYTitFormat;		// if "", Y axis title is not printed
  float         fLegendXMin;
  float         fLegendYMin;
  float         fLegendXMax;
  float         fLegendYMax;
  TString       fCanvasName;
  int           fCanvasSizeX;
  int           fCanvasSizeY;
  float         fCanvasLeftMargin;
  float         fCanvasRightMargin;
  float         fCanvasBottomMargin;
  float         fCanvasTopMargin;
  TCanvas*      fCanvas;
  TString       fOutputFn;

  hist_data_t(const char* DsID = "", const char* JobName = "", const char* Module = "", const char* HistName = "") {
    fHist        = nullptr;
    if (DsID && (DsID[0] != 0)) fFile = get_hist_file(DsID,JobName);
    else                        fFile = nullptr;
    fBook        = nullptr;
    fName        = HistName;
    fModule      = Module;
    if (fFile != nullptr) {
      fHist        = gh1(fFile->fName,fModule,HistName);
      if (fHist != nullptr) init();
    }
  }
  
  hist_data_t(const stn_book* Book, const char* DsID = "", const char* JobName = "", const char* Module = "", const char* HistName = "") {
    fHist        = nullptr;
    fBook        = Book;
    fName        = HistName;
    fModule      = Module;
    fFile        = Book->FindHistFile(DsID,"",JobName);
    if (fFile != nullptr) {
      fHist        = gh1(fFile->fName,fModule,HistName);
      if (fHist != nullptr) init();
    }
  }
  
  hist_data_t(const stn_catalog* Catalog, const char* BookName, const char* DsID = "",
	      const char* JobName = "",   const char* Module = "", const char* HistName = "") {
    fHist        = nullptr;
    fBook        = Catalog->FindBook(BookName);
    fFile        = fBook->FindHistFile(DsID,"",JobName);
    fName        = HistName;
    fModule      = Module;
    if (fFile != nullptr) {
      fHist        = gh1(fFile->fName,fModule,HistName);
      if (fHist != nullptr) init();
    }
  }
  
  hist_data_t(TH1* Hist, const char* JobName = "", const char* Module = "") {
    // Hist is supposed not to be a null pointer, still check
    fHist        = Hist;
    fFile        = nullptr;            // don't need it, fHist is already defined
    fBook        = nullptr;            // same reason
    fModule      = Module;
    fName        = "";
    if (Hist != nullptr) {
      fName        = Hist->GetName();
      init();
    }
  }

  void  init() {
    fCanvasName  = "";
    fCanvasSizeX = 1200.;
    fCanvasSizeY =  800.;
    fCanvasLeftMargin   = -1.;
    fCanvasRightMargin  = -1.;
    fCanvasBottomMargin = -1.;
    fCanvasTopMargin    = -1.;
    fNewName     = "";
    fLabel         = "";
    fLabelXMin     = 0.15;
    fLabelYMin     = 0.86;
    fLabelFontSize = 0.03;
    fLabelFont     = 52; // lower left corner
    fLineColor   = -1;
    fLineWidth   =  1;
    fMarkerSize  = -1;
    fMarkerStyle = -1;
    fMarkerColor = -1;
    fFillColor   = -1;
    fFillStyle   = -1;
    fXMin        =  0;
    fXMax        = -1;
    fYMin        =  0;
    fYMax        = -1;
    fStats       =  1;
    fOptStat     = -1;
    fStatBoxXMin =  0.70;
    fStatBoxYMin =  0.72;
    fStatBoxXMax =  0.90;
    fStatBoxYMax =  0.90;
    fRebin       = -1;
    fYLogScale   =  0;
    fDrawOpt     = "";
    fScale       = -1;
    fLumiSF      = -1; // alternative to fScale
    fPlotName    = "";
    fPlotLabel   = "";
    fXAxisTitle  = "";
    fYAxisTitle  = "";
    fAxisFontSize = -1.;
    fYTitFormat  = "N / %10.3f";
    fLegendXMin  = 0.65;
    fLegendYMin  = 0.15;
    fLegendXMax  = 0.90;
    fLegendYMax  = 0.40;
  }

//-----------------------------------------------------------------------------
// get histogram file for a given dataset ID
//-----------------------------------------------------------------------------
  hist_file_t*  get_hist_file(const char* DsID, const char* JobName) {
    printf("hist_data::get_hist_file dsid: %s  job: %s fBook = %p\n",DsID,JobName,fBook);
  
    hist_file_t* hf = fBook->FindHistFile(DsID,"",JobName);
  
    if (hf == nullptr) {
      printf("hist_data::get_hist_file ERROR: cant find hist file for dsid=%s, job=%s; return NULL\n",
	     DsID,JobName);
    }
    return hf;
  }

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  int  print(const char* Fn = "") {
    TString fn = Fn;
    if (fn == "") fn = fOutputFn;

    if (fn == "") {
      printf(" hist_data::print ERROR: canvas is not defined. BAIL OUT\n");
      return -1;
    }
    
    if (fCanvas == nullptr) {
      printf(" hist_data::print ERROR: canvas is not defined. BAIL OUT\n");
      return -1;
    }
    
    fCanvas->Modified();
    fCanvas->Update();
    
    fCanvas->Print(fn.Data());
    return 0;
  }

};

#endif
