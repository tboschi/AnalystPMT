/*
 * Data Analyst for ANNIE PMTs root file
 *
 * Author: Tommaso Boschi
 */

#ifndef utils_H
#define utils_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>

#include "PMTData.h"
//#include "EventReco.h"

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


//Useful structures
struct Coord
{
	int x;
	int z;
};

struct Analysis
{
	double E;	//Energy
	double T;	//Time
	double P;	//Peak
	double W;	//Width

//	int ID;		//Pulse ID
};


class Utils
{
	public:
		//Return single instance of this class
		static Utils *GetUtils();

		void GetTree();
		PMTData *GetPMT();

		void SetDir(std::string dir);
		bool OpenIns(std::string fname);
		bool OpenOuts(std::string fname);
		void SetConfig(std::string cfn);
		void Close();
//		void SetHistOpt();

		//Math stuff
		int LocMaximum(float *data, int j, double &p);
		double Integrate(float *data, int j, double bw);
		double CI(float *data, int x2, double fmax, double tau);
		Coord Mapper(int ID);

		//Return functions for parameters
		bool GetDebug();
		int GetVerbosity();
		double GetHysRatio();
		double GetThrPeak();
		int GetThrEvent();
		double GetThrSignal();
		int GetShapingTime();
		int GetSample();
		double GetBinWidth();
		int GetBuffer();
		int GetPrintGraph();
		double GetPercent();
		int GetEvtLength();
		double GetLowB();
		double GetUpB();
		unsigned int GetNumTrg();

		void SetDebug(bool emp);
		void SetVerbosity(int emp);
		void SetHysRatio(double emp);
		void SetThrPeak(double emp);
		void SetThrEvent(int emp);
		void SetThrSignal(double emp);
		void SetShapingTime(int emp);
		void SetSample(int emp);
		void SetBinWidth(double emp);
		void SetBuffer(int emp);
		void SetPrintGraph(int emp);
		void SetPercent(double emp);
		void SetEvtLength(int emp);
		void SetLowB(double emp);
		void SetUpB(double emp);
		void SetNumTrg(int emp);

		//Max and Min finder in vectors or maps
		template<typename T> T MaxV(std::vector<T> &V)
		{
			T ret = V.at(0);
			for (int i = 1; i < V.size(); i++)
				if (V.at(i) > ret)
					ret = V.at(i);
			return ret;
		}
		template<typename T> T MinV(std::vector<T> &V)
		{
			T ret = V.at(0);
			for (int i = 1; i < V.size(); i++)
				if (V.at(i) < ret)
					ret = V.at(i);
			return ret;
		}
		template<typename K, typename V> V MaxM(std::map<K, V> const &M)
		{	
			typename std::map<K, V>::const_iterator iM = M.begin();
			V ret = iM->second;
			K max = iM->first;
		
			for ( ; iM != M.end(); iM++)
				if (iM->second > ret)
				{
					ret = iM->second;
					max = iM->first;
				}
			return ret;
		}
		template<typename K, typename V> V MinM(std::map<K, V> const &M)
		{
			typename std::map<K, V>::const_iterator iM = M.begin();
			V ret = iM->second;
			K min = iM->first;
		
			for ( ; iM != M.end(); iM++)
				if (iM->second < ret)
				{
					ret = iM->second;
					min = iM->first;
				}
			return ret;
		}

		//Handler for in and out root file
		TFile *InFile, *OutFile;

	private:

		//Singleton
		Utils(){};
		Utils(Utils const&){};
		Utils &operator = (Utils const&){};
		static Utils *in_Utils;

		//Private funcs
		std::string GetBaseName(std::string fn);
		bool Exist(std::string &fileName);

		//Private paramters
/*
		static double fThrPeak;	//Peak thr for pulse
		static double fHysRatio;	//Hysteresis for thershold
		static int iThrEvent;		//On PMT thr for event
		static int iThrSignal;		//Signal Thr for accepting pulse as event
		static int iShape;		//Shaping time intervl
		static int iVerbosity;		//Verbosity level
		static bool bDebug;		//Debuggin option
		static bool bGraph;		//Shape analysis option

		static double fBinWidth;	//Bin width
		static double fPercent;	//Peak position, in percentage of Event length
		static int iBuffer;		//Event length
		static int iSample;		//Event length
		static int iEvtLength;	//Event length

		static unsigned int iNumTrg;	//Num of triggers in file
*/
		double fThrPeak;	//Peak thr for pulse
		double fHysRatio;	//Hysteresis for thershold
		int iThrEvent;		//On PMT thr for event
		int fThrSignal;		//Signal Thr for accepting pulse as event
		int iShape;		//Shaping time intervl
		int iVerbosity;		//Verbosity level
		bool bDebug;		//Debuggin option
		int iGraph;		//Graph printing frequency

		double fBinWidth;	//Bin width
		double fPercent;	//Peak position, in percentage of Event length
		int iBuffer;		//Event length
		int iSample;		//Event length
		int iEvtLength;		//Event length
		double fLowB;		//Lower bound for TOF
		double fUpB;		//Upper bound for TOF

		unsigned int iNumTrg;	//Num of triggers in file
	
		PMTData* PMT;

		//Other variables
		std::string DirName;
		std::ofstream fout;
};

#endif
