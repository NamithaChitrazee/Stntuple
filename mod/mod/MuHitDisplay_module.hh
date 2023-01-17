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
#include "Offline/RecoDataProducts/inc/StrawDigi.hh"

#include "Offline/RecoDataProducts/inc/CrvRecoPulse.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"

#include "Offline/BTrkData/inc/TrkStrawHit.hh"
#include "Offline/RecoDataProducts/inc/KalRepPtrCollection.hh"

#include "Offline/RecoDataProducts/inc/HelixSeed.hh"

#include "Offline/TrkReco/inc/DoubletAmbigResolver.hh"

#include "TApplication.h"

#include "Stntuple/base/TNamedHandle.hh"
#include "Stntuple/gui/TStnVisManager.hh"

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
  string        _shfCollTag;            // straw hit flags
  string        _comboHitCollTag;
  string        _chfCollTag;            // straw hit flags
  string        _sdCollTag;		// strawDigiCollTag
  string        _sdmcCollTag;
  string        _swCollTag;
  
  string        _helixSeedCollTag;	// helix seed collection tag
  string        _ksfCollTag;
  string        _kffCollTag;
  string        _trackCollTag;
  string        _simpCollTag;
  string        _timeClusterCollTag;
  string        _phiClusterCollTag;
  string        _caloHitCollTag;
  string        _trkExtrapol;
  string        _trkCalMatch;
  string        _pidCollTag;
  string        _ppTag;			// primary particle tag
  string        _vdHitsCollTag;
  string        _defaultView;           // view open in the first window
  
  GenId         _generatorID;

  double        _minEnergyDep;          // unused
  double        _timeWindow;            // unused

  // StrawHitFlag  fGoodHitMask;           // unused
  // StrawHitFlag  fBadHitMask;            // unused
  // size_t        _minHits;               // unused
  double        _minSimpMomentum;       // to initialize Simp Block
//-----------------------------------------------------------------------------
// Control for CRV-specific viewing
//-----------------------------------------------------------------------------
  bool				_showCRVOnly;
  bool				_showTracks;

  fhicl::ParameterSet           _vmConfig;
//-----------------------------------------------------------------------------
// end of input parameters
// Options to control the display
// hit flag bits which should be ON and OFF
//-----------------------------------------------------------------------------
  TApplication*                               fApplication;

  const mu2e::GenParticleCollection*          _genpColl;        // 

  const mu2e::StrawDigiCollection*            _sdColl;   //
  const mu2e::StrawDigiMCCollection*          _sdmcColl; //
  const mu2e::StrawDigiADCWaveformCollection* _swColl;   //
  const mu2e::StrawHitFlagCollection*         _shfColl;  //
  
  const mu2e::CaloHitCollection*              _caloHitColl;      //
  const mu2e::CaloClusterCollection*          _caloClusterColl;  //

  const mu2e::StepPointMCCollection*          _spmcColl;         // on virtual detectors
  const mu2e::SimParticleCollection*          _simpColl;         //

  mu2e::CrvRecoPulseCollection*		      fCrvPulseColl_Right;
  mu2e::CrvRecoPulseCollection*		      fCrvPulseColl_Left;
  mu2e::CrvRecoPulseCollection*		      fCrvPulseColl_TopDS;
  mu2e::CrvRecoPulseCollection*		      fCrvPulseColl_TopTS;
  mu2e::CrvRecoPulseCollection*		      fCrvPulseColl_Dwnstrm;
  mu2e::CrvRecoPulseCollection*		      fCrvPulseColl_Upstrm;
  
  std::unique_ptr<McUtilsToolBase>            _mcUtils;
//-----------------------------------------------------------------------------
// need to detect the first call to initialize fVisManager
//-----------------------------------------------------------------------------
  int                                         _firstCall;    
  TStnVisManager*                             fVisManager;
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

  const mu2e::GenParticleCollection*            GetGenppColl      () { return _genpColl       ; }
  const mu2e::SimParticleCollection*            GetSimpColl       () { return _simpColl       ; }
  const mu2e::StepPointMCCollection*            GetSpmcColl       () { return _spmcColl       ; }
  const mu2e::StrawDigiCollection*              GetSdColl         () { return _sdColl         ; }
  const mu2e::StrawDigiADCWaveformCollection*   GetSwColl         () { return _swColl         ; }
  const mu2e::StrawDigiMCCollection*            GetSdmcColl       () { return _sdmcColl       ; }

  TSimpBlock*                                   GetSimpBlock      () { return fSimpBlock      ; }

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
