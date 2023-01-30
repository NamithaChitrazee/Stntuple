#ifndef TStnVisManager_hh
#define TStnVisManager_hh

#include "TObjArray.h"
#include "TGDoubleSlider.h"
#include "TGButton.h"
#include "TGTextEntry.h"
#include "TGTextBuffer.h"

#include "Stntuple/obj/AbsEvent.hh"
#include "Stntuple/base/TVisManager.hh"

class TControlBar;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;
class TGMainFrame;

class TStnView;

class TSubdetector;
class TExtrapolator;

namespace mu2e {
  class KalSeed;
}

namespace stntuple {
  class TEvdTimeCluster;
}

//-----------------------------------------------------------------------------
class TStnVisManager : public TVisManager {
public:
					// different view types
  enum {
    kUndefined = -1,
    kXY        =  1,
    kRZ        =  2,
    kTZ        =  3,
    kCal       =  4,
    kCrv       =  5,
    kVST       =  6			// VST view
  };

//-----------------------------------------------------------------------------
// command codes
//-----------------------------------------------------------------------------
  enum CommandIdentifiers {
    M_TRACKER_XY,
    M_TRACKER_RZ,
    M_CALORIMETER_XY,
    M_CRV_XY,
    M_EXIT,

    M_OPTION_EVENT_STATUS,

    M_HELP_CONTENTS,
    M_HELP_SEARCH,
    M_HELP_ABOUT
  };

  enum WidgetIdentities{
    TIMESLIDER_ID = 10,
    TIMELOW_DISP = 11,
    TIMEHIGH_DISP = 12,
    UPDATER_BTN = 13
  };

//-----------------------------------------------------------------------------
//  data members
//-----------------------------------------------------------------------------
protected:
  TGMainFrame*        fMain;
  TGMenuBar           *fMenuBar;	  // !
  TGPopupMenu         *fMenu; // !
  TGPopupMenu         *fMenuHelp;	  // !

  TGLayoutHints       *fMenuBarLayout;	  // !
  TGLayoutHints       *fMenuBarItemLayout; // !
  TGLayoutHints       *fMenuBarHelpLayout; // !

  TGTextButton        *trkrBtnXY, *trkrBtnRZ, *calBtn, *crvBtn, *updaterBtn;
  TGDoubleHSlider     *timeWindowSlider;
  TGTextBuffer        *timeWindowLowBuff, *timeWindowHighBuff;
  TGTextEntry         *timeWindowLowDisp, *timeWindowHighDisp;
//-----------------------------------------------------------------------------
// vis. manager also holds a list of objects to be displayed.
// The list has to be the same for all the views
//-----------------------------------------------------------------------------
  TObjArray*          fListOfDetectors;
  TSubdetector*       fClosestSubdetector;

  TExtrapolator*      fExtrapolator;

  const art::Event*   fEvent;

  stntuple::TEvdTimeCluster*    fSelectedTimeCluster;
  stntuple::TEvdTimeCluster*    fSelectedPhiCluster;

  int                 fDebugLevel;
					// to display all the data in a given time window 
					// vis manager would enforces the same time limits on all views,
					// later those can be redefined individually
  float               fTMin;
  float               fTMax;

  int                 fMinStation;
  int                 fMaxStation;
                                        // min and max momentum values for displayed MC particles
  float               fMinMcMomentum;
  float               fMaxMcMomentum;

  float               fMbTime;

  float               fBField;          // by defautl , 1 T, but could be less. Need to know to display straight
                                        // cosmics

  int                 fDisplayStrawDigiMC;
  int                 fDisplayStrawHitsXY;

  int                 fDisplayHelices; 
  int                 fDisplayTracks; 
  int                 fDisplaySimParticles; 
  int                 fDisplayOnlyTCHits;      // will be useless longer term, as the time cluster should 
                                               // include all hits within the time interval, 
                                               // further selections - based on the hit flags
  int                 fIgnoreComptonHits;      // if 1, do not show hits marked as 'compton' (flag='bgr')
//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:

  TStnVisManager(const char* name = "TStnVisManager",	const char* title = "TStnVisManager");

  virtual ~TStnVisManager();

  static TStnVisManager* Instance();
  // ****** accessors

  //Interface Handlers

  virtual int    InitGui  (const char* Title);
  virtual int    InitViews();

  void           HandleButtons();
  void           HandleSlider();
  void           HandleText(); //char * text);

  TSubdetector*  GetClosestSubdetector() { return fClosestSubdetector; }
  TExtrapolator* GetExtrapolator() { return fExtrapolator; }

  TObjArray*     GetListOfDetectors() { return fListOfDetectors; }

  void           AddDetector(TObject* det) { fListOfDetectors->Add(det); }

  const art::Event* Event() { return fEvent; }

  stntuple::TEvdTimeCluster*  SelectedTimeCluster() { return fSelectedTimeCluster; }
  stntuple::TEvdTimeCluster*  SelectedPhiCluster () { return fSelectedPhiCluster;  }

