
#include "Stntuple/mod/TrkFragmentAna_module.hh"

namespace mu2e {

//-----------------------------------------------------------------------------
unsigned int reverseBits(unsigned int num) {
  unsigned int numOfBits = 10; // sizeof(num) * 8; // Number of bits in an unsigned int

  unsigned int reversedNum = 0;
  for (unsigned int i = 0; i < numOfBits; ++i) {
    if ((num & (1 << i)) != 0)
      reversedNum |= 1 << ((numOfBits - 1) - i);
  }

  return reversedNum;
}

//-----------------------------------------------------------------------------
unsigned int correctedTDC(unsigned int TDC) {
  uint32_t corrected_tdc = ((TDC & 0xFFFF00) + (0xFF  - (TDC & 0xFF)));
  return corrected_tdc;
}

//-----------------------------------------------------------------------------
// NWords : the number of short words
//-----------------------------------------------------------------------------
  void TrkFragmentAna::printFragment(const artdaq::Fragment* Frag, int NWords) {
//-----------------------------------------------------------------------------
// print fragments in HEX, for the tracker, the data has to be in 2-byte words
//-----------------------------------------------------------------------------
    ushort* buf = (ushort*) (Frag->dataBegin());

    int loc     = 0;
      
    for (int i=0; i<NWords; i++) {
      if (loc == 0) printf(" 0x%08x: ",i*2);

      ushort  word = buf[i];
      printf("0x%04x ",word);

      loc += 1;
      if (loc == 8) {
        printf("\n");
        loc = 0;
      }
    }
      
    if (loc != 0) printf("\n");
  }

// ======================================================================

