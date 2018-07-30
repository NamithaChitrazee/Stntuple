///////////////////////////////////////////////////////////////////////////////
// Class StntupleMaker : fills Stntuple (P.Murat)
// ------------------------------------------
// order of the data blocks is essential - they are filled according to the
// order in which they are declared...
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include <string>
#include <cstdio>

#include "Stntuple/obj/AbsEvent.hh"
#include "Stntuple/obj/TStnEvent.hh"

#include "fhiclcpp/ParameterSet.h"

#include <assert.h>
#include <iostream>
#include <iomanip>

#include "TNamed.h"
#include "TH1.h"
#include "TString.h"
#include "TProfile.h"
#include "TFolder.h"
#include "TSystem.h"

#include "Stntuple/obj/TStnNode.hh"
#include "Stntuple/obj/TStnErrorLogger.hh"
#include "Stntuple/obj/TStnTrackBlock.hh"
#include "Stntuple/obj/TStnHelixBlock.hh"
#include "Stntuple/obj/TStrawDataBlock.hh"
#include "Stntuple/obj/TCalDataBlock.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"

//  #include "Stntuple/obj/TStnTriggerBlock.hh"

#include "Stntuple/mod/StntupleModule.hh"
#include "Stntuple/mod/StntupleGlobals.hh"
// #include "Stntuple/mod/StntupleMaker_module.hh"

#include "Stntuple/mod/InitStntupleDataBlocks.hh"
#include "Stntuple/mod/StntupleUtilities.hh"

#include "RecoDataProducts/inc/TrkFitDirection.hh"
#include "BTrk/TrkBase/TrkParticle.hh"

#include "Stntuple/base/TNamedHandle.hh"
#include "Stntuple/alg/TStntuple.hh"
// #include "Stntuple/mod/StntupleGlobals.hh"

#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "TrkReco/inc/DoubletAmbigResolver.hh"
#include "TrkDiag/inc/KalDiag.hh"

using namespace std; 

// ClassImp(StntupleMaker)

static const char rcsid[] = "$Name:  $";
// stntuple_get_version is autogenerated, see Stntuple/scripts/create_print_header_routine.sh
void stntuple_get_version(char* ver, char* test);
namespace mu2e {
class StntupleMaker : public StntupleModule {
//------------------------------------------------------------------------------
//  data members
//------------------------------------------------------------------------------
protected:
					// process name, default - PROD
  std::string      fProcessName;
					// switches for individual branches
  int              fMakeCalData;
  int              fMakeClusters;
  int              fMakeGenp;
  int              fMakePid;
  int              fMakeSimp;
  int              fMakeStepPointMC;
  int              fMakeStrawData;
  int              fMakeTracks;
  int              fMakeTrackStrawHits;
  int              fMakeTimeClusters;
  int              fMakeHelices;
  int              fMakeTrackSeeds;
  int              fMakeTrigger;
  int              fMakeVdetHits;
//-----------------------------------------------------------------------------
// module parameters
// generator
//-----------------------------------------------------------------------------
  std::string              fG4ModuleLabel;
  std::string              fGeneratorModuleLabel;    // defines collection to save, default: "" (all)
  std::string              fMakeStrawHitModuleLabel;
  std::string              fMakeStrawDigiModuleLabel;
  std::string              fMakeSimpModuleLabel;     // name of the module produced SimParticleCollection
  std::string              fVDCollName;
  std::string              fMakeVdetHitsModuleLabel; // name of the module produced SimParticleCollection

  std::vector<std::string> fShortHelixBlockName;
  std::vector<std::string> fShortHelixModuleLabel;
  std::vector<std::string> fShortTrackSeedBlockName;
  std::vector<std::string> fShortTrackSeedModuleLabel;
  std::vector<std::string> fTimeClusterBlockName;
  std::vector<std::string> fTimeClusterModuleLabel;
  std::vector<std::string> fHelixBlockName;
  std::vector<std::string> fHelixModuleLabel;
  std::vector<std::string> fTrackSeedBlockName;
  std::vector<std::string> fTrackSeedModuleLabel;
  std::vector<std::string> fTrackBlockName;
  std::vector<std::string> fTrkRecoModuleLabel;
  std::vector<std::string> fTrkExtrapolModuleLabel;
  std::vector<std::string> fTrkCaloMatchModuleLabel;
  std::vector<std::string> fPidBlockName;
  std::vector<std::string> fPidModuleLabel;
  std::vector<std::string> fTrackStrawHitBlockName;

