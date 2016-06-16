#include "PulseFinder.h"


PulseFinder::PulseFinder(std::string dir) :
	verb (1),
	thr_pek (0.02),
	thr_evt (10),
	thr_sig (4),
	shape (10),
	bw (0.002),
	wl (40000),
	sample (1)
{
	Dir = dir;
	if (verb > 3) std::cout << "Dir " << Dir << std::endl;
	std::size_t fnd = Dir.find('/');
	if (Dir.at(Dir.length()-1) == '/')
		Dir.erase(Dir.end()-1, Dir.end());
	if (verb > 3) std::cout << "Dir " << Dir << std::endl;
}

PulseFinder::~PulseFinder()
{
	CleanHist();
	Close();
//	delete PMT;
}

void PulseFinder::LoopTrg()
{
	newF->cd();

	mc = 0;

	std::string hname;
	PMT->GetEntry(PMT->fChain->GetEntriesFast()-1);
	ntrg = int(PMT->Trigger/sample);
	std::cout << "Number of entries: " << ntrg << std::endl;

	std::cout << "div " << 0.000001*ntrg*bw*wl*sample;

	mDR.clear();
	vER.clear();

	hDouble = new TH1F("hdouble", "double", int(0.1*wl), 0, wl*bw);
	
	hPulse = new TH1F("hpulse", "pulse", int(0.1*wl), 0, wl*bw);
	hCharg = new TH1F("hcharg", "charge", int(0.1*wl), 0, wl*bw);
	hRWMon = new TH1F("hrwmon", "RWM", int(wl), 0, wl*bw);
	hPtot = new TH1F("hptot", "events time", int(0.1*wl), 0, wl*bw);
	hCtot = new TH1F("hctot", "events charge", int(0.1*wl), 0, wl*bw);
	hEvent = new TH1I("hevent", "events freq", int(0.1*wl), 0, wl*bw);
	hEner = new TH2F("hener", "energy", 8, -0.5, 7.5, 8, -0.5, 7.5);
	hTime = new TH2F("htime", "time", 8, -0.5, 7.5, 8, -0.5, 7.5);
	hSNtot = new TH1F("hsntot", "signal and noise", 5000, 0, 10);
	hSignl = new TH1F("hsignl", "signal", 5000, 0, 10);
	hNoise = new TH1F("hnoise", "dark noise", 5000, 0, 10);
//	hPeak = new TH1F("hpeak", "", 5000, 0, 10);
	hDark = new TH2F("hdark", "dark noise per pmt", 8, -0.5, 7.5, 8, -0.5, 7.5);

	nEvent = new TNtuple("nevent", "event data", "baseline:peak:p2v:charge:energy:tcfd:zeroc:tof");

	for (unsigned int i = 0; i < ntrg; i++)
	{
		for (int k = 0; k < sample; k++)	//loop on spills
		{
			PMT->GetEntry(256*i+k);
			if (verb)
				std::cout << "Trigger " << PMT->Trigger << std::endl;
			LoopPMT(i, k);
			FindEvents(i, k);
			for (int j = 0; j < vEvt.size(); j++)
			{
				if (verb)
					std::cout << "Event " << j << std::endl;
	
				CatchEvents(i, k, j);
				FillEvents(i, k, j);
				FillHist(i, k, j);
				SNcount(i, k, j);
				PMT->GetEntry(256*i+k);
				Save2DHist(PMT->Trigger, j);
				Print2DStats(PMT->Trigger, j);
				ResetHist();
      		}
			if (vEvt.size() == 0)
      		{
				SNcount(i, k);
				ResetHist();
      		}
      	}
      }
	SaveDRHist();
	Save1DHist();
	if (graph) SaveGraph();
	Print1DStats();

	fout << "Multipurpose counter: " << mc << std::endl;
}

void PulseFinder::LoopPMT(int trg, int spl)
{
	vI.clear();
	vT.clear();
//	vH.clear();
	vBin.clear();
	vPos.clear();
	vID.clear();
	vEntry.clear();
	RWM = 0;

	for (int j = 0; j < 256; j += sample)
	{
		PMT->GetEntry(256*trg+j+spl);
		if (verb > 2)
		{
			std::cout << "Entry " << 256*trg+j+spl << "\t";
			std::cout << "PMTID " << PMT->PMTID << std::endl;
		}
		if (PMT->CardID < 21) FindPulses(256*trg+j+spl, spl);
		else if (PMT->CardID == 21 && PMT->Channel == 2)
		{
			for (int k = 0; k < wl; k++)
				hRWMon->SetBinContent(k, PMT->Data[k]);
			RWM = hRWMon->FindFirstBinAbove(0.5);
		}
	}

}

