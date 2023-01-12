//-----------------------------------------------------------------------------
// a 'dataset' here is a histogram file plus data fields for plotting attributes
//-----------------------------------------------------------------------------

#include "Stntuple/val/stn_catalog.hh"


//-----------------------------------------------------------------------------
// book is a list of datasets, list owns its pointers
//-----------------------------------------------------------------------------

stn_catalog::stn_catalog(const char* Name) : TNamed(Name,Name) {
  // fName    = Name;
  fListOfBooks = new TObjArray();
  fListOfBooks->SetOwner(kTRUE);
}
  
stn_catalog::~stn_catalog() {
  delete fListOfBooks;
}

int stn_catalog::AddBook(stn_book* Book) {
  int rc = fListOfBooks->GetEntriesFast();
  fListOfBooks->Add(Book);
  Book->fCatalog = this;
  return rc;
}

//-----------------------------------------------------------------------------
stn_book* stn_catalog::FindBook(const char* Name) const {
  stn_book* book(nullptr);
  
  book = (stn_book*) fListOfBooks->FindObject(Name);

  if (book == nullptr) {
    printf(" get_book ERROR: cant find book=%s, return NULL\n",Name);
  }
  return book;
}
