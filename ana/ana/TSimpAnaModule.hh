///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef Stntuple_ana_TSimpAnaModule_hh
#define Stntuple_ana_TSimpAnaModule_hh

#include "TH1.h"
#include "TH2.h"

#include "Stntuple/loop/TStnModule.hh"

#include "Stntuple/obj/TGenpBlock.hh"
#include "Stntuple/obj/TSimpBlock.hh"
#include "Stntuple/obj/TStepPointMCBlock.hh"

#include "Stntuple/alg/TStntuple.hh"

namespace stntuple {

class TSimpAnaModule: public TStnModule {
public:
//-----------------------------------------------------------------------------
//  histogram structs
//-----------------------------------------------------------------------------
  struct EventHist_t {
    TH1F*    fNGenp;
    TH1F*    fMcMom;
    TH1D*    fDioMom;
    TH1F*    fMcCosTh;
    TH1F*    fRv;
    TH1F*    fZv[2];
  };

  struct GenpHist_t {
    TH1F*    fPdgCode[2];
    TH1F*    fGenID;
    TH1F*    fZ0;
    TH1F*    fT0;
    TH1F*    fR0;
    TH1F*    fP;
    TH1F*    fCosTh;
  };

  struct SimpHist_t {
    TH1F*    fPdgCode[2];
    TH1F*    fNStrawHits;
    TH1F*    fMomTargetEnd[2];
    TH1F*    fMomTrackerFront[2];
    TH1F*    fT0;
  };

//-----------------------------------------------------------------------------
//  parameter structs
//-----------------------------------------------------------------------------
  struct SimPar_t {
    TGenParticle*   fGenp;
    TSimParticle*   fParticle;
    TStepPointMC*   fVD10;       // VD hit right after the ST (not currently filled)
    TStepPointMC*   fTFront;     // VD hit at the tracker front
    TStepPointMC*   fTMid;       // VD hit at the tracker center
    TStepPointMC*   fTBack;      // VD hit at the tracker back
    double          fEleE;       // generator particle energy
  };

  struct EventPar_t {
    int             fNGenp;
    TGenParticle*   fParticle;   // generator "signal" particle
    float           fGenE;       // generator (signal) particle energy
    int             fNSimp;
    int             fNCrvClusters;
    int             fNCrvPulses;
    int             fNCrvCoincidences;
    double          fDioLOWt;
    double          fDioLLWt;
    SimPar_t*       fSimPar;
  };

//-----------------------------------------------------------------------------
  enum { kNEventHistSets = 100 };
  enum { kNGenpHistSets  = 100 };
  enum { kNSimpHistSets  = 100 };

  struct Hist_t {
    EventHist_t*   fEvent[kNEventHistSets];
    GenpHist_t*    fGenp [kNGenpHistSets ];
    SimpHist_t*    fSimp [kNSimpHistSets ];
  };

//-----------------------------------------------------------------------------
//  data members
//-----------------------------------------------------------------------------
public:
  TGenpBlock*       fGenpBlock;
  TSimpBlock*       fSimpBlock;

  EventPar_t        fEvtPar;
  SimPar_t          fSimPar;

  Hist_t            fHist;

  TGenParticle*     fParticle;      // signal electron or muon

  int               fNElectrons;
  int               fNMuons[2];          // [0]: total, [1]: 'trigger', P > 90
  int               fNCEElectrons;       // N(electrons) with 90 < MomTargetEnd < 110 MeV/c
  int               fNCEEvents;          // cumulative count of events with fNCEElectrons == 1
  int               fNCEPositrons;       // N(positrons, PDG==-11) with 85 < MomTargetEnd < 95 MeV/c
  int               fNCEPosEvents;       // cumulative count of events with fNCEPositrons == 1
  int               fNMultiTrkParticles; // N(particles) with MomTargetEnd (VD10) > 80 MeV/c in this event
  int               fNMultiTrkEvents;    // cumulative count of events with fNMultiTrkParticles >= 2
  int               fMinNStrawHits;

  int               fPDGCode;
  int               fMCProcessCode;

  TStntuple*        fStnt;

//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:
  TSimpAnaModule(const char* name="SimpAna", const char* title="SimpAna");
  ~TSimpAnaModule();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void SetPDGCode      (int Code) { fPDGCode       = Code; }
  void SetMCProcessCode(int Code) { fMCProcessCode = Code; }
//-----------------------------------------------------------------------------
// overloaded methods of TStnModule
//-----------------------------------------------------------------------------
  virtual int BeginJob() override;
  virtual int BeginRun() override;
  virtual int Event   (int ientry) override;
  virtual int EndJob  () override;
//-----------------------------------------------------------------------------
// other methods
//-----------------------------------------------------------------------------
  void BookHistograms();
  void FillHistograms();

  void BookEventHistograms(EventHist_t* Hist, const char* Folder);
  void BookGenpHistograms (GenpHist_t*  Hist, const char* Folder);
  void BookSimpHistograms (SimpHist_t*  Hist, const char* Folder);

  void FillEventHistograms(EventHist_t* Hist, EventPar_t* Evp, double Weight = 1.);
  void FillGenpHistograms (GenpHist_t*  Hist, TGenParticle* Genp);
  void FillSimpHistograms (SimpHist_t*  Hist, TSimParticle* Simp);

  void Debug();

  ClassDefOverride(stntuple::TSimpAnaModule, 0)
};

}
#endif
