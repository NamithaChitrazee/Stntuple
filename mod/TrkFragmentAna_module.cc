
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

// ======================================================================

  TrkFragmentAna::TrkFragmentAna(fhicl::ParameterSet const& PSet) : 
    THistModule (PSet                  ,"TrkFragmentAna"),
    _diagLevel  (PSet.get<int>         ("diagLevel"  )), 
    _minNBytes  (PSet.get<int>         ("minNBytes"  )), 
    _maxNBytes  (PSet.get<int>         ("maxNBytes"  )), 
    _trkfCollTag(PSet.get<std::string> ("trkfCollTag")) {
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
    _Hist.channel[i].nhits   = frag_dir.make<TH1F>(Form("ch_%02i_nhits" ,i),Form("Channel %02i nhits"  ,i),100, 0.,   100.);
    _Hist.channel[i].time[0] = frag_dir.make<TH1F>(Form("ch_%02i_time0" ,i),Form("Channel %02i time[0]",i),250, 0., 25000.);
    _Hist.channel[i].time[1] = frag_dir.make<TH1F>(Form("ch_%02i_time1" ,i),Form("Channel %02i time[0]",i),250, 0., 25000.);
    _Hist.channel[i].tot [0] = frag_dir.make<TH1F>(Form("ch_%02i_tot0"  ,i),Form("Channel %02i time[0]",i),100, 0.,   100.);
    _Hist.channel[i].tot [1] = frag_dir.make<TH1F>(Form("ch_%02i_tot1"  ,i),Form("Channel %02i time[0]",i),100, 0.,   100.);
    _Hist.channel[i].pmp     = frag_dir.make<TH1F>(Form("ch_%02i_pmp"   ,i),Form("Channel %02i pmp"    ,i),100, 0.,    10.);
  }
  //   for (int j=0; j<10; j++) {
  //     _Hist.h_wf [i][j] = trkDir.make<TH1F>(Form("h_wf_ch_%02i_%i",i,j),Form("Channel [%02i][%i] waveform",i,j),20, 0.,20.);
  //   }
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
  for (/* auto */ const artdaq::Fragment& frag : *handle) {
    ushort* buf = (ushort*) (frag.dataBegin());
    int nbytes  = buf[0];
    // int fsize   = frag.size();

    nfrag      += 1;
    nbtot      += nbytes;

    analyze_fragment(&frag,&_Hist.frag);

    if (_diagLevel > 2) {
//-----------------------------------------------------------------------------
// print fragments in HEX, for the tracker, the data has to be in 2-byte words
//-----------------------------------------------------------------------------
      int nw      = nbytes/2;
      int loc     = 0;
      
      for (int i=0; i<nw; i++) {
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
}

//-----------------------------------------------------------------------------
// void TrkFragmentAna::analyze_tracker(const mu2e::TrackerFragment& Fragment) {
  void TrkFragmentAna::analyze_fragment(const artdaq::Fragment* Fragment, FragmentHist_t* Hist) {


  int size   = Fragment->size();

  short* data = (short*) Fragment->dataBegin();

  int nbytes = data[0];

  DtcDataHeaderPacket_t* dh = (DtcDataHeaderPacket_t*) (data);

  int npackets = dh->nPackets;
  int nhits    = npackets/2;

  Hist->nbytes->Fill(nbytes);
  Hist->dsize->Fill(size-nbytes);
  Hist->npackets->Fill(npackets);
  Hist->nhits->Fill(nhits);
  Hist->valid->Fill(dh->valid);

  int nhits_ch[kNChannels];
  for (int i=0; i<kNChannels; i++) nhits_ch[i] = 0;

  for (int ihit=0; ihit<nhits; ihit++) {
//-----------------------------------------------------------------------------
// first packet, 16 bytes, or 8 ushort's is the data header packet
//-----------------------------------------------------------------------------
    TrackerFragment::TrackerDataPacket* hit = (TrackerFragment::TrackerDataPacket*) (data+ihit*0x10+0x08);
    int ich = hit->StrawIndex;
    
    float tdc_sf(25e-3); // 25 psec (approx ? - double check)

                                        // fix the channel number
    if (ich > 128) ich = ich-128;
    nhits_ch[ich] += 1;
    _Hist.channel[ich].time[0]->Fill(hit->TDC0()*tdc_sf);
    _Hist.channel[ich].time[1]->Fill(hit->TDC1()*tdc_sf);
    _Hist.channel[ich].tot [0]->Fill(hit->TOT0);
    _Hist.channel[ich].tot [1]->Fill(hit->TOT1);
    _Hist.channel[ich].pmp    ->Fill(hit->PMP);
  }

  for (int i=0; i<kNChannels; i++) {
    _Hist.channel[i].nhits->Fill(nhits_ch[i]);
  }

//-----------------------------------------------------------------------------
// this part needs to be changed 
//-----------------------------------------------------------------------------
  _Hist.event.nhits->Fill(nhits);
  for (int ich=0; ich<kNChannels; ich++) {
    for (int ihit=0; ihit<nhits_ch[ich]; ihit++) 
    _Hist.event.nhits_vs_ch->Fill(ich);
  }
}

} // end namespace mu2e

DEFINE_ART_MODULE(mu2e::TrkFragmentAna)
