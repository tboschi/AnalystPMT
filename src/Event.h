//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jul  8 14:08:25 2016 by ROOT version 6.06/00
// from TTree Event/Event's pulses tree container
// found on file: test/result/rDataR145S0p1T3Jul__3.root
//////////////////////////////////////////////////////////

#ifndef Event_h
#define Event_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class Event {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           EvtLength;
   Int_t           PMTID;
   Float_t         Baseline;
   Float_t         Peak;
   Float_t         P2V;
   Float_t         Time;
   Float_t         Width;
   Float_t         Charge;
   Float_t         Energy;
   Float_t         Area;
   Float_t         TOF;
   Float_t         Next;
   Bool_t          VETO;
   Bool_t          MRD2;
   Bool_t          MRD3;
   Float_t         Pulse[100];   //[iEL]

   // List of branches
   TBranch        *b_iEL;   //!
   TBranch        *b_iPMT;   //!
   TBranch        *b_fBaseLine;   //!
   TBranch        *b_fPeak;   //!
   TBranch        *b_fValley;   //!
   TBranch        *b_fTime;   //!
   TBranch        *b_fWidth;   //!
   TBranch        *b_fCharge;   //!
   TBranch        *b_fEnergy;   //!
   TBranch        *b_fArea;   //!
   TBranch        *b_fTOF;   //!
   TBranch        *b_fNext;   //!
   TBranch        *b_bVETO;   //!
   TBranch        *b_bMRD2;   //!
   TBranch        *b_bMRD3;   //!
   TBranch        *b_Pulse;   //!

   Event(TTree *tree=0);
   virtual ~Event();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Event_cxx
Event::Event(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test/result/rDataR145S0p1T3Jul__3.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("test/result/rDataR145S0p1T3Jul__3.root");
      }
      f->GetObject("Event",tree);

   }
   Init(tree);
}

Event::~Event()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Event::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Event::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Event::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EvtLength", &EvtLength, &b_iEL);
   fChain->SetBranchAddress("PMTID", &PMTID, &b_iPMT);
   fChain->SetBranchAddress("Baseline", &Baseline, &b_fBaseLine);
   fChain->SetBranchAddress("Peak", &Peak, &b_fPeak);
   fChain->SetBranchAddress("P2V", &P2V, &b_fValley);
   fChain->SetBranchAddress("Time", &Time, &b_fTime);
   fChain->SetBranchAddress("Width", &Width, &b_fWidth);
   fChain->SetBranchAddress("Charge", &Charge, &b_fCharge);
   fChain->SetBranchAddress("Energy", &Energy, &b_fEnergy);
   fChain->SetBranchAddress("Area", &Area, &b_fArea);
   fChain->SetBranchAddress("TOF", &TOF, &b_fTOF);
   fChain->SetBranchAddress("Next", &Next, &b_fNext);
   fChain->SetBranchAddress("VETO", &VETO, &b_bVETO);
   fChain->SetBranchAddress("MRD2", &MRD2, &b_bMRD2);
   fChain->SetBranchAddress("MRD3", &MRD3, &b_bMRD3);
   fChain->SetBranchAddress("Pulse", Pulse, &b_Pulse);
   Notify();
}

Bool_t Event::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Event::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Event::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Event_cxx