  std::vector<std::string> fStepPointMCBlockName;
  std::vector<std::string> fStepPointMCModuleLabel;
  std::vector<std::string> fStepPointMCProductName;

  std::vector<int>         fFitParticle;
  std::vector<int>         fFitDirection;

  std::string              fCaloCrystalHitMaker;
  std::string              fCaloClusterMaker;
  
  double                   fMinTActive  ;  // start of the active window
  double                   fMinECrystal ;  // 

  TNamed*                  fVersion;

  TNamedHandle*            fDarHandle;
  TNamedHandle*            fKalDiagHandle;
  TNamedHandle*            fTimeOffsetsHandle;

  DoubletAmbigResolver*    fDar;
  KalDiag*                 fKalDiag;
  SimParticleTimeOffset*   fTimeOffsets;

//------------------------------------------------------------------------------
// function members
//------------------------------------------------------------------------------
public:
					// constructors and destructor

  StntupleMaker(fhicl::ParameterSet const& pset);

  ~StntupleMaker();
//-----------------------------------------------------------------------------
// functions of the module
//-----------------------------------------------------------------------------
  void GetDefTrackCollName(char* Name);

					// ****** setters

//-----------------------------------------------------------------------------
// overloaded virtual functions of EDFilter
//-----------------------------------------------------------------------------
  virtual bool beginRun(art::Run& ARun);
  virtual bool endRun  (art::Run& ARun);
  virtual void beginJob();
  virtual void endJob  ();
  virtual bool filter  (AbsEvent& event);

  //  ClassDef(StntupleMaker,0)
};



//------------------------------------------------------------------------------
// constructors
//------------------------------------------------------------------------------
StntupleMaker::StntupleMaker(fhicl::ParameterSet const& PSet): 
  StntupleModule        (PSet,"StntupleMaker")
  , fProcessName        (PSet.get<string>      ("processName"    ))
  , fMakeCalData        (PSet.get<int>         ("makeCalData"    ))
  , fMakeClusters       (PSet.get<int>         ("makeClusters"   ))
  , fMakeGenp           (PSet.get<int>         ("makeGenp"       ))
  , fMakePid            (PSet.get<int>         ("makePid"        ))
  , fMakeSimp           (PSet.get<int>         ("makeSimp"       ))
  , fMakeStepPointMC    (PSet.get<int>         ("makeStepPointMC"))
  , fMakeStrawData      (PSet.get<int>         ("makeStrawData"  ))
  , fMakeTracks         (PSet.get<int>         ("makeTracks"     ))
  , fMakeTrackStrawHits (PSet.get<int>         ("makeTrackStrawHits"))
  , fMakeTimeClusters   (PSet.get<int>         ("makeTimeClusters"  ))
  , fMakeHelices        (PSet.get<int>         ("makeHelices"    ))
  , fMakeTrackSeeds     (PSet.get<int>         ("makeTrackSeeds" ))
  , fMakeTrigger        (PSet.get<int>         ("makeTrigger"    ))
  , fMakeVdetHits       (PSet.get<int>         ("makeVirtualHits"))
  
