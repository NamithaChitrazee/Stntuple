//--------------------------------------------------------------------------
// Description:
// -----------
// Class InitStntuple : books tree and does other initializations 
//                            for STNTUPLE
//
// Nov 23 2000 P.Murat
//------------------------------------------------------------------------
#include <string>
#include <cstdio>

#include <assert.h>
#include <iostream>
#include <iomanip>

#include "TH1.h"
#include "TProfile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TTime.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "Stntuple/obj/TStnDBManager.hh"

#include "Stntuple/obj/TStnTriggerTable.hh"
#include "Stntuple/obj/TStnDataBlock.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "Stntuple/obj/TStnNode.hh"
#include "Stntuple/obj/TStnEvent.hh"
#include "Stntuple/obj/TStnErrorLogger.hh"
#include "Stntuple/alg/TStntuple.hh"

#include "Stntuple/mod/InitStntupleDataBlocks.hh"

// class TObjArray;

#include "Stntuple/mod/StntupleModule.hh"

namespace mu2e {
class InitStntuple : public StntupleModule {
//------------------------------------------------------------------------------
//  data members
//------------------------------------------------------------------------------
protected:

  Int_t                    fLastRun;
  Float_t                  fSumInstLum; //! avg inst lum for evaluating
  Int_t                    fnLum;       //! exe speed
  Float_t                  fCpuSpeed;   //! MHz of CPU

//------------------------------------------------------------------------------
// function members
//------------------------------------------------------------------------------
public:
					// constructors and destructor

  InitStntuple(fhicl::ParameterSet const& Pset);

  ~InitStntuple();
				        // ****** accessors
//-----------------------------------------------------------------------------
// overloaded virtual functions of EDFilter
//-----------------------------------------------------------------------------
  virtual void beginRun(const art::Run&   r);
  virtual void analyze (const AbsEvent&   e);
  virtual void endRun  (const art::Run&   r);
  virtual void beginJob();
  virtual void endJob  ();
					// ****** functions of the module

