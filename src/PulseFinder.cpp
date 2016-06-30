#include "PulseFinder.h"


PulseFinder::PulseFinder()
{
	Utl = Utl->GetUtils();

	iVerb = Utl->GetVerbosity();
	fBW = Utl->GetBinWidth();
		
	PMT = Utl->GetPMT();

	LoopTrg();
/*
	Dir = dir;
	if (iVerb > 3) std::cout << "Dir " << Dir << std::endl;
	std::size_t fnd = Dir.find('/');
	if (Dir.at(Dir.length()-1) == '/')
		Dir.erase(Dir.end()-1, Dir.end());
	if (iVerb > 3) std::cout << "Dir " << Dir << std::endl;
*/
}

PulseFinder::~PulseFinder()
{
	CleanAll();
	Utl->Close();
//	delete PMT;
}

//Loop over the triggers, divide them by the number of samples
void PulseFinder::LoopTrg()
{
	Utl->OutFile->cd();

	//Counters
	int mc = 0;
	int cM = 0, cI = 0, cO = 0, GC = 0;

	//Compute entries
	PMT->GetEntry(PMT->fChain->GetEntriesFast()-1);
	Utl->SetNumTrg(int(PMT->Trigger/Utl->GetSample()));
	std::cout << "Number of entries: " << PMT->fChain->GetEntriesFast() << std::endl;
	std::cout << "Number of triggers: " << Utl->GetSample()*Utl->GetNumTrg() << std::endl;

	//Creates histograms
	NewHist();

	int Trg0;
	int TrgLabel;
	for (unsigned int i = 0; i < Utl->GetNumTrg(); ++i)	//Loop on full buffers
	{
		for (int k = 0; k < Utl->GetSample(); ++k)	//Loop on sub buffers (trigger)
		{
			CleanER();

			//Change entry
			Trg0 = 256*i+k;			//Entry
			PMT->GetEntry(Trg0);	
			TrgLabel = PMT->Trigger;	//Trigger

			if (iVerb)
				std::cout << "Trigger " << TrgLabel << std::endl;

			//Loop over all PMT IDs, take care of clearing mPulse
			LoopPMT(Trg0);
			FindEvents(TrgLabel);

			if (iVerb)
				std::cout << "Event size " << vEvt.size() << std::endl;

			LoopEvents(TrgLabel);
      		}
	}
	Save_Hist();
//	Stat_Hist();

	std::cout << "Multipurpose counter for double pulse counting: " << mc << std::endl;
}

//Loop over all PMT IDs, take care of clearing mPulse
void PulseFinder::LoopPMT(int trg)
{
	vVETO.clear();
	vMRD2.clear();
	vMRD4.clear();
	RWM = 0;

	double peak, y, y_;
	int time;

	PMT->GetEntry(249+256*(trg/256));	//Catch the RWM
	for (; RWM < Utl->GetBuffer(); ++RWM)
		if (PMT->Data[RWM] >= 0.5) break;
	RWM %= Utl->GetBuffer();
	if (iVerb > 2)
		std::cout << "RWM " << RWM << std::endl;

	for (int j = 0; j < 256; j += Utl->GetSample())
	{
		//Change entry
		PMT->GetEntry(trg+j);
		if (iVerb > 2)
		{
			std::cout << "Entry " << trg+j << "\t";
			std::cout << "PMTID " << PMT->PMTID << std::endl;
		}
		if (PMT->CardID < 21) FindPulses(trg+j);
		else if (PMT->Channel != 2)
		{
			for (int k = 0; k < Utl->GetBuffer(); ++k)		//k is bin position
			{
				time = 0;
				peak = 0;

				y = PMT->Data[k];
				if (k == 0) y_ = y;
				else y_ = PMT->Data[k-1];
		
				if ((y_ < Utl->GetThrPeak()) && (y > Utl->GetThrPeak()))	//There is a pulse!
				{
					time = Utl->LocMaximum(PMT->Data, k, peak);	//Rough estimate of peak time

					if (PMT->Channel == 0)
						vVETO.push_back(time*fBW);
					if (PMT->Channel == 1)
						vMRD2.push_back(time*fBW);
					if (PMT->Channel == 3)
						vMRD4.push_back(time*fBW);

					if (time >= k)
						k = time + Utl->GetShapingTime();	//FFW must be validated
				}
			}
		}
	}
}

