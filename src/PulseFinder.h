/*
 * Find pulse for PMT root file
 *
 * Author: Tommaso Boschi
 */

#ifndef pulsefinder_H
#define pulsefinder_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>

#include "Utils.h"
#include "EventReco.h"
#include "PMTData.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TBranch.h"
#include "TKey.h"
#include "TObject.h"
#include "TBranch.h"
#include "TGraph.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TNtuple.h"
#include "TLine.h"


class PulseFinder 
{
	public:
		PulseFinder();
		~PulseFinder();

		void LoopTrg();
		void LoopPMT(int trg);
		void FindPulses(int ent);
		void FindEvents(int trg);
		void LoopEvents(int trg);
		void Fill1DHist(EventReco *ER);
		void Fill2DHist(int ID, Analysis Par);
		void FillRateHist();
		void GraphAVG(TGraph *g, int &cc);
		void NewHist();
		void Save_Hist();
		void Save_2DHist(int trg, int evt);
		void Save_GRHist(int trg, int ID, int evt);
		void ResetHist();
		void CleanAll();
		void CleanER();
//		void Print_Stats();
//		void Print_2DStats(int trg, int evt);

	private:


		Utils *Utl;
		PMTData *PMT;

		int iVerb;			//Verbosity level
		double fBW;			//BinWidth
		int RWM;			//RWM bin
		int mc, cM, cI, cO, GC;		//Counters


		std::stringstream ssName;	//Histogram name
		std::fstream fout;

		EventReco::EventReco *ER;
		std::map<int, std::vector<EventReco*> > mPulseER;
		std::map<int, std::vector<double> > mPulseT;
		std::vector<int> vTime;
		std::vector<double> vVETO, vMRD2, vMRD4;
		std::vector<double> vEvt;
		std::vector<double> vE, vT, vP, vW;
		std::map<int, double> mDR, mER;		//DarkRate and EventRate pulses count
		std::map<int, double> mLength;		//total time for rate count

		TH1I *hCard21;
		TH1F *hPulse, *hPtrig, *hPfile, *hEvent, *hEntry, *hBinWd;
		TH1F *hBaseLine, *hPeak, *hValley, *hTime, *hWidth, *hCharge, *hEnergy, *hTOF;
		TH2F *h2Ener, *h2Time, *h2Peak, *h2Widt, *h2EvRa, *h2Dark;

		TGraph *gPulse, *gMean, *giTOF, *goTOF;

		TTree *tEvent;
		float fBaseLine;
		float fPeak;
		float fValley;
		float fTime;
		float fWidth;
		float fCharge;
		float fEnergy;
		float fTOF;
		bool bVETO;
		bool bMRD2;
		bool bMRD4;
};

#endif
