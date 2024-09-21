//-----------------------------------------------------------------------------
// a 'dataset' here is a histogram file plus data fields for plotting attributes
//-----------------------------------------------------------------------------
#include "Stntuple/val/stn_dataset.hh"

//-----------------------------------------------------------------------------
stn_dataset_t::stn_dataset_t(const char* DsID) : TNamed(DsID,DsID) {
  fBook        = nullptr;
  fListOfHistFiles = new TObjArray();
  fListOfHistFiles->SetOwner(kTRUE);
  
  fNEvents     = -1;
  fNGenEvents  = -1;
  fEMin        =  1.;                 // make sure that non-initialized case is easy to see
  fEMax        = -1.;
  fMinRun      = -1;
  fMaxRun      = -1;
}

//-----------------------------------------------------------------------------
stn_dataset_t::~stn_dataset_t() {
  delete fListOfHistFiles;
}

//-----------------------------------------------------------------------------
hist_file_t* stn_dataset_t::NewHistFile(const char* Dir, const char* ProductionJob, const char* AnalysisJob) {
  hist_file_t* hf(nullptr);
  
  if (ProductionJob[0] != '\0') {
    if (AnalysisJob[0] != '\0') {
//-----------------------------------------------------------------------------
// file written by a Stntuple job : assume ".hist" extension
//-----------------------------------------------------------------------------
      hf = new hist_file_t(Form("%s/%s.%s.%s.hist",Dir,id(),ProductionJob,AnalysisJob),this,ProductionJob,AnalysisJob);
    }
    else {
//-----------------------------------------------------------------------------
// file written by production job : assume ".root" extension
//-----------------------------------------------------------------------------
      hf = new hist_file_t(Form("%s/%s.%s.root",Dir,id(),ProductionJob),this,ProductionJob,AnalysisJob);
    }
  }
  else {
    hf = new hist_file_t(Form("%s/%s.%s.hist",Dir,id(),AnalysisJob),this,ProductionJob,AnalysisJob);
  }

  if (hf) fListOfHistFiles->Add(hf);
  else {
    printf(" stn_book::NewHistFile ERROR: cant find dataset for dsid: %s. Bail out\n",id());
  }

  return hf;
}


//-----------------------------------------------------------------------------
hist_file_t* stn_dataset_t::FindHistFile(const char* ProductionJob, const char* AnalysisJob) const {
  int nf = fListOfHistFiles->GetEntries();

  printf("stn_dataset::%s : nf = %i ProductionJob=\'%s\' AnalysisJob=\'%s\'\n",
         __func__,nf,ProductionJob,AnalysisJob);
  
  hist_file_t* hf(nullptr);

  for (int i=0; i<nf; i++) {
    hist_file_t* hff = (hist_file_t*) fListOfHistFiles->At(i);
    printf("stn_dataset::%s : hff ProductionJob=\'%s\' AnalysisJob=\'%s\'\n",
           __func__,hff->fProductionJob.Data(),hff->fAnalysisJob.Data());
    if ((hff->fProductionJob == ProductionJob) and (hff->fAnalysisJob == AnalysisJob)) {
      printf("stn_dataset::%s : hist_file FOUND\n",__func__);
      hf = hff;
      break;
    }
  }
  if (hf == nullptr) {
    printf("ERROR in stn_dataset::%s: cant find dataset for ProductionJob=\'%s\' and AnalysisJob=\'%s\'. Bail out\n",
           __func__,ProductionJob,AnalysisJob);
  }
  printf("stn_dataset::%s : hist_file FOUND hf=%p\n",__func__,(void*)hf);
  return hf;
}
