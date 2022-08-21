///////////////////////////////////////////////////////////////////////////////
// interactive 2D event display. 
//
// $Id: MuHitDisplay_module.cc,v 1.6 2014/09/20 17:54:06 murat Exp $
// $Author: murat $
// $Date: 2014/09/20 17:54:06 $
//
// Contact person:  Pavel Murat, Gianantonio Pezzulo
///////////////////////////////////////////////////////////////////////////////
#ifndef Stntuple_mod_MuHitDisplay
#define Stntuple_mod_MuHitDisplay

#include <iostream>
#include <string>

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Principal/Selector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Offline/ConditionsService/inc/ConditionsHandle.hh"
#include "Offline/ConditionsService/inc/AcceleratorParams.hh"
#include "Offline/TrackerConditions/inc/StrawResponse.hh"

#include "Offline/GeometryService/inc/GeometryService.hh"
#include "Offline/GeometryService/inc/GeomHandle.hh"

#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/CalorimeterGeom/inc/DiskCalorimeter.hh"
#include "Offline/CalorimeterGeom/inc/Calorimeter.hh"
#include "Offline/Mu2eUtilities/inc/SortedStepPoints.hh"
#include "Offline/Mu2eUtilities/inc/TrackTool.hh"

#include "Offline/MCDataProducts/inc/GenParticle.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"
#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"

#include "BTrk/TrkBase/HelixParams.hh"
#include "BTrk/KalmanTrack/KalHit.hh"

#include "Offline/RecoDataProducts/inc/CaloHit.hh"
#include "Offline/RecoDataProducts/inc/CaloCluster.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/StrawHitFlag.hh"

#include "Offline/RecoDataProducts/inc/CrvRecoPulse.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"

#include "Offline/BTrkData/inc/TrkStrawHit.hh"
#include "Offline/RecoDataProducts/inc/KalRepPtrCollection.hh"

#include "Offline/TrkReco/inc/DoubletAmbigResolver.hh"

#include "TApplication.h"

#include "Stntuple/base/TNamedHandle.hh"
#include "Stntuple/gui/TStnVisManager.hh"
#include "Stntuple/gui/THeaderVisNode.hh"
#include "Stntuple/gui/TCalVisNode.hh"
#include "Stntuple/gui/TCrvVisNode.hh"
#include "Stntuple/gui/TTrkVisNode.hh"

#include "Stntuple/gui/TMcTruthVisNode.hh"

#include "Stntuple/alg/TStnTrackID.hh"

#include "Stntuple/print/TAnaDump.hh"
#include "Stntuple/mod/THistModule.hh"

#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "Stntuple/obj/TSimpBlock.hh"

#include "Offline/Mu2eUtilities/inc/McUtilsToolBase.hh"

using namespace std;
using CLHEP::Hep3Vector;

namespace mu2e {

class MuHitDisplay : public THistModule {
private:
//-----------------------------------------------------------------------------
// Input parameters: Module labels 
//-----------------------------------------------------------------------------
  string        _moduleLabel;	             // this module label
  string        _processName;
  string        _genpCollTag;
  string        _spmcCollTag;
  string        _caloClusterCollTag;
  string        _crvRecoPulseCollTag;

  string        _strawHitCollTag;
  string        _comboHitCollTag;
  string        _strawDigiMCCollTag;
  string        _strawHitFlagCollTag;
  
  string        _trackCollTag;
  string        _simpCollTag;
  string        _timeClusterCollTag;
  string        _caloHitCollTag;
  string        _trkExtrapol;
  string        _trkCalMatch;
  string        _pidCollTag;
  string        _ppTag;			// primary particle tag
  string        _vdHitsCollTag;
  
  GenId         _generatorID;

  double        _minEnergyDep;
  double        _timeWindow;

  StrawHitFlag  fGoodHitMask;
  StrawHitFlag  fBadHitMask;
  size_t        _minHits;
  double        _minSimpMomentum;

