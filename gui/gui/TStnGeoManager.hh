#ifndef Stntuple_gui_TStnGeoManager_hh
#define Stntuple_gui_TStnGeoManager_hh

#include "TGeoVolume.h"
#include "TString.h"
#include "TGeoManager.h"

#include "Stntuple/geom/TStnTracker.hh"

class TEvdCrvSection;

class TStnGeoManager : public TNamed {
public:
  enum { kNCrvSections = 22 };
  
  TGeoNode*       fTop; 

  TGeoNode*       fDs2Vacuum; 
  TGeoNode*       fDs3Vacuum; 
  TGeoNode*       fSttMother; 
  TGeoNode*       fCalMother; 
  TGeoNode*       fTrkMother;
  TGeoNode*       fMbsMother;

  TEvdCrvSection* fCrvSection[kNCrvSections];

  TGeoManager*    fGeoManager;

  TStnTracker*    fTracker;

  int             fTransp;

  TStnGeoManager(const char* Name = "");
  TStnGeoManager(const char* Name, const char* Fn, int OriginalColors = 0);
  ~TStnGeoManager();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  TEvdCrvSection* CrvSection(int I) { return fCrvSection[I] ; }
//-----------------------------------------------------------------------------
// setters
//-----------------------------------------------------------------------------
  void SetRecursiveVisibility(TGeoVolume* Vol, int OnOff);
  void SetRecursiveVisibility(TGeoNode*   Vol, int OnOff);

  void SetDefaultColorTransp            (int Transp);
  void SetRecursiveColorTransp          (TGeoVolume* Vol , Int_t Color, Int_t Transp);
  void SetRecursiveColorTranspByName    (TGeoNode*   Vol , const char* Name   , Int_t Color, Int_t Transp);
  void SetRecursiveColorTranspByMaterial(TGeoNode*   Node, const char* MatName, Int_t Color, Int_t Transp);

  void SetRecursiveVisibilityColorTranspByNameAndMaterial(TGeoNode*   Top         ,
							  const char* Name        ,
							  const char* MatName     ,
							  int         Visibility  ,
							  int         Color       ,
							  int         Transparency);
    
  void SetRecursiveVisibilityByName    (TGeoNode* Node, const char* NamePattern, int OnOff);
  void SetRecursiveVisibilityByMaterial(TGeoNode* Node, const char* Material   , int OnOff);

  void HideTsCoils (int OriginalColors);
  void HideDsCoils (int OriginalColors);
  void HideBuilding(int OriginalColors);
//-----------------------------------------------------------------------------
// Mu2e-specific - Node name starts with 'Pattern' assume it is unique
//-----------------------------------------------------------------------------
  TGeoNode* FindNodeByName      (TGeoNode*   Top, const char* Name      );
  TGeoNode* FindNodeByVolumeName(TGeoNode*   Top, const char* VolumeName);
  TGeoNode* FindNodeByVolumeName(TGeoVolume* Top, const char* VolumeName);
//-----------------------------------------------------------------------------
// other methods
//-----------------------------------------------------------------------------
  void DrawCRV();
  void DrawStrawTracker();
  void DrawCalorimeter();
  void DrawCalorimeterDisk();
  void DrawExtShielding();
  void DrawDetectorSolenoid();
  void DrawDetectorSolenoidDev2();
//-----------------------------------------------------------------------------
// geometry initialization
//-----------------------------------------------------------------------------
  int InitGeometry(); 
  int InitCalorimeterGeometry();
  int InitCrvGeometry();
  int InitTrackerGeometry();

  ClassDef(TStnGeoManager,0)
};



















#endif

