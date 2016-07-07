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

#include "Utils.h"
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
//#include "TNtuple.h"


class EventReco
{
	public:
		EventReco(int ent, int bin, int rwm);
		EventReco(TGraph* iG);
		~EventReco();

		void Init(int length);
		TGraph *LoadGraph();
		TGraph *LoadDeriv();
//		void FillN(TNtuple *nT);
		void LoadParam();
		void Print();

		void SetBaseLine();		//Set to zero the avg of the first 20 points
		void SetPeak();			//Peak of pulse
		void SetValley();		//Valley of pulse
		void SetCharge();		//Charge, sum of bin around peak
		void SetEnergy();		//Energy = integral from tcfd to zc
		void SetArea();			//Energy = integral of the whole pulse
		void SettCFD();			//Start of pulse
		void SetZeroC();		//width of pulse = Zero Crossin
		void SetTOF();			//time of flight = zeroc-tcfd
		double GetBaseLine();
		double GetPeak();
		int GettPeak();
		double GetValley();
		int GettValley();
		double GettP2V();
		double GetCharge();
		double GetEnergy();
		double GetArea();
		double GettCFD();
		double GetZeroC();
		double GetTOF();
		int GetEntry();
//		int GetTRG();
//		int GetID();
//		int GetEVT();
//		double GetBW();
//		int GetEL();
//		int GetWL();
//		int GetSample();
//		double GetPC();
//		void SetConfig(std::string cfn);

	private:

		Utils *Utl;
		PMTData *PMT;

		double fBaseLine;	//AVG of first 20 points of graph
		double fPeak;		//Maximum of graph
		double fValley;		//Minimum of graph
		double fCharge;		//Sum of 5 points around peak
		double fEnergy;		//Area between tCFD and end of pulse
		double fArea;		//Integral of the whole pulse
		double ftCFD;		//Real start of pulse
		double fZeroC;		//Time between tCFD and end of pulse
		double fTOF;		//Diff between tCFD and reference (RWM)
		int iPeak;		//gloabl pos of Peak	//No Set function
		int iValley;		//local  pos of Valley	//No Set function

		double fBW, fPC;
		int iEL, iWL, iRWM;
		int NewP;
		int iEnt;

		TGraph *gPulse, *gDeriv;
//		TNtuple *nPulse;
		float *Pulse;

};

#endif