  int          ProcessNewRun      (const art::Run* ARun);
  int          InitTriggerTable   (int RunNumber);
  // int       InitRunSummary     ();
					// ****** setters
  //  ClassDef(InitStntuple,0)
};


//------------------------------------------------------------------------------
// constructors
//------------------------------------------------------------------------------
InitStntuple::InitStntuple(fhicl::ParameterSet const& Pset): 
  StntupleModule   (Pset,"InitStntuple")
{
//-----------------------------------------------------------------------------
// dont create subdirectories for the modules: they will have different folders
//-----------------------------------------------------------------------------
  THistModule::fgMakeSubdirs = 0;

  fLastRun      = -1;
}


//------------------------------------------------------------------------------
InitStntuple::~InitStntuple() {
  // do not need to delete anything
}


//------------------------------------------------------------------------------
void InitStntuple::beginJob() {

  THistModule::beforeBeginJob();

  // book the tree, for ROOT 3 kludge split mode: set it always to
  // "non-split,old"
  // header block, however is always written in split mode

  fgTree      = new TTree("STNTUPLE", "STNTUPLE");
  fnLum       = 0;
  fSumInstLum = 0.0;

  FILE* pipe;
  pipe = gSystem->OpenPipe(
     "cat /proc/cpuinfo | grep MHz | tail -1 | awk '{print $4}'","r");
  fscanf(pipe,"%f",&fCpuSpeed);
  gSystem->ClosePipe(pipe);

  THistModule::afterBeginJob();
}

//------------------------------------------------------------------------------
int InitStntuple::InitTriggerTable(int RunNumber) {
  // string        trigger_name;
  // string        trigger_table_name;
  // unsigned long bit;
  // unsigned long level;
  // unsigned long trigger_tag;
  // unsigned long trigger_table_tag;
  // int           trigger_id;

				// delete all the previous definitions 

  TStnDBManager* dbm = TStnDBManager::Instance();
  TStnTriggerTable* trigger_table;

  trigger_table = (TStnTriggerTable*) dbm->GetTable("TriggerTable");

  trigger_table->Delete();
//-----------------------------------------------------------------------------
// data run, so far all trigger tags (versions) are set to 1, 
// trigger bit assignment is arbitrary
//-----------------------------------------------------------------------------
  if (RunNumber < 1200) {
    trigger_table->AddTrigger(new TStnTrigger( 0, 0,"RecoPath"                   ,1));
    trigger_table->AddTrigger(new TStnTrigger( 1, 1,"cprSeedDeM_trigger"         ,1));
    trigger_table->AddTrigger(new TStnTrigger( 2, 2,"tprSeedDeM_trigger"         ,1));
    trigger_table->AddTrigger(new TStnTrigger( 3, 3,"cprLowPSeedDeM_trigger"     ,1));
    trigger_table->AddTrigger(new TStnTrigger( 4, 4,"tprLowPSeedDeM_trigger"     ,1));
    trigger_table->AddTrigger(new TStnTrigger( 5, 5,"cprCosmicSeedDeM_trigger"   ,1));
    trigger_table->AddTrigger(new TStnTrigger( 6, 6,"tprCosmicSeedDeM_trigger"   ,1));
    trigger_table->AddTrigger(new TStnTrigger( 7, 7,"tprHelixCalibIPADeM_trigger",1));
    trigger_table->AddTrigger(new TStnTrigger( 8, 8,"tprHelixIPADeM_trigger"     ,1));
    trigger_table->AddTrigger(new TStnTrigger( 9, 9,"caloCalibCosmic_trigger"    ,1));
    trigger_table->AddTrigger(new TStnTrigger(10,10,"caloMVACE_trigger"          ,1));
    trigger_table->AddTrigger(new TStnTrigger(11,11,"unbiased_trigger"           ,1));
    trigger_table->AddTrigger(new TStnTrigger(12,12,"caloPhoton_trigger"         ,1));
    trigger_table->AddTrigger(new TStnTrigger(31,31,"p1"                         ,1)); // a kludge to get rid of su2020 warnings
  }
  else if (RunNumber <= 1210) {
//-----------------------------------------------------------------------------
// runs 1200-1210: this may or may not be correct, consider the following 
// just an example
//-----------------------------------------------------------------------------
    trigger_table->AddTrigger(new TStnTrigger(   0,   0,"MixPath"                          ,1));
    trigger_table->AddTrigger(new TStnTrigger(   1,   1,"p1"                               ,1));

    trigger_table->AddTrigger(new TStnTrigger( 100, 100,"tprDeM_highP_stopTarg"       ,1));
    trigger_table->AddTrigger(new TStnTrigger( 101, 101,"tprDeP_highP_stopTarg"       ,1));
    trigger_table->AddTrigger(new TStnTrigger( 110, 110,"tprDeM_lowP_stopTarg"        ,1));
    trigger_table->AddTrigger(new TStnTrigger( 111, 111,"tprDeP_lowP_stopTarg"        ,1));
    trigger_table->AddTrigger(new TStnTrigger( 120, 120,"tprHelixDeM_ipa"             ,1));
    trigger_table->AddTrigger(new TStnTrigger( 121, 121,"tprHelixDeM_ipa_phiScaled"   ,1));
    trigger_table->AddTrigger(new TStnTrigger( 130, 130,"tprHelixDeM"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 131, 131,"tprHelixDeP"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 132, 132,"tprHelixUeM"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 133, 133,"tprHelixUeP"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 150, 150,"cprDeM_highP_stopTarg"       ,1));
    trigger_table->AddTrigger(new TStnTrigger( 151, 151,"cprDeP_highP_stopTarg"       ,1));
    trigger_table->AddTrigger(new TStnTrigger( 160, 160,"cprDeM_lowP_stopTarg"        ,1));
    trigger_table->AddTrigger(new TStnTrigger( 161, 161,"cprDeP_lowP_stopTarg"        ,1));
    trigger_table->AddTrigger(new TStnTrigger( 170, 170,"cprHelixDeM"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 171, 171,"cprHelixDeP"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 172, 172,"cprHelixUeM"                 ,1));
    trigger_table->AddTrigger(new TStnTrigger( 173, 173,"cprHelixUeP"                 ,1));

    trigger_table->AddTrigger(new TStnTrigger( 180, 180,"apr_highP_stopTarg"          ,1));
    trigger_table->AddTrigger(new TStnTrigger( 190, 190,"apr_lowP_stopTarg"           ,1));
    trigger_table->AddTrigger(new TStnTrigger( 195, 190,"aprHelix"                    ,1));

    trigger_table->AddTrigger(new TStnTrigger( 200, 200,"caloFast_photon"             ,1));
    trigger_table->AddTrigger(new TStnTrigger( 201, 201,"caloFast_MVANNCE"            ,1));
    trigger_table->AddTrigger(new TStnTrigger( 202, 202,"caloFast_cosmic"             ,1));
    trigger_table->AddTrigger(new TStnTrigger( 220, 220,"caloFast_RMC"                ,1));

    trigger_table->AddTrigger(new TStnTrigger( 300, 300,"cst"                         ,1));
    trigger_table->AddTrigger(new TStnTrigger( 310, 310,"cstTimeCluster"              ,1));

    trigger_table->AddTrigger(new TStnTrigger( 400, 400,"minBias_SDCount"             ,1));
    trigger_table->AddTrigger(new TStnTrigger( 410, 410,"minBias_CDCount"             ,1));

  }
  else {
    printf("ERROR: Run Number = %8i, no trigger table defined\n",RunNumber);
  }

  return 0;
}

//------------------------------------------------------------------------------
Int_t InitStntuple::ProcessNewRun(const art::Run* ARun) {

  // InitRunSummary     ();
  int rn = ARun->run();

  InitTriggerTable   (rn);
  // InitDeadList       ();

  TStntuple::Init(rn);

  return 0;
}

//------------------------------------------------------------------------------
void InitStntuple::beginRun(const art::Run&  aRun) {
  // fetch calibration constants for a new run

  THistModule::beforeBeginRun(aRun);

  int runnum  = aRun.run();
  //  int mc_flag = 1; // env->monteFlag();

  if (runnum != fLastRun) {
    ProcessNewRun(&aRun);
    fLastRun = runnum;
  }

  THistModule::afterBeginRun(aRun);
}

//------------------------------------------------------------------------------
void InitStntuple::analyze(const AbsEvent& AnEvent) {
  // event entry point: initialize all the registered data blocks with the event 
  // data
  // it is assumed that the tree itself is filled in FillStntupleModule
  // order in which branches are filled may be important - for example,
  // it is better to fill track branch before the electron branch,
  // missing Et branch logically is the last one
  // assume that InitStntuple is executed before any other STNTUPLE-related
  // module
  // it decides whether we are about to close the file and to open a new one

  THistModule::beforeEvent(AnEvent);
//-----------------------------------------------------------------------------
// connect to the error reporting facility
//-----------------------------------------------------------------------------
//  TStnErrorLogger* logger = Event()->GetErrorLogger();

  //  printf(">>> InitStntuple::filter: Couldn't connect the ErrorLogger\n");

//   logger->Connect("Report(Int_t, const char*)",
// 		  "StntupleModule",
// 		  this,
// 		  "LogError(const char*)");
//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
  unsigned long etime = (unsigned long)(gSystem->Now());
  Event()->Init((AbsEvent*) &AnEvent,0);
  etime = (unsigned long)(gSystem->Now()) - etime;

  //compute avg inst lum
  TStnHeaderBlock* fHeaderBlock = 
    (TStnHeaderBlock*) Event()->GetDataBlock("HeaderBlock");
  if(fHeaderBlock) {
    float ilum = fHeaderBlock->InstLum()*1.0e-30;
    if(ilum>0.1 && ilum < 10000.0) {
      fSumInstLum += ilum;
      fnLum++;
    }
    // store the cpu speed in units of int(GHz*5)
    int speed = int(fCpuSpeed/1000.0*5.0);
    if(speed>255) speed = 255;
    //store event filling time in s*10, TTime is in ms
    int ietime = int(float(etime)/100.0);
    if(ietime>=(1<<24)) ietime=((1<<24)-1);
    fHeaderBlock->fCpu = (ietime<<8 | speed);
  }
//-----------------------------------------------------------------------------
// disconnect from the error reporting signal and return back to AC++
//-----------------------------------------------------------------------------
//   logger->Disconnect("Report(Int_t,const char*)",
// 		     this,"LogError(const char*)");

  THistModule::afterEvent(AnEvent);
}

//------------------------------------------------------------------------------
void InitStntuple::endRun(const art::Run& ARun) {
  THistModule::beforeEndRun(ARun);
  THistModule::afterEndRun (ARun);
}

//-----------------------------------------------------------------------------
void InitStntuple::endJob() {

  THistModule::beforeEndJob();

  if(fnLum>0) {
    std::cout <<"InitStntuple::endJob avg inst lum = "
	 <<fSumInstLum/fnLum << " e30 " << std::endl;
  } else {
    std::cout <<"InitStntuple::endJob avg inst lum = "
	 << " undefined " << std::endl;
  }

  THistModule::afterEndJob();
}

} //end namespace mu2e

DEFINE_ART_MODULE(mu2e::InitStntuple)