void PulseFinder::FindPulses(int ent)
{
	double peak = 0, y = 0, y_ = 0;
	double thr = Utl->GetThrPeak();
	int time = 0;

	for (int k = 0; k < Utl->GetBuffer(); ++k)		//k is bin position
	{
		y = PMT->Data[k];
		if (k == 0) y_ = y;
		else y_ = PMT->Data[k-1];

		if ((y_ > thr) && (y < thr))
			thr = Utl->GetThrPeak()*(1 + (Utl->GetHysRatio() > 0 ? 1.0/Utl->GetHysRatio() : 0));

		if ((y_ < thr) && (y > thr))	//There is a pulse! k is peak bin position
		{
			thr = Utl->GetThrPeak()*(1 - (Utl->GetHysRatio() > 0 ? 1.0/Utl->GetHysRatio() : 0));

			time = Utl->LocMaximum(PMT->Data, k, peak);	//Rough estimate of peak time
//			mPulseT[PMT->PMTID].push_back(fBW*time);		//and save it to map

			//Change entry
			ER = new EventReco(ent, time, RWM);		//Create EventReco object
			mPulseER[PMT->PMTID].push_back(ER);

			if (iVerb > 1)
			{
				std::cout << "Pulse ID " << PMT->PMTID << " @\t";
		       		std::cout << k << " " << time << "\t-> " << time*fBW << std::endl;
			}
			if (iVerb > 2)			
			{
				std::cout << "y_   " << y_ << "\t";
				std::cout << "y    " << y << "\t";
				std::cout << "time " << time << "\t";
				std::cout << "peak " << peak << "\t";
				std::cout << "ID   " << PMT->PMTID << "\n";
			}

//			hCharg->Fill(bw*time, ener);

//			hPulse->Fill(time*fBW);	//Fill hist with times
//			if (hPulse->GetBinContent(time/10.0) > 1) ++mc;

			hPtrig->Fill(time*fBW);
			hPfile->Fill(time*fBW);

			if (time >= k)
				k = time + Utl->GetShapingTime();	//FFW must be validated
		}
	}

//	hPfile->Add(hPulse);

//	hPulse->Reset();
}

void PulseFinder::FindEvents(int trg)
{
	if (iVerb)
		std::cout << "Looking for events...\n";
	int b, b_, tBIN = 0, bc = 0;
	double thr = Utl->GetThrEvent();
	double tAVG = 0, tPOS = 0;
	vEvt.clear();

	for (int i = 0; i < 0.01*Utl->GetBuffer(); ++i)
	{
		b = hPtrig->GetBinContent(i);		//current bin
		if (i == 0) b_ = b;
		else b_ = hPtrig->GetBinContent(i-1);	//previous bin

		if ((b_ > thr) && (b < thr))
		{
			thr = Utl->GetThrEvent();
			vEvt.push_back(tAVG/tBIN);	//Bin position
			hEvent->Fill(tAVG/tPOS);	//Bin content
			hBinWd->Fill(bc*hPtrig->GetXaxis()->GetBinWidth(i));
			if (iVerb > 3)
			{
				std::cout << "yBin " << i << " w/ ";
				std::cout << tAVG/tPOS << "\t@ ";
				std::cout << vEvt.at(vEvt.size()-1) << std::endl;
				std::cout << "bc " << bc*hPtrig->GetXaxis()->GetBinWidth(i) << std::endl;
			}

//			if (b > Utl->GetThrEvent()/2.0)
			tAVG = 0;
			tPOS = 0;
			tBIN = 0;
			bc = 0;
		}

		if (b > thr)
		{
			thr = 0.5;	//0 or 1? To be defined better
			tAVG += b*hPtrig->GetXaxis()->GetBinCenter(i);
			tPOS += hPtrig->GetXaxis()->GetBinCenter(i);
			tBIN += b;
			if (iVerb > 3)
			{
				std::cout << "nBin " << i << " w/ ";
				std::cout << b << "\t@ ";
				std::cout << hPtrig->GetXaxis()->GetBinCenter(i) << std::endl;
			}	
			++bc;
		}
	}

	hEntry->Fill(hPtrig->GetEntries());
}

