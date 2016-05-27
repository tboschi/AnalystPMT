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

struct Coord
{
	int x;
	int z;
};

class PulseFinder
{
	public:
		PulseFinder(std::string dir = ".");
		~PulseFinder();

		void LoopTrg();
		void LoopPMT(int trg);
		void FindPulses(int ent);
		void FindEvents(int trg);
		void CatchEvents(int trg, int evt);
		void FillEvents(int trg, int evt);
		void FillHist(int trg, int evt);
		void SNcount(int trg, int evt = 0);
		void SaveDRHist();
		void Save2DHist(int trg, int evt);
		void Save1DHist();
		void SaveGraph();
		void GraphAVG(TGraph *g, int &cc);
		int LocMaximum(int j, double &p);
		double Integrate(int j);
		bool OpenIns(std::string fname);
		bool OpenOuts(std::string fname);
		void GetTree();
//		void SetHistOpt();
		void Print1DStats();
		void Print2DStats(int trg, int evt);
		void CleanHist();
		void ResetHist();
		void Close();
		bool Exist(std::string &fileName);
		std::string GetBaseName(std::string fn);
		void SetConfig(std::string cfn);
		double GetThrPeak();
		double GetThrHyst();
		int GetThrEvent();
		int GetThrSignal();
		int GetShapingTime();
		double GetBW();
		int GetEL();
		double GetPC();
		void SetThrPeak(double thr = 0.02);
		void SetThrHyst(double thr = 0.01);
		void SetThrEvent(int thr = 10);
		void SetThrSignal(int thr = 4);
		void SetShapingTime(int sha = 10);
		void SetBW(double binw = 0.002);
		void SetEL(int evnl = 500);
		void SetPC(double perc = 10);
		void SetDebug(int on = 0);
		void SetGraph(int on = 1);
		void SetVerbosity(int on = 0);
		template<typename T> T MaxV(std::vector<T> &V);
		template<typename T> T MinV(std::vector<T> &V);
		template<typename K, typename V> V MaxM(std::map<K, V> const &M);
		template<typename K, typename V> V MinM(std::map<K, V> const &M);

	protected:

		double bw;	//Bin width
		int el;		//Event length
		double pc;	//Peak position, in percentage of Event length
		PMTData *PMT;

	private:

		double thr_pek;	//Peak thr for pulse
		double r_hys;	//Hysteresis for thershold
		int thr_evt;	//On PMT thr for event
		int thr_sig;	//Signal Thr for accepting pulse as event
		int shape;	//Shaping time intervl
		int verb;	//Verbosity level
		bool debug;	//Debuggin option
		bool graph;	//Shape analysis option

		int mc;		//Multipurpose counter

		std::stringstream ssName;	//Histogram name
		std::fstream fout;
		TFile *oldF, *newF;
		TTree *TreeData;
		TH1F *hSNtot, *hSignl, *hNoise, *hPulse, *hCharg, *hRWMon, *hPtot, *hCtot, *hDouble;
		TH1I *hEvent;
		TH2F *hEner, *hTime, *hDark;
		std::vector<EventReco*> vER;
		TNtuple *nEvent;
		TGraph *gReco, *gMean, *giRWM, *goRWM;
		TLine *lThr1, *lThr2;

		std::string Dir;		//Inside vectors and vars
		std::vector<Coord> vPos; 
//		std::vector<double> vH;
		std::vector<double> vI, vT, vEvt;
		std::vector<int> vID, vBin, vEntry;
		std::map<int, double> mI, mT, mDR;
		std::map<int, Coord> mPos;
		int RWM;

		std::string ConfigFile;		//Utilities
};

#endif
