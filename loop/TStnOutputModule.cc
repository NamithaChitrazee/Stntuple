//-----------------------------------------------------------------------------
// Feb 25 2001 P.Murat: base class for STNTUPLE output module, 
// provides the basic implementation
//
// 2024-04-28 P.Murat: keep track only of runs and subruns: for Mu2e, 
//                     the event number-level is a too fine granularity level
//-----------------------------------------------------------------------------
#include "TSystem.h"
#include "TObjString.h"
#include "TChain.h"
#include "TFile.h"

#include "Stntuple/obj/TStnEvent.hh"
#include "Stntuple/obj/TStnNode.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "Stntuple/obj/TStnDataBlock.hh"

#include "Stntuple/loop/TStnAna.hh"
#include "Stntuple/loop/TStnOutputModule.hh"
#include "Stntuple/loop/TStnInputModule.hh"

ClassImp(TStnOutputModule)

//_____________________________________________________________________________
TStnOutputModule::TStnOutputModule(const char* FileName):
  TStnModule("Output", "STNTUPLE Output Module")
{
  fFileName     = FileName;
  fDatasetID    = "dsid";
  fProject      = "project";
  fFileNumber   = 0;
  fMaxFileSize  = 8000;
  fDropList     = new TObjArray(10);
  fKeepList     = new TObjArray(10);
  fTree         = 0; 
  TTree::SetMaxTreeSize(8000000000LL);
}

//_____________________________________________________________________________
TStnOutputModule::~TStnOutputModule()
{
  // destructor: module owns its histograms
  fDropList->Delete();
  delete fDropList;
  fKeepList->Delete();
  delete fKeepList;
  //delete fTree;   // Pasha could you explain this to me?
  if (fFile)
    fFile->Close();
  delete fFile;
}

//_____________________________________________________________________________
int TStnOutputModule::OpenNewFile(const char* Filename) 
{
  // if output module is defined, then we most probably want to copy some 
  // events (the whole events!) into the output tree.
  // thus enable all the input branches

  Int_t rc, split_level;

  TDirectory* dir = gDirectory;
  fFileName = Form("%s.%i",fFileName.Data(),gSystem->GetPid());

  fFile = new TFile(fFileName,"recreate");

  if (fFile->IsOpen()) {
				// clone input tree
    fTree = new TTree("STNTUPLE","STNTUPLE");
					// now need to loop over all the 
					// branches of the input tree and
					// register the corresponding blocks
    TObjArray* list = GetAna()->GetEvent()->GetListOfOutputNodes();
    TIter it(list);

    TBranch*          input_branch;
    TBranch*          output_branch;
    TStnNode*         node;
    const char*       class_name;
    const char*       branch_name;
    Int_t             basket_size, comp_level;

    while ((node = (TStnNode*) it.Next())) {
      branch_name  = node->GetName();
      input_branch = node->GetBranch();
      class_name   = node->GetDataBlock()->ClassName();

					// take split parameter from the input
					// at a time of the 1st implementation
					// this was not possible
      if (input_branch) {
	basket_size = input_branch->GetBasketSize();
	comp_level  = input_branch->GetCompressionLevel();
	split_level = input_branch->GetSplitLevel();
      }
      else {
					// non-split by default....
	basket_size = 64000;
	comp_level  =  1;
	split_level = -1;
      }
      output_branch = fTree->Branch(branch_name,class_name,
				    node->GetDataBlockAddress(),
				    basket_size,
				    split_level);
      output_branch->SetCompressionLevel(comp_level);
      output_branch->SetAutoDelete(kFALSE);
    }
					// create DB area
    fFile->mkdir("db");

    rc = 0;

    if (PrintLevel() != 0) {
      printf(" ============ opened %s \n",fFile->GetName());
    }
  }
  else {
    Error("OpenNewFile",Form("Can\'t open output file %s",Filename));
    rc = -1;
  }
  gDirectory    = dir;

  fLoRun        = 100000000;
  fLoSubrun     = 100000000;
  fHiRun        = -1;
  fHiSubrun     = -1;

  return rc;
}

//_____________________________________________________________________________
int TStnOutputModule::BeginJob() 
{
  // if output module is defined, then we most probably want to copy some 
  // events (the whole events!) into the output tree.
  // thus enable all the input branches

  if (PrintLevel() > 1) {
    printf("\nKeepList: \n");
    fKeepList->Print();
    printf("\nDropList: \n");
    fDropList->Print();
  }

  if (PrintLevel() != 0) {
    printf(" ============ %s \n",GetName());
    printf(" maxfilesize = %i\n",fMaxFileSize);
  }

  return OpenNewFile(fFileName.Data());
}