  , fG4ModuleLabel            (PSet.get<string>        ("g4ModuleLabel"           ))
  , fGeneratorModuleLabel     (PSet.get<string>        ("generatorModuleLabel"    ))
  , fMakeStrawHitModuleLabel  (PSet.get<string>        ("makeStrawHitModuleLabel" ))
  , fMakeStrawDigiModuleLabel (PSet.get<string>        ("makeStrawDigiModuleLabel"))
  , fMakeSimpModuleLabel      (PSet.get<string>        ("makeSimpModuleLabel"     ))
  , fVDCollName               (PSet.get<string>        ("vdCollName"              ))
  , fMakeVdetHitsModuleLabel  (PSet.get<string>        ("makeVdetHitsModuleLabel" ))
  , fShortHelixBlockName      (PSet.get<vector<string>>("shortHelixBlockName"      ))
  , fShortHelixModuleLabel    (PSet.get<vector<string>>("shortHelixModuleLabel"    ))
  , fShortTrackSeedBlockName  (PSet.get<vector<string>>("shortTrackSeedBlockName"  ))
  , fShortTrackSeedModuleLabel(PSet.get<vector<string>>("shortTrackSeedModuleLabel")) 
  , fTimeClusterBlockName     (PSet.get<vector<string>>("timeClusterBlockName"    ))
  , fTimeClusterModuleLabel   (PSet.get<vector<string>>("timeClusterModuleLabel"  ))
  , fHelixBlockName           (PSet.get<vector<string>>("helixBlockName"          ))
  , fHelixModuleLabel        (PSet.get<vector<string>>("helixModuleLabel"        ))
  , fTrackSeedBlockName      (PSet.get<vector<string>>("trackSeedBlockName"      ))
  , fTrackSeedModuleLabel    (PSet.get<vector<string>>("trackSeedModuleLabel"    ))
  , fTrackBlockName          (PSet.get<vector<string>>("trackBlockName"          ))
  , fTrkRecoModuleLabel      (PSet.get<vector<string>>("trkRecoModuleLabel"      ))
  , fTrkExtrapolModuleLabel  (PSet.get<vector<string>>("trkExtrapolModuleLabel"  ))
  , fTrkCaloMatchModuleLabel (PSet.get<vector<string>>("trkCaloMatchModuleLabel" ))
  , fPidBlockName            (PSet.get<vector<string>>("pidBlockName"            ))
  , fPidModuleLabel          (PSet.get<vector<string>>("pidModuleLabel"          ))
  , fTrackStrawHitBlockName  (PSet.get<vector<string>>("trackStrawHitBlockName"  ))
  
  , fStepPointMCBlockName    (PSet.get<vector<string>>("stepPointMCBlockName"    ))
  , fStepPointMCModuleLabel  (PSet.get<vector<string>>("stepPointMCModuleLabel"  ))
  , fStepPointMCProductName  (PSet.get<vector<string>>("stepPointMCProductName"  ))

  , fFitParticle            (PSet.get<vector<int>>        ("fitParticle" ))
  , fFitDirection           (PSet.get<vector<int>>        ("fitDirection"))

