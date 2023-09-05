
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
    _dumpDTCRegisters(PSet.get<int>         ("dumpDTCRegisters"))
  {
  }

//--------------------------------------------------------------------------------
// book histograms
//--------------------------------------------------------------------------------
void TrkFragmentAna::beginJob() {
  art::ServiceHandle<art::TFileService> tfs;

  art::TFileDirectory top_dir     = tfs->mkdir("trk");
  art::TFileDirectory frag_dir    = tfs->mkdir("trk/frag_0");

  _Hist.event.nhits       = top_dir.make<TH1F>("nhits"   , "nhits total"  , 1000, 0.,  1000.);
  _Hist.event.nhits_vs_ch = top_dir.make<TH1F>("nh_vs_ch", "nh vs ch"     ,  100, 0.,   100.);
  _Hist.event.nbtot       = top_dir.make<TH1F>("nbtot"   , "nbytes total" ,10000, 0., 10000.);
  _Hist.event.nfrag       = top_dir.make<TH1F>("nfrag"   , "n fragments"  ,  100, 0.,   100.);

  _Hist.frag.nbytes       = frag_dir.make<TH1F>("nbytes"  , "n bytes"     ,10000,    0., 10000.);
  _Hist.frag.dsize        = frag_dir.make<TH1F>("dsize"   , "size()-nb"   ,  200, -100.,   100.);
  _Hist.frag.npackets     = frag_dir.make<TH1F>("npackets", "n packets"   , 1000,    0.,  1000.);
  _Hist.frag.nhits        = frag_dir.make<TH1F>("nhits"   , "n hits"      ,  300,    0.,   300.);
  _Hist.frag.valid        = frag_dir.make<TH1F>("valid"   , "valid"       ,    2,    0.,     2.);

  printf("[mu2e::TrkFragmentAna] pointer to the module: 0x%8p\n",(void*) this);

  for (int i=0; i<kNChannels; i++) {
    art::TFileDirectory chan_dir = tfs->mkdir(Form("trk/frag_0/ch_%02i",i));
    _Hist.channel[i].nhits   = chan_dir.make<TH1F>(Form("ch_%02i_nhits" ,i),Form("Channel %02i nhits"  ,i),100, 0.,   100.);
    _Hist.channel[i].time[0] = chan_dir.make<TH1F>(Form("ch_%02i_time0" ,i),Form("Channel %02i time[0]",i),250, 0., 100000.);
    _Hist.channel[i].time[1] = chan_dir.make<TH1F>(Form("ch_%02i_time1" ,i),Form("Channel %02i time[0]",i),250, 0., 100000.);
    _Hist.channel[i].tot [0] = chan_dir.make<TH1F>(Form("ch_%02i_tot0"  ,i),Form("Channel %02i time[0]",i),100, 0.,   100.);
    _Hist.channel[i].tot [1] = chan_dir.make<TH1F>(Form("ch_%02i_tot1"  ,i),Form("Channel %02i time[0]",i),100, 0.,   100.);
    _Hist.channel[i].pmp     = chan_dir.make<TH1F>(Form("ch_%02i_pmp"   ,i),Form("Channel %02i pmp"    ,i),100, 0.,    10.);
    _Hist.channel[i].dt      = chan_dir.make<TH1F>(Form("ch_%02i_dt"    ,i),Form("Channel %02i T(i+1)-T(i)",i),5000,  0.,50);

    for (int j=0; j<10; j++) {
      _Hist.channel[i].wf[j] = chan_dir.make<TH1F>(Form("h_wf_ch_%02i_%i",i,j),Form("Channel [%02i][%i] waveform",i,j),20, 0.,20.);
    }
  }

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


    analyze_fragment(&frag,&_Hist.frag);

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

    int nhits_ch[kNChannels];

    for (int i=0; i<kNChannels; i++) {
      nhits_ch[i]            = 0;
      _data.channel[i].nhits = 0;
    }

    for (int ihit=0; ihit<nhits; ihit++) {
//-----------------------------------------------------------------------------
// first packet, 16 bytes, or 8 ushort's is the data header packet
//-----------------------------------------------------------------------------
      TrackerFragment::TrackerDataPacket* hit = (TrackerFragment::TrackerDataPacket*) (data+ihit*0x10+_dataHeaderOffset+0x08);
      int ich = hit->StrawIndex;

      float tdc_sf(5/256.); // TDC bin width (from Richie)

      // fix the channel number
      //    printf("channel #: %3i\n",ich);
    
      if (ich > 128) ich = ich-128;

      if (ich > 95) {
        _error = -1;
        printf ("ERROR: ich = %i, BAIL OUT\n",ich);
        return;
      }

      nhits_ch[ich] += 1;

      int nh = _data.channel[ich].nhits;
      _data.channel[ich].hit[nh] = hit;
      _data.channel[ich].nhits += 1;

      _Hist.channel[ich].time[0]->Fill(hit->TDC0()*tdc_sf);
      _Hist.channel[ich].time[1]->Fill(hit->TDC1()*tdc_sf);
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

      _Hist.channel[ich].wf[ihit]->Reset();
      for (int is=0; is<15; is++) {
        _Hist.channel[ich].wf[ihit]->Fill(is,adc[is]);
      }
    }

    for (int i=0; i<kNChannels; i++) {
      int nh = _data.channel[i].nhits;
      _Hist.channel[i].nhits->Fill(nh);
      for (int ih=1; ih<nh; ih++) {
        int dt = (_data.channel[i].hit[ih]->TDC0()-_data.channel[i].hit[ih-1]->TDC0())*5/256.*1.e-3;
        _Hist.channel[i].dt->Fill(dt);
      }
    }
//-----------------------------------------------------------------------------
// this part needs to be changed 
//-----------------------------------------------------------------------------
    _Hist.event.nhits->Fill(nhits);
    for (int ich=0; ich<kNChannels; ich++) {
      for (int ihit=0; ihit<nhits_ch[ich]; ihit++) {
        _Hist.event.nhits_vs_ch->Fill(ich);
      }
    }
  }

} // end namespace mu2e

DEFINE_ART_MODULE(mu2e::TrkFragmentAna)