void PulseFinder::FindPulses(int ent, int spl)
{
	double peak = 0, ener = 0, y = 0, y_ = 0;
	double thr = thr_pek;
	int time = 0;
	Coord pos;

	for (int k = 0; k < wl; k++)
	{
		y = PMT->Data[k];
		if (k == 0) y_ = y;
		else y_ = PMT->Data[k-1];

		if ((y_ > thr) && (y < thr))
			thr = thr_pek*(1 + (r_hys > 0 ? 1.0/r_hys : 0));

		if ((y_ < thr) && (y > thr))	//There is a pulse!
		{
			thr = thr_pek*(1 - (r_hys > 0 ? 1.0/r_hys : 0));

			time = LocMaximum(k, peak);
			ener = Integrate(time, bw);
//			ener = Integrate(time, 1);
			pos.x = PMT->PMTx;
			pos.z = PMT->PMTz;

			ER = new EventReco(ConfigFile, PMT, vEntry.at(j), vBin.at(j), RWM, trg, ID, evt);

			//sync push back
			vPos.push_back(pos);		//PMT location
			vI.push_back(ener);		//ener of pulse
			vT.push_back(bw*time);		//real time of pulse
//			vH.push_back(peak);		//peak height
			vBin.push_back(time);		//bw of pulse
			vID.push_back(PMT->PMTID);	//ID
			vEntry.push_back(ent);		//Tree entry

			if (verb > 1)
			{
				std::cout << "Pulse ID " << PMT->PMTID << " @\t";
		       		std::cout << k << "\t-> " << time << std::endl;
			}
			if (verb > 2)			
			{
				std::cout << "y_   " << y_ << "\t";
				std::cout << "y    " << y << "\t";
				std::cout << "ener " << ener << "\t";
				std::cout << "time " << time << "\t";
				std::cout << "peak " << peak << "\t";
				std::cout << "ID   " << PMT->PMTID << "\t";
				std::cout << "posx " << pos.x << "\t";
				std::cout << "posz " << pos.z << "\n";
			}

//			hPulse->Fill(bw*time);	//Fill hist with times
			hCharg->Fill(bw*time, ener);	//Fill hist with times

			hDouble->Fill(bw*time);
			if (hDouble->GetBinContent(time/10.0) > 0) mc++;

			if (time >= k)
				k = time + 100/sample;		//FFW must be validated
		}
	}
	hPulse->Add(hDouble);
	hDouble->Reset();
}

void PulseFinder::FindEvents(int trg, int spl)
{
	if (verb)
		std::cout << "Looking for events in trigger " << trg << "...\n";
	double b, b_, thr = thr_evt;
	vEvt.clear();
	for (int i = 0; i < 0.1*wl; i++)
	{
		b = hPulse->GetBinContent(i);
		if (i == 0) b_ = b;
		else b_ = hPulse->GetBinContent(i-1);

		if ((b_ > thr) && (b < thr))
			thr = thr_evt*(1 + (r_hys > 0 ? 1.0/r_hys : 0));

		if ((b_ < thr) && (b > thr))
		{
			thr = thr_evt*(1 - (r_hys > 0 ? 1.0/r_hys : 0));

			vEvt.push_back(bw*(10*i-5));
//			hPtot->Fill(bw*i*10);
			hPtot->Add(hPulse);
			hCtot->Add(hCharg);
			if (verb)
			{
				std::cout << "Bin " << b << "\t";
				std::cout << "@ " << 10*i - 5 << std::endl;
			}
		}
		if (b > thr_evt/3.0)
			hEvent->Fill(b);
	}
}

