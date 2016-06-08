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
		void LoadDeriv();
		void FillN(TNtuple *nT);
		void LoadN();
		void Print();
		TGraph *GetGraph();
		TGraph *GetDeriv();
		void SetBL();			//Set to zero the avg of the first 20 points
		void SetPK();			//Peak of pulse
		void SetVL();			//Valley of pulse
		void SetCH();			//Charge, sum of bin around peak
		void SetEN();			//Energy = integral from tcfd to zc
		void SetCFD();			//Start of pulse
		void SetZC();			//width of pulse = Zero Crossin
		void SetTOF();			//time of flight = zeroc-tcfd
		void SetBW(double binw);	//bin width
		void SetEL(int evnl);		//lenght of an event
		void SetWL(int evnl);		//time window lenght
		void SetSample(int evnl);	//Sample of digitiser
		void SetPC(double perc);	//Peak pos in percent
		double CI(double *data, int x2, double fmax, double tau);
		double GetBL();
		double GetPK();
		double GettPK();
		double GetVL();
		double GettVL();
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
		int GetWL();
		int GetSample();
		double GetPC();
		void SetConfig(std::string cfn);

	private:
		double Baseline, Peak, Valley, Charge, Energy, tCFD, ZeroC, TOF;
		int tPeak, tValley;
		int RWM, TRG, ID, EVT;
		double bw, pc;
		int el, wl, np, sample;

		TGraph *gPulse, *gDeriv;
		TNtuple *nPulse;
		double *Pulse;
};

#endif