//Fill events and selec signal from noise
void PulseFinder::LoopEvents(int trg)
{
	if (iVerb)
		std::cout << "Looping over the events...\n";

	std::vector<EventReco*>::iterator iE;
	int PG = Utl->GetPrintGraph();

	if (iVerb > 2)
		std::cout << "Map size " << mPulseER.size() << std::endl;

	for (int ID = 1; ID < 61; ++ID)
	{
		iE = mPulseER[ID].begin();	//Loop over pulseER
		for ( ; iE != mPulseER[ID].end(); ++iE, ++GC)	//Loop on pulses
		{
			ER = *(iE);		//Get pointer from EventReco
			ER->LoadParam();

			if (vEvt.size() == 0)
			{
				mLength[ID] += fBW*Utl->GetBuffer();
				++mDR[ID];
			}
			else for (int k = 0; k < vEvt.size(); ++k)
			{
				if (iVerb)
					std::cout << "Looping over event " << k << std::endl;

				mLength[ID] += fBW*Utl->GetBuffer();
				++mER[ID];

				if (fabs(ER->GettPeak()*fBW - vEvt.at(k)) <= Utl->GetThrSignal())
				{
					mLength[ID] -= Utl->GetThrSignal();	//Reduce time window

					bVETO = 0;
					for (int i = 0; i < vVETO.size(); ++i)
					{
						if (fabs(ER->GettPeak()*fBW - vVETO.at(i)) <= Utl->GetThrSignal())
						{
							bVETO = 1;
							break;
						}
						else hCard21->Fill(0);
					}
	
					bMRD2 = 0;
					for (int i = 0; i < vMRD2.size(); ++i)
					{
						if (fabs(ER->GettPeak()*fBW - vMRD2.at(i)) <= Utl->GetThrSignal())
						{
							bMRD2 = 1;
							break;
						}
						else hCard21->Fill(1);
					}
	
					bMRD4 = 0;
					for (int i = 0; i < vMRD4.size(); ++i)
					{
						if (fabs(ER->GettPeak()*fBW - vMRD4.at(i)) <= Utl->GetThrSignal())
						{
							bMRD4 = 1;
							break;
						}
						else hCard21->Fill(3);
					}

					Fill1DHist(ER);		//Histograms ready to be saved
				}
				else
					++mDR[ID];

				//Average graph computed
			}

			gPulse = ER->LoadGraph();
			GraphAVG(gMean, cM);
			if (ER->GetTOF() > Utl->GetLowB() && ER->GetTOF() < Utl->GetUpB())
				GraphAVG(giTOF, cI);
			else GraphAVG(goTOF, cO);

			if (Utl->GetPrintGraph() > 0 ? (GC % Utl->GetPrintGraph() == 1) : 0)	//Prints a graph
				Save_GRHist(trg, ID, iE - mPulseER[ID].begin());
		}
	}

	ResetHist();
}




//void PulseFinder::FillHist(int trg, int spl, int evt)
void PulseFinder::Fill1DHist(EventReco *ER)
{
	if (iVerb)
		std::cout << "Filling histograms...\n";
/*
	int ID, px, pz;
	std::map<int, Coord>::const_iterator imP;
	for (imP = mPos.begin(); imP != mPos.end(); imP++)
	{
		ID = imP->first;
//		px = *(imP).x;
//		pz = *(imP).z;
		hEner->SetBinContent(imP->second.x+1, imP->second.z+1, mI[ID]);
		hTime->SetBinContent(imP->second.x+1, imP->second.z+1, mT[ID]);
//		hTime->SetBinContent(px+1, pz+1, mT[ID]);
		if (iVerb > 2)
		{
			std::cout << "ID   " << ID << "\t";
			std::cout << "ener " << mI[ID] << "\t";
			std::cout << "time " << mT[ID] << "\t";
			std::cout << "x    " << px << "\t";
			std::cout << "z    " << pz << std::endl;
		}
	
*/	
	fBaseLine = ER->GetBaseLine();
	fPeak = ER->GetPeak();
	fValley = ER->GettP2V();
	fTime = ER->GettCFD();
	fWidth = ER->GetZeroC();
	fCharge = ER->GetCharge();
	fEnergy = ER->GetEnergy();
	fTOF = ER->GetTOF();

	hBaseLine->Fill(fBaseLine);
	hPeak->Fill(fPeak);
	hValley->Fill(fValley);
	hTime->Fill(fTime);
	hWidth->Fill(fWidth);
	hCharge->Fill(fCharge);
	hEnergy->Fill(fEnergy);
	hTOF->Fill(fTOF);

	tEvent->Fill();
}

void PulseFinder::Fill2DHist(int ID, Analysis Par)
{
	Coord Pos = Utl->Mapper(ID);

//	h2Ener->SetBinContent(Pos.x+1, Pos.z+1, Par.E);
//	h2Time->SetBinContent(Pos.x+1, Pos.z+1, Par.T);
//	h2Peak->SetBinContent(Pos.x+1, Pos.z+1, Par.P);
//	h2Widt->SetBinContent(Pos.x+1, Pos.z+1, Par.W);
}