  TrkFragmentAna::TrkFragmentAna(fhicl::ParameterSet const& PSet) : 
    THistModule      (PSet                  ,"TrkFragmentAna"),
    _diagLevel       (PSet.get<int>         ("diagLevel"       )), 
    _minNBytes       (PSet.get<int>         ("minNBytes"       )), 
    _maxNBytes       (PSet.get<int>         ("maxNBytes"       )), 
    _dataHeaderOffset(PSet.get<int>         ("dataHeaderOffset")), 
    _trkfCollTag     (PSet.get<std::string> ("trkfCollTag"     )),
    _dumpDTCRegisters(PSet.get<int>         ("dumpDTCRegisters")),
    _referenceChannel(PSet.get<int>         ("referenceChannel")),
    _analyzeFragments(PSet.get<int>         ("analyzeFragments"))
  {
  }

//-----------------------------------------------------------------------------
void TrkFragmentAna::book_histograms(int RunNumber) {
  art::ServiceHandle<art::TFileService> tfs;

  art::TFileDirectory top_dir     = tfs->mkdir("trk");
  art::TFileDirectory frag_dir    = tfs->mkdir("trk/frag_0");

  _Hist.event.nhits       = top_dir.make<TH1F>("nhits"    , Form("run %06i: nhits total" ,RunNumber), 1000, 0.,  1000.);
  _Hist.event.nhits_vs_ch = top_dir.make<TH1F>("nh_vs_ch" , Form("run %06i: nh vs ch"    ,RunNumber),  100, 0.,   100.);
  _Hist.event.nbtot       = top_dir.make<TH1F>("nbtot"    , Form("run %06i: nbytes total",RunNumber),10000, 0., 10000.);
  _Hist.event.nfrag       = top_dir.make<TH1F>("nfrag"    , Form("run %06i: n fragments" ,RunNumber),  100, 0.,   100.);

  _Hist.frag.nbytes       = frag_dir.make<TH1F>("nbytes"  , Form("run %06i: n bytes"     ,RunNumber),10000,    0., 10000.);
  _Hist.frag.dsize        = frag_dir.make<TH1F>("dsize"   , Form("run %06i: size()-nb"   ,RunNumber),  200, -100.,   100.);
  _Hist.frag.npackets     = frag_dir.make<TH1F>("npackets", Form("run %06i: n packets"   ,RunNumber), 1000,    0.,  1000.);
  _Hist.frag.nhits        = frag_dir.make<TH1F>("nhits"   , Form("run %06i: n hits"      ,RunNumber),  300,    0.,   300.);
  _Hist.frag.valid        = frag_dir.make<TH1F>("valid"   , Form("run %06i: valid"       ,RunNumber),    2,    0.,     2.);

  printf("[mu2e::TrkFragmentAna] pointer to the module: 0x%8p\n",(void*) this);

  for (int i=0; i<kNChannels; i++) {
    art::TFileDirectory chan_dir = tfs->mkdir(Form("trk/frag_0/ch_%02i",i));
    _Hist.channel[i].nhits   = chan_dir.make<TH1F>(Form("ch_%02i_nhits",i),Form("run %06i: ch %02i nhits"  ,RunNumber,i),100, 0.,    100.);
    _Hist.channel[i].time[0] = chan_dir.make<TH1F>(Form("ch_%02i_time0",i),Form("run %06i: ch %02i time[0]",RunNumber,i),250, 0., 100000.);
    _Hist.channel[i].time[1] = chan_dir.make<TH1F>(Form("ch_%02i_time1",i),Form("run %06i: ch %02i time[0]",RunNumber,i),250, 0., 100000.);
    _Hist.channel[i].tot [0] = chan_dir.make<TH1F>(Form("ch_%02i_tot0" ,i),Form("run %06i: ch %02i time[0]",RunNumber,i),100, 0.,    100.);
    _Hist.channel[i].tot [1] = chan_dir.make<TH1F>(Form("ch_%02i_tot1" ,i),Form("run %06i: ch %02i time[0]",RunNumber,i),100, 0.,    100.);
    _Hist.channel[i].pmp     = chan_dir.make<TH1F>(Form("ch_%02i_pmp"  ,i),Form("run %06i: ch %02i pmp"    ,RunNumber,i),100, 0.,     10.);
    _Hist.channel[i].dt0     = chan_dir.make<TH1F>(Form("ch_%02i_dt0"  ,i),Form("run %06i: ch %02i T0(i+1)-T0(i)",RunNumber,i)      ,50000,  0.,50);
    _Hist.channel[i].dt1     = chan_dir.make<TH1F>(Form("ch_%02i_dt1"  ,i),Form("run %06i: ch %02i T1(i+1)-T1(i)",RunNumber,i)      ,50000,  0.,50);
    _Hist.channel[i].dt0r    = chan_dir.make<TH1F>(Form("ch_%02i_dt0r" ,i),Form("run %06i: ch %02i T0(ich,0)-T0(ref,0)",RunNumber,i),50000,-25.,25);
    _Hist.channel[i].dt1r    = chan_dir.make<TH1F>(Form("ch_%02i_dt1r" ,i),Form("run %06i: ch %02i T1(ich,0)-T1(ref,0)",RunNumber,i),50000,-25.,25);

    for (int j=0; j<kMaxNHitsPerChannel; j++) {
      _Hist.channel[i].wf[j] = chan_dir.make<TH1F>(Form("h_wf_ch_%02i_%i",i,j),Form("run %06i: ch [%02i][%i] waveform",RunNumber,i,j),20, 0.,20.);
    }
  }

}

//-----------------------------------------------------------------------------
void TrkFragmentAna::beginRun(const art::Run& aRun) {
  int rn  = aRun.run();

  if (rn <= 285) _dataHeaderOffset =  0;
  else           _dataHeaderOffset = 32;

  book_histograms(rn);
}

//--------------------------------------------------------------------------------
void TrkFragmentAna::beginJob() {
}

//-----------------------------------------------------------------------------
void TrkFragmentAna::endJob() {
  printf("[mu2e::TrkFragmentAna] pointer to the module: 0x%8p\n",(void*) this);
}

//--------------------------------------------------------------------------------
// assume that we only have tracker fragment(s)
//-----------------------------------------------------------------------------
void TrkFragmentAna::analyze(const art::Event& event) {
  //art::EventNumber_t eventNumber = event.event();

  int    nbtot = 0;
  int    nfrag = 0;

  for (int i=0; i<100; i++) _nwf[i] = 0;

  auto handle = event.getValidHandle<std::vector<artdaq::Fragment> >(_trkfCollTag);
//-----------------------------------------------------------------------------
// calculate the fragment size manually - big thank you to designers (:
//----------------------------------------------------------------------------- 
  _error = 0;

  int ifrag = 0;
  for (/* auto */ const artdaq::Fragment& frag : *handle) {
    ushort* buf = (ushort*) (frag.dataBegin());
    int nbytes  = buf[0];
    int fsize   = frag.sizeBytes();

    nfrag      += 1;
    nbtot      += nbytes;

    if (_analyzeFragments) analyze_fragment(&frag,&_Hist.frag);

    if (_diagLevel > 2) {
      printf("%s: ---------- TRK fragment # %3i nbytes: %5i fsize: %5i\n",__func__,ifrag,nbytes,fsize);
      printFragment(&frag,nbytes/2);
    }
    ifrag++;
  }

  _Hist.event.nbtot->Fill(nbtot);
  _Hist.event.nfrag->Fill(nfrag);

  if (_diagLevel > 1) {
    if ((nbtot >= _minNBytes) and (nbtot <= _maxNBytes)) {
      printf(" Run : %5i subrun: %5i event: %8i nfrag: %3i nbytes: %5i\n", 
	     event.run(),event.subRun(),event.event(), nfrag, nbtot);
    }
  }
//-----------------------------------------------------------------------------
// go into interactive mode, 
// fInteractiveMode = 0 : do not stop
// fInteractiveMode = 1 : stop after each event (event display mode)
// fInteractiveMode = 2 : stop only in the end of run, till '.q' is pressed
//-----------------------------------------------------------------------------
  TModule::analyze(event);
//-----------------------------------------------------------------------------
// DTC registers
//-----------------------------------------------------------------------------
  if (_dumpDTCRegisters) {
    auto h = event.getValidHandle<std::vector<artdaq::Fragment>>("daq:TRKDTC");

    for (/* auto */ const artdaq::Fragment& frag : *h) {
      int *buf  = (int*) (frag.dataBegin());
      int nreg  = buf[0];
      int fsize = frag.sizeBytes();
      printf("%s: -------- DTC registers dump n(reg)=%5i size: %5i\n",__func__,nreg,fsize);
      printFragment(&frag,2+4*nreg);
    }
  }
  
}

//-----------------------------------------------------------------------------
// void TrkFragmentAna::analyze_tracker(const mu2e::TrackerFragment& Fragment) {
  void TrkFragmentAna::analyze_fragment(const artdaq::Fragment* Fragment, FragmentHist_t* Hist) {


    int size   = Fragment->size();

    short* data = (short*) Fragment->dataBegin();

    int nbytes = data[0];

    DtcDataHeaderPacket_t* dh = (DtcDataHeaderPacket_t*) (data + _dataHeaderOffset);

    int npackets = dh->nPackets;
    int nhits    = npackets/2;

    //  printf("nhits : %3i\n",nhits);

    Hist->nbytes->Fill(nbytes);
    Hist->dsize->Fill(size-nbytes);
    Hist->npackets->Fill(npackets);
    Hist->nhits->Fill(nhits);
    Hist->valid->Fill(dh->valid);

    for (int i=0; i<kNChannels; i++) {
      _data.channel[i].nhits = 0;
    }

    for (int ihit=0; ihit<nhits; ihit++) {
//-----------------------------------------------------------------------------
// first packet, 16 bytes, or 8 ushort's is the data header packet
//-----------------------------------------------------------------------------
      TrackerFragment::TrackerDataPacket* hit ;
      hit     = (TrackerFragment::TrackerDataPacket*) (data+ihit*0x10+_dataHeaderOffset+0x08);
      int ich = hit->StrawIndex;

      float tdc_sf(5/256.); // TDC bin width (from Richie)

      if (ich > 128) ich = ich-128;

      if (ich > 95) {
        printf ("ERROR in %s: ich = %i, BAIL OUT\n",__func__,ich);
        _error = -1;
        return;
      }

      int nh = _data.channel[ich].nhits;

      if (nh >= kMaxNHitsPerChannel) {
        printf ("ERROR in %s: ich = %i, N(hits) >= %i BAIL OUT\n",__func__,ich,kMaxNHitsPerChannel);
        _error = -2;
        return;
      }

      _data.channel[ich].hit[nh] = hit;
      _data.channel[ich].nhits  += 1;

      uint32_t corr_tdc0 = correctedTDC(hit->TDC0());
      uint32_t corr_tdc1 = correctedTDC(hit->TDC1());

      _Hist.channel[ich].time[0]->Fill(corr_tdc0*tdc_sf);
      _Hist.channel[ich].time[1]->Fill(corr_tdc1*tdc_sf);

      _Hist.channel[ich].tot [0]->Fill(hit->TOT0);
      _Hist.channel[ich].tot [1]->Fill(hit->TOT1);
      _Hist.channel[ich].pmp    ->Fill(hit->PMP);
//-----------------------------------------------------------------------------
// waveforms in a given channel
//-----------------------------------------------------------------------------
      uint16_t adc[15];

      adc[ 0] = reverseBits(hit->ADC00);
      adc[ 1] = reverseBits(hit->ADC01A + (hit->ADC01B << 6));
      adc[ 2] = reverseBits(hit->ADC02);

      TrackerFragment::TrackerADCPacket* ahit = (TrackerFragment::TrackerADCPacket*) ((uint16_t*)hit+6);


      adc[ 3] = reverseBits(ahit->ADC0);
      adc[ 4] = reverseBits(ahit->ADC1A + (ahit->ADC1B << 6));
      adc[ 5] = reverseBits(ahit->ADC2);
      adc[ 6] = reverseBits(ahit->ADC3);
      adc[ 7] = reverseBits(ahit->ADC4A + (ahit->ADC4B << 6));
      adc[ 8] = reverseBits(ahit->ADC5);
      adc[ 9] = reverseBits(ahit->ADC6);
      adc[10] = reverseBits(ahit->ADC7A + (ahit->ADC7B << 6));
      adc[11] = reverseBits(ahit->ADC5);
      adc[12] = reverseBits(ahit->ADC6);
      adc[13] = reverseBits(ahit->ADC10A + (ahit->ADC10B << 6));
      adc[14] = reverseBits(ahit->ADC11);

      _Hist.channel[ich].wf[nh]->Reset();
      for (int is=0; is<15; is++) {
        _Hist.channel[ich].wf[nh]->Fill(is,adc[is]);
      }
    }

    for (int i=0; i<kNChannels; i++) {
      int nh = _data.channel[i].nhits;
      _Hist.channel[i].nhits->Fill(nh);
      for (int ih=1; ih<nh; ih++) {
        uint32_t corr_tdc0_ih  = correctedTDC(_data.channel[i].hit[ih]->TDC0());
        uint32_t corr_tdc1_ih  = correctedTDC(_data.channel[i].hit[ih]->TDC1());
        uint32_t corr_tdc0_ih1 = correctedTDC(_data.channel[i].hit[ih-1]->TDC0());
        uint32_t corr_tdc1_ih1 = correctedTDC(_data.channel[i].hit[ih-1]->TDC1());

        int dt0 = (corr_tdc0_ih-corr_tdc0_ih1)*5/256.*1.e-3;
        _Hist.channel[i].dt0->Fill(dt0);
        int dt1 = (corr_tdc1_ih-corr_tdc1_ih1)*5/256.*1.e-3;
        _Hist.channel[i].dt1->Fill(dt1);
      }
    }
//-----------------------------------------------------------------------------
// reference channel first
//-----------------------------------------------------------------------------
    int nhr = _data.channel[_referenceChannel].nhits;
    if (nhr == 0) {
      float dt = -1e6;
      for (int i=0; i<kNChannels; i++) {
        int nh = _data.channel[i].nhits;
        _Hist.channel[i].nhits->Fill(nh);
        if (nh > 0) { 
          _Hist.channel[i].dt0r->Fill(dt);
          _Hist.channel[i].dt1r->Fill(dt);
        }
      }
    }
    else {
//-----------------------------------------------------------------------------
// there is at least one hit in the reference channel
//-----------------------------------------------------------------------------
      float t0r = correctedTDC(_data.channel[_referenceChannel].hit[0]->TDC0());
      float t1r = correctedTDC(_data.channel[_referenceChannel].hit[0]->TDC1());

      for (int i=0; i<kNChannels; i++) {
        int nh = _data.channel[i].nhits;
        _Hist.channel[i].nhits->Fill(nh);
        if (nh > 0) {
          uint32_t t0 = correctedTDC(_data.channel[i].hit[0]->TDC0());
          float dt0r  = (t0-t0r)*5/256.*1.e-3;
          _Hist.channel[i].dt0r->Fill(dt0r);
          uint32_t t1 = correctedTDC(_data.channel[i].hit[0]->TDC1());
          float dt1r  = (t1-t1r)*5/256.*1.e-3;
          _Hist.channel[i].dt1r->Fill(dt1r);
        }
      }
    }
//-----------------------------------------------------------------------------
// this part needs to be changed 
//-----------------------------------------------------------------------------
    _Hist.event.nhits->Fill(nhits);
    for (int ich=0; ich<kNChannels; ich++) {
      int nh = _data.channel[ich].nhits;
      for (int ihit=0; ihit<nh; ihit++) {
        _Hist.event.nhits_vs_ch->Fill(ich);
      }
    }
  }

} // end namespace mu2e

DEFINE_ART_MODULE(mu2e::TrkFragmentAna)
