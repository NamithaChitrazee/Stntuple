//-----------------------------------------------------------------------------
// a 'dataset' here is a histogram file plus data fields for plotting attributes
//-----------------------------------------------------------------------------
#include "Stntuple/val/stn_dataset.hh"
#include "Stntuple/val/hist_file.hh"
#include "Stntuple/val/stn_book.hh"

//-----------------------------------------------------------------------------
stn_book::stn_book(const char* Name, const char* HistDir) : TNamed(Name,Name) {
  fHistDir = HistDir;
  printf("stn_book::stn_book fHistDir: %-40s %-s\n",fHistDir.Data(),GetName());
  
  fListOfDatasets  = new TObjArray();
  fListOfDatasets->SetOwner(kTRUE);
}

stn_book::~stn_book() {
  delete fListOfDatasets;
}

stn_dataset_t* stn_book::NewDataset(const char* DsID, const char* Name,
                          long int NEvents, long int NGenEvents,
                          float LumiSF) {
    
  stn_dataset_t* d = new stn_dataset_t(DsID);
  TString name(Name);
  if (name == "") d->fName = DsID;
  else            d->fName = Name;

  d->fNEvents    = NEvents;
  d->fNGenEvents = NGenEvents;
  d->fLumiSF     = LumiSF;
  d->fBook       = this;
    
  fListOfDatasets->Add(d);
  return d;
}

//-----------------------------------------------------------------------------
stn_dataset_t* stn_book::FindDataset(const char* DsID) const {
  stn_dataset_t* ds(nullptr);

  int nds = fListOfDatasets->GetEntriesFast();
  
  for(int i=0; i<nds; i++) {
    stn_dataset_t* d = (stn_dataset_t*) fListOfDatasets->UncheckedAt(i);
    if (d->fName == DsID) {
      ds = d;
      break;
    }
  }
  if (ds == nullptr) {
    printf(" get_dataset ERROR: cant find dsid=%s in book=%s, return NULL\n",
           DsID,GetName());
  }
  return ds;
}

//-----------------------------------------------------------------------------
hist_file_t*   stn_book::FindHistFile(const char* DsID, const char* ProductionJob, const char* AnalysisJob) const {
  hist_file_t* hf (nullptr);
    
  stn_dataset_t* ds = FindDataset(DsID);
  if (ds == nullptr) {
    printf(" stn::book::FindHistFile ERROR: cant find dataset dsidid=\'%s\'\n",DsID);
  }
  else {
    hf = ds->FindHistFile(ProductionJob,AnalysisJob);
  }
  return hf;
}


//-----------------------------------------------------------------------------
hist_file_t* stn_book::NewHistFile(const char* DsID, const char* ProductionJob, const char* AnalysisJob) {

  hist_file_t* hf(nullptr);
    
  stn_dataset_t* ds = FindDataset(DsID);
  if (ds) hf = ds->NewHistFile(fHistDir.Data(),ProductionJob, AnalysisJob);

  return hf;
}
