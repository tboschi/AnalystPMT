#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "PMTData.h"
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
	std::string fold = ".";
	if (argc > 1) fold = argv[1];
	std::stringstream ssL;
	std::string Line, Input;
	std::vector<std::string> RF_In;

	ssL.str("");
	ssL.clear();
	ssL << "find ";
	ssL << fold;
       	ssL << " -name \"r*root\" > rootfiles.list";
	std::cout << ssL.str() << std::endl;
	system(ssL.str().c_str());

	std::ifstream fin("rootfiles.list");
	while (getline(fin, Line))
	{
		ssL.str("");
		ssL.clear();
		ssL << Line;
		while (!ssL.eof())
		{
			ssL >> Input;
			RF_In.push_back(Input);
		}
	}
	fin.close();

	Utils *Utl = Utils::GetUtils(); 
	Utl->SetDir(fold);
	Utl->SetConfig("config");

	ssL.str("");
	ssL.clear();
	ssL << fold << "Merged.root";
	std::cout << "Result in " << ssL.str() << std::endl;

	TFile *rootF, *outF = new TFile(ssL.str().c_str(), "RECREATE");

	//New objects
	double xrange = Utl->GetBuffer()*Utl->GetBinWidth();
	int nbins = int(0.1*Utl->GetBuffer());
	TH1F *tPfile = new TH1F("tpfile", "pulses in file", nbins, 0, xrange);	
	TH1F *tEvent = new TH1F("tevent", "events freq", nbins/20, 0, xrange);
	TH1F *tEntry = new TH1F("tentry", "entries freq", nbins/20, 0, xrange);
	TH1F *tBinWd = new TH1F("tbinwd", "event width", 40, 0, 8);

