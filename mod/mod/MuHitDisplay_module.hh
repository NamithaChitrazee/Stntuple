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
  string        fTimeClusterModuleLabel;
  string        _caloHitCollTag;
  string        fTrkExtrapol;
  string        fTrkCalMatch;
  string        fPidCollTag;
  
  GenId         _generatorID;
  string        _trackerStepPoints;

  double        _minEnergyDep;
  double        _timeWindow;

  StrawHitFlag  fGoodHitMask;
  StrawHitFlag  fBadHitMask;
  size_t        _minHits;

  bool          fDisplayBackgroundHits;
  bool          fPrintHits;
//-----------------------------------------------------------------------------
// Control for CRV-specific viewing
//-----------------------------------------------------------------------------
  bool				_showCRVOnly;
  bool				_showTracks;
  bool				foundTrkr;
  bool				foundTrkr_StrawHitColl;
  bool				foundTrkr_StrawDigiMCColl;
  bool				foundTrkr_StrawHitPosColl;
  bool				foundTrkr_StrawHitFlagColl;
  bool				foundCalo_CrystalHitColl;
  bool				foundCalo_ClusterColl;
  bool				foundCalo;

  fhicl::ParameterSet          _vmConfig;
//-----------------------------------------------------------------------------
// end of input parameters
// Options to control the display
// hit flag bits which should be ON and OFF
//-----------------------------------------------------------------------------
  TApplication*                               fApplication;

  const mu2e::GenParticleCollection*    _genParticleColl;         // 

  const mu2e::ComboHitCollection*       fShComboHitColl;     // 
  const mu2e::ComboHitCollection*       fComboHitColl;     // 

  const mu2e::StrawHitFlagCollection*   fStrawHitFlagColl; //
  const mu2e::StrawDigiMCCollection*    _strawDigiMCColl; //
  
  const mu2e::CaloHitCollection*        fListOfCrystalHits;//
  const mu2e::CaloClusterCollection*    fListOfClusters;   //
  
  const mu2e::StepPointMCCollection*    _spmcColl;         // on virtual detectors
  const mu2e::SimParticleCollection*    _simParticleColl;  //

  const mu2e::TimeClusterCollection*    fTimeClusterColl;  //

  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Right;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Left;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_TopDS;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_TopTS;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Dwnstrm;
  mu2e::CrvRecoPulseCollection*		fCrvPulseColl_Upstrm;
  
  const mu2e::KalRepPtrCollection*            _kalRepPtrColl;

  std::unique_ptr<McUtilsToolBase>            _mcUtils;


  mu2e::TimeCluster*     fTimeCluster;
  int                    fNClusters;
  int                    fNTracks[4];
  int                    _firstCall;
		
  TStnVisManager*        fVisManager;
  TStnHeaderBlock*       fHeaderBlock;

  TStnTrackID*           fTrackID;

  const Tracker*         fTracker;    // straw tracker geometry

  TNamedHandle*          fDarHandle;
  DoubletAmbigResolver*  fDar;

public:
  explicit MuHitDisplay(fhicl::ParameterSet const& pset);
  virtual ~MuHitDisplay();

  int      getData(const art::Event* Evt);
  void     InitVisManager();
  int      getCRVSection(int shieldNumber);

  void     printCaloCluster(const CaloCluster* Cl, const char* Opt);
//-----------------------------------------------------------------------------
// overloaded virtual methods of the base class
//-----------------------------------------------------------------------------
  virtual void     beginJob();
  virtual void     beginRun(const art::Run& aRun);
  virtual void     endRun  (const art::Run& aRun);
  virtual void     analyze (const art::Event& Evt);

  //  ClassDef(mu2e::MuHitDisplay,0)
};
}
#endif
