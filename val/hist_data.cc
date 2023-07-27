//-----------------------------------------------------------------------------
// a 'dataset' here is a histogram file plus data fields for plotting attributes
// hist_data_t::fHist is supposed to be defined on exit
// need libStntuple_val.so
//-----------------------------------------------------------------------------
#include "TFile.h"

#include "Stntuple/val/stntuple_val_functions.hh"

#include "Stntuple/val/hist_file.hh"
#include "Stntuple/val/hist_data.hh"

#include "Stntuple/val/stn_book.hh"
#include "Stntuple/val/stn_catalog.hh"

//-----------------------------------------------------------------------------
hist_data_t::hist_data_t(const char* DsID, const char* JobName, const char* Module, const char* HistName) {
    fHist        = nullptr;
    if (DsID && (DsID[0] != 0)) fFile = get_hist_file(DsID,JobName);
    else                        fFile = nullptr;
    fBook        = nullptr;
    fName        = HistName;
    fModule      = Module;
    if (fFile != nullptr) {
      fHist        = (TH1*) gh1(fFile->GetName(),fModule,HistName);
      if (fHist != nullptr) init();
    }
  }

//-----------------------------------------------------------------------------
hist_data_t::hist_data_t(stn_book* Book, const char* DsID, const char* JobName, const char* Module, const char* HistName) {
    fHist        = nullptr;
    fBook        = Book;
    fName        = HistName;
    fModule      = Module;
    fFile        = Book->FindHistFile(DsID,"",JobName);
    if (fFile != nullptr) {
      fHist        = (TH1*) gh1(fFile->GetName(),fModule,HistName);
      if (fHist != nullptr) init();
    }
  }

//-----------------------------------------------------------------------------
// histogram file created by production job (analysis job name = "")
//-----------------------------------------------------------------------------
hist_data_t::hist_data_t(stn_book* Book, const char* DsID, const char* ProductionJob, const char* FullHistName) {
  const char* analysis_job = "";
  
  fHist        = nullptr;
  fBook        = Book;
  fName        = FullHistName;
  fModule      = "";
  fFile        = Book->FindHistFile(DsID,ProductionJob,analysis_job);
  if (fFile != nullptr) {
    TFile* f = TFile::Open(fFile->GetName());
    if (f == nullptr) {
      printf("ERROR in hist_data_t: file %s doesn't exist, bail out\n",fFile->GetName());
    }
    else {
      fHist    = (TH1*) f->Get(FullHistName);
      if (fHist != nullptr) init();
    }
  }
}

//-----------------------------------------------------------------------------
hist_data_t::hist_data_t(stn_catalog* Catalog, const char* BookName, const char* DsID,
	      const char* JobName,   const char* Module, const char* HistName) {
    fHist        = nullptr;
    fBook        = Catalog->FindBook(BookName);
    fFile        = fBook->FindHistFile(DsID,"",JobName);
    fName        = HistName;
    fModule      = Module;
    if (fFile != nullptr) {
      fHist        = gh1(fFile->GetName(),fModule,HistName);
      if (fHist != nullptr) init();
    }
  }

//-----------------------------------------------------------------------------
hist_data_t::hist_data_t(TH1* Hist, const char* JobName, const char* Module) {
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

//-----------------------------------------------------------------------------
void  hist_data_t::init() {
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
  fLegendFontSize = -1;
}

//-----------------------------------------------------------------------------
// get histogram file for a given dataset ID
//-----------------------------------------------------------------------------
hist_file_t* hist_data_t::get_hist_file(const char* DsID, const char* JobName) {
  printf("hist_data::get_hist_file dsid: %s  job: %s fBook = %p\n",DsID,JobName,static_cast<void*>(fBook));
  
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
int  hist_data_t::print(const char* Fn) {
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
