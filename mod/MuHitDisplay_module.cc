///////////////////////////////////////////////////////////////////////////////
// interactive 2D event display. 
//
// $Id: MuHitDisplay_module.cc,v 1.6 2014/09/20 17:54:06 murat Exp $
// $Author: murat $
// $Date: 2014/09/20 17:54:06 $
//
// Contact person:  Pavel Murat, Gianantonio Pezzulo
///////////////////////////////////////////////////////////////////////////////

#include "Stntuple/mod/MuHitDisplay_module.hh"

// ClassImp(mu2e::MuHitDisplay)

namespace mu2e {
//-----------------------------------------------------------------------------
MuHitDisplay::MuHitDisplay(fhicl::ParameterSet const& pset) :
  THistModule              (pset,            "MuHitDisplay"),

  _moduleLabel             (pset.get<string>("module_label")),
  _processName             (pset.get<string>("processName", "")),
  _genpCollTag             (pset.get<string>("genpCollTag")),
  _spmcCollTag             (pset.get<string>("spmcCollTag")),
  _caloClusterCollTag      (pset.get<string>("caloClusterCollTag"  )),
  _crvRecoPulseCollTag     (pset.get<string>("crvRecoPulsesCollTag")),
  
  _strawHitCollTag         (pset.get<string>("strawHitCollTag")),
  _comboHitCollTag         (pset.get<string>("comboHitCollTag")),
  _strawDigiMCCollTag      (pset.get<string>("strawDigiMCCollTag")),
  _strawHitFlagCollTag     (pset.get<string>("strawHitFlagCollTag")),
  
  _trackCollTag            (pset.get<string>("trackCollTag")),
  _simpCollTag             (pset.get<string>("simpCollTag")),
  fTimeClusterModuleLabel  (pset.get<string>("timeClusterCollTag")),
  _caloHitCollTag   (pset.get<string>("caloHitCollTag")),
  fTrkExtrapol             (pset.get<string>("trkExtrapol")),
  fTrkCalMatch             (pset.get<string>("trkCalMatch")),
  fPidCollTag              (pset.get<string>("pidCollTag")),

  _generatorID             (pset.get<mu2e::GenId>   ("generatorID", mu2e::GenId::CeEndpoint)),
  _trackerStepPoints       (pset.get<string>        ("trackerStepPoints")),
  _minEnergyDep            (pset.get<double>        ("minEnergyDep", 0)),
  _timeWindow              (pset.get<double>        ("timeWindow", 1.e6)),
  fGoodHitMask             (pset.get<vector<string>>("goodHitMask")),
  fBadHitMask              (pset.get<vector<string>>("badHitMask")),
  _minHits                 (pset.get<unsigned>      ("minHits")),

  fDisplayBackgroundHits   (pset.get<bool>("displayBackgroundHits", false)),
  fPrintHits               (pset.get<bool>("printHits", false)),
  _showCRVOnly             (pset.get<bool>("showCRVOnly", false)),
  _showTracks              (pset.get<bool>("showTracks")),

