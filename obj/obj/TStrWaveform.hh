//-----------------------------------------------------------------------------
//  2022-09-24 P.Murat - TStrWaveform
//-----------------------------------------------------------------------------
#ifndef TStrWaveform_hh
#define TStrWaveform_hh

#include <math.h>
#include "TMath.h"
#include "TObject.h"
#include "TBuffer.h"

class TStrWaveform : public TObject {
public:
					// data
  int     fNWords;
  ushort* fData;                        // [fNWords] array of fNWords shorts

public:
                                        // constructors and destructors
  TStrWaveform(int ID = -1);
  virtual ~TStrWaveform();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  int     NWords   () { return fNWords ; }
  ushort  Data(int I) { return fData[I]; }
  ushort* Data()      { return fData   ; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void    Set(int NWords, const ushort* Data);
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  void Clear(Option_t* opt = "");
  void Print(Option_t* opt = "") const;
//-----------------------------------------------------------------------------
// schema evolution
//-----------------------------------------------------------------------------
//  void ReadV1(TBuffer &R__b);

  ClassDef (TStrWaveform,1)
};

#endif
