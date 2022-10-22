//-----------------------------------------------------------------------------
//  2014-01-26 P.Murat - TStrawHit
//-----------------------------------------------------------------------------
#ifndef TStrawHit_hh
#define TStrawHit_hh

#include <math.h>
#include "TMath.h"
#include "TObject.h"
#include "TBuffer.h"

class TStrawHit : public TObject {
public:
//-----------------------------------------------------------------------------
// constants imported from DataProducts/inc/StrawId.hh
// indexing of the calibration and HV ends 
//-----------------------------------------------------------------------------
  enum { eCal = 0, eHv = 1 } ;

  constexpr static ushort _layermsk    = 0x1;                 // mask for layer field
  constexpr static ushort _strawmsk    = 0x7F;                // mask for straw field
  constexpr static ushort _preampmsk   = 0x7E;                // mask for preamp
  constexpr static ushort _panelmsk    = 0x380;               // mask for panel field
  constexpr static ushort _preampsft   = 1;                   // shift for preamp field
  constexpr static ushort _panelsft    = 7;                   // shift for panel field
  constexpr static ushort _facemsk     = 0x80;                // mask for face field
  constexpr static ushort _facesft     = 7;                   // shift for face field
  constexpr static ushort _planemsk    = 0xFC00;              // mask for plane field
  constexpr static ushort _planesft    = 10;                  // shift for plane field
  constexpr static ushort _stationmsk  = 0xF800;              // mask for station field
  constexpr static ushort _stationsft  = 11;                  // shift for station field
  constexpr static ushort _invalid     = 0xFFFF;              // invalid identifier
  constexpr static ushort _nstraws     = 96;                  // number of straws per panel
  constexpr static ushort _nlayers     = 2;                   // number of layers per panel ; do we need it, see below
  constexpr static ushort _npanels     = 6;                   // number of panels per plane
  constexpr static ushort _nfaces      = 2;                   // number of faces in a plane
  constexpr static ushort _nplanes     = 36;                  // number of planes
  constexpr static ushort _nstations   = _nplanes/2;          // number of stations
  constexpr static ushort _nupanels    = _npanels * _nplanes; // number of unique panels
  constexpr static ushort _nustraws    = _nupanels* _nstraws; // number of unique straws
  constexpr static ushort _nustrawends = _nustraws*2;         // number of unique straw ends
  constexpr static ushort _ntotalfaces = _nfaces*_nplanes;    // tot N(faces)
  constexpr static ushort _maxval      = ((_nplanes -1) << _planesft) + ((_npanels -1) << _panelsft) + _nstraws; // maximum Id as uint16 value

protected:
					// data
  int     fStrawID;			// sid | (mcflag << 16)
					// -------------------- MC info, don't write out for the data
  int     fGenID;                       // generator ID + StrawDigiFlag << 16
  int     fSimID;			// sim particle ID
  int     fPdgID;			// sim particle PDG ID
  int     fMotherPdgID;	                // mother PDG ID
	  				// ----------------------------- floats
  float   fTime[2];			// 
  float   fTOT[2];                      // 
  float   fEDep;			// energy deposition for the hit
  float   fMcMom;			// MC particle momentum

public:
                                        // constructors and destructors
  TStrawHit(int I = -1);
  virtual ~TStrawHit();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  int   StrawID    () const { return (fStrawID & 0x0000ffff )               ; }
  int   MCFlag     () const { return (fStrawID & 0xffff0000 ) >> 16         ; }
		     
  int   Station    () const { return (fStrawID & _stationmsk) >> _stationsft; }
  int   Panel      () const { return (fStrawID & _panelmsk  ) >> _panelsft  ; }
  int   Face       () const { return (fStrawID & _facemsk   ) >> _facesft   ; }
  int   Layer      () const { return (fStrawID & _layermsk  )               ; }
  int   Straw      () const { return (fStrawID & _strawmsk  )               ; }
  int   Preamp     () const { return (fStrawID & _preampmsk ) >> _preampsft ; }

  float Time  (int I) const { return fTime[I]; }
  float TOT   (int I) const { return fTOT [I]; }
  int   Dt         () const { return fTime[1]-fTime[0]; }

  int   PdgID      () const { return fPdgID;       }
  int   MotherPdgID() const { return fMotherPdgID   ; }
  int   GenID      () const { return fGenID & 0XFFFF; }
  int   SimID      () const { return fSimID;       }

					// just one byte
  int   StrawDigiFlag () const { return (fGenID >> 16) & 0xFF; }

  float EDep       () const { return fEDep;        }
  float McMom      () const { return fMcMom;       }
//-----------------------------------------------------------------------------
// modifiers, assume TOT = tot[0] | (tot[1] << 16
//-----------------------------------------------------------------------------
  void    Set(int   StrawID, float* Time , float* TOT  , 
	      int   GenID  , int    SimID, 
	      int   PdgID  , int    MotherPdgID, 
	      float EDep   , float  McMom);
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  void Clear(Option_t* opt = "");
  void Print(Option_t* opt = "") const;
//-----------------------------------------------------------------------------
// schema evolution
//-----------------------------------------------------------------------------
  void ReadV1(TBuffer& R__b);
  void ReadV2(TBuffer& R__b);

  ClassDef (TStrawHit,3)
};

#endif
