#ifndef Stntuple_gui_TEvdTimeClusterVisNode_hh
#define Stntuple_gui_TEvdTimeClusterVisNode_hh

#include "Gtypes.h"
#include "TClonesArray.h"

#include "canvas/Persistency/Common/Ptr.h"

#include "Stntuple/gui/TStnVisNode.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"

class TStnTrackBlock;
class TSimpBlock;
class TStnHelixBlock;
class TStnTimeClusterBlock;

namespace stntuple {
  class TEvdTimeCluster;
}

class TEvdTimeClusterVisNode: public TStnVisNode {
public:
  
protected:

  const mu2e::TimeClusterCollection* fTcColl;
  const mu2e::ComboHitCollection*    fChColl;

  TStnTimeClusterBlock*              fTcBlock;

  std::string                        fTcCollTag;   // time cluster coll tag
  std::string                        fChCollTag;   // combo hit collection
  std::string                        fSdmcCollTag; // straw digi collection

  Color_t                            fTcColor;

  TObjArray*                         fListOfTimeClusters; // list of EvdTimeCLusters

public:
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
  TEvdTimeClusterVisNode();
  TEvdTimeClusterVisNode(const char* Name, TStnTimeClusterBlock* TcBlock = nullptr);

  virtual ~TEvdTimeClusterVisNode();

//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  TObjArray* GetListOfTimeClusters() { return fListOfTimeClusters; }
  Color_t    GetTcColor           () { return fTcColor;   }

  int        NTimeClusters        ()      { return fListOfTimeClusters->GetEntriesFast(); }

  stntuple::TEvdTimeCluster*  EvdTimeCluster(int I) { 
    return (stntuple::TEvdTimeCluster*)    fListOfTimeClusters->At(I); 
  }

  std::string& TcCollTag()   { return fTcCollTag   ; }
  std::string& ChCollTag()   { return fChCollTag   ; }
  std::string& SdmcCollTag() { return fSdmcCollTag ; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void SetChCollTag  (std::string& CollTag) { fChCollTag   = CollTag; }
  void SetSdmcCollTag(std::string& CollTag) { fSdmcCollTag = CollTag; }
  void SetTcCollTag  (std::string& CollTag) { fTcCollTag   = CollTag; }

  void  SetTcColor  (Color_t     Color) { fTcColor   = Color; }

//-----------------------------------------------------------------------------
// overloaded methods of TVisNode
//-----------------------------------------------------------------------------
  virtual int  InitEvent();

  virtual void PaintTZ (Option_t* option = "");
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  virtual int  DistancetoPrimitiveTZ(Int_t px, Int_t py);

  virtual void Clear(const char* Opt = "")       ;
  virtual void Print(const char* Opt = "") const ; // **MENU**

  ClassDef(TEvdTimeClusterVisNode,0)
};


#endif
