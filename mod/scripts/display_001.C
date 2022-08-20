//-----------------------------------------------------------------------------
// test macro : fill and display histogram of N(combo hits) in the event
// no includes needed
// hide variables - dos it have any effect ?
//-----------------------------------------------------------------------------
namespace {
  TH1F*     hist(nullptr);	       // to be accessed interactively
  TCanvas*  c   (nullptr);
}
//-----------------------------------------------------------------------------
// Mode = 0: begin job (run)
// Mode = 1: event
// Mode = 2: end job (run)
//-----------------------------------------------------------------------------
void display_001(int Mode, TModule* Module) {
  printf("display_001 called: Mode = %i, Module = %p\n",Mode,Module);

  TStnVisManager* vm = TStnVisManager::Instance();

  if (Mode == 0) {  
//-----------------------------------------------------------------------------
// book histograms , perform other initializations
//-----------------------------------------------------------------------------
    c = new TCanvas;
    c->cd();
    hist    = new TH1F ("hist","hist",200,0,200);
    hist->Draw();
  }
  else if (Mode == 1) {
//-----------------------------------------------------------------------------
// fill histograms 
//-----------------------------------------------------------------------------
    TTrkVisNode* tvn = (TTrkVisNode*) vm->FindNode("TrkVisNode");

    const mu2e::ComboHitCollection* chc = tvn->GetComboHitColl();

    int nh = chc->size();

    hist->Fill(nh);

    printf("nh:  %i\n",nh);

    c->Modified();
    c->Update();
  }
  else if (Mode == 2) {
//-----------------------------------------------------------------------------
// end run
//-----------------------------------------------------------------------------
  }
}
