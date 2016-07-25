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

	Event tEva((TTree*)inF->Get("tEvent"));
	Event tNos((TTree*)inF->Get("tNoise"));

	std::string sCut;
	std::cout << "Cut: ";
	getline(std::cin, sCut);
	
	TCut mCut(sCut.c_str());

	std::string sOut;
	std::cout << "Save as: ";
	getline(std::cin, sOut);
	sOut += ".root";

	TFile *outF = new TFile(sOut.c_str(), "RECREATE");

	tEva.fChain->Draw(">>event1", mCut);
	tNos.fChain->Draw(">>event2", mCut);
	TEventList *Elist1 = (TEventList*) gDirectory->Get("event1");
	TEventList *Elist2 = (TEventList*) gDirectory->Get("event2");

	std::cout << "EL1 " << Elist1->GetN() << std::endl;
	std::cout << "EL2 " << Elist2->GetN() << std::endl;

	tEva.fChain->GetEntry(0);
	TGraph gEva(tEva.EvtLength);
	TGraph gemp(tEva.EvtLength);
	for (Long64_t i = 0; i < Elist1->GetN(); ++i)
	{
		tEva.fChain->GetEntry(Elist1->GetEntry(i));
		for (int j = 0; j < tEva.EvtLength; j++)
			gemp.SetPoint(j, j, tEva.Pulse[j]);
		GraphAVG(gEva, gemp, Elist1->GetN());
	}

	tNos.fChain->GetEntry(0);
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

	gEva.SetTitle("Event");
	gNos.SetTitle("Noise");
	gEva.SetLineColor(4);
	gNos.SetLineColor(2);

	outF->cd();
	gEva.Write("EventG");
	gNos.Write("NoiseG");

	Elist1->Delete();
	Elist2->Delete();
	inF->Delete();
	outF->Delete();

	return 0;
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