//	TNtuple *tEvent = new TNtuple("nevent", "event data", "baseline:peak:p2v:charge:energy:tcfd:zeroc:tof");

	TH1F *tBaseLine = new TH1F("tbaseline", "Baseline", 250, -0.005, 0.005);
	TH1F *tPeak = new TH1F("tpeak", "Peak", 500, 0, 5);
	TH1F *tValley = new TH1F("tvalley", "Valley from peak", 200, 0, 1);
	TH1F *tTime = new TH1F("ttime", "Time", nbins, 0, xrange);
	TH1F *tWidth = new TH1F("twidth", "Pulse width", 500, -0.1, 0.9);
	TH1F *tCharge = new TH1F("tcharge", "Charge", 500, -0.005, 0.075);
	TH1F *tEnergy = new TH1F("tenergy", "Energy", 500, -0.005, 0.095);
	TH1F *tTOF = new TH1F("ttof", "Time of flight", nbins, -xrange, xrange);
	TH1F *tNext = new TH1F("tnext", "Time of flight", nbins, 0, xrange);
	TH1I *tCard21 = new TH1I("tcard21", "Card 21 on event", 4, 0, 4);

	TH2F *t2Dark = new TH2F("t2dark", "dark noise per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);
	TH2F *t2EvRa = new TH2F("t2evra", "event rate per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);

	TGraph *tMean = new TGraph(Utl->GetEvtLength());
	TGraph *tiTOF = new TGraph(Utl->GetEvtLength());
	TGraph *toTOF = new TGraph(Utl->GetEvtLength());

	//Old objects to catch
	TH1F *hPfile;
	TH1F *hEvent;
	TH1F *hEntry;
	TH1F *hBinWd;
	TH1F *hBaseLine;
	TH1F *hPeak;
	TH1F *hValley;
	TH1F *hTime;
	TH1F *hWidth;
	TH1F *hCharge;
	TH1F *hEnergy;
	TH1F *hTOF;
	TH1F *hNext;
	TH1I *hCard21;
	TH2F *h2Dark;
	TH2F *h2EvRa;
	TGraph *gMean;
	TGraph *giTOF;
	TGraph *goTOF;
	TTree *tTree;

	TDirectory * dir;
	TChain *tDaisy = new TChain("tEvent");

	for (int v = 0; v < RF_In.size(); v++)
	{
		Utl->OpenIns(RF_In.at(v));
		std::cout << "Opening " << Utl->InFile->GetName() << std::endl;

//Copy objects

//		dir = (TDirectory*) rootF->Get("Event");
//		dir->GetObject("nevent", nevent);
//		TList *tlist;

//		dir = (TDirectory*) Utl->InFile->Get("");
//		dir->GetObject("tevent", tevent);
//		tTree = (TTree*) Utl->InFile->Get("tevent");
		tDaisy->Add(Utl->InFile->GetName());

		hBaseLine = (TH1F*) Utl->InFile->Get("hbaseline");
		hPeak = (TH1F*) Utl->InFile->Get("hpeak");
		hValley = (TH1F*) Utl->InFile->Get("hvalley");
		hTime = (TH1F*) Utl->InFile->Get("htime");
		hWidth = (TH1F*) Utl->InFile->Get("hwidth");
		hCharge = (TH1F*) Utl->InFile->Get("hcharge");
		hEnergy = (TH1F*) Utl->InFile->Get("henergy");
		hTOF = (TH1F*) Utl->InFile->Get("htof");
		hNext = (TH1F*) Utl->InFile->Get("hnext");
		hCard21 = (TH1I*) Utl->InFile->Get("hcard21");
		hPfile = (TH1F*) Utl->InFile->Get("hpfile");
		hEvent = (TH1F*) Utl->InFile->Get("hevent");
		hEntry = (TH1F*) Utl->InFile->Get("hentry");
		hBinWd = (TH1F*) Utl->InFile->Get("hbinwd");
		h2Dark = (TH2F*) Utl->InFile->Get("h2dark");
		h2EvRa = (TH2F*) Utl->InFile->Get("h2evra");

		gMean = (TGraph*) Utl->InFile->Get("gmean");
		giTOF = (TGraph*) Utl->InFile->Get("gitof");
		goTOF = (TGraph*) Utl->InFile->Get("gotof");

		double x, y, y0;
		for (int i = 0; i < Utl->GetEvtLength(); i++)
		{
			tMean->GetPoint(i, x, y);
			gMean->GetPoint(i, x, y0);
			tMean->SetPoint(i, x, y+y0/RF_In.size());

			tiTOF->GetPoint(i, x, y);
			giTOF->GetPoint(i, x, y0);
			tiTOF->SetPoint(i, x, y+y0/RF_In.size());

			toTOF->GetPoint(i, x, y);
			goTOF->GetPoint(i, x, y0);
			toTOF->SetPoint(i, x, y+y0/RF_In.size());
		}

		double scale = 1.0;

		tPfile->Add(hPfile, scale);
		tEvent->Add(hEvent, scale);
		tEntry->Add(hEntry, scale);
		tBinWd->Add(hBinWd, scale);
		tBaseLine->Add(hBaseLine, scale);
		tPeak->Add(hPeak, scale);
		tValley->Add(hValley, scale);
		tTime->Add(hTime, scale);
		tWidth->Add(hWidth, scale);
		tCharge->Add(hCharge, scale);
		tEnergy->Add(hEnergy, scale);
		tTOF->Add(hTOF, scale);
		tNext->Add(hNext, scale);
		tCard21->Add(hCard21, scale);
		t2Dark->Add(h2Dark, 1.0/RF_In.size());
		t2EvRa->Add(h2EvRa, 1.0/RF_In.size());
	}

//Write


	//Tree
//	TTree *tTreeEvent = TTree::MergeTrees(tList);
//	tTreeEvent->SetName("tEvent");

	outF->cd();
	//Histograms
	tPfile->SetStats(kFALSE);
	tEvent->SetStats(kTRUE);
	tEntry->SetStats(kTRUE);
	tBinWd->SetStats(kTRUE);
	t2Dark->SetStats(kFALSE);
	t2Dark->SetContour(40);
	t2Dark->SetOption("COLZ10TEXT");
	t2EvRa->SetStats(kFALSE);
	t2EvRa->SetContour(40);
	t2EvRa->SetOption("COLZ10TEXT");



	tPfile->GetXaxis()->SetTitle("time");
	tEvent->GetXaxis()->SetTitle("pmt fired");
	tEntry->GetXaxis()->SetTitle("entries");
	tBinWd->GetXaxis()->SetTitle("width");
	t2Dark->GetXaxis()->SetTitle("x");
	t2Dark->GetYaxis()->SetTitle("z");
	t2EvRa->GetXaxis()->SetTitle("x");
	t2EvRa->GetYaxis()->SetTitle("z");
	tMean->GetXaxis()->SetTitle("time");
	tiTOF->GetXaxis()->SetTitle("time");
	toTOF->GetXaxis()->SetTitle("time");
	tMean->GetYaxis()->SetTitle("amplitude");
	tiTOF->GetYaxis()->SetTitle("amplitude");
	toTOF->GetYaxis()->SetTitle("amplitude");

	tMean->SetTitle("Average");
	tiTOF->SetTitle("In coincidence");
	toTOF->SetTitle("Out coincidence");

//	tTreeEvent->Write();

	tBaseLine->Write();
	tPeak->Write();
	tValley->Write();
	tTime->Write();
	tWidth->Write();
	tCharge->Write();
	tEnergy->Write();
	tTOF->Write();
	tNext->Write();
	tCard21->Write();

	tPfile->Write();
	tEvent->Write();
	tEntry->Write();
	tBinWd->Write();
	t2Dark->Write();
	t2EvRa->Write();
	tMean->Write("tMean");
	tiTOF->Write("tiTOF");
	toTOF->Write("toTOF");

	tDaisy->Merge(outF, 1000);

//	outF->Close();
	Utl->InFile->Close();

	return 0;
}
