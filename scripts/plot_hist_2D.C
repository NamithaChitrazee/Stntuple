///////////////////////////////////////////////////////////////////////////////
// this file is to be included, can't use standalone mode
// it contains two plot_hist_1D functions with different call signatures
//
// overlay two histograms with the same ModuleName/HistName from two files
// defined by Ds1 and Ds2, HistName like "spmc_1/mom"
// if printing is requested, the destination directory is defined by gEnv->GetValue("FiguresDir",".");
///////////////////////////////////////////////////////////////////////////////
#ifndef __plot_hist_2D__
#define __plot_hist_2D__

#include "TH1.h"
#include "TH2.h"

#include "Stntuple/val/stntuple_val_functions.hh"
#include "Stntuple/val/hist_data.hh"

#include "Stntuple/scripts/plot_utilities.hh"
//-----------------------------------------------------------------------------
// plot one hist
//-----------------------------------------------------------------------------
void plot_hist_2D(hist_data_t* Hd, int Print = 0) {
  
  char figure_name[200];
//-----------------------------------------------------------------------------
// figure out clone histogram names
//-----------------------------------------------------------------------------
  TString h1name(Hd->fName);

  if (Hd->fNewName == "") h1name.ReplaceAll("/","_");
  else                    h1name = Hd->fNewName;

  hist_file_t* hf1 = Hd->fFile;

  if (hf1 == nullptr) {
    printf("plot_hist_2D in trouble, exit\n");
    return ;
  }
  
  TH2F* hpx1 = (TH2F*) gh2(hf1->GetName(),Hd->fModule,Hd->fName)->Clone(h1name);
  Hd->fHist  = hpx1;
  if (Hd->fRebin > 0) hpx1->Rebin(Hd->fRebin);
//-----------------------------------------------------------------------------
// scale, if requested
//-----------------------------------------------------------------------------
  if (Hd->fScale > 0) hpx1->Scale(Hd->fScale);
  
  stn_dataset_t* ds1 = hf1->fDataset;
//-----------------------------------------------------------------------------
// create a canvas
//-----------------------------------------------------------------------------
  TString canvas_name(Hd->fModule);
  canvas_name += "_";
  canvas_name += h1name;

  if (Hd->fCanvasName != "") canvas_name = Hd->fCanvasName;

  int cx = (Hd->fCanvasSizeX < 0) ? 1200 : Hd->fCanvasSizeX;
  int cy = (Hd->fCanvasSizeY < 0) ?  800 : Hd->fCanvasSizeY;

  TCanvas* c = new TCanvas(canvas_name.Data(),canvas_name.Data(),cx,cy);
  c->SetLogy(Hd->fYLogScale);
  if(Hd->fCanvasLeftMargin   >= 0.) c->SetLeftMargin  (Hd->fCanvasLeftMargin);
  if(Hd->fCanvasRightMargin  >= 0.) c->SetRightMargin (Hd->fCanvasRightMargin);
  if(Hd->fCanvasBottomMargin >= 0.) c->SetBottomMargin(Hd->fCanvasBottomMargin);
  if(Hd->fCanvasTopMargin    >= 0.) c->SetTopMargin   (Hd->fCanvasTopMargin);
//-----------------------------------------------------------------------------
// the two histograms correspond to slightly different NPOT
//-----------------------------------------------------------------------------
  hpx1->SetLineColor(kRed-3);
  hpx1->SetLineWidth(2);
  hpx1->SetTitle("");
  if (Hd->fXMin < Hd->fXMax) hpx1->GetXaxis()->SetRangeUser(Hd->fXMin,Hd->fXMax);
  if (Hd->fYMin < Hd->fYMax) hpx1->GetYaxis()->SetRangeUser(Hd->fYMin,Hd->fYMax);
  
  if (Hd->fXAxisTitle != ""   ) hpx1->GetXaxis()->SetTitle(Hd->fXAxisTitle.Data());
  if (Hd->fYAxisTitle != ""   ) hpx1->GetYaxis()->SetTitle(Hd->fYAxisTitle.Data());
  if (Hd->fAxisFontSize >= 0. ) {
    hpx1->GetXaxis()->SetTitleSize(Hd->fAxisFontSize);
    hpx1->GetXaxis()->SetLabelSize(Hd->fAxisFontSize);
    hpx1->GetYaxis()->SetTitleSize(Hd->fAxisFontSize);
    hpx1->GetYaxis()->SetLabelSize(Hd->fAxisFontSize);
  }

  if (Hd->fMarkerStyle >=0) hpx1->SetMarkerStyle(Hd->fMarkerStyle);
  if (Hd->fMarkerColor >=0) hpx1->SetMarkerColor(Hd->fMarkerColor);
  if (Hd->fMarkerSize  >=0) hpx1->SetMarkerSize (Hd->fMarkerSize );

  if (Hd->fStats == 0) hpx1->SetStats(0);
  hpx1->Draw();
//-----------------------------------------------------------------------------
// position statbox - need to update the canvas first
//-----------------------------------------------------------------------------
  c->Modified();
  c->Update();
  plot_stat_box(hpx1,Hd->fOptStat,Hd->fStatBoxXMin,Hd->fStatBoxYMin,Hd->fStatBoxXMax,Hd->fStatBoxYMax);
//-----------------------------------------------------------------------------
// add legend - not sure what legend is needed
//-----------------------------------------------------------------------------
  float xmin{0.65}, ymin{0.25}, xmax{0.90}, ymax{0.35};
  if (Hd->fLegendXMin > 0) {             // redefine the legend position, normalized coords
    xmin = Hd->fLegendXMin;
    ymin = Hd->fLegendYMin;
    xmax = Hd->fLegendXMax;
    ymax = Hd->fLegendYMax;
  }
  
  TLegend* leg = new TLegend(xmin,ymin,xmax,ymax);
  leg->AddEntry(hpx1,Hd->fLabel.Data(),"pl");  // "pl"
  leg->SetBorderSize(0);
  leg->Draw();
//-----------------------------------------------------------------------------
// write DS names inside the plot
//-----------------------------------------------------------------------------
  TString label;

  if (Hd->fPlotLabel != "") label = Hd->fPlotLabel;
  else                      label = Hd->fName;

  // lower left corner
  draw_label_ndc(label.Data(),Hd->fLabelXMin,Hd->fLabelYMin,Hd->fLabelFontSize,Hd->fLabelFont); 

  c->Modified(); c->Update();
//-----------------------------------------------------------------------------
// do we need to add something else? Print = -1 serves that purpose
//-----------------------------------------------------------------------------
  printf(" Hd->fPlotName = %s\n",Hd->fPlotName.Data());
  
  if (Hd->fPlotName == "") {
    TString hn1(Hd->fName);

    TString fol1 = hn1(0,hn1.Index('/'));
    TString nam1 = hn1(hn1.Index('/')+1,hn1.Length());

    printf("fol1=:%s  nam1:%s Hd->fName:%s\n",fol1.Data(),nam1.Data(),Hd->fName.Data());

    if (Hd->fYLogScale == 1) Hd->fPlotName = Form("%s_%s_log",fol1.Data(),nam1.Data());
    else                     Hd->fPlotName = Form("%s_%s_lin",fol1.Data(),nam1.Data());
  }
  
  if (Print == -1) {
    Hd->fHist     = hpx1;
    Hd->fOutputFn = Form("%s/eps/%s.eps",gEnv->GetValue("FiguresDir","./"),Hd->fPlotName.Data());
  }
  else { 
//-----------------------------------------------------------------------------
// .png files are written into /png/ subdirectory
//-----------------------------------------------------------------------------
    if (Print == 1) {
      c->Print(Form("%s/eps/%s.eps",gEnv->GetValue("FiguresDir","./"),Hd->fPlotName.Data())) ;
    }
  } 

  Hd->fCanvas = c;
  return;
}