  float          MbTime() { return fMbTime; }
  float          BField() { return fBField; }

  void           GetTimeWindow(float& TMin, float& TMax) {
    TMin = fTMin;
    TMax = fTMax;
  }

  int            DisplayHelices     () const { return fDisplayHelices     ; }
  int            DisplayOnlyTCHits  () const { return fDisplayOnlyTCHits  ; }
  int            DisplaySimParticles() const { return fDisplaySimParticles; }
  int            DisplayStrawDigiMC () const { return fDisplayStrawDigiMC ; }
  int            DisplayStrawHitsXY () const { return fDisplayStrawHitsXY ; }
  int            DisplayTracks      () const { return fDisplayTracks      ; }
  
  int            IgnoreComptonHits  () const { return fIgnoreComptonHits  ; }

  float          MinMcMomentum      () const { return fMinMcMomentum; }
  float          MaxMcMomentum      () const { return fMaxMcMomentum; }

  int            MinStation         () const { return fMinStation; }
  int            MaxStation         () const { return fMaxStation; }

  float          TMin               () const { return fTMin; }
  float          TMax               () const { return fTMax; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void           SetEvent(const art::Event* Evt) { fEvent = Evt; }

  void           SetDisplayStrawDigiMC (int Flag) { fDisplayStrawDigiMC  = Flag; }
  void           SetDisplayStrawHitsXY (int Flag) { fDisplayStrawHitsXY  = Flag; }
  void           SetDisplayHelices     (int Flag) { fDisplayHelices      = Flag; }
  void           SetDisplayTracks      (int Flag) { fDisplayTracks       = Flag; }
  void           SetDisplaySimParticles(int Flag) { fDisplaySimParticles = Flag; }
  void           SetDisplayOnlyTCHits  (int Flag) { fDisplayOnlyTCHits   = Flag; }
  void           SetIgnoreComptonHits  (int Flag) { fIgnoreComptonHits   = Flag; }

  void           SetStations(int IMin, int IMax) override;

  void           SetMinMcMomentum(float Mom) { fMinMcMomentum = Mom; }
  void           SetMaxMcMomentum(float Mom) { fMaxMcMomentum = Mom; }

  void           SetSelectedTimeCluster(stntuple::TEvdTimeCluster* Tc) { fSelectedTimeCluster = Tc; }
  void           SetSelectedPhiCluster (stntuple::TEvdTimeCluster* Pc) { fSelectedPhiCluster  = Pc; }
  
  void   SetTimeWindow(float TMin, float TMax) override {
    fTMin = TMin;
    fTMax = TMax;
  }

  void  SetMbTime(float MbTime) { fMbTime = MbTime; }
  void  SetBField(float BField) { fBField = BField; }
//-----------------------------------------------------------------------------
// print functions
//-----------------------------------------------------------------------------
  void PrintTrack(const mu2e::KalSeed* KSeed);
//-----------------------------------------------------------------------------
// GUI
//-----------------------------------------------------------------------------
  virtual void  DoCheckButton() override;
  virtual void  DoRadioButton() override;
  virtual void  Quit         () override;   //
  virtual int   EndRun       () override;

  virtual void  InitEvent    () override; 
  void          UpdateViews  ();


  virtual TCanvas*  NewCanvas(const char* Name,
			      const char* Title,
			      Int_t       SizeX,
			      Int_t       SizeY) override;

  virtual int GetViewID(const char* View) override;

  virtual void OpenView(const char* View) override;
  virtual void OpenView(TStnView* Mother, int Px1, int Py, int Px2, int Py2) override;

  Int_t   OpenTrkXYView();
  Int_t   OpenTrkXYView(TStnView* Mother, Axis_t x1, Axis_t y1, Axis_t x2, Axis_t y2);
  
  Int_t   OpenTrkRZView();
  Int_t   OpenTrkRZView(TStnView* Mother, Axis_t x1, Axis_t y1, Axis_t x2, Axis_t y2);

  Int_t   OpenTrkTZView();
  Int_t   OpenTrkTZView(TStnView* Mother, Axis_t x1, Axis_t y1, Axis_t x2, Axis_t y2);

  Int_t   OpenCalView();
  Int_t   OpenCalView  (TStnView* Mother, Axis_t x1, Axis_t y1, Axis_t x2, Axis_t y2);
  
  Int_t   OpenCrvView();
  Int_t   OpenCrvView  (TStnView* Mother, Axis_t x1, Axis_t y1, Axis_t x2, Axis_t y2);
  
  Int_t   OpenVSTView();
  Int_t   OpenVSTView  (TStnView* Mother, Axis_t x1, Axis_t y1, Axis_t x2, Axis_t y2);
  
  void    CloseWindow();
//-----------------------------------------------------------------------------
// commands associated with buttons
//-----------------------------------------------------------------------------
  void    NextEvent        ();   //
  void    PrintColls(const char* Tag);

 
  // ClassDef(TStnVisManager, 0)
  };
#endif
