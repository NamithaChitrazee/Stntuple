//////////////////////////////////////////////////////////////////////////////
// use of debug bits: bits 0-2 are reserved
//  0  : all events
//  1  : passed events
//  2  : rejected events
//
//  5  : NP >= 2 (with >= 20 straw hits)
//
///////////////////////////////////////////////////////////////////////////////
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TDirectory.h"

#include "Stntuple/loop/TStnAna.hh"
#include "Stntuple/base/TStnDataset.hh"
#include "Stntuple/loop/TStnInputModule.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "Stntuple/alg/TStntuple.hh"
#include "Stntuple/val/stntuple_val_functions.hh"

#include "Stntuple/ana/TSimpAnaModule.hh"

ClassImp(stntuple::TSimpAnaModule)

namespace stntuple {
//-----------------------------------------------------------------------------
TSimpAnaModule::TSimpAnaModule(const char* name, const char* title): TStnModule(name,title) {
  fPDGCode       = 0;
  fMCProcessCode = -1;
  fMinNStrawHits = 20;
  fNCEElectrons  = 0;
  fNCEEvents     = 0;
  fStnt          = TStntuple::Instance();
}

//-----------------------------------------------------------------------------
TSimpAnaModule::~TSimpAnaModule() {
}

//-----------------------------------------------------------------------------
int TSimpAnaModule::BeginJob() {
  RegisterDataBlock("GenpBlock", "TGenpBlock", &fGenpBlock);
  RegisterDataBlock("SimpBlock", "TSimpBlock", &fSimpBlock);

  BookHistograms();
  return 0;
}

//-----------------------------------------------------------------------------
int TSimpAnaModule::BeginRun() {
  TStnDataset* ds = GetAna()->GetInputModule()->GetDataset(0);

  if (ds->GetMcFlag() != 0) {
    int pdg_code     = ds->GetPDGCode();
    int process_code = ds->GetMCProcessCode();
    if (pdg_code != 0 && process_code >= 0) {
      fPDGCode       = pdg_code;
      fMCProcessCode = process_code;
    }
  }

  int rn = GetHeaderBlock()->RunNumber();
  TStntuple::Init(rn);

  printf("%s::BeginRun: run: %6i, MCProcessCode: %5i PDGCode: %5i\n",
         GetName(), rn, fMCProcessCode, fPDGCode);
  return 0;
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::BookHistograms() {
  TFolder* fol;
  TFolder* hist_folder;
  char     folder_name[200];

  DeleteHistograms();
  hist_folder = (TFolder*) GetFolder()->FindObject("Hist");
//-----------------------------------------------------------------------------
// book event histograms
//-----------------------------------------------------------------------------
  int book_event_histset[kNEventHistSets];
  for (int i=0; i<kNEventHistSets; i++) book_event_histset[i] = 0;

  book_event_histset[0] = 1;     // all events
  book_event_histset[1] = 1;     // events with N(electrons) > 0
  book_event_histset[2] = 1;     // events with N(muons) > 1

  for (int i=0; i<kNEventHistSets; i++) {
    if (book_event_histset[i] != 0) {
      sprintf(folder_name,"evt_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fEvent[i] = new EventHist_t;
      BookEventHistograms(fHist.fEvent[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// book Genp histograms
//-----------------------------------------------------------------------------
  int book_genp_histset[kNGenpHistSets];
  for (int i=0; i<kNGenpHistSets; i++) book_genp_histset[i] = 0;

  book_genp_histset[0] = 1;     // all particles

  for (int i=0; i<kNGenpHistSets; i++) {
    if (book_genp_histset[i] != 0) {
      sprintf(folder_name,"gen_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fGenp[i] = new GenpHist_t;
      BookGenpHistograms(fHist.fGenp[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// book simp histograms
//-----------------------------------------------------------------------------
  int book_simp_histset[kNSimpHistSets];
  for (int i=0; i<kNSimpHistSets; i++) book_simp_histset[i] = 0;

  book_simp_histset[ 0] = 1;     // all particles
  book_simp_histset[ 1] = 1;     // all particles with N(straw hits) > fMinNStrawHits
  book_simp_histset[10] = 1;     // electrons with N(straw hits) > fMinNStrawHits
  book_simp_histset[20] = 1;     // positrons with N(straw hits) > fMinNStrawHits
  book_simp_histset[30] = 1;     // muons     with N(straw hits) > fMinNStrawHits
  book_simp_histset[40] = 1;     // pions     with N(straw hits) > fMinNStrawHits

  for (int i=0; i<kNSimpHistSets; i++) {
    if (book_simp_histset[i] != 0) {
      sprintf(folder_name,"sim_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fSimp[i] = new SimpHist_t;
      BookSimpHistograms(fHist.fSimp[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// sim_CE: exactly one electron (PDG==11) with 90 < MomTargetEnd < 110 MeV/c
//-----------------------------------------------------------------------------
  {
    sprintf(folder_name,"sim_CE");
    fol = (TFolder*) hist_folder->FindObject(folder_name);
    if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
    fHist.fSimp[50] = new SimpHist_t;
    BookSimpHistograms(fHist.fSimp[50],Form("Hist/%s",folder_name));
  }
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::BookEventHistograms(EventHist_t* Hist, const char* Folder) {
  HBook1F(Hist->fNGenp  ,"ngenp"   ,Form("%s: N(Gen Particles)"              ,Folder),500,0,500,Folder);
  HBook1F(Hist->fMcMom  ,"mc_mom"  ,Form("%s: Conversion Electron Momentum"  ,Folder),1000,0,200,Folder);
  HBook1D(Hist->fDioMom ,"dio_mom" ,Form("%s: DIO momentum"                  ,Folder),1000,50,150,Folder);
  HBook1F(Hist->fMcCosTh,"mc_costh",Form("%s: Conversion Electron Cos(Theta)",Folder),100,-1,1,Folder);
  HBook1F(Hist->fRv     ,"rv"      ,Form("%s: R(Vertex)"                     ,Folder),100,0,1000,Folder);
  HBook1F(Hist->fZv[0]  ,"zv"      ,Form("%s: Z(Vertex)"                     ,Folder),300,0,15000,Folder);
  HBook1F(Hist->fZv[1]  ,"zv_1"   ,Form("%s: Z(Vertex) [5400-6400]"         ,Folder),200,5400,6400,Folder);
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::BookGenpHistograms(GenpHist_t* Hist, const char* Folder) {
  HBook1F(Hist->fP         ,"p"          ,Form("%s: Momentum"      ,Folder),4000,     0,200,Folder);
  HBook1F(Hist->fPdgCode[0],"pdg_code_0" ,Form("%s: PDG Code[0]"  ,Folder),200,  -100,100,Folder);
  HBook1F(Hist->fPdgCode[1],"pdg_code_1" ,Form("%s: PDG Code[1]"  ,Folder),500, -2500,2500,Folder);
  HBook1F(Hist->fGenID     ,"gen_id"     ,Form("%s: Generator ID"  ,Folder),100,     0,100,Folder);
  HBook1F(Hist->fZ0        ,"z0"         ,Form("%s: Z0"            ,Folder),500,  5400,6400,Folder);
  HBook1F(Hist->fT0        ,"t0"         ,Form("%s: T0"            ,Folder),1000,    0,10000,Folder);
  HBook1F(Hist->fR0        ,"r"          ,Form("%s: R0"            ,Folder),100,     0,100,Folder);
  HBook1F(Hist->fCosTh     ,"cos_th"     ,Form("%s: Cos(Theta)"    ,Folder),200,   -1.,1.,Folder);
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::BookSimpHistograms(SimpHist_t* Hist, const char* Folder) {
  HBook1F(Hist->fPdgCode[0]       ,"pdg_0" ,Form("%s: PDG code[0]"     ,Folder),200,-100,100,Folder);
  HBook1F(Hist->fPdgCode[1]       ,"pdg_1" ,Form("%s: PDG code[1]"     ,Folder),200,-100,100,Folder);
  HBook1F(Hist->fT0               ,"time"  ,Form("%s: Time, ns"         ,Folder),200,   0,2000,Folder);
  HBook1F(Hist->fNStrawHits       ,"nsth"  ,Form("%s: N straw hits"     ,Folder),200,   0,200,Folder);
  HBook1F(Hist->fMomTargetEnd[0]  ,"ptrg_0",Form("%s: CE mom at VD10"  ,Folder),400,   0,400,Folder);
  HBook1F(Hist->fMomTargetEnd[1]  ,"ptrg_1",Form("%s: CE mom at VD10"  ,Folder),400,  90,110,Folder);
  HBook1F(Hist->fMomTrackerFront[0],"ptfr_0",Form("%s: CE mom at VD13" ,Folder),400,   0,400,Folder);
  HBook1F(Hist->fMomTrackerFront[1],"ptfr_1",Form("%s: CE mom at VD13" ,Folder),400,  90,110,Folder);
}

//_____________________________________________________________________________
void TSimpAnaModule::FillHistograms() {
//-----------------------------------------------------------------------------
// event histograms
//-----------------------------------------------------------------------------
  FillEventHistograms(fHist.fEvent[0],&fEvtPar);

  if (fNElectrons >= 1)                         FillEventHistograms(fHist.fEvent[1],&fEvtPar);
  if ((fNMuons[1] >= 1) and (fNMuons[0] >= 2)) FillEventHistograms(fHist.fEvent[2],&fEvtPar);
//-----------------------------------------------------------------------------
// Simp histograms
//-----------------------------------------------------------------------------
  TSimParticle* simp;
  for (int i=0; i<fEvtPar.fNSimp; i++) {
    simp = fSimpBlock->Particle(i);
    FillSimpHistograms(fHist.fSimp[0],simp);
    int pdg_code = simp->PDGCode();
    if (simp->NStrawHits() > fMinNStrawHits) {
      FillSimpHistograms(fHist.fSimp[1],simp);
      if (pdg_code == 11) {
	FillSimpHistograms(fHist.fSimp[10],simp);
      }
      else if (pdg_code == -11) {
	FillSimpHistograms(fHist.fSimp[20],simp);
      }
      else if (abs(pdg_code) == 13) {
	FillSimpHistograms(fHist.fSimp[30],simp);
      }
      else if (abs(pdg_code) == 211) {
	FillSimpHistograms(fHist.fSimp[40],simp);
      }
    }
  }
//-----------------------------------------------------------------------------
// sim_CE: exactly one electron (PDG==11) with 90 < MomTargetEnd < 110 MeV/c
//-----------------------------------------------------------------------------
  if (fNCEElectrons == 1) {
    for (int i=0; i<fEvtPar.fNSimp; i++) {
      simp = fSimpBlock->Particle(i);
      if (simp->PDGCode() == 11) {
        float p_vd10 = simp->fMomTargetEnd;
        if (p_vd10 > 90 and p_vd10 < 110) {
          FillSimpHistograms(fHist.fSimp[50],simp);
          break;
        }
      }
    }
  }
//-----------------------------------------------------------------------------
// fill GENP histograms
// GEN_0: all particles
//-----------------------------------------------------------------------------
  TGenParticle* genp;
  for (int i=0; i<fEvtPar.fNGenp; i++) {
    genp = fGenpBlock->Particle(i);
    FillGenpHistograms(fHist.fGenp[0],genp);
  }
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::FillEventHistograms(EventHist_t* Hist, EventPar_t* Evp, double Weight) {
  double         cos_th(-2), dio_wt(-1.), xv(-1.e6), yv(-1.e6), rv(-1.e6), zv(-1.e6), p(-1.);
  TLorentzVector mom;

  if (Evp->fParticle) {
    Evp->fParticle->Momentum(mom);
    p      = mom.P();
    cos_th = mom.Pz()/p;
    xv     = Evp->fParticle->Vx()+3904.;
    yv     = Evp->fParticle->Vy();
    rv     = sqrt(xv*xv+yv*yv);
    zv     = Evp->fParticle->Vz();
    dio_wt = TStntuple::DioWeightAl(p);
  }

  Hist->fNGenp  ->Fill(Evp->fNGenp, Weight);
  Hist->fMcMom  ->Fill(p,    Weight);
  Hist->fDioMom ->Fill(p,    dio_wt*Weight);
  Hist->fMcCosTh->Fill(cos_th, Weight);
  Hist->fRv     ->Fill(rv,   Weight);
  Hist->fZv[0]  ->Fill(zv,   Weight);
  Hist->fZv[1]  ->Fill(zv,   Weight);
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::FillGenpHistograms(GenpHist_t* Hist, TGenParticle* Genp) {
  TLorentzVector mom;
  Genp->Momentum(mom);

  float p      = mom.P();
  float cos_th = mom.CosTheta();
  float x0     = Genp->Vx()+3904.;
  float y0     = Genp->Vy();
  float z0     = Genp->Vz();
  float t0     = Genp->T();
  float r0     = sqrt(x0*x0+y0*y0);
  int   gen_id = Genp->GetStatusCode();

  Hist->fPdgCode[0]->Fill(Genp->GetPdgCode());
  Hist->fPdgCode[1]->Fill(Genp->GetPdgCode());
  Hist->fGenID     ->Fill(gen_id);
  Hist->fZ0        ->Fill(z0);
  Hist->fT0        ->Fill(t0);
  Hist->fR0        ->Fill(r0);
  Hist->fP         ->Fill(p);
  Hist->fCosTh     ->Fill(cos_th);
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::FillSimpHistograms(SimpHist_t* Hist, TSimParticle* Simp) {
  Hist->fPdgCode[0]        ->Fill(Simp->fPdgCode);
  Hist->fPdgCode[1]        ->Fill(Simp->fPdgCode);
  Hist->fMomTargetEnd[0]   ->Fill(Simp->fMomTargetEnd);
  Hist->fMomTargetEnd[1]   ->Fill(Simp->fMomTargetEnd);
  Hist->fMomTrackerFront[0]->Fill(Simp->fMomTrackerFront);
  Hist->fMomTrackerFront[1]->Fill(Simp->fMomTrackerFront);
  Hist->fNStrawHits        ->Fill(Simp->fNStrawHits);
  Hist->fT0                ->Fill(Simp->StartPos()->T());
}

//-----------------------------------------------------------------------------
int TSimpAnaModule::Event(int ientry) {

  fGenpBlock->GetEntry(ientry);
  fSimpBlock->GetEntry(ientry);
//-----------------------------------------------------------------------------
// at some point TVdetBlock got obsolete; virtual detector hits are now stored
// in TStepPointMCBlock - don't try to read the old block
//-----------------------------------------------------------------------------
  fEvtPar.fDioLOWt          =  1.;
  fEvtPar.fDioLLWt          =  1.;
  fEvtPar.fNCrvClusters     = -1;
  fEvtPar.fNCrvPulses       = -1;
  fEvtPar.fNCrvCoincidences = -1;
  fEvtPar.fNGenp            = fGenpBlock->NParticles();
  fEvtPar.fParticle         = NULL;
  fEvtPar.fGenE             = -1.;
  fEvtPar.fNSimp            = fSimpBlock->NParticles();
//-----------------------------------------------------------------------------
// look for signal particle defined by the PDG code and the generator code
//-----------------------------------------------------------------------------
  TLorentzVector mom;

  for (int i=fEvtPar.fNGenp-1; i>=0; i--) {
    TGenParticle* genp = fGenpBlock->Particle(i);
    int pdg_code       = genp->GetPdgCode();
    int process_code   = genp->GetStatusCode();
    if ((abs(pdg_code) == fPDGCode) && (process_code == fMCProcessCode)) {
      fEvtPar.fParticle = genp;
      genp->Momentum(mom);
      fEvtPar.fGenE     = mom.Energy();
      break;
    }
  }
//-----------------------------------------------------------------------------
// calculate DIO weights once per event
//-----------------------------------------------------------------------------
  if (fEvtPar.fGenE > 0) {
    fEvtPar.fDioLOWt = TStntuple::DioWeightAl   (fEvtPar.fGenE);
    fEvtPar.fDioLLWt = TStntuple::DioWeightAl_LL(fEvtPar.fGenE);
  }
//-----------------------------------------------------------------------------
// set up SimPar (signal particle pointers)
//-----------------------------------------------------------------------------
  fSimPar.fParticle = fSimpBlock->Particle(0);
  fSimPar.fVD10     = NULL;
  fSimPar.fTFront   = NULL;
  fSimPar.fTMid     = NULL;
  fSimPar.fTBack    = NULL;
  fSimPar.fGenp     = fEvtPar.fParticle;

  if (fSimPar.fGenp) fSimPar.fEleE = fSimPar.fGenp->Energy();
  else               fSimPar.fEleE = -1;

  fEvtPar.fSimPar   = &fSimPar;
//-----------------------------------------------------------------------------
// count particles
//-----------------------------------------------------------------------------
  fNMuons[0]    = 0;
  fNMuons[1]    = 0;
  fNElectrons   = 0;
  fNCEElectrons = 0;

  for (int i=0; i<fEvtPar.fNSimp; i++) {
    TSimParticle* simp = fSimpBlock->Particle(i);
    int pdg_code       = simp->PDGCode();
    float p            = simp->StartMom()->P();
    if ((abs(pdg_code) == 13) or (abs(pdg_code) == 211)) {
      if (simp->NStrawHits() >= fMinNStrawHits) {
	if (p > 60) fNMuons[0] += 1;
	if (p > 90) fNMuons[1] += 1;
      }
    }
    else if (pdg_code == 11) {
      if (simp->NStrawHits() >= fMinNStrawHits) {
	if (p > 90) fNElectrons += 1;
      }
      float p_vd10 = simp->fMomTargetEnd;
      if (p_vd10 > 90 and p_vd10 < 110) fNCEElectrons += 1;
    }
  }

  if (fNCEElectrons == 1) fNCEEvents += 1;

  FillHistograms();

  Debug();

  return 0;
}

//-----------------------------------------------------------------------------
void TSimpAnaModule::Debug() {
  if (GetDebugBit(5) == 1) {
    int np = 0;
    for (int i=0; i<fEvtPar.fNSimp; i++) {
      TSimParticle* simp = fSimpBlock->Particle(i);
      float time         = simp->StartPos()->T();
      if ((simp->NStrawHits() >= 20) and (time > 500)) np++;
    }
    if (np >= 2) GetHeaderBlock()->Print(Form("np = %5i",np));
  }

  if (GetDebugBit(6) == 1) {
    if (fNCEElectrons == 1) {
      GetHeaderBlock()->Print(Form("NCEEvents(running)=%5i",fNCEEvents));
    }
  }
}

//_____________________________________________________________________________
int TSimpAnaModule::EndJob() {
  printf("----- end job: ---- %s\n",GetName());
  if (GetDebugBit(6) == 1) {
    printf("%s::EndJob: N(events, single CE electron, 90<p_VD10<110 MeV/c): %5i\n",
           GetName(), fNCEEvents);
  }
  return 0;
}

}
