#ifndef eventreco_H
#define eventreco_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>

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

class EventReco
{
	public:
		EventReco(std::string cfn, PMTData *PMT, int ent, int bin, int rwm, int trg, int id, int evt);
		void LoadGraph();
		void FillN(TNtuple *nT);
		void LoadN();
		void Print();
		TGraph *GetGraph();
		void SetBL();		//Set to zero the avg of the first 20 points
		void SetPK();
		void SetCH();
		void SetEN();
		void SetCFD();
		void SetZC();
		void SetTOF();
		void SetBW(double binw);
		void SetEL(int evnl);
		void SetPC(double perc);
		double CI(double *data, int x2, double fmax, double tau);
		void SetConfig(std::string cfn);
		double GetBL();
		double GetPK();
		double GettPK();
		double GetCH();
		double GetEN();
		double GetCFD();
		double GetZC();
		double GetTOF();
		int GetTRG();
		int GetID();
		int GetEVT();
		double GetBW();
		int GetEL();
		double GetPC();

	private:
		double Baseline, Peak, Charge, Energy, tCFD, ZeroC, TOF;
		int tPeak;
		int RWM, TRG, ID, EVT;
		double bw, pc;
		int el, np;

		TGraph *gPulse;
		TNtuple *nPulse;
		double *Pulse;
};

#endif