  , fCaloCrystalHitMaker(PSet.get<string> ("caloCrystalHitsMaker"))
  , fCaloClusterMaker   (PSet.get<string> ("caloClusterMaker"    ))
  , fMinTActive         (PSet.get<double> ("minTActive" ))
  , fMinECrystal        (PSet.get<double> ("minECrystal"))
{

  char  ver[20], text[200];
  stntuple_get_version(ver,text);

  fVersion      = new TNamed(ver,text);
  TModule::fFolder->Add(fVersion);

  fhicl::ParameterSet pset_to = PSet.get<fhicl::ParameterSet>("TimeOffsets");
  
  int len = pset_to.get_names().size();
  if (len > 0) fTimeOffsets       = new SimParticleTimeOffset(pset_to);
  else         fTimeOffsets       = NULL;

  fTimeOffsetsHandle = new TNamedHandle("TimeOffsetsHandle",fTimeOffsets);

  fDar           = new DoubletAmbigResolver (PSet.get<fhicl::ParameterSet>("DoubletAmbigResolver"),0.,0,0);
  fDarHandle     = new TNamedHandle("DarHandle",fDar);

  fKalDiag       = new KalDiag(PSet.get<fhicl::ParameterSet>("KalDiag",fhicl::ParameterSet()));
  fKalDiagHandle = new TNamedHandle("KalDiagHandle",fKalDiag);

  fFolder->Add(fDarHandle);
  fFolder->Add(fKalDiagHandle);
  fFolder->Add(fTimeOffsetsHandle);
}


//------------------------------------------------------------------------------
StntupleMaker::~StntupleMaker() {
  delete fTimeOffsets;
  delete fDar;
  delete fDarHandle;
  delete fVersion;
}


//------------------------------------------------------------------------------
bool StntupleMaker::beginRun(art::Run& aRun) {

  static int first_begin_run = 1;

  THistModule::beforeBeginRun(aRun);

  if (first_begin_run) {
//-----------------------------------------------------------------------------
// if we runnning stnmaker_prod.exe, save revision of the TCL file in STNTUPLE
//-----------------------------------------------------------------------------
    first_begin_run = 0;
    const char* c = gSystem->Getenv("STNMAKER_PROD_TCL");
    if (c) TModule::fFolder->Add(new TNamed("STNMAKER_PROD_TCL",c));
    else   TModule::fFolder->Add(new TNamed("STNMAKER_PROD_TCL","unknown"));
  }

  THistModule::afterBeginRun(aRun);

  return 1;
}

//------------------------------------------------------------------------------
bool StntupleMaker::endRun(art::Run& aRun ) {
  THistModule::beforeEndRun(aRun);
  THistModule::afterEndRun (aRun);
  return 1;
}


//------------------------------------------------------------------------------
void StntupleMaker::endJob() {

  THistModule::beforeEndJob();
  THistModule::afterEndJob ();

}

//------------------------------------------------------------------------------
void StntupleMaker::beginJob() {

  int split_mode, compression_level, buffer_size;

  string      _iname1;	// data instance name

  THistModule::beforeBeginJob();

  // create data blocks and branches

  fgStntupleFolder->Add(new TNamed("ProcessName"     ,fProcessName));

					// for the moment do it by hands...
					// create default branches to go into 
					// STNTUPLE

  split_mode        = THistModule::SplitLevel();
  compression_level = THistModule::CompressionLevel();
  buffer_size       = THistModule::BufferSize();
//-----------------------------------------------------------------------------
// header block is always there
//-----------------------------------------------------------------------------
  TStnHeaderBlock* header = (TStnHeaderBlock*) Event()->GetDataBlock("HeaderBlock");
  header->AddCollName("mu2e::StrawHitCollection",fMakeStrawHitModuleLabel.data(),"");
//-----------------------------------------------------------------------------
// calorimeter hit data
// this is not RAW hit data yet...
//-----------------------------------------------------------------------------
  if (fMakeCalData) {
    TStnDataBlock* cal_data;

    cal_data = AddDataBlock("CalDataBlock","TCalDataBlock",
			    StntupleInitMu2eCalDataBlock,
			    buffer_size,
			    split_mode,
			    compression_level);
    if (cal_data) {
      cal_data->AddCollName("mu2e::CaloCrystalHitCollection",fCaloCrystalHitMaker.data(),"");
    }
  }
//-----------------------------------------------------------------------------
// straw hit data
//-----------------------------------------------------------------------------
  if (fMakeStrawData) {
    TStnDataBlock* straw_data;

    straw_data = AddDataBlock("StrawDataBlock","TStrawDataBlock",
			      StntupleInitMu2eStrawDataBlock,
			      buffer_size,
			      split_mode,
			      compression_level);
    if (straw_data) {
      straw_data->AddCollName("mu2e::StrawHitCollection",fMakeStrawHitModuleLabel.data(),"");
    }
  }
//----------------------------------------------------------------------------
// short helix data (from SeedFit modules)
//----------------------------------------------------------------------------
  if (fMakeTracks) {
    TStnDataBlock* helix_data, *trackSeed_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fShortHelixBlockName.size();

    for (int i=0; i<nblocks; i++) {
      
      block_name = fShortHelixBlockName[i].data();

      helix_data = AddDataBlock(block_name, 
				"TStnHelixBlock",
				StntupleInitMu2eHelixBlock,
				buffer_size,
				split_mode,
				compression_level);
      
      //      SetResolveLinksMethod(block_name,StntupleInitMu2eHelixBlockLinks);
      
      if (helix_data) {
	helix_data->AddCollName("mu2e::HelixSeedCollection", fShortHelixModuleLabel[i].data(),"");
	helix_data->AddCollName("ShortHelixBlockName"      , fShortHelixBlockName[i].data()    ,"");
      }
    }
    
    
    // short trackSeed collections
    nblocks = fShortTrackSeedBlockName.size();

    for (int i=0; i<nblocks; i++) {
      
      block_name = fShortTrackSeedBlockName[i].data();

      trackSeed_data = AddDataBlock(block_name, 
				    "TStnTrackSeedBlock",
				    StntupleInitMu2eTrackSeedBlock,
				    buffer_size,
				    split_mode,
				    compression_level);

      //     SetResolveLinksMethod(block_name,StntupleInitMu2eTrackSeedBlockLinks);
      
      if (trackSeed_data) {
	trackSeed_data->AddCollName("mu2e::KalSeedCollection"  ,fShortTrackSeedModuleLabel[i].data(),"");
	trackSeed_data->AddCollName("ShortTrackSeedBlockName"  ,fShortTrackSeedBlockName[i].data(),"");
      }
    }
    
  }
//--------------------------------------------------------------------------------
// helix data
//--------------------------------------------------------------------------------
  if (fMakeTimeClusters) {
    TStnDataBlock* timeCluster_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fTimeClusterBlockName.size();

    for (int i=0; i<nblocks; i++) {
      
      block_name = fTimeClusterBlockName[i].data();
      
      timeCluster_data = AddDataBlock(block_name, 
				      "TStnTimeClusterBlock",
				      StntupleInitMu2eTimeClusterBlock,
				      buffer_size,
				      split_mode,
				      compression_level);
      
      //      SetResolveLinksMethod(block_name,StntupleInitMu2eTimeClusterBlockLinks);
      
      if (timeCluster_data) {
	timeCluster_data->AddCollName("mu2e::HelixSeedCollection"  ,fHelixModuleLabel[i].data()      ,"");
 	timeCluster_data->AddCollName("mu2e::TimeClusterCollection",fTimeClusterModuleLabel[i].data(),"");
     }
    }
  }

//--------------------------------------------------------------------------------
// helix data
//--------------------------------------------------------------------------------
  if (fMakeHelices) {
    TStnDataBlock* helix_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fHelixBlockName.size();

    for (int i=0; i<nblocks; i++) {
      
      block_name = fHelixBlockName[i].data();

      helix_data = AddDataBlock(block_name, 
				    "TStnHelixBlock",
				    StntupleInitMu2eHelixBlock,
				    buffer_size,
				    split_mode,
				    compression_level);
      
      //      SetResolveLinksMethod(block_name,StntupleInitMu2eHelixBlockLinks);
      
      if (helix_data) {
	helix_data->AddCollName("mu2e::HelixSeedCollection"  ,fHelixModuleLabel[i].data()   ,"");
 	helix_data->AddCollName("mu2e::TimeClusterCollection",fTimeClusterModuleLabel[i].data(),"");
     }
    }
  }
//--------------------------------------------------------------------------------
// trackSeed data
//--------------------------------------------------------------------------------
  if (fMakeTrackSeeds) {
    TStnDataBlock* trackSeed_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fTrackSeedBlockName.size();

    for (int i=0; i<nblocks; i++) {
      
      block_name = fTrackSeedBlockName[i].data();

      trackSeed_data = AddDataBlock(block_name, 
				    "TStnTrackSeedBlock",
				    StntupleInitMu2eTrackSeedBlock,
				    buffer_size,
				    split_mode,
				    compression_level);

      //     SetResolveLinksMethod(block_name,StntupleInitMu2eTrackSeedBlockLinks);
      
      if (trackSeed_data) {
	trackSeed_data->AddCollName("mu2e::HelixSeedCollection",fHelixModuleLabel[i].data()    ,"");
	trackSeed_data->AddCollName("mu2e::KalSeedCollection"  ,fTrackSeedModuleLabel[i].data(),"");
	trackSeed_data->AddCollName("HelixBlockName"           ,fHelixBlockName[i].data()       ,"");
	trackSeed_data->AddCollName("ShortHelixBlockName"      ,fShortHelixBlockName[i].data(),"");
	trackSeed_data->AddCollName("ShortTrackSeedBlockName"  ,fShortTrackSeedBlockName[i].data(),"");
      }

      SetResolveLinksMethod(block_name,StntupleInitMu2eTrackSeedBlockLinks);
    }
  }

//-----------------------------------------------------------------------------
// track straw hits
//-----------------------------------------------------------------------------
  if (fMakeTrackStrawHits) {
    TStnDataBlock  *track_straw_hit_data;
    const char     *block_name;

    int nblocks = fTrackStrawHitBlockName.size();

    for (int i=0; i<nblocks; i++) {
					// always store defTracks for the 
					// default process in the "TrackBlock"

      block_name = fTrackStrawHitBlockName[i].data();
      track_straw_hit_data = AddDataBlock(block_name,
					  "TTrackStrawHitBlock",
					  StntupleInitMu2eTrackStrawHitBlock,
					  buffer_size,
					  split_mode,
					  compression_level);

      if (track_straw_hit_data) {
	track_straw_hit_data->AddCollName("mu2e::KalRepCollection"              ,fTrkRecoModuleLabel[i].data   (),"");
	track_straw_hit_data->AddCollName("mu2e::StrawHitCollection"            ,fMakeStrawHitModuleLabel.data (),"");
	track_straw_hit_data->AddCollName("mu2e::PtrStepPointMCVectorCollection",fMakeStrawDigiModuleLabel.data(),"");
	
	//      SetResolveLinksMethod(block_name,StntupleInitMu2eTrackBlockLinks);
      }
    }
  }
//-----------------------------------------------------------------------------
// 2 trigger data branches: one for the trigger data, 
// another one for the emulated trigger data
// always NON-SPLIT
//-----------------------------------------------------------------------------
  // if (fMakeTrigger.value()) {
  //   TStnTriggerBlock *data;
  //   data = (TStnTriggerBlock*) AddDataBlock("TriggerBlock","TStnTriggerBlock",
  // 					    StntupleInitTriggerBlock,
  // 					    buffer_size.value(),
  // 					    -1,
  // 					    compression_level.value());
  //   data->SetL3Source(fL3Source.value());
  // }
//-----------------------------------------------------------------------------
// track branches: for ROOT v3 to use streamers one has to specify split=-1
//-----------------------------------------------------------------------------
  if (fMakeTracks) {
    TStnDataBlock *track_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fTrackBlockName.size();

    for (int i=0; i<nblocks; i++) {
					// always store defTracks for the 
					// default process in the "TrackBlock"

      block_name = fTrackBlockName[i].data();
      track_data = AddDataBlock(block_name,
				"TStnTrackBlock",
				StntupleInitMu2eTrackBlock,
				buffer_size,
				split_mode,
				compression_level);


         //      SetResolveLinksMethod(block_name,StntupleInitMu2eTrackBlockLinks);
      int       nshortblocks = fHelixBlockName.size();
      if (track_data) {
	track_data->AddCollName("mu2e::KalRepCollection"              ,fTrkRecoModuleLabel[i].data()     ,"");
	track_data->AddCollName("mu2e::ComboHitCollection"            ,fMakeStrawHitModuleLabel.data()   ,"");
	track_data->AddCollName("mu2e::StrawDigiMCCollection"         ,fMakeStrawDigiModuleLabel.data()  ,"");
	track_data->AddCollName("mu2e::PtrStepPointMCVectorCollection",fMakeStrawDigiModuleLabel.data()  ,"");
	track_data->AddCollName("mu2e::TrkCaloIntersectCollection"    ,fTrkExtrapolModuleLabel [i].data(),"");
	track_data->AddCollName("mu2e::CaloClusterCollection"         ,fCaloClusterMaker.data()          ,"");
	track_data->AddCollName("mu2e::TrackClusterMatchCollection"   ,fTrkCaloMatchModuleLabel[i].data(),"");
	track_data->AddCollName("mu2e::PIDProductCollection"          ,fPidModuleLabel[i].data()         ,"");
	track_data->AddCollName("mu2e::StepPointMCCollection"         ,fG4ModuleLabel.data()             ,"");
	track_data->AddCollName("DarHandle"                           ,GetName()                         ,"DarHandle");
	track_data->AddCollName("KalDiagHandle"                       ,GetName()                         ,"KalDiagHandle");

	if (i <nshortblocks) {
	  track_data->AddCollName("ShortHelixBlockName"      ,fShortHelixBlockName[i].data(),"");
	  track_data->AddCollName("ShortTrackSeedBlockName"  ,fShortTrackSeedBlockName[i].data(),"");
	  track_data->AddCollName("mu2e::HelixSeedCollection",fHelixModuleLabel[i].data()       ,"");
	  track_data->AddCollName("mu2e::KalSeedCollection"  ,fTrackSeedModuleLabel[i].data()   ,"");
	  track_data->AddCollName("HelixBlockName"           ,fHelixBlockName[i].data()       ,"");
	  track_data->AddCollName("TrackSeedBlockName"       ,fTrackSeedBlockName[i].data()   ,"");
	}
      }
      
      if (i <nshortblocks) SetResolveLinksMethod(block_name,StntupleInitMu2eTrackBlockLinks);
    }
    
  }

//----------------------------------------------------------------------------
// resolve the links for TrackSeed and Helix Blocks
//----------------------------------------------------------------------------
  if (fMakeTracks) {
    const char    *block_name;
    int            nblocks;
    TStnDataBlock* helix_data, *trackSeed_data;

    nblocks = fHelixBlockName.size();

    for (int i=0; i<nblocks; i++) {
      block_name = fHelixBlockName[i].data();
      helix_data = Event()->GetDataBlock(block_name);
      if (helix_data) {
	helix_data->AddCollName("mu2e::KalSeedCollection"  ,fTrackSeedModuleLabel[i].data(),"");
	helix_data->AddCollName("ShortTrackSeedBlockName"  ,fShortTrackSeedBlockName[i].data(),"");
      }
      SetResolveLinksMethod(block_name,StntupleInitMu2eHelixBlockLinks);
    }
    
    nblocks = fTrackSeedBlockName.size();

    for (int i=0; i<nblocks; i++) {
      block_name     = fTrackSeedBlockName[i].data();
      trackSeed_data = Event()->GetDataBlock(block_name);
      if (trackSeed_data) {
	trackSeed_data->AddCollName("mu2e::KalRepCollection"   ,fTrkRecoModuleLabel[i].data()  ,"");	
      }
      //      SetResolveLinksMethod(block_name,StntupleInitMu2eTrackSeedBlockLinks);
    }
  }
//-----------------------------------------------------------------------------
// clusters 
//-----------------------------------------------------------------------------
  if (fMakeClusters) {
    TStnDataBlock* cluster_data;

    cluster_data = AddDataBlock("ClusterBlock",
				"TStnClusterBlock",
				StntupleInitMu2eClusterBlock,
				buffer_size,
				split_mode,
				compression_level);

    SetResolveLinksMethod("ClusterBlock",StntupleInitMu2eClusterBlockLinks);

    if (cluster_data) {
      cluster_data->AddCollName("mu2e::CaloClusterCollection",fCaloClusterMaker.data(),"");
    }
  }
//-----------------------------------------------------------------------------
// PID - one PID block per track block
//-----------------------------------------------------------------------------
  if (fMakePid) {
    TStnDataBlock *pid_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fTrackBlockName.size();

    for (int i=0; i<nblocks; i++) {
      block_name = fPidBlockName[i].data();
      pid_data   = AddDataBlock(block_name,
				"TStnPidBlock",
				StntupleInitMu2ePidBlock,
				buffer_size,
				split_mode,
				compression_level);

      SetResolveLinksMethod(block_name,StntupleInitMu2ePidBlockLinks);

      if (pid_data) {
	pid_data->AddCollName("mu2e::AvikPIDNewProductCollection",fPidModuleLabel[i].data(),"");
      }
    }
  }
//-----------------------------------------------------------------------------
// generator particles 
//-----------------------------------------------------------------------------
  if (fMakeGenp) {
    TStnDataBlock* genp_data;

    genp_data = AddDataBlock("GenpBlock",
			     "TGenpBlock",
			     StntupleInitMu2eGenpBlock,
			     buffer_size,
			     split_mode,
			     compression_level);

    genp_data->AddCollName("mu2e::GenParticleCollection",fGeneratorModuleLabel.data(),"");
    //    SetResolveLinksMethod("GenpBlock",StntupleInitMu2eClusterBlockLinks);

    if (genp_data) {
      genp_data->AddCollName("mu2e::GenParticleCollection","","");
    }
  }
//-----------------------------------------------------------------------------
// simulated particles 
//-----------------------------------------------------------------------------
  if (fMakeSimp) {
    TStnDataBlock* simp_data;

    simp_data = AddDataBlock("SimpBlock",
			     "TSimpBlock",
			     StntupleInitMu2eSimpBlock,
			     buffer_size,
			     split_mode,
			     compression_level);

    //    SetResolveLinksMethod("GenpBlock",StntupleInitMu2eClusterBlockLinks);

    if (simp_data) {
      simp_data->AddCollName("mu2e::SimParticleCollection",""                             ,"");
      simp_data->AddCollName("mu2e::StrawHitCollection"   ,fMakeStrawHitModuleLabel.data(),"");

      simp_data->AddCollName("mu2e::StepPointMCCollection",
			     fMakeSimpModuleLabel.data()    ,
			     fVDCollName.data());
    }
  }
//-----------------------------------------------------------------------------
// StepPointMC collections - could be several
//-----------------------------------------------------------------------------
  if (fMakeStepPointMC) {
    TStnDataBlock *block_data;
    const char    *block_name;
    int            nblocks;
    
    nblocks = fStepPointMCBlockName.size();

    for (int i=0; i<nblocks; i++) {
					// always store defTracks for the 
					// default process in the "TrackBlock"

      block_name = fStepPointMCBlockName[i].data();
      block_data = AddDataBlock(block_name,
				"TStepPointMCBlock",
				StntupleInitMu2eStepPointMCBlock,
				buffer_size,
				split_mode,
				compression_level);

      //      SetResolveLinksMethod(block_name,StntupleInitMu2eTrackBlockLinks);

      if (block_data) {
	block_data->AddCollName("mu2e::StepPointMCCollection",
				fStepPointMCModuleLabel[i].data(),
				fStepPointMCProductName[i].data());
      }
    }
  }
//-----------------------------------------------------------------------------
// hits on virtual detectors (StepPointMC's)
//-----------------------------------------------------------------------------
  if (fMakeVdetHits) {
    TStnDataBlock* vdet_hit_data;

    vdet_hit_data = AddDataBlock("VdetBlock",
				 "TVdetDataBlock",
				 StntupleInitMu2eVirtualDataBlock,
				 buffer_size,
				 split_mode,
				 compression_level);

    if (vdet_hit_data) {
      vdet_hit_data->AddCollName("mu2e::StepPointMCCollection",fMakeVdetHitsModuleLabel.data(),"virtualdetector");
      vdet_hit_data->AddCollName("TimeOffsetsHandle"          ,GetName()                      ,"TimeOffsetsHandle");
    }
  }  


  THistModule::afterEndJob();
}

//_____________________________________________________________________________
bool StntupleMaker::filter(AbsEvent& AnEvent) {

  // when execution comes here al the registered data blocks are already
  // initialized with the event data. Left: variables in the data blocks
  // which depend on the variable defined in other blocks, like track number
  // for a muon or an electron - the idea is that these are defined during the
  // 2nd loop in FillStntupleModule, where ResolveLinks methods are called
  // for each data block

//-----------------------------------------------------------------------------
// connect to the error reporting facility
//-----------------------------------------------------------------------------
//  TStnErrorLogger* logger = Event()->GetErrorLogger();
//   logger->Connect("Report(Int_t, const char*)",
// 		  "StntupleModule",
// 		  this,
// 		  "LogError(const char*)");
//-----------------------------------------------------------------------------
// disconnect from the error reporting signal and return back to AC++
//-----------------------------------------------------------------------------
//   logger->Disconnect("Report(Int_t,const char*)",
// 		     this,"LogError(Int_t,const char*)");

  return 1;
}


//------------------------------------------------------------------------------
void StntupleMaker::GetDefTrackCollName(char* Name) {
  // put in a working kludge first

  strcpy(Name,"default");
}


// //_____________________________________________________________________________
// int StntupleMaker::InitCalDataBlock(TStnDataBlock* Block) {
//   int mode = 0;
//   AbsEvent* event = AbsEnv::instance()->theEvent();
//   return StntupleInitMu2eCalDataBlock(Block,event,mode);
// }

// //_____________________________________________________________________________
// int StntupleMaker::InitHeaderBlock(TStnDataBlock* Block) {
//   int mode = 0;
//   AbsEvent* event = AbsEnv::instance()->theEvent();
//   return StntupleInitMu2eHeaderBlock(Block,event,mode);
// }

// //_____________________________________________________________________________
// int StntupleMaker::InitTrackBlock(TStnDataBlock* Block) {
//   int mode = 0;
//   AbsEvent* event = AbsEnv::instance()->theEvent();
//   return StntupleInitMu2eTrackBlock(Block,event,mode);
// }

//_____________________________________________________________________________
// int StntupleMaker::InitTriggerBlock(TStnDataBlock* Block) {
//   int mode = 0;
//   AbsEvent* event = AbsEnv::instance()->theEvent();
//   return StntupleInitMu2eTriggerBlock(Block,event,mode);
// }

} // end namespace mu2e

using mu2e::StntupleMaker;

DEFINE_ART_MODULE(StntupleMaker);
