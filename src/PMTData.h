//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun  6 15:22:56 2016 by ROOT version 6.06/00
// from TTree PMTData/PMTData
// found on file: test/DataR73S0p1T3Jun__1.root
//////////////////////////////////////////////////////////

#ifndef PMTData_h
#define PMTData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class PMTData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   ULong64_t       Trigger;
   Int_t           LastSync;
   Int_t           SequenceID;
   Int_t           StartTimeSec;
   Int_t           StartTimeNSec;
   ULong64_t       StartCount;
   Int_t           TriggerNumber;
   ULong64_t       TriggerCounts[5];   //[TriggerNumber]
   UInt_t          Rate;
   Int_t           CardID;
   Int_t           Channel;
   Int_t           PMTID;
   Int_t           PMTf;
   Int_t           PMTx;
   Int_t           PMTy;
   Int_t           PMTz;
   Int_t           BufferSize;
   Float_t         Data[40000];   //[BufferSize]

   // List of branches
   TBranch        *b_Trigger;   //!
   TBranch        *b_LastSync;   //!
   TBranch        *b_SequenceID;   //!
   TBranch        *b_StartTimeSec;   //!
   TBranch        *b_StartTimeNSec;   //!
   TBranch        *b_StartCount;   //!
   TBranch        *b_TriggerNumber;   //!
   TBranch        *b_TriggerCounts;   //!
   TBranch        *b_Rate;   //!
   TBranch        *b_CardID;   //!
   TBranch        *b_Channel;   //!
   TBranch        *b_PMTID;   //!
   TBranch        *b_PMTf;   //!
   TBranch        *b_PMTx;   //!
   TBranch        *b_PMTy;   //!
   TBranch        *b_PMTz;   //!
   TBranch        *b_BufferSize;   //!
   TBranch        *b_Data;   //!

   PMTData(TTree *tree=0);
   virtual ~PMTData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PMTData_cxx
PMTData::PMTData(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test/DataR73S0p1T3Jun__1.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("test/DataR73S0p1T3Jun__1.root");
      }
      f->GetObject("PMTData",tree);

   }
   Init(tree);
}

PMTData::~PMTData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t PMTData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t PMTData::LoadTree(Long64_t entry)
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

void PMTData::Init(TTree *tree)
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

   fChain->SetBranchAddress("Trigger", &Trigger, &b_Trigger);
   fChain->SetBranchAddress("LastSync", &LastSync, &b_LastSync);
   fChain->SetBranchAddress("SequenceID", &SequenceID, &b_SequenceID);
   fChain->SetBranchAddress("StartTimeSec", &StartTimeSec, &b_StartTimeSec);
   fChain->SetBranchAddress("StartTimeNSec", &StartTimeNSec, &b_StartTimeNSec);
   fChain->SetBranchAddress("StartCount", &StartCount, &b_StartCount);
   fChain->SetBranchAddress("TriggerNumber", &TriggerNumber, &b_TriggerNumber);
   fChain->SetBranchAddress("TriggerCounts", TriggerCounts, &b_TriggerCounts);
   fChain->SetBranchAddress("Rate", &Rate, &b_Rate);
   fChain->SetBranchAddress("CardID", &CardID, &b_CardID);
   fChain->SetBranchAddress("Channel", &Channel, &b_Channel);
   fChain->SetBranchAddress("PMTID", &PMTID, &b_PMTID);
   fChain->SetBranchAddress("PMTf", &PMTf, &b_PMTf);
   fChain->SetBranchAddress("PMTx", &PMTx, &b_PMTx);
   fChain->SetBranchAddress("PMTy", &PMTy, &b_PMTy);
   fChain->SetBranchAddress("PMTz", &PMTz, &b_PMTz);
   fChain->SetBranchAddress("BufferSize", &BufferSize, &b_BufferSize);
   fChain->SetBranchAddress("Data", Data, &b_Data);
   Notify();
}

Bool_t PMTData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void PMTData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t PMTData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef PMTData_cxx