  _vmConfig                (pset.get<fhicl::ParameterSet>("visManager", fhicl::ParameterSet()))
{

  fApplication = 0;
  
  fHeaderBlock = new TStnHeaderBlock();

  fVisManager = TStnVisManager::Instance();

  _mcUtils = std::make_unique<McUtilsToolBase>();
    
  fTrackID = new TStnTrackID();
  
  foundTrkr = false;
  foundCalo = false;
  
  fCrvPulseColl_Right   = new CrvRecoPulseCollection();
  fCrvPulseColl_Left    = new CrvRecoPulseCollection();
  fCrvPulseColl_TopDS   = new CrvRecoPulseCollection();
  fCrvPulseColl_TopTS   = new CrvRecoPulseCollection();
  fCrvPulseColl_Dwnstrm = new CrvRecoPulseCollection();
  fCrvPulseColl_Upstrm  = new CrvRecoPulseCollection();

  fDar                  = new DoubletAmbigResolver (pset.get<fhicl::ParameterSet>("DoubletAmbigResolver"),0.,0,0);
  fDarHandle            = new TNamedHandle("DarHandle",fDar);

  fFolder->Add(fDarHandle);

  _kalRepPtrColl        = nullptr;
  _firstCall            = 1;
}

//-----------------------------------------------------------------------------
MuHitDisplay::~MuHitDisplay() {

  if (fApplication) delete fApplication;

  delete fCrvPulseColl_Right;
  delete fCrvPulseColl_Left;
  delete fCrvPulseColl_TopDS;
  delete fCrvPulseColl_TopTS;
  delete fCrvPulseColl_Dwnstrm;
  delete fCrvPulseColl_Upstrm;

  //  delete fKalDiag;
  delete fDar;
}

//-----------------------------------------------------------------------------
void MuHitDisplay::beginJob() {

  //     const char oname[] = "MuHitDisplay::beginJob";
  int    tmp_argc(0);
  char** tmp_argv(0);

  if (!gApplication) {
    fApplication = new TApplication("MuHitDisplay_module", &tmp_argc, tmp_argv);
  }

//-----------------------------------------------------------------------------
// define collection names to be used for initialization
//-----------------------------------------------------------------------------
  TModule::fDump->SetStrawDigiMCCollTag(_strawDigiMCCollTag.data());
  TModule::beginJob();
}

//-----------------------------------------------------------------------------
void MuHitDisplay::beginRun(const art::Run& Run) {
  mu2e::GeomHandle<mu2e::Tracker> handle;
  fTracker = handle.get();

  mu2e::ConditionsHandle<mu2e::AcceleratorParams> accPar("ignored");
  float mbtime = accPar->deBuncherPeriod;

  TStnVisManager* vm = TStnVisManager::Instance();
  vm->SetMbTime(mbtime);

  TModule::beginRun(Run);
}


//-----------------------------------------------------------------------------
void MuHitDisplay::endRun(const art::Run& Run) {
  TStnVisManager* vm = TStnVisManager::Instance();
  vm->SetEvent(nullptr);
//-----------------------------------------------------------------------------
// if a ROOT macro is defined, execute it
//-----------------------------------------------------------------------------
  TModule::endRun(Run);
}


//-----------------------------------------------------------------------------
// initialize the visualization manager
// TStnVisManager is responsible for deleting all nodes created here
//-----------------------------------------------------------------------------
void MuHitDisplay::InitVisManager() {
  char oname [100];
  sprintf(oname,"%s:%s",moduleDescription().moduleLabel().data(),"InitVisManager");

  TStnVisManager* vm = TStnVisManager::Instance();

  vm->SetTitleNode(new THeaderVisNode("HeaderVisNode", fHeaderBlock));
//-----------------------------------------------------------------------------
// init CRV views - 6 of those
//-----------------------------------------------------------------------------
  TCrvView*    view[6];
  TCrvVisNode* node;

  for (int i=0; i<6; i++) {
    view[i] = new TCrvView(i);
    view[i]->SetTimeWindow(0, 1695);
  }

  node  = new TCrvVisNode("CrvVisNode#0", 0);
  node->SetRecoPulsesCollection(&fCrvPulseColl_Right);
  view[0]->AddNode(node);
    
  node = new TCrvVisNode("CrvVisNode#1", 1);
  node->SetRecoPulsesCollection(&fCrvPulseColl_Left);
  view[1]->AddNode(node);

  node = new TCrvVisNode("CrvVisNode#2", 2);
  node->SetRecoPulsesCollection(&fCrvPulseColl_TopDS);
  view[2]->AddNode(node);

  node = new TCrvVisNode("CrvVisNode#3", 3);
  node->SetRecoPulsesCollection(&fCrvPulseColl_Dwnstrm);
  view[3]->AddNode(node);

  node = new TCrvVisNode("CrvVisNode#4", 4);
  node->SetRecoPulsesCollection(&fCrvPulseColl_Upstrm);
  view[4]->AddNode(node);

    // '8' is not a typo...
  node = new TCrvVisNode("CrvVisNode#5", 8);
  node->SetRecoPulsesCollection(&fCrvPulseColl_TopTS);
  view[5]->AddNode(node);

  for (int i=0; i<6; i++) {
    vm->AddView(view[i]);
  }
//-----------------------------------------------------------------------------
// parse the configuration module parameters
//-----------------------------------------------------------------------------
  int debug_level = _vmConfig.get<int>("debugLevel");
  vm->SetDebugLevel(debug_level);
    
  int display_straw_digi_mc = _vmConfig.get<int>("displayStrawDigiMC");
  vm->SetDisplayStrawDigiMC(display_straw_digi_mc);
//-----------------------------------------------------------------------------
// do the geometry
//-----------------------------------------------------------------------------
  art::ServiceHandle<mu2e::GeometryService> geom;

  mu2e::GeomHandle<mu2e::DiskCalorimeter> dc_handle;
  const mu2e::DiskCalorimeter* dc = dc_handle.get();
//-----------------------------------------------------------------------------
// TCalVisNode: calorimeter, CaloCrystalHits and CaloCrystals
// cal_node[0] : first  disk
// cal_node[1] : second disk
//-----------------------------------------------------------------------------
  TCalView*         cal_view;
  TCalVisNode*      cal_node[2];

  cal_view = new TCalView(0);
  cal_node[0] = new TCalVisNode("CalVisNode#0", &dc->disk(0), 0);
  cal_node[0]->SetListOfClusters(&fListOfClusters);
  cal_node[0]->SetListOfCrystalHits(&fListOfCrystalHits);
  cal_node[0]->SetTimeClusterColl(&fTimeClusterColl);
  cal_view->AddNode(cal_node[0]);
  vm->AddView(cal_view);
  
  cal_view = new TCalView(1);
  cal_node[1] = new TCalVisNode("CalVisNode#1", &dc->disk(1), 1);
  cal_node[1]->SetListOfClusters   (&fListOfClusters);
  cal_node[1]->SetListOfCrystalHits(&fListOfCrystalHits);
  cal_node[1]->SetTimeClusterColl  (&fTimeClusterColl);
  cal_view->AddNode(cal_node[1]);
  vm->AddView(cal_view);
//-----------------------------------------------------------------------------
// TrkVisNode: tracker, tracks and straw hits
// add tracker node to two views - RZ and XY
//-----------------------------------------------------------------------------
  TTrkVisNode* tnode = new TTrkVisNode ("TrkVisNode", fTracker, NULL);

  tnode->SetStrawHitColl    (&fShComboHitColl  );
  tnode->SetComboHitColl    (&fComboHitColl    );
  //  tnode->SetStrawHitFlagColl(&fStrawHitFlagColl);
  tnode->SetTimeClusterColl (&fTimeClusterColl );
  tnode->SetKalRepPtrColl   (&_kalRepPtrColl   );
  tnode->SetStrawDigiMCColl (&_strawDigiMCColl );
  tnode->SetSimParticleColl (&_simParticleColl );
  tnode->SetSpmcColl        (&_spmcColl );
//-----------------------------------------------------------------------------
// XY view : tracker + calorimeter
//-----------------------------------------------------------------------------
  TStnView* vxy = new TStnView(TStnView::kXY,-1,"XYView","XY View");
  vxy->AddNode(tnode);
  vxy->AddNode(cal_node[0]);
  vxy->AddNode(cal_node[1]);
  vm->AddView(vxy);
//-----------------------------------------------------------------------------
// RZ view : tracker only, so far
//-----------------------------------------------------------------------------
  TStnView* vrz = new TStnView(TStnView::kRZ,-1,"RZView","RZ View");
  vrz->AddNode(tnode);
  vm->AddView(vrz);
//-----------------------------------------------------------------------------
// TZ view : TTrkNode only, so far
//-----------------------------------------------------------------------------
  TStnView* vtz = new TStnView(TStnView::kTZ,-1,"TZView","TZ View");
  vtz->AddNode(tnode);
  vm->AddView(vtz);
//-----------------------------------------------------------------------------
// upon startup, open a window with XY view
//-----------------------------------------------------------------------------
  vm->OpenTrkXYView();
}

//-----------------------------------------------------------------------------
// get data from the event record
//-----------------------------------------------------------------------------
int MuHitDisplay::getData(const art::Event* Evt) {
    char oname[100];
    sprintf(oname,"%s::%s",moduleDescription().moduleLabel().data(),"getData");

//-----------------------------------------------------------------------------
//  CRV pulse information
//-----------------------------------------------------------------------------
    art::Handle<CrvRecoPulseCollection> pulsesHandle;
    Evt->getByLabel(_crvRecoPulseCollTag, pulsesHandle);
    
    if (pulsesHandle.isValid()) {
      // printf(">>> [%s] MSG: CrvRecoPulsesCollection by %s, found. CONTINUE\n", oname, _crvRecoPulseCollTag.data());
      const mu2e::CrvRecoPulseCollection* fCrvPulseColl = (CrvRecoPulseCollection*) pulsesHandle.product();
      
      // Clear the map pointers in preperation to (re)fill them with new information
      fCrvPulseColl_Right->clear();
      fCrvPulseColl_Left->clear();
      fCrvPulseColl_TopDS->clear();
      fCrvPulseColl_TopTS->clear();
      fCrvPulseColl_Dwnstrm->clear();
      fCrvPulseColl_Upstrm->clear();
      printf(">>> Section-collections cleared\n");
      
      mu2e::GeomHandle<mu2e::CosmicRayShield> CRS;
      
      // Loop over the RecoPulses in the collection and sort each pulse/bar into its appropriate section
      // for (mu2e::CrvRecoPulseCollection::const_iterator icrpc = fCrvPulseColl->begin(), ecrpc = fCrvPulseColl->end(); icrpc != ecrpc; ++icrpc) {
      int    crvCollSize = fCrvPulseColl->size();
      for (int ic=0; ic < crvCollSize; ++ic) {
	mu2e::CrvRecoPulse                   icprc       = fCrvPulseColl->at(ic);
	const mu2e::CRSScintillatorBarIndex &CRVBarIndex = icprc.GetScintillatorBarIndex();
	
	switch (getCRVSection(CRS->getBar(CRVBarIndex).id().getShieldNumber())) {
	case 0:
	  fCrvPulseColl_Right->  push_back(icprc);
	  break;
	case 1:
	  fCrvPulseColl_Left->   push_back(icprc);
	  break;
	case 2:
	  fCrvPulseColl_TopDS->  push_back(icprc);
	  break;
	case 3:
	  fCrvPulseColl_Dwnstrm->push_back(icprc);
	  break;
	case 4:
	  fCrvPulseColl_Upstrm-> push_back(icprc);
	  break;
	case 5:
	case 6:
	case 7:
	  break;
	case 8:
	  fCrvPulseColl_TopTS->push_back(icprc);
	  break;
	}
      }
								
      printf(">>> Section-collections filled\n");
    }
    else {
      printf(">>> [%s] WARNING: CrvRecoPulsesCollection by %s is missing. CONTINUE.\n",
	     __func__, _crvRecoPulseCollTag.data());
    }
    
    if (_showCRVOnly) { //If only displaying the CRV, skip everything else
      printf("!!!!! ONLY SHOWING THE CRV\n");
      return 0;
    }
    else {
//-----------------------------------------------------------------------------
//  MC truth - gen particles
//-----------------------------------------------------------------------------
      art::Handle<GenParticleCollection> gensHandle;
      Evt->getByLabel(_genpCollTag, gensHandle);

      if (gensHandle.isValid()) _genParticleColl = gensHandle.product();
      else {
	_genParticleColl = 0;
	printf(">>> [%s] WARNING: GenParticleCollection by %s is missing. CONTINUE\n",
	       oname, _genpCollTag.data());
      }
//-----------------------------------------------------------------------------
//  StepPointMCs - on virtual detectors
//-----------------------------------------------------------------------------
      art::Handle<StepPointMCCollection> vdStepsHandle;
      Evt->getByLabel(_spmcCollTag, vdStepsHandle);

      if (vdStepsHandle.isValid()) _spmcColl = vdStepsHandle.product();
      else                         _spmcColl = NULL;
//-----------------------------------------------------------------------------
// SimParticle's
//-----------------------------------------------------------------------------
      art::Handle<mu2e::SimParticleCollection> simpHandle;
      Evt->getByLabel(_simpCollTag, simpHandle);

      if (simpHandle.isValid()) _simParticleColl = simpHandle.product();
      else                      _simParticleColl = NULL;
//-----------------------------------------------------------------------------
//  straw hit information
//-----------------------------------------------------------------------------
      art::Handle<ComboHitCollection> chH;

      Evt->getByLabel(_comboHitCollTag, chH);
      if (chH.isValid()) fComboHitColl = chH.product();
      else {
	printf(">>> [%s] ERROR: ComboHitCollection by %s is missing. BAIL OUT\n",
	       oname, _comboHitCollTag.data());
	return -1;
      }

      Evt->getByLabel(_strawHitCollTag, chH);
      if (chH.isValid()) fShComboHitColl = chH.product();
      else {
	printf(">>> [%s] ERROR: ComboHitCollection by %s is missing. BAIL OUT\n",
	       oname, _strawHitCollTag.data());
	return -1;
      }

      art::Handle<StrawDigiMCCollection> handle;
      art::Selector sel_straw_digi_mc(art::ProductInstanceNameSelector("") &&
				      art::ProcessNameSelector(_processName) &&
				      art::ModuleLabelSelector(_strawDigiMCCollTag));
      Evt->get(sel_straw_digi_mc, handle);
      if (handle.isValid()) {
	_strawDigiMCColl = handle.product();
	if (_strawDigiMCColl->size() <= 0) {
	  printf(">>> [%s] WARNING:StrawDigiMCCollection by %s has zero length. CONTINUE\n",
		 oname, _strawDigiMCCollTag.data());
	}
      }
      else {
	printf(">>> [%s] ERROR: mu2e::StrawDigiMCCollection by %s is missing. BAIL OUT\n",
	       oname, _strawDigiMCCollTag.data());
	return -1;
      }

      art::Handle<mu2e::StrawHitFlagCollection> shfH;
      Evt->getByLabel(_strawHitFlagCollTag, shfH);

      if (shfH.isValid()) fStrawHitFlagColl = shfH.product();
      else                fStrawHitFlagColl = nullptr;
//-----------------------------------------------------------------------------
// calorimeter crystal hit data
//-----------------------------------------------------------------------------
      art::Handle<CaloHitCollection> ccHandle;
      Evt->getByLabel(_caloHitCollTag.data(), ccHandle);

      if (ccHandle.isValid()) {
	fListOfCrystalHits = (CaloHitCollection*) ccHandle.product();
      }
      else {
	fListOfCrystalHits = NULL;
	printf(">>> [%s] ERROR: CaloHitCollection by %s is missing. BAIL OUT\n",
	       oname, _caloHitCollTag.data());
      }
//-----------------------------------------------------------------------------
// calorimeter cluster data
//-----------------------------------------------------------------------------
      art::Handle<CaloClusterCollection> calo_cluster_handle;
      Evt->getByLabel(_caloClusterCollTag, "", calo_cluster_handle);

      if (calo_cluster_handle.isValid()) {
	fListOfClusters = calo_cluster_handle.product();
      }
      else {
	fListOfClusters = NULL;
	printf(">>> [%s] ERROR: CaloClusterCollection by %s is missing. BAIL OUT\n",
	       oname, _caloClusterCollTag.data());
      }
//-----------------------------------------------------------------------------
// timepeaks 
//-----------------------------------------------------------------------------
      fTimeClusterColl = NULL;
      fTimeCluster     = NULL;

      art::Handle<TimeClusterCollection> tpch;

      if (_showTracks){
	Evt->getByLabel(fTimeClusterModuleLabel, "", tpch);
      } 
      else {
//-----------------------------------------------------------------------------
// not sure I understand the clause
//-----------------------------------------------------------------------------
	Evt->getByLabel(fTimeClusterModuleLabel, tpch);
      }
      if (tpch.isValid()) {
	fTimeClusterColl = tpch.product();
//-----------------------------------------------------------------------------
// find the right time peak to display - display the first one with the track
// 2018-10-12 P.Murat: dont enforce! 
//-----------------------------------------------------------------------------
	// const TimeCluster* tp;
	// int ipeak = -1;
	// if (fTimeClusterColl != NULL) {
	//   int ntp = fTimeClusterColl->size();
	//   for (int i = 0; i<ntp; i++) {
	//     tp = &fTimeClusterColl->at(i);
	//     // if (tp->CprIndex() >= 0) {
	//     fTimeCluster = tp;
	//     ipeak = i;
	//     break;
	//     // }
	//   }
	// }
	// fVisManager->SetTimeCluster(ipeak);
      }
//-----------------------------------------------------------------------------
// tracking data - downstream moving electrons
//-----------------------------------------------------------------------------
      art::Handle<KalRepPtrCollection> krepHandle;
      Evt->getByLabel(_trackCollTag.data(), "", krepHandle);

      fNTracks[0]    = 0;
      _kalRepPtrColl = nullptr;
      if (krepHandle.isValid()) {
	_kalRepPtrColl = krepHandle.product();
	fNTracks[0] = _kalRepPtrColl->size();
      }
    }
    return 0;
  }

//-----------------------------------------------------------------------------
  void MuHitDisplay::analyze(const art::Event& Evt) {
    const char* oname = "MuHitDisplay::analyze";

    printf("[%s] RUN: %10i EVENT: %10i\n", oname, Evt.run(), Evt.event());
//-----------------------------------------------------------------------------
// init VisManager - failed to do it in beginJob - what is the right place for doing it?
// get event data and initialize data blocks
//-----------------------------------------------------------------------------
    if (_firstCall == 1) {
      _firstCall = 0;
      InitVisManager();
    }

    int rc = getData(&Evt);
    if (rc < 0) {
      printf(">>> [%s] ERROR: not all data products present, BAIL OUT\n", oname);
      return;
    }
    
    fVisManager->SetEvent(&Evt);
    fVisManager->DisplayEvent();
//-----------------------------------------------------------------------------
// go into interactive mode, 
// fInteractiveMode = 1 : stop after each event (event display mode)
// fInteractiveMode = 2 : stop only in the end of run, till '.q' is pressed
//-----------------------------------------------------------------------------
    TModule::analyze(Evt);
    return;
  } 

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
  int MuHitDisplay::getCRVSection(int shieldNumber) {
    int CRVSection = -1;
    switch (shieldNumber) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:  CRVSection = 0; break;  //R
      case 6:
      case 7:
      case 8:  CRVSection = 1; break;  //L
      case 9:  CRVSection = 8; break;  //TS T
      case 10:
      case 11:
      case 12: CRVSection = 2; break;  //T
      case 13: CRVSection = 3; break;  //D
      case 14: CRVSection = 4; break;  //U
      case 15: CRVSection = 5; break;  //CU
      case 16: CRVSection = 6; break;  //CD
      case 17: CRVSection = 7; break;  //CT
      }
    return CRVSection;
  }

}

using mu2e::MuHitDisplay;
DEFINE_ART_MODULE(MuHitDisplay);