  bool          fDisplayBackgroundHits;
  bool          fPrintHits;
//-----------------------------------------------------------------------------
// Control for CRV-specific viewing
//-----------------------------------------------------------------------------
  bool				_showCRVOnly;
  bool				_showTracks;
  // bool				foundTrkr;
  bool				foundTrkr_StrawHitColl;
  bool				foundTrkr_StrawDigiMCColl;
  bool				foundTrkr_StrawHitPosColl;
  bool				foundTrkr_StrawHitFlagColl;
  bool				foundCalo_CrystalHitColl;
  bool				foundCalo_ClusterColl;
  //  bool				foundCalo;

  fhicl::ParameterSet          _vmConfig;
//-----------------------------------------------------------------------------
// end of input parameters
// Options to control the display
// hit flag bits which should be ON and OFF
//-----------------------------------------------------------------------------
  TApplication*                         fApplication;

  const mu2e::GenParticleCollection*    _genpColl;         // 

  const mu2e::ComboHitCollection*       _strawHitColl;     // 
  const mu2e::ComboHitCollection*       _comboHitColl;     // 

  const mu2e::StrawHitFlagCollection*   _strawHitFlagColl; //
  const mu2e::StrawDigiMCCollection*    _strawDigiMCColl;  //
  
  const mu2e::CaloHitCollection*        _caloHitColl;      //
  const mu2e::CaloClusterCollection*    _caloClusterColl;  //
  
  const mu2e::StepPointMCCollection*    _spmcColl;         // on virtual detectors
  const mu2e::SimParticleCollection*    _simpColl;         //

  const mu2e::TimeClusterCollection*    _timeClusterColl;  //

  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Right;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Left;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_TopDS;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_TopTS;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Dwnstrm;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Upstrm;
  
  const mu2e::KalRepPtrCollection*      _kalRepPtrColl;

  std::unique_ptr<McUtilsToolBase>      _mcUtils;

  // mu2e::TimeCluster*     _timeCluster;
  int                    fNClusters;
  int                    fNTracks[4];
  int                    _firstCall;
		
  TStnVisManager*        fVisManager;
//-----------------------------------------------------------------------------
// reuse STNTUPLE data blocks
//-----------------------------------------------------------------------------
  TStnHeaderBlock*       fHeaderBlock;
  TSimpBlock*            fSimpBlock;

  TStnTrackID*           fTrackID;

  const Tracker*         fTracker;    // straw tracker geometry

  TNamedHandle*          fDarHandle;
  DoubletAmbigResolver*  fDar;

public:
  explicit MuHitDisplay(fhicl::ParameterSet const& pset);
  virtual ~MuHitDisplay();

  int      getData      (const art::Event* Evt);
  int      getCRVSection(int   shieldNumber   );

  const mu2e::ComboHitCollection*    GetComboHitColl   () { return _comboHitColl   ; }
  const mu2e::GenParticleCollection* GetGenppColl      () { return _genpColl       ; }
  const mu2e::ComboHitCollection*    GetStrawHitColl   () { return _strawHitColl   ; }
  const mu2e::KalRepPtrCollection*   GetKalRepPtrColl  () { return _kalRepPtrColl  ; }
  const mu2e::SimParticleCollection* GetSimpColl       () { return _simpColl       ; }
  const mu2e::StepPointMCCollection* GetSpmcColl       () { return _spmcColl       ; }
  const mu2e::TimeClusterCollection* GetTimeClusterColl() { return _timeClusterColl; }

  TSimpBlock*                        GetSimpBlock      () { return fSimpBlock      ; }

  void     InitVisManager();

  // void     printCaloCluster(const CaloCluster* Cl, const char* Opt);
//-----------------------------------------------------------------------------
// overloaded virtual methods of the base class - EDAnalyzer 
//-----------------------------------------------------------------------------
  virtual void     beginJob();
  virtual void     beginRun(const art::Run& aRun);
  virtual void     endRun  (const art::Run& aRun);
  virtual void     analyze (const art::Event& Evt);

  //  ClassDef(mu2e::MuHitDisplay,0)
};
}
#endif
