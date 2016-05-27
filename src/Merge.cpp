#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGraph.h"

bool Graph(std::string cfn);
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

	ssL.str("");
	ssL.clear();
	ssL << fold << "Merged.root";
	std::cout << "Result in " << ssL.str() << std::endl;

	TFile *rootF, *outF = new TFile(ssL.str().c_str(), "RECREATE");
	TH1F *hSNtot, *hSignl, *hNoise, *hPtot, *hCtot;
	TH1I *hEvent;
	TH2F *hDarkN;
	TGraph *gMean, *giRWM, *goRWM;

	TH1F *TPtot = new TH1F("tptot", "time distribution", 4000, 0, 80);
	TH1F *TCtot = new TH1F("tctot", "charge distribution", 4000, 0, 80);
	TH1F *TSNtot = new TH1F("tsntot", "signal and noise", 5000, 0, 10);
	TH1F *TSignl = new TH1F("tsignl", "signal", 5000, 0, 10);
	TH1F *TNoise = new TH1F("tnoise", "dark noise", 5000, 0, 10);
	TH1I *TEvent = new TH1I("tevent", "event", 80, 0, 80);
	TH2F *TDarkN = new TH2F("tdarkn", "dark noise per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);

	TGraph *TGMean = new TGraph();
	TGraph *TGiRWM = new TGraph();
	TGraph *TGoRWM = new TGraph();


	for (int v = 0; v < RF_In.size(); v++)
	{
		rootF = new TFile(RF_In.at(v).c_str(), "UPDATE");	//open files
		if (rootF->IsZombie()) 
		{
			std::cout << "ZOMBIE!\n";
			rootF->Close();
			continue;
		}
		else std::cout << "Opening " << RF_In.at(v) << std::endl;

//Copy objects

		rootF->cd();
	
		hPtot = (TH1F*) rootF->Get("hptot"); 
		hCtot = (TH1F*) rootF->Get("hctot"); 
		hSNtot = (TH1F*) rootF->Get("hsntot"); 
		hSignl = (TH1F*) rootF->Get("hsignl"); 
		hNoise = (TH1F*) rootF->Get("hnoise"); 
		hEvent = (TH1I*) rootF->Get("hevent"); 
		hDarkN = (TH2F*) rootF->Get("hdark"); 

		if (Graph)
		{
			std::cout << "Graph mode on " << std::endl;
			gMean = (TGraph*) rootF->Get("gmean");
			giRWM = (TGraph*) rootF->Get("girwm");
			goRWM = (TGraph*) rootF->Get("gorwm");

			double x, y, y0;
			std::cout << "GetN " << gMean->GetN() << std::endl;
			for (int i = 0; i < gMean->GetN(); i++)
			{
				TGMean->GetPoint(i, x, y);
				gMean->GetPoint(i, x, y0);
				TGMean->SetPoint(i, x, y+y0/RF_In.size());
	
				TGiRWM->GetPoint(i, x, y);
				giRWM->GetPoint(i, x, y0);
				TGiRWM->SetPoint(i, x, y+y0/RF_In.size());
	
				TGoRWM->GetPoint(i, x, y);
				goRWM->GetPoint(i, x, y0);
				TGoRWM->SetPoint(i, x, y+y0/RF_In.size());
			}
		}

		TPtot->Add(hPtot);
		TCtot->Add(hCtot);
		TSNtot->Add(hSNtot);
		TSignl->Add(hSignl);
		TNoise->Add(hNoise);
		TEvent->Add(hEvent);
		TDarkN->Add(hDarkN, 1.0/RF_In.size());

	}

//Write

	outF->cd();

	//Histograms
	TPtot->GetXaxis()->SetTitle("time");
	TCtot->GetXaxis()->SetTitle("time");
	TSNtot->GetXaxis()->SetTitle("amplitude");
	TSignl->GetXaxis()->SetTitle("amplitude");
	TNoise->GetXaxis()->SetTitle("amplitude");
	TEvent->GetXaxis()->SetTitle("pmt fired");
	TDarkN->GetXaxis()->SetTitle("x");
	TDarkN->GetYaxis()->SetTitle("z");
	TSNtot->SetLineColor(1);
	TSignl->SetLineColor(2);
	TNoise->SetLineColor(4);
	TPtot->SetStats(kFALSE);
	TCtot->SetStats(kFALSE);
	TSNtot->SetStats(kFALSE);
	TSignl->SetStats(kFALSE);
	TNoise->SetStats(kFALSE);
	TEvent->SetStats(kFALSE);
	TDarkN->SetStats(kFALSE);
	TSNtot->Rebin(10);
	TSignl->Rebin(10);
	TNoise->Rebin(10);
	TDarkN->SetContour(40);
	TDarkN->SetOption("COLZTEXT");

	//Graphs
	TGMean->GetXaxis()->SetTitle("time");
	TGMean->GetYaxis()->SetTitle("Data");
	TGMean->SetTitle("Average pulse");
	TGMean->SetLineColor(1);

	TGiRWM->GetXaxis()->SetTitle("time");
	TGiRWM->GetYaxis()->SetTitle("Data");
	TGiRWM->SetTitle("Coincidence pulse");
	TGiRWM->SetLineColor(2);

	TGoRWM->GetXaxis()->SetTitle("time");
	TGoRWM->GetYaxis()->SetTitle("Data");
	TGoRWM->SetTitle("Not Coincidence pulse");
	TGoRWM->SetLineColor(4);

	outF->Write();
	TGMean->Write("tgmean");
	TGiRWM->Write("tgirwm");
	TGoRWM->Write("tgorwm");

	std::cout << std::endl;

	outF->Close();
	rootF->Close();

	delete TGMean;

	return 0;
}

bool SetConfig(std::string cfn)
{
	std::ifstream fin(cfn.c_str());
	std::string Line, var;
	bool val;
	std::stringstream ssL;
	while (getline(fin, Line))
	{
		if (Line[0] == '#') continue;
		else
		{
			ssL.str("");
			ssL.clear();
			ssL << Line;
			ssL >> var >> val;
			if (var == "graph") return val;
		}
	}
	fin.close();
}

