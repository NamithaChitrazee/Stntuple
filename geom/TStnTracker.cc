///////////////////////////////////////////////////////////////////////////////
// May 04 2013 P.Murat
// 
///////////////////////////////////////////////////////////////////////////////
#include "TVirtualX.h"
#include "TPad.h"
#include "TStyle.h"
#include "TVector3.h"
#include "TLine.h"
#include "TArc.h"
#include "TArrow.h"
#include "TMath.h"
#include "TBox.h"
#include "TEllipse.h"
#include "TObjArray.h"

#include "Stntuple/geom/TStnTracker.hh"

ClassImp(TStnTracker)
//_____________________________________________________________________________
TStnTracker::TStnTracker(): TObject() {
  fInitialized      = 0;
}

//_____________________________________________________________________________
TStnTracker::TStnTracker(GeomData_t* Geom): TObject() {
  fInitialized = 0;
  Init(Geom);
}

//-----------------------------------------------------------------------------
TStnTracker::~TStnTracker() {
  // for (int i=0; i<fNDisks; i++) {
  //   delete fDisk[i];
  // }
}

//-----------------------------------------------------------------------------
int TStnTracker::Init(GeomData_t* Geom) {
  int rc(0);

  if (fInitialized != 0) {
    printf(">>> TStnTracker::Init ERROR: an attempt to reinitialize, BAIL OUT\n");
    return -1;
  }
  
  fEnvelope = new TGeoTube(Geom->fRMin, Geom->fRMax, Geom->fHalfLength);

  fMotherVolume = new TGeoVolume("tracker",fEnvelope);
  // fVol->SetMedium(fMedia.fDetectorVacuum);
  // bar->SetLineColor(kBlue-2);
  // bar->SetTransparency(80.);
  
  // TGeoRotation    rot0("r0",0,0,0);
  // TGeoTranslation tr0 (Geom->fX0,Geom->fY0,Geom->fZ0);
      
  // TGeoCombiTrans* com0 = new TGeoCombiTrans(tr0, rot0);

  // fMu2eGeom->AddNode(fMother,1,com0);
  return rc;
}

//-----------------------------------------------------------------------------
// so far assume that 'HitID' is just a sequential channel number
//-----------------------------------------------------------------------------
// int TStnTracker::DiskNumber(int HitID) {
//   int      dn(-1), first, last;
//   TDisk*   disk;

//   for (int i=0; i<fNDisks; i++) {
//     disk = fDisk[i];
//     first = disk->FirstChanOffset();
//     last  = first+disk->NCrystals()-1;
//     if ((HitID >= first) && (HitID <= last)) {
//       dn = i;
//       break;
//     }
//   }
//   return dn;
// }
// //-----------------------------------------------------------------------------
// // so far assume that 'HitID' is just a sequential channel number
// //-----------------------------------------------------------------------------
// double TStnTracker::CrystalRadius(int HitID) {
//   int     dn, offset;
//   double  r;

//   dn = DiskNumber(HitID);

//   TDisk* disk = Disk(dn);

//   offset      = HitID-disk->FirstChanOffset();

//   r = disk->Crystal(offset)->Radius();
 
//   return r;
// }

//-----------------------------------------------------------------------------
int TStnTracker::InitEvent() {

  int               rc(0);
  // TStnCrystal*      crystal;
  // TCalHitData*      hit;
  // TDisk*            disk;
  // double            edisk[2];

// //-----------------------------------------------------------------------------
// // handle case of non-initialized calirimeter gently - don't want jobs to crash
// //-----------------------------------------------------------------------------
//   if (fInitialized < 0) return -1;

//   Clear();

//   edisk[0] = 0;
//   edisk[1] = 0;
//   nhits    = CalDataBlock->NHits();
  
//   for (int i=0; i<nhits; i++) {
//     hit     = CalDataBlock->CalHitData(i);
//     hit_id  = hit->ID();

//     dn      = DiskNumber(hit_id);
//     disk    = Disk(dn);
//     loc     = hit_id-disk->FirstChanOffset();
//     crystal = disk->Crystal(loc);

//     if (crystal != NULL) {
//       crystal->AddHit(hit);
//       edisk[dn] += hit->Energy();
//     }
//     else {
//       printf(">>> ERROR in TStnTracker::InitEvent : hit_id=%5i not assigned\n",hit_id);
//       rc = -1;
//     }
//   }

//   fDisk[0]->SetEnergy(edisk[0]);
//   fDisk[1]->SetEnergy(edisk[1]);

//   fEnergy = edisk[0]+edisk[1];

  return rc;
}

//-----------------------------------------------------------------------------
void TStnTracker::Clear(Option_t* Opt) {
}

//-----------------------------------------------------------------------------
void TStnTracker::Print(Option_t* Opt) const {
  printf(">>> ERROR: TStnTracker::Print not implemented yet aa\n");
  
}


