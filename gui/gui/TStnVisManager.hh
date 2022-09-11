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

  int                 fMinStation;
  int                 fMaxStation;
  int                 fTimeCluster;
  int                 fDebugLevel;
					// to display all the data in a given time window 
					// vis manager would enforces the same time limits on all views,
					// later those can be redefined individually
  float               fTMin;
  float               fTMax;
  float               fMbTime;

  int                 fDisplayStrawDigiMC;
  int                 fDisplayStrawHitsXY;
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

  int            DisplayStrawDigiMC() { return fDisplayStrawDigiMC; }
  
  int            MinStation() { return fMinStation; }
  int            MaxStation() { return fMaxStation; }
  int            TimeCluster() { return fTimeCluster; }

  float          TMin() { return fTMin; }
  float          TMax() { return fTMax; }

  float          MbTime() { return fMbTime; }

  void           GetTimeWindow(float& TMin, float& TMax) {
    TMin = fTMin;
    TMax = fTMax;
  }

  int DisplayStrawHitsXY() { return fDisplayStrawHitsXY; }
  //-----------------------------------------------------------------------------
  // modifiers
  //-----------------------------------------------------------------------------
  void SetEvent(const art::Event* Evt) { fEvent = Evt; }

  void SetDisplayStrawDigiMC(int Display) {
    fDisplayStrawDigiMC = Display;
  }

  void SetDisplayStrawHitsXY(int Flag) {
    fDisplayStrawHitsXY = Flag;
  }

  void SetStations(int IMin, int IMax) override;
  void SetTimeCluster(int I) override;
  
  void   SetTimeWindow(float TMin, float TMax) override {
    fTMin = TMin;
    fTMax = TMax;
  }

  void  SetMbTime(float MbTime) { fMbTime = MbTime; }

  void  UpdateViews();

  virtual int EndRun() override ;

  virtual TCanvas*  NewCanvas(const char* Name,
			      const char* Title,
			      Int_t       SizeX,
			      Int_t       SizeY) override;

  virtual int GetViewID(const char* View) override;

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
  void    Quit             ();   //
  
  // ClassDef(TStnVisManager, 0)
};
#endif