//Histograms
void PulseFinder::NewHist()
{
	double xrange = Utl->GetBuffer()*fBW;
	int nbins = int(0.1*Utl->GetBuffer());
	hPulse = new TH1F("hpulse", "Pulses", nbins, 0, xrange);
	hPtrig = new TH1F("hptrig", "Pulses in trigger", nbins/10, 0, xrange);
	hPfile = new TH1F("hpfile", "Pulses in file", nbins, 0, xrange);	
	hEvent = new TH1F("hevent", "Events freq", nbins/20, 0, xrange);
	hEntry = new TH1F("hentry", "Entries freq", nbins/20, 0, xrange);
	hBinWd = new TH1F("hbinwd", "Event width", 40, 0, 8);

//	nEvent = new TNtuple("nevent", "event data", "baseline:peak:p2v:charge:energy:tcfd:zeroc:tof");

	hBaseLine = new TH1F("hbaseline", "Baseline", 250, -0.005, 0.005);
	hPeak = new TH1F("hpeak", "Peak", 500, 0, 5);
	hValley = new TH1F("hvalley", "Valley from peak", 500, 0, 1);
	hTime = new TH1F("htime", "Time", nbins, 0, xrange);
	hWidth = new TH1F("hwidth", "Pulse width", 500, -0.1, 0.9);
	hCharge = new TH1F("hcharge", "Charge", 500, -0.005, 0.075);
	hEnergy = new TH1F("henergy", "Energy", 500, -0.005, 0.095);
	hTOF = new TH1F("htof", "Time of flight", nbins, -xrange, xrange);
	hCard21 = new TH1I("hcard21", "Card 21 on event", 4, 0, 4);

//	h2Ener = new TH2F("h2ener", "Energy", 8, -0.5, 7.5, 8, -0.5, 7.5);
//	h2Time = new TH2F("h2time", "Time", 8, -0.5, 7.5, 8, -0.5, 7.5);
//	h2Peak = new TH2F("h2peak", "Peak", 8, -0.5, 7.5, 8, -0.5, 7.5);
//	h2Widt = new TH2F("h2widt", "width", 8, -0.5, 7.5, 8, -0.5, 7.5);
	h2Dark = new TH2F("h2dark", "Dark noise per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);
	h2EvRa = new TH2F("h2evra", "Event rate per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);

	gMean = new TGraph(Utl->GetEvtLength());
	giTOF = new TGraph(Utl->GetEvtLength());
	goTOF = new TGraph(Utl->GetEvtLength());

	tEvent = new TTree("tEvent", "Event's pulses tree container");
	tEvent->Branch("Baseline", &fBaseLine, "fBaseLine/F");
	tEvent->Branch("Peak", &fPeak, "fPeak/F");
	tEvent->Branch("P2V", &fValley, "fValley/F");
	tEvent->Branch("Time", &fTime, "fTime/F");
	tEvent->Branch("Width", &fWidth, "fWidth/F");
	tEvent->Branch("Charge", &fCharge, "fCharge/F");
	tEvent->Branch("Energy", &fEnergy, "fEnergy/F");
	tEvent->Branch("TOF", &fTOF, "fTOF/F");
	tEvent->Branch("VETO", &bVETO, "bVETO/O");
	tEvent->Branch("MRD2", &bMRD2, "bMRD2/O");
	tEvent->Branch("MRD4", &bMRD4, "bMRD4/O");
}

void PulseFinder::FillRateHist()
{
	int ID;
	Coord Pos;
//	if (iVerb > 1)
		std::cout << "mDR size " << mDR.size() << std::endl;
		std::cout << "mER size " << mER.size() << std::endl;

//	std::map<int, double>::const_iterator imP;
	for (int ID = 1; ID < 61; ++ID)
//	for (imP = mDR.begin(); imP != mDR.end(); imP++)
	{
		Pos = Utl->Mapper(ID);

		mDR[ID] /= mLength[ID];
		mER[ID] /= mLength[ID];
		mDR[ID] *= 1000000;
		mER[ID] *= 1000000;

		if (iVerb > 1)
		{
			std::cout << ID << " Dark  rate " << mDR[ID] << "Hz\t";
			std::cout << ID << " Event rate " << mER[ID] << "Hz\n";
		}

		h2Dark->SetBinContent(Pos.x+1, Pos.z+1, mDR[ID]);
		h2EvRa->SetBinContent(Pos.x+1, Pos.z+1, mER[ID]);
	}
}

void PulseFinder::GraphAVG(TGraph *g, int &cc)
{
	double x, y0, y;
	++cc;
	for (int i = 0; i < gPulse->GetN(); ++i)
	{
		gPulse->GetPoint(i, x, y0);
		g->GetPoint(i, x, y);
		g->SetPoint(i, fBW*i, ((cc-1)*y+y0)/cc);
		if (y0 > 1000) std::cout << "Warning " << cc << std::endl;
	}
}

//Utility stuff
void PulseFinder::Save_Hist()
{
	FillRateHist();
	if (iVerb)
		std::cout << "Saving 1D histograms...\n";
	
	Utl->OutFile->cd();

	hPfile->SetStats(kFALSE);
	hEvent->SetStats(kTRUE);
	hEntry->SetStats(kTRUE);
	hBinWd->SetStats(kTRUE);
	h2Dark->SetStats(kFALSE);
	h2Dark->SetContour(40);
	h2Dark->SetOption("COLZ10TEXT");
	h2EvRa->SetStats(kFALSE);
	h2EvRa->SetContour(40);
	h2EvRa->SetOption("COLZ10TEXT");


	hPfile->GetXaxis()->SetTitle("time");
	hEvent->GetXaxis()->SetTitle("pmt fired");
	hEntry->GetXaxis()->SetTitle("entries");
	hBinWd->GetXaxis()->SetTitle("width");

	h2Dark->GetXaxis()->SetTitle("x");
	h2Dark->GetYaxis()->SetTitle("z");
	h2Dark->GetZaxis()->SetRangeUser(Utl->MinM(mDR)-1, Utl->MaxM(mDR)+1);
	h2EvRa->GetXaxis()->SetTitle("x");
	h2EvRa->GetYaxis()->SetTitle("z");
	h2EvRa->GetZaxis()->SetRangeUser(Utl->MinM(mDR)-1, Utl->MaxM(mDR)+1);

	gMean->GetXaxis()->SetTitle("time");
	giTOF->GetXaxis()->SetTitle("time");
	goTOF->GetXaxis()->SetTitle("time");
	gMean->GetYaxis()->SetTitle("amplitude");
	giTOF->GetYaxis()->SetTitle("amplitude");
	goTOF->GetYaxis()->SetTitle("amplitude");

	gMean->SetTitle("Average");
	giTOF->SetTitle("In coincidence");
	goTOF->SetTitle("Out coincidence");

	tEvent->Write();

	hBaseLine->Write();
	hPeak->Write();
	hValley->Write();
	hTime->Write();
	hWidth->Write();
	hCharge->Write();
	hEnergy->Write();
	hTOF->Write();
	hCard21->Write();

	hPfile->Write();
	hEvent->Write();
	hEntry->Write();
	hBinWd->Write();
	h2Dark->Write();
	h2EvRa->Write();
	gMean->Write("gmean");
	giTOF->Write("gitof");
	goTOF->Write("gotof");
}

void PulseFinder::Save_2DHist(int trg, int evt)
{
	if (iVerb)
		std::cout << "Saving 2D histograms...\n";

	Utl->OutFile->cd("2D");

	//1D plot
	hPtrig->SetStats(kFALSE);
	hPtrig->GetXaxis()->SetTitle("time");
	if (evt == 0)
	{
		ssName.str("");
		ssName.clear();
		ssName << hPtrig->GetName();
		ssName << "_" << trg;
		hPtrig->Write(ssName.str().c_str());
	}

	//2D plots
//	h2Ener->SetContour(40);
//	h2Time->SetContour(40);
//	h2Peak->SetContour(40);
//	h2Widt->SetContour(40);

//	h2Ener->SetStats(kFALSE);
//	h2Time->SetStats(kFALSE);
//	h2Peak->SetStats(kFALSE);
//	h2Widt->SetStats(kFALSE);

	h2Ener->GetXaxis()->SetTitle("x");
	h2Ener->GetYaxis()->SetTitle("z");
	h2Time->GetXaxis()->SetTitle("x");
	h2Time->GetYaxis()->SetTitle("z");
	h2Peak->GetXaxis()->SetTitle("x");
	h2Peak->GetYaxis()->SetTitle("z");
//	h2Widt->GetXaxis()->SetTitle("x");
//	h2Widt->GetYaxis()->SetTitle("z");

	h2Ener->SetOption("COLZ10TEXT");
	h2Time->SetOption("COLZ10TEXT");
	h2Peak->SetOption("COLZ10TEXT");
//	h2Widt->SetOption("COLZ10TEXT");

	ssName.str("");
	ssName.clear();
	ssName << h2Ener->GetName();
	ssName << "_" << trg << "_" << evt;
	if (iVerb > 2)
	{
		std::cout << "hEner " << "max " << Utl->MaxV(vE);
		std::cout << " min " << Utl->MinV(vE) << std::endl;
	}	
	h2Ener->GetZaxis()->SetRangeUser(Utl->MinV(vE), Utl->MaxV(vE));
	h2Ener->Write(ssName.str().c_str());

	ssName.str("");
	ssName.clear();
	ssName << h2Time->GetName();
	ssName << "_" << trg << "_" << evt;
	if (iVerb > 2)
	{	
		std::cout << "hTime " << "max " << Utl->MaxV(vT);
		std::cout << " min " << Utl->MinV(vT) << std::endl;
	}
	double bw = fBW;
	h2Time->GetZaxis()->SetRangeUser(Utl->MinV(vT)-10*bw, Utl->MaxV(vT)+10*bw);
	h2Time->Write(ssName.str().c_str());

	ssName.str("");
	ssName.clear();
	ssName << h2Peak->GetName();
	ssName << "_" << trg << "_" << evt;
	if (iVerb > 2)
	{
		std::cout << "hPeak " << "max " << Utl->MaxV(vP);
		std::cout << " min " << Utl->MinV(vP) << std::endl;
	}	
	h2Peak->GetZaxis()->SetRangeUser(Utl->MinV(vP), Utl->MaxV(vP));
	h2Peak->Write(ssName.str().c_str());

//	ssName.str("");
//	ssName.clear();
//	ssName << h2Widt->GetName();
//	ssName << "_" << trg << "_" << evt;
//	if (iVerb > 2)
//	{
//		std::cout << "hWidt " << "max " << Utl->MaxV(vW);
//		std::cout << " min " << Utl->MinV(vW) << std::endl;
//	}	
//	h2Widt->GetZaxis()->SetRangeUser(Utl->MinV(vW), Utl->MaxV(vW));
//	h2Widt->Write(ssName.str().c_str());
}

void PulseFinder::Save_GRHist(int trg, int ID, int evt)
{
	Utl->OutFile->cd("GR");

	Coord Pos = Utl->Mapper(ID);

	ssName.str("");
	ssName.clear();
	ssName << "pulse_" << trg;
	ssName << "_x" << Pos.x;
	ssName << "z" << Pos.z;
	ssName << "_" << evt;

	gPulse->GetXaxis()->SetTitle("time");
	gPulse->GetYaxis()->SetTitle("Data");
	gPulse->SetTitle("Pulse");

	gPulse->Write(ssName.str().c_str());
}

void PulseFinder::ResetHist()
{
	hPulse->Reset();
	hPtrig->Reset();
//	h2Ener->Reset();
//	h2Time->Reset();
//	h2Peak->Reset();
//	h2Widt->Reset();
}

void PulseFinder::CleanER()
{
	std::map<int, std::vector<EventReco*> >::iterator iM = mPulseER.begin();	//Loop over pulses
	for ( ; iM != mPulseER.end(); ++iM)
	{
		std::vector<EventReco*>::iterator iV = iM->second.begin();	//Loop over pulses
		for ( ; iV != iM->second.end(); ++iV)
			delete *(iV);
	}

	mPulseER.clear();
//	mPulseT.clear();
}

void PulseFinder::CleanAll()
{
	CleanER();
	mDR.clear();
	mER.clear();
	mLength.clear();

	hPulse->Delete();
	hPtrig->Delete(); 
	hPfile->Delete();
	hEvent->Delete();
	hEntry->Delete();
	hBinWd->Delete();
	         
	hBaseLine->Delete();
	hPeak->Delete();
	hValley->Delete();
	hTime->Delete();
	hWidth->Delete();
	hCharge->Delete();
	hEnergy->Delete();
	hTOF->Delete();
	hCard21->Delete();
         
//	h2Ener->Delete();
//	h2Time->Delete();
//	h2Peak->Delete();
//	h2Widt->Delete();
	h2Dark->Delete();
	h2EvRa->Delete();

	gMean->Delete();
	giTOF->Delete();
	goTOF->Delete();
}
