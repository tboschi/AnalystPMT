#include "PulseFinder.h"

void PulseFinder::SaveGraph()
{
	newF->cd("Event");
/*
	for (int i = 0; i < vER.size(); i++)
	{

		vER.at(i)->LoadGraph();
		vER.at(i)->LoadN();
		if (verb > 2)
			vER.at(i)->Print();
		vER.at(i)->FillN(nEvent);
	}
*/
	nEvent->Write();

	bool con = concurr >= 0;
	int cM = 0;
	int cI = 0;
	int cO = 0;

	gMean = new TGraph();
	if (con)
	{
		giRWM = new TGraph();
		goRWM = new TGraph();
	}
//	gDiff = new TGraph();

	for (int i = 0; i < vER.size(); i++)
	{
		ssName.str("");
		ssName.clear();
		ssName << "pulse_" << vER.at(i)->GetTRG();
		ssName << "_" << vER.at(i)->GetID();
		ssName << "_" << vER.at(i)->GetEVT();
		gReco = vER.at(i)->GetGraph();
		GraphAVG(gMean, cM);
		if (con)
		{
			if (fabs(vER.at(i)->GetTOF()) <= 1)
				GraphAVG(giRWM, cI);
			else GraphAVG(goRWM, cO);
		}

		gReco->GetXaxis()->SetTitle("time");
		gReco->GetYaxis()->SetTitle("Data");
		gReco->SetTitle("Pulse");

//		if (i % 50 == 0)
		{
			gReco->Write(ssName.str().c_str());
//			gDiff = vER.at(i)->GetDeriv();
			ssName << "_d";
//			gDiff->Write(ssName.str().c_str());
		}
		delete vER.at(i);
	}

	newF->cd();

	gMean->GetXaxis()->SetTitle("time");
	gMean->GetYaxis()->SetTitle("Data");
	gMean->SetTitle("Average pulse");
	gMean->SetLineColor(1);
	gMean->Write("gmean");

	if (con)
	{
		giRWM->GetXaxis()->SetTitle("time");
		giRWM->GetYaxis()->SetTitle("Data");
		giRWM->SetTitle("Coincidence pulse");
		giRWM->SetLineColor(2);
		giRWM->Write("girwm");

		goRWM->GetXaxis()->SetTitle("time");
		goRWM->GetYaxis()->SetTitle("Data");
		goRWM->SetTitle("Not Coincidence pulse");
		goRWM->SetLineColor(4);
		goRWM->Write("gorwm");
	}

	std::cout << "cM " << cM << std::endl;
	if (con)
	{
		std::cout << "cI " << cI << std::endl;
		std::cout << "cO " << cO << std::endl;
	}

	delete gMean;
	if (con)
	{
		delete giRWM;
		delete goRWM;
	}
//	delete gDiff;

}

void PulseFinder::GraphAVG(TGraph *g, int &cc)
{
	double x, y0, y;
	 ++cc;
	for (int i = 0; i < gReco->GetN(); i++)
	{
		gReco->GetPoint(i, x, y0);
		g->GetPoint(i, x, y);
		g->SetPoint(i, bw*i, ((cc-1)*y+y0)/cc);
		if (y0 > 1000) std::cout << cc << std::endl;
	}
}

