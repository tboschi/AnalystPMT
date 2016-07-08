#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "Event.h"

#include "TApplication.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TCut.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TChain.h"
#include "TEventList.h"

void GraphAVG(TGraph &g, TGraph &p, Long64_t max);
int main(int argc, char **argv)
{
	TFile *inF = new TFile(argv[1]);

	TApplication theApp("theApp", &argc, argv); 
	TCanvas *c1 = new TCanvas("c1", "Average", 10, 10, 1000, 1000);

	/*
	std::string fold = ".";
	if (argc > 1) fold = argv[1];
	std::stringstream ssL;
	std::string Line, Input;
	std::vector<std::string> RF_In;

	ssL.str("");
	ssL.clear();
	ssL << "find ";
	ssL << fold;
       	ssL << " -name \"*merged*root\" > rootfiles.list";
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
	ssL << fold << "twograph.root";
	std::cout << "Result in " << ssL.str() << std::endl;
*/

//	std::cout << "d1" << std::endl;
//	TFile *outF = new TFile("twograph.root", "RECREATE");

	Event tEva((TTree*)inF->Get("tEvent"));
	Event tNos((TTree*)inF->Get("tNoise"));

	std::string sCut;
//	while (true)
//	{
///		gROOT->Reset();

		std::cout << "Cut: ";
		getline(std::cin, sCut);
//		if (sCut == "x") break;
		TCut mCut(sCut.c_str());

		tEva.fChain->Draw(">>event1", mCut);
		tNos.fChain->Draw(">>event2", mCut);
		TEventList *Elist1 = (TEventList*) gDirectory->Get("event1");
		TEventList *Elist2 = (TEventList*) gDirectory->Get("event2");

		std::cout << "EL1 " << Elist1->GetN() << std::endl;
		std::cout << "EL2 " << Elist2->GetN() << std::endl;

		tEva.fChain->SetEventList(Elist1);
		tEva.fChain->GetEntry(Elist2->GetEntry(0));
		TGraph gEva(tEva.EvtLength);
		TGraph gemp(tEva.EvtLength);
		for (Long64_t i = 0; i < Elist1->GetN(); ++i)
		{
			tEva.fChain->GetEntry(Elist1->GetEntry(i));
			for (int j = 0; j < tEva.EvtLength; j++)
				gemp.SetPoint(j, j, tEva.Pulse[j]);
			GraphAVG(gEva, gemp, Elist1->GetN());
		}
	
		tNos.fChain->SetEventList(Elist2);
		tNos.fChain->GetEntry(Elist2->GetEntry(0));
		TGraph gNos(tNos.EvtLength);
		for (Long64_t i = 0; i < Elist2->GetN(); ++i)
		{
			tNos.fChain->GetEntry(Elist2->GetEntry(i));
			for (int j = 0; j < tNos.EvtLength; j++)
				gemp.SetPoint(j, j, tNos.Pulse[j]);
			GraphAVG(gNos, gemp, Elist2->GetN());
		}
		double maxE = 0, maxN = 0, x, y;
		for (int i = 0; i < gEva.GetN(); i++)
		{
			gEva.GetPoint(i, x, y);
			if (y > maxE)
				maxE = y;
			gNos.GetPoint(i, x, y);
			if (y > maxN)
				maxN = y;
		}
	
		gEva.SetName("Event");
		gNos.SetName("Noise");
		gEva.SetLineColor(4);
		gNos.SetLineColor(2);
		c1->cd();
		if (maxN >= maxE)
		{
			gNos.Draw();
			gEva.Draw("SAME");
		}
		if (maxN < maxE)
		{
			gEva.Draw();
			gNos.Draw("SAME");
		}

		c1->Update();

		Elist1->Delete();
		Elist2->Delete();
//	}

	theApp.Run();
	/*
	std::cout << "d0" << std::endl;
	outF->cd();
	gEva.Write("gE");
	gNos.Write("gN");
	*/
}

void GraphAVG(TGraph &g, TGraph &p, Long64_t max)
{
	double x, y0, y;
	for (int i = 0; i < p.GetN(); ++i)
	{
		p.GetPoint(i, x, y0);
		g.GetPoint(i, x, y);
		g.SetPoint(i, i, y+y0/max);
	}
}