void PulseFinder::CatchEvents(int trg, int spl, int evt)	//And Event Reco
{
	if (verb)
		std::cout << "Comparing pulses and events...\n";
	if (verb > 1) std::cout << "Event size " << vEvt.size() << std::endl;
	if (verb > 1) std::cout << "Pulse size " << vID.size() << std::endl;

	mI.clear();
	mT.clear();
	mPos.clear();

	int ID, ers;
	for (int j = 0; j < vID.size(); j++)
	{
		ID = vID.at(j);
		if (verb > 2)
		{
			std::cout << "vT " << vT.at(j) << "\t";
			std::cout << "vEvt " << vEvt.at(evt) << "\t";
			std::cout << "thr " << 10*bw*thr_sig << std::endl;
		}
		if (fabs(vT.at(j) - vEvt.at(evt)) <= 10*bw*thr_sig)
		{
			if (graph)
			{
//				std::cout << "t " << vT.at(j) << "\tb " << vBin.at(j) << std::endl;
				ers = vER.size();
				vER.push_back(new EventReco(ConfigFile, PMT, vEntry.at(j), vBin.at(j), RWM, trg, ID, evt));
				vER.at(ers)->LoadGraph();
				vER.at(ers)->LoadN();
				if (verb > 2)
					vER.at(ers)->Print();
				vER.at(ers)->FillN(nEvent);
				if (vER.at(ers)->GetEN() > mI[ID])
				{
					mI[ID] = vER.at(ers)->GetEN();
					mT[ID] = vER.at(ers)->GetCFD();
					mPos[ID].x = vPos.at(j).x;
					mPos[ID].z = vPos.at(j).z;
				}
			}
			else
				if (vI.at(j) > mI[ID])
				{
	
					mI[ID] = vI.at(j);
					mT[ID] = vT.at(j);
					mPos[ID].x = vPos.at(j).x;
					mPos[ID].z = vPos.at(j).z;
				}
			if (verb > 2)
			{
				std::cout << "Evt  " << vEvt.at(evt) << "\t";
				std::cout << "ID   " << ID << "\t";
				std::cout << "ener " << mI[ID] << "\t";
				std::cout << "time " << mT[ID] << "\t";
				std::cout << "x    " << mPos[ID].x << "\t";
				std::cout << "z    " << mPos[ID].z << std::endl;
			}

		}
	}
}

void PulseFinder::FillEvents(int trg, int spl, int evt)
{
	if (verb)
		std::cout << "Filling the PMTs..." << std::endl;

	int ID, px, pz, time;
	double peak;

	int k = vEvt.at(evt)/bw;

	for (int j = 0; j < 256; j += sample)
	{
		PMT->GetEntry(256*trg+j+spl);
		ID = PMT->PMTID;
		if (PMT->CardID > 20) continue;

		if (mPos.find(ID) == mPos.end())
		{
			time = LocMaximum(k, peak);
			mT[ID] = bw*time;
			mI[ID] = Integrate(time, graph ? bw*100/sample : bw);
			mPos[ID].x = PMT->PMTx;
			mPos[ID].z = PMT->PMTz;
			if (verb > 2)
			{
				std::cout << "Evt  " << vEvt.at(evt) << "\t";
				std::cout << "ID   " << ID << "\t";
				std::cout << "ener " << mI[ID] << "\t";
				std::cout << "time " << mT[ID] << "\t";
				std::cout << "x    " << mPos[ID].x << "\t";
				std::cout << "z    " << mPos[ID].z << std::endl;
			}
		}
//		else std::cout << "ID   " << ID << std::endl;
	}
}

void PulseFinder::FillHist(int trg, int spl, int evt)
{
	if (verb)
		std::cout << "Filling hitograms...\n";

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
		if (verb > 2)
		{
			std::cout << "ID   " << ID << "\t";
			std::cout << "ener " << mI[ID] << "\t";
			std::cout << "time " << mT[ID] << "\t";
			std::cout << "x    " << px << "\t";
			std::cout << "z    " << pz << std::endl;
		}
	}
}		

void PulseFinder::SNcount(int trg, int spl, int evt)
{
	int ID;
	for (int j = 0; j < vID.size(); j++)
	{
		ID = vID.at(j);
		if (vEvt.size() == 0)
		{
			hNoise->Fill(vI.at(j));
			mDR[ID]++;
		}
		else 
		{
			if (fabs(vT.at(j) - vEvt.at(evt)) <= 10*bw*thr_sig)
			{
				hSignl->Fill(vI.at(j));
			}
			else if (fabs(vT.at(j) - vEvt.at(evt)) > 20*bw*thr_sig)
			{
				hNoise->Fill(vI.at(j));
				mDR[ID]++;
			}
		}
	}
}