//-----------------------------------------------------------------------------
// plot one hist
//-----------------------------------------------------------------------------
void plot_hist_2d(plot_data_t* Plot, int Print = 0, const char* Format = "eps") {
  
  char figure_name[200];
//-----------------------------------------------------------------------------
// figure out clone histogram names
//-----------------------------------------------------------------------------
  hist_data_t* hd = &Plot->hd[0];
  TString h1name(hd->fName);

  if (hd->fNewName == "") h1name.ReplaceAll("/","_");
  else                    h1name = hd->fNewName;

  hist_file_t* hf1 = hd->fFile;

  if (hf1 == nullptr) {
    printf("plot_hist_2D in trouble, exit\n");
    return ;
  }
  
  TH2F* hpx1 = (TH2F*) gh2(hf1->GetName(),hd->fModule,hd->fName)->Clone(h1name);
  hd->fHist  = hpx1;
//-----------------------------------------------------------------------------
// rebinning: first check the histogram, then - default for the plot
//-----------------------------------------------------------------------------
  int rebin = hd->fRebin;
  if (rebin <= 0) rebin = Plot->fRebin;
  if (rebin > 1) hd->fHist->Rebin(rebin);
//-----------------------------------------------------------------------------
// scale, if requested
//-----------------------------------------------------------------------------
  if (hd->fScale  > 0) hpx1->Scale(hd->fScale);
  if (hd->fLumiSF > 0) hpx1->Scale(hd->fLumiSF);
 
  stn_dataset_t* ds1(nullptr);
  if (hf1) ds1 = hf1->fDataset;
//-----------------------------------------------------------------------------
// create a canvas
//-----------------------------------------------------------------------------
  TString canvas_name("c_plot_hist_2d");
  
  if (Plot->fCanvasName != "") canvas_name = Plot->fCanvasName;
  else if (Plot->fName  != "") canvas_name = "c_"+Plot->fName;
  else {
    if (ds1) {
      canvas_name += ds1->GetName();
      canvas_name += "_";
    }
    canvas_name += hd->fModule;
    canvas_name += "_";
    canvas_name += h1name;
  }

  int cx = Plot->fCanvasSizeX;
  int cy = Plot->fCanvasSizeY;
//-----------------------------------------------------------------------------
// initially, create canvas with an empty name, set scales
//-----------------------------------------------------------------------------
  TCanvas* c = new TCanvas(canvas_name,canvas_name,cx,cy);
  c->SetLogx(Plot->fXLogScale);
  c->SetLogy(Plot->fYLogScale);
  
  if(hd->fCanvasLeftMargin   >= 0.) c->SetLeftMargin  (hd->fCanvasLeftMargin);
  if(hd->fCanvasRightMargin  >= 0.) c->SetRightMargin (hd->fCanvasRightMargin);
  if(hd->fCanvasBottomMargin >= 0.) c->SetBottomMargin(hd->fCanvasBottomMargin);
  if(hd->fCanvasTopMargin    >= 0.) c->SetTopMargin   (hd->fCanvasTopMargin);
//-----------------------------------------------------------------------------
// color
//-----------------------------------------------------------------------------
  hpx1->SetLineColor(kRed-3);
  hpx1->SetLineWidth(2);
  hpx1->SetTitle("");
  if (Plot->fXMin < Plot->fXMax) hpx1->GetXaxis()->SetRangeUser(Plot->fXMin,Plot->fXMax);
  if (Plot->fYMin < Plot->fYMax) hpx1->GetYaxis()->SetRangeUser(Plot->fYMin,Plot->fYMax);
  
  if (Plot->fXAxisTitle != ""   ) hpx1->GetXaxis()->SetTitle(Plot->fXAxisTitle.Data());
  if (Plot->fYAxisTitle != ""   ) hpx1->GetYaxis()->SetTitle(Plot->fYAxisTitle.Data());
  if (hd->fAxisFontSize >= 0. ) {
    hpx1->GetXaxis()->SetTitleSize(hd->fAxisFontSize);
    hpx1->GetXaxis()->SetLabelSize(hd->fAxisFontSize);
    hpx1->GetYaxis()->SetTitleSize(hd->fAxisFontSize);
    hpx1->GetYaxis()->SetLabelSize(hd->fAxisFontSize);
  }

  if (hd->fMarkerStyle >=0) hpx1->SetMarkerStyle(hd->fMarkerStyle);
  if (hd->fMarkerColor >=0) hpx1->SetMarkerColor(hd->fMarkerColor);
  if (hd->fMarkerSize  >=0) hpx1->SetMarkerSize (hd->fMarkerSize );

  if (hd->fStats == 0) hpx1->SetStats(0);
  hpx1->Draw();
//-----------------------------------------------------------------------------
// position statbox - need to update the canvas first
//-----------------------------------------------------------------------------
  c->Modified();
  c->Update();
  plot_stat_box(hpx1,hd->fOptStat,Plot->fStatBoxXMin,Plot->fStatBoxYMin,Plot->fStatBoxXMax,Plot->fStatBoxYMax);
//-----------------------------------------------------------------------------
// add legend - not sure what legend is needed
//-----------------------------------------------------------------------------
  float xmin{0.65}, ymin{0.25}, xmax{0.90}, ymax{0.35};
  if (Plot->fLegendXMin > 0) {             // redefine the legend position, normalized coords
    xmin = Plot->fLegendXMin;
    ymin = Plot->fLegendYMin;
    xmax = Plot->fLegendXMax;
    ymax = Plot->fLegendYMax;
  }
  
  TLegend* leg = new TLegend(xmin,ymin,xmax,ymax);
  leg->AddEntry(hpx1,hd->fLabel.Data(),"pl");  // "pl"
  leg->SetBorderSize(0);
  leg->Draw();
//-----------------------------------------------------------------------------
// write DS names inside the plot
//-----------------------------------------------------------------------------
  TString label;

  if (Plot->fLabel != "") label = hd->fLabel;
  else                    label = hd->fName;

  // lower left corner
  draw_label_ndc(label.Data(),Plot->fLabelXMin,Plot->fLabelYMin,Plot->fLabelFontSize,Plot->fLabelFont); 

  c->Modified(); c->Update();
//-----------------------------------------------------------------------------
// do we need to add something else? Print = -1 serves that purpose
//-----------------------------------------------------------------------------
  printf(" Hd->fPlotName = %s\n",Plot->fName.Data());
  
  if (Plot->fName == "") {
    TString hn1(Plot->fName);

    TString fol1 = hn1(0,hn1.Index('/'));
    TString nam1 = hn1(hn1.Index('/')+1,hn1.Length());

    printf("fol1=:%s  nam1:%s Hd->fName:%s\n",fol1.Data(),nam1.Data(),Plot->fName.Data());

    if (Plot->fYLogScale == 1) Plot->fName = Form("%s_%s_log",fol1.Data(),nam1.Data());
    else                       Plot->fName = Form("%s_%s_lin",fol1.Data(),nam1.Data());
  }
  
//-----------------------------------------------------------------------------
// determine the output file name. Extension defines the file format (default: ".eps")
//-----------------------------------------------------------------------------
  TString ext(Format);
  ext.ToLower();
  Plot->fOutputFn = Form("%s/%s/%s.%s",gEnv->GetValue("FiguresDir","./"),ext.Data(),Plot->fName.Data(),ext.Data());
  if (Print == 1) {
    c->Print(Form("%s",Plot->fOutputFn.Data())) ;
  }
  Plot->fCanvas   = c;

  return;
}


