#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

//#include "PMTData.h"
#include "Utils.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TChain.h"
#include "TDirectory.h"

int main(int argc, char **argv)
{
	std::vector<std::string> RF_In;
	std::stringstream ssL;
	for (int i = 1; i < argc; i++)
	{
		ssL.str("");
		ssL.clear();
		ssL << argv[i];
		RF_In.push_back(ssL.str());
	}

	Utils *Utl = Utils::GetUtils(); 
	Utl->SetDir(".");
	Utl->SetConfig("config");

	ssL.str("");
	ssL.clear();
	ssL << "SuperMerged.root";
	std::cout << "Result in " << ssL.str() << std::endl;

	TFile *rootF, *outF = new TFile(ssL.str().c_str(), "RECREATE");

	//New objects
	double xrange = Utl->GetBuffer()*Utl->GetBinWidth();
	int nbins = int(0.1*Utl->GetBuffer());
	TH1F *sPfile = new TH1F("spfile", "pulses in file", nbins, 0, xrange);	
	TH1F *sEvent = new TH1F("sevent", "events freq", nbins/20, 0, xrange);
	TH1F *sEvenT = new TH1F("sevenT", "events time", nbins/10, 0, xrange);
	TH1F *sEntry = new TH1F("sentry", "entries freq", nbins/20, 0, xrange);
	TH1F *sBinWd = new TH1F("sbinwd", "event width", 40, 0, 8);

//	TNtuple *tEvent = new TNtuple("nevent", "event data", "baseline:peak:p2v:charge:energy:tcfd:zeroc:tof");

	TH1F *sBaseLine = new TH1F("sbaseline", "Baseline", 250, -0.005, 0.005);
	TH1F *sPeak = new TH1F("speak", "Peak", 500, 0, 5);
	TH1F *sValley = new TH1F("svalley", "Valley from peak", 200, 0, 1);
	TH1F *sTime = new TH1F("stime", "Time", nbins, 0, xrange);
	TH1F *sWidth = new TH1F("swidth", "Pulse width", 500, -0.1, 0.9);
	TH1F *sCharge = new TH1F("scharge", "Charge", 500, -0.005, 0.075);
	TH1F *sEnergy = new TH1F("senergy", "Energy", 500, -0.005, 0.095);
	TH1F *sArea = new TH1F("sarea", "Area", 500, 0, 0.1);
	TH1F *sTOF = new TH1F("stof", "Time of flight", nbins, -xrange, xrange);
	TH1F *sNext = new TH1F("snext", "Time of flight", nbins, 0, xrange);
//	TH1I *tCard21 = new TH1I("tcard21", "Card 21 on event", 4, 0, 4);

	TH2F *s2Dark = new TH2F("s2dark", "dark noise per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);
	TH2F *s2EvRa = new TH2F("s2evra", "event rate per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);
	TH2F *s2EtDR = new TH2F("st2etdr", "event to noise ratio", 8, -0.5, 7.5, 8, -0.5, 7.5);

	TGraph *sMean = new TGraph(Utl->GetEvtLength());
	TGraph *siTOF = new TGraph(Utl->GetEvtLength());
	TGraph *soTOF = new TGraph(Utl->GetEvtLength());
	TGraph *sNois = new TGraph(Utl->GetEvtLength());

	//Old objects to catch
	TH1F *hPfile;
	TH1F *hEvent;
	TH1F *hEvenT;
	TH1F *hEntry;
	TH1F *hBinWd;
	TH1F *hBaseLine;
	TH1F *hPeak;
	TH1F *hValley;
	TH1F *hTime;
	TH1F *hWidth;
	TH1F *hCharge;
	TH1F *hEnergy;
	TH1F *hArea;
	TH1F *hTOF;
	TH1F *hNext;
//	TH1I *hCard21;
	TH2F *h2Dark;
	TH2F *h2EvRa;
	TH2F *h2EtDR;
	TGraph *gMean;
	TGraph *giTOF;
	TGraph *goTOF;
	TGraph *gNois;

	TChain *sDaisE = new TChain("tEvent");
	TChain *sDaisN = new TChain("tNoise");

	for (int v = 0; v < RF_In.size(); v++)
	{
		Utl->OpenIns(v, RF_In.at(v));
		std::cout << "Opening " << Utl->InFile->GetName() << std::endl;

		std::string TreeName = std::string(Utl->InFile->GetName()) + "/tEvent";
		sDaisE->Add(TreeName.c_str());
		TreeName = std::string(Utl->InFile->GetName()) + "/tNoise";
		sDaisN->Add(TreeName.c_str());

		hBaseLine = (TH1F*) Utl->InFile->Get("tbaseline");
		hPeak = (TH1F*) Utl->InFile->Get("tpeak");
		hValley = (TH1F*) Utl->InFile->Get("tvalley");
		hTime = (TH1F*) Utl->InFile->Get("ttime");
		hWidth = (TH1F*) Utl->InFile->Get("twidth");
		hCharge = (TH1F*) Utl->InFile->Get("tcharge");
		hEnergy = (TH1F*) Utl->InFile->Get("tenergy");
		hArea = (TH1F*) Utl->InFile->Get("tarea");
		hTOF = (TH1F*) Utl->InFile->Get("ttof");
		hNext = (TH1F*) Utl->InFile->Get("tnext");
//		hCard21 = (TH1I*) Utl->InFile->Get("hcard21");
		hPfile = (TH1F*) Utl->InFile->Get("tpfile");
		hEvent = (TH1F*) Utl->InFile->Get("tevent");
		hEvenT = (TH1F*) Utl->InFile->Get("tevenT");
		hEntry = (TH1F*) Utl->InFile->Get("tentry");
		hBinWd = (TH1F*) Utl->InFile->Get("tbinwd");
		h2Dark = (TH2F*) Utl->InFile->Get("t2dark");
		h2EvRa = (TH2F*) Utl->InFile->Get("t2evra");
		h2EtDR = (TH2F*) Utl->InFile->Get("t2etdr");

		gMean = (TGraph*) Utl->InFile->Get("tmean");
		giTOF = (TGraph*) Utl->InFile->Get("titof");
		goTOF = (TGraph*) Utl->InFile->Get("totof");
		gNois = (TGraph*) Utl->InFile->Get("tnois");

		/*
		double x, y, y0;
		for (int i = 0; i < Utl->GetEvtLength(); i++)
		{
			sMean->GetPoint(i, x, y);
			gMean->GetPoint(i, x, y0);
			sMean->SetPoint(i, x, y+y0/RF_In.size());

			siTOF->GetPoint(i, x, y);
			giTOF->GetPoint(i, x, y0);
			siTOF->SetPoint(i, x, y+y0/RF_In.size());

			soTOF->GetPoint(i, x, y);
			goTOF->GetPoint(i, x, y0);
			soTOF->SetPoint(i, x, y+y0/RF_In.size());

			sNois->GetPoint(i, x, y);
			gNois->GetPoint(i, x, y0);
			sNois->SetPoint(i, x, y+y0/RF_In.size());
		}
		*/
		
		double scale = 1.0;

		sPfile->Add(hPfile, scale);
		sEvent->Add(hEvent, scale);
		sEvenT->Add(hEvenT, scale);
		sEntry->Add(hEntry, scale);
		sBinWd->Add(hBinWd, scale);
		sBaseLine->Add(hBaseLine, scale);
		sPeak->Add(hPeak, scale);
		sValley->Add(hValley, scale);
		sTime->Add(hTime, scale);
		sWidth->Add(hWidth, scale);
		sCharge->Add(hCharge, scale);
		sEnergy->Add(hEnergy, scale);
		sArea->Add(hArea, scale);
		sTOF->Add(hTOF, scale);
		sNext->Add(hNext, scale);
//		tCard21->Add(hCard21, scale);
		s2Dark->Add(h2Dark, 1.0/RF_In.size());
		s2EvRa->Add(h2EvRa, 1.0/RF_In.size());
		s2EtDR->Add(h2EtDR, 1.0/RF_In.size());
	}

	Coord Pos;
	double dD, dE;
	for (int ID = 1; ID < 61; ID++)
	{
		Pos = Utl->Mapper(ID);
		dD = s2Dark->GetBinContent(Pos.x+1, Pos.z+1);
		dE = s2EvRa->GetBinContent(Pos.x+1, Pos.z+1);
		s2EtDR->SetBinContent(Pos.x+1, Pos.z+1, dE/dD);
	}

//Write

	
	outF->cd();
	//Histograms
	sPfile->SetStats(kFALSE);
	sEvent->SetStats(kTRUE);
	sEvenT->SetStats(kTRUE);
	sEntry->SetStats(kTRUE);
	sBinWd->SetStats(kTRUE);
	s2Dark->SetStats(kFALSE);
	s2Dark->SetContour(40);
	s2Dark->SetOption("COLZ10TEXT");
	s2EvRa->SetStats(kFALSE);
	s2EvRa->SetContour(40);
	s2EvRa->SetOption("COLZ10TEXT");
	s2EtDR->SetStats(kFALSE);
	s2EtDR->SetContour(40);
	s2EtDR->SetOption("COLZ10TEXT");

	sPfile->GetXaxis()->SetTitle("time");
	sEvent->GetXaxis()->SetTitle("pmt fired");
	sEvenT->GetXaxis()->SetTitle("pmt fired");
	sEntry->GetXaxis()->SetTitle("entries");
	sBinWd->GetXaxis()->SetTitle("width");
	s2Dark->GetXaxis()->SetTitle("x");
	s2Dark->GetYaxis()->SetTitle("z");
	s2EvRa->GetXaxis()->SetTitle("x");
	s2EvRa->GetYaxis()->SetTitle("z");
	s2EtDR->GetXaxis()->SetTitle("x");
	s2EtDR->GetYaxis()->SetTitle("z");
	sMean->GetXaxis()->SetTitle("time");
	siTOF->GetXaxis()->SetTitle("time");
	soTOF->GetXaxis()->SetTitle("time");
	sNois->GetXaxis()->SetTitle("time");
	sMean->GetYaxis()->SetTitle("amplitude");
	siTOF->GetYaxis()->SetTitle("amplitude");
	soTOF->GetYaxis()->SetTitle("amplitude");
	sNois->GetYaxis()->SetTitle("amplitude");

	sMean->SetTitle("Average of signals");
	siTOF->SetTitle("In coincidence");
	soTOF->SetTitle("Out coincidence");
	sNois->SetTitle("Average of noise");

//	sTreeEvent->Write();
	sBaseLine->Write();
	sPeak->Write();
	sValley->Write();
	sTime->Write();
	sWidth->Write();
	sCharge->Write();
	sEnergy->Write();
	sArea->Write();
	sTOF->Write();
	sNext->Write();
//	sCard21->Write();

	sPfile->Write();
	sEvent->Write();
	sEvenT->Write();
	sEntry->Write();
	sBinWd->Write();
	s2Dark->Write();
	s2EvRa->Write();
	s2EtDR->Write();
	sMean->Write("tMean");
	siTOF->Write("tiTOF");
	soTOF->Write("toTOF");
	sNois->Write("tNois");

	outF->cd();
	sDaisE->Merge(outF, 1111, "keep");
	sDaisN->Merge(outF, 1111);

//	outF->Close();
	Utl->InFile->Close();

	return 0;
}