//_____________________________________________________________________________
int TStnOutputModule::BeginRun()
{
  char         dir_name[200];
  TDirectory*  olddir;
					// check if directory for the DB has
					// already been created
  olddir = gDirectory;
  if (! fFile->GetKey("db")) {
					// create DB subdirectory
    fFile->mkdir("db");
  }
  fFile->cd("db");
					// now check if subdirectory for this
					// run exists
  int runnum = fAna->GetHeaderBlock()->RunNumber();
  sprintf(dir_name,"run_%i",runnum);
  if (! gDirectory->GetKey(dir_name)) {
					// create subdirectory for a given run
					// and write out the structures defined
					// by DB manager
    gDirectory->mkdir(dir_name);
    gDirectory->cd   (dir_name);
    ((TObject*) fAna->GetDBManager())->Write();
  }
  
  fLastRun = runnum;

  olddir->cd();
  return 0;
}

//_____________________________________________________________________________
int TStnOutputModule::Event(Int_t I) {
  // write the event out, I here is the entry number in the current tree
  // of the _INPUT_ chain....

  int rc = 0;
//-----------------------------------------------------------------------------
// This place looks very kludgy - and I don't like it very much...
// assume that the header block has already been read in, such that 
// TStnEvent::fCurentEntry is set correctly. make sure we read the whole event
//-----------------------------------------------------------------------------
  fAna->GetEvent()->ReadTreeEntry(I);

  TStnHeaderBlock* hb = GetHeaderBlock();
  int rn  = hb->RunNumber();
  int srn = hb->SubrunNumber();
//-----------------------------------------------------------------------------
// book-keeping - keep track of low and high run/subrun numbers
//-----------------------------------------------------------------------------
  if (rn < fLoRun) {
    fLoRun    = rn;
    fLoSubrun = srn;
  }
  else if (rn == fLoRun) {
    if (srn < fLoSubrun) fLoSubrun = srn;
  }

  if (rn > fHiRun) {
    fHiRun    = rn;
    fHiSubrun = srn;
  }
  else if (rn == fHiRun) {
    if (srn > fHiSubrun) fHiSubrun = srn;
  }

					// size in MBytes
  int mbytes_written = (int) (fFile->GetBytesWritten()/1000000);
  if (mbytes_written >= fMaxFileSize) {
					// file is ALREADY too large, close it
					// event "I" goes into the next file
    fFile->Write();
    fFile->Close();
    delete fFile;
				// rename the old file into x.00
    TString fn = Form("nts.mu2e.%s.%s.%06i_%08i.stn",fDatasetID.Data(),fProject.Data(),
                      fLoRun,fLoSubrun);

    TString cmd = Form("mv %s %s",fFileName.Data(),fn.Data());
    printf("TStnOutputModule::Event: executing : %s\n",cmd.Data());

    gSystem->Exec(cmd.Data());

    fFileNumber++;
					// now open the new file

    rc = OpenNewFile(Form("%s.%02i",fFileName.Data(),fFileNumber));

					// create proper DB subdirectory
    BeginRun();
  }
					// and, finally, write out the event
  fTree->Fill();

  return rc;
}

//_____________________________________________________________________________
int TStnOutputModule::EndRun()
{
  return 0;
}

//-----------------------------------------------------------------------------
// in the end, rename the output file properly 
//-----------------------------------------------------------------------------
int TStnOutputModule::EndJob()
{
  fFile->Write();
  fFile->Close();

  TString fn = Form("nts.mu2e.%s.%s.%06i_%08i.stn",fDatasetID.Data(),fProject.Data(),
                    fLoRun,fLoSubrun);

  TString cmd = Form("mv %s %s",fFileName.Data(),fn.Data());
  printf("TStnOutputModule::Event: executing : %s\n",cmd.Data());

  gSystem->Exec(cmd.Data());

  return 0;
}

//_____________________________________________________________________________
void TStnOutputModule::DropDataBlock(const char* name) {
  fDropList->Add(new TObjString(name));
}

//_____________________________________________________________________________
void TStnOutputModule::KeepDataBlock(const char* name) {
  fKeepList->Add(new TObjString(name));
}