//-----------------------------------------------------------------------------
// compare two histograms with different ModuleName/HistName's from two different files
// HistName like "spmc_1/mom"
//-----------------------------------------------------------------------------
void plot_hist_2D(hist_data_t* Hist1,  hist_data_t*  Hist2, int Print = 0) {
  
  char figure_name[200];

//-----------------------------------------------------------------------------
// figure out clone histogram names
//-----------------------------------------------------------------------------
  TString h1name(Hist1->fName), h2name(Hist2->fName);

  if (Hist1->fNewName == "") h1name.ReplaceAll("/","_");
  else                       h1name = Hist1->fNewName;

  if (Hist2->fNewName == "") h2name.ReplaceAll("/","_");
  else                       h2name = Hist2->fNewName;

  hist_file_t* hf1 = Hist1->fFile;
  hist_file_t* hf2 = Hist2->fFile;
  
  TH2F* hpx1 = (TH2F*) gh1(hf1->GetName(),Hist1->fModule,Hist1->fName)->Clone(h1name);
  Hist1->fHist = hpx1;
  
  if (Hist1->fRebin > 0) hpx1->Rebin(Hist1->fRebin);
  
  TH2F* hpx2 = (TH2F*) gh2(hf2->GetName(),Hist2->fModule,Hist2->fName)->Clone(h2name);
  Hist2->fHist = hpx2;
  
  if (Hist2->fRebin > 0) hpx2->Rebin(Hist2->fRebin);

  stn_dataset_t* ds1 = hf1->fDataset;
  stn_dataset_t* ds2 = hf2->fDataset;
//-----------------------------------------------------------------------------
// create a canvas
//-----------------------------------------------------------------------------
  TString canvas_name(Hist1->fModule);
  canvas_name += "_";
  canvas_name += h1name;
  canvas_name += "_vs_";
  canvas_name += Hist2->fModule;
  canvas_name += "_";
  canvas_name += h2name;

  int cx = (Hist1->fCanvasSizeX < 0) ? 1200 : Hist1->fCanvasSizeX;
  int cy = (Hist1->fCanvasSizeY < 0) ?  800 : Hist1->fCanvasSizeY;

  TCanvas* c = new TCanvas(canvas_name.Data(),canvas_name.Data(),cx,cy);
  c->SetLogy(Hist1->fYLogScale);
  if(Hist1->fCanvasLeftMargin   >= 0.) c->SetLeftMargin  (Hist1->fCanvasLeftMargin);
  if(Hist1->fCanvasRightMargin  >= 0.) c->SetRightMargin (Hist1->fCanvasRightMargin);
  if(Hist1->fCanvasBottomMargin >= 0.) c->SetBottomMargin(Hist1->fCanvasBottomMargin);
  if(Hist1->fCanvasTopMargin    >= 0.) c->SetTopMargin   (Hist1->fCanvasTopMargin);

//-----------------------------------------------------------------------------
// the two histograms correspond to slightly different NPOT
//-----------------------------------------------------------------------------
  hpx1->SetLineColor(kRed-3);
  hpx1->SetLineWidth(2);
  hpx1->SetTitle("");

  if (Hist1->fMarkerStyle >=0) hpx1->SetMarkerStyle(Hist1->fMarkerStyle);
  if (Hist1->fMarkerColor >=0) hpx1->SetMarkerColor(Hist1->fMarkerColor);
  if (Hist1->fMarkerSize  >=0) hpx1->SetMarkerSize (Hist1->fMarkerSize );

  if (Hist1->fXMin < Hist1->fXMax) hpx1->GetXaxis()->SetRangeUser(Hist1->fXMin,Hist1->fXMax);
  if (Hist1->fXAxisTitle != ""   ) hpx1->GetXaxis()->SetTitle(Hist1->fXAxisTitle.Data());

  if (Hist1->fYMin < Hist1->fYMax) hpx1->GetYaxis()->SetRangeUser(Hist1->fYMin,Hist1->fYMax);
  if (Hist1->fYAxisTitle != ""   ) hpx1->GetYaxis()->SetTitle(Hist1->fYAxisTitle.Data());
  if (Hist1->fAxisFontSize >= 0. ) {
    hpx1->GetXaxis()->SetTitleSize(Hist1->fAxisFontSize);
    hpx1->GetXaxis()->SetLabelSize(Hist1->fAxisFontSize);
    hpx1->GetYaxis()->SetTitleSize(Hist1->fAxisFontSize);
    hpx1->GetYaxis()->SetLabelSize(Hist1->fAxisFontSize);
  }

  if (Hist1->fStats == 0         ) hpx1->SetStats(0);

  hpx1->Draw();

  hpx2->SetLineWidth(1);

  if (Hist2->fLineColor >=0) hpx2->SetLineColor(Hist2->fLineColor);
  if (Hist2->fFillStyle >=0) hpx2->SetFillStyle(Hist2->fFillStyle);
  if (Hist2->fFillColor >=0) hpx2->SetFillColor(Hist2->fFillColor);

  if (Hist2->fMarkerStyle >=0) hpx2->SetMarkerStyle(Hist2->fMarkerStyle);
  if (Hist2->fMarkerColor >=0) hpx2->SetMarkerColor(Hist2->fMarkerColor);
  if (Hist2->fMarkerSize  >=0) hpx2->SetMarkerSize (Hist2->fMarkerSize );

  printf("ds2 marker: %i\n",Hist2->fMarkerStyle);
  if (Hist2->fMarkerStyle >=0) hpx2->Draw("sames");
  else                         hpx2->Draw("sames");
//-----------------------------------------------------------------------------
// position statboxes - need to update the canvas first
//-----------------------------------------------------------------------------
  c->Modified();
  c->Update();
  plot_stat_box(hpx1,Hist1->fOptStat,0.65,0.65,0.9,0.9);
  plot_stat_box(hpx2,Hist2->fOptStat,0.65,0.40,0.9,0.65);
//-----------------------------------------------------------------------------
// add legend
//-----------------------------------------------------------------------------
  float xmin{0.65}, ymin{0.25}, xmax{0.90}, ymax{0.35};
  if (Hist1->fLegendXMin > 0) { // redefine the legend position, normalized coords
    xmin = Hist1->fLegendXMin;
    ymin = Hist1->fLegendYMin;
    xmax = Hist1->fLegendXMax;
    ymax = Hist1->fLegendYMax;
  }
  
  TLegend* leg = new TLegend(xmin,ymin,xmax,ymax);
  leg->AddEntry(hpx1,Hist1->fLabel.Data(),"pl");  // "pl"
  leg->AddEntry(hpx2,Hist2->fLabel.Data(),"f");
  leg->SetBorderSize(0);
  leg->Draw();
//-----------------------------------------------------------------------------
// write DS names inside the plot
//-----------------------------------------------------------------------------
  TString label;

  if (Hist1->fPlotLabel != "") {
    label = Hist1->fPlotLabel;
  }
  else {
    label = Hist1->fName;
    if (Hist1->fName != Hist2->fName) {
      label += " vs ";
      label += Hist2->fName;
    }
  }

  // lower left corner
  draw_label_ndc(label.Data(),Hist1->fLabelXMin,Hist1->fLabelYMin,Hist1->fLabelFontSize,Hist1->fLabelFont); 

  c->Modified(); c->Update();
//-----------------------------------------------------------------------------
// do we need to add something else? Print = -1 serves that purpose
//-----------------------------------------------------------------------------
  if (Hist1->fPlotName == "") {
    TString hn1(Hist1->fName), hn2(Hist2->fName);

    TString fol1 = hn1(0,hn1.Index('/'));
    TString nam1 = hn1(hn1.Index('/')+1,hn1.Length());

    TString fol2 = hn2(0,hn2.Index('/'));
    TString nam2 = hn2(hn2.Index('/')+1,hn2.Length());

    if (Hist1->fYLogScale == 1) {
      Hist1->fPlotName = Form("%s_%s_vs_%s_%s_log",
			      Hist1->fName.Data(),fol1.Data(),fol2.Data(),nam1.Data());
    }
    else {
      Hist1->fPlotName = Form("%s_%s_vs_%s_%s_lin",
			      Hist1->fName.Data(),fol1.Data(),fol2.Data(),nam1.Data());
    }
  }
  
  if (Print == -1) {
    Hist1->fCanvas   = c;
    Hist1->fHist     = hpx1;
    Hist1->fOutputFn = Form("%s/eps/%s.eps",gEnv->GetValue("FiguresDir","./"),Hist1->fPlotName.Data());
  }
  else { 
//-----------------------------------------------------------------------------
// .png files are written into /png/ subdirectory
//-----------------------------------------------------------------------------
    if (Print == 1) {
      c->Print(Form("%s/eps/%s.eps",gEnv->GetValue("FiguresDir","./"),Hist1->fPlotName.Data())) ;
    }
  }
  
  Hist1->fCanvas   = c;
  return;
}

#endif
