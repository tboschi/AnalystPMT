#include "PulseFinder.h"

void PulseFinder::SaveDRHist()
{
	int ID;
	std::map<int, Coord>::const_iterator imP;
	for (imP = mPos.begin(); imP != mPos.end(); imP++)
	{
		ID = imP->first;
		std::cout << "dark rate " << mDR[ID];
		mDR[ID] /= 0.000001*ntrg*bw*wl*sample;
		hDark->SetBinContent(imP->second.x+1, imP->second.z+1, mDR[ID]);
		std::cout << "\t" << mDR[ID] << std::endl;
	}

	newF->cd();

	hDark->SetContour(40);
	hDark->SetStats(kFALSE);
	hDark->SetOption("COLZ10TEXT");
	hDark->GetXaxis()->SetTitle("x");
	hDark->GetYaxis()->SetTitle("z");


	hDark->GetZaxis()->SetRangeUser(MinM(mDR), MaxM(mDR));
	hDark->Write();
}

int PulseFinder::LocMaximum(int j, double &p)
{
	double max = 0;
	int time = j;
	for (int i = j; i < j+shape+1; i++)
	{
		if ((i < 0) || (i > wl)) continue;
		if (PMT->Data[i] > max)
		{
			max = PMT->Data[i];
			time = i;
		}
	}
	p = max;
	return time;
}

double PulseFinder::Integrate(int j, double bw)
{
	double sum = 0;
//	for (int i = j-shape/2; i < j+1+shape/2; i++)
	for (int i = j-2; i < j+3; i++)
	{
		if ((i < 0) || (i > wl)) continue;
//		if (i == j) continue;	//Get rid of peak
		sum += PMT->Data[i];
	}
	return sum > 0.001 ? bw*sum : bw*0.001;
//	return bw*sum;
}

bool PulseFinder::OpenIns(std::string fname)
{
	oldF = new TFile(fname.c_str(), "UPDATE");		//open file -> should be a loop on vector
	if (oldF->IsZombie()) 
	{
		std::cout << "ZOMBIE!\n";
		Close();
		return false;
	}
	else return true;
}

bool PulseFinder::OpenOuts(std::string fname)
{
	std::string noExt = GetBaseName(fname);		//Create result output file
	std::string outName = Dir + "/result/d" + noExt + ".dat";

	if (!Exist(outName))
	{
		if (fout.is_open()) fout.close();
		fout.open(outName.c_str(), std::fstream::out);
		std::cout << "Results are saved in: " << outName << std::endl;
		outName = Dir + "/result/r" + noExt + ".root";
		std::cout << "Root is saved in: " << outName << std::endl;
		newF = new TFile(outName.c_str(), "RECREATE");	//Open output root file
		if (newF->IsZombie()) 
		{
			std::cout << "Not able to open ROOT file. Skipping...\n\n";
			Close();
			return false;
		}
		else
		{
			newF->mkdir("2D");
			newF->mkdir("Event");
			return true;
		}
	}
	else 
	{
		std::cout << "File already analysed. Skipping...\n\n";
		return false;
	}

}

void PulseFinder::GetTree()
{
	oldF->cd();
	TIter nextkey(oldF->GetListOfKeys());		//Get first tree on each file
	TKey *key;
	while ((key = (TKey*)nextkey()))
	{
		TObject *obj = key->ReadObj();
		if (obj->IsA()->InheritsFrom(TTree::Class()))
		{
			TreeData = (TTree*)obj;		//assign found tree to TreeData
			break;
		}
	}

	PMT = new PMTData(TreeData);		//Create object PMT
}

void PulseFinder::Save1DHist()
{
	if (verb)
		std::cout << "Saving 1D histograms...\n";
	

	newF->cd();

	hPtot->SetStats(kFALSE);
	hCtot->SetStats(kFALSE);
	hEvent->SetStats(kFALSE);
	hSNtot->SetStats(kFALSE);
	hSignl->SetStats(kFALSE);
	hNoise->SetStats(kFALSE);

	hPtot->GetXaxis()->SetTitle("time");
	hCtot->GetXaxis()->SetTitle("time");
	hEvent->GetXaxis()->SetTitle("pmt fired");
	hSNtot->GetXaxis()->SetTitle("amplitude");
	hSignl->GetXaxis()->SetTitle("amplitude");
	hNoise->GetXaxis()->SetTitle("amplitude");

	hSNtot->SetLineColor(1);
	hSignl->SetLineColor(2);
	hNoise->SetLineColor(4);

	hSNtot->Add(hSignl);
	hSNtot->Add(hNoise);

	hPtot->Write();
	hCtot->Write();
	hEvent->Write();
	hSNtot->Write();
	hSignl->Write();
	hNoise->Write();
}

void PulseFinder::Save2DHist(int trg, int evt)
{
	if (verb)
		std::cout << "Saving 2D histograms...\n";

	newF->cd("2D");

	hEner->SetContour(40);
	hTime->SetContour(40);

	hPulse->SetStats(kFALSE);
	hCharg->SetStats(kFALSE);
	hRWMon->SetStats(kFALSE);
	hEner->SetStats(kFALSE);
	hTime->SetStats(kFALSE);

	hPulse->GetXaxis()->SetTitle("time");
	hCharg->GetXaxis()->SetTitle("time");
	hRWMon->GetXaxis()->SetTitle("time");
	hEner->GetXaxis()->SetTitle("x");
	hEner->GetYaxis()->SetTitle("z");
	hTime->GetXaxis()->SetTitle("x");
	hTime->GetYaxis()->SetTitle("z");

	hEner->SetOption("COLZ10TEXT");
	hTime->SetOption("COLZ10TEXT");

	if (evt == 0)
	{
		ssName.str("");
		ssName.clear();
		ssName << hPulse->GetName();
		ssName << "_" << trg;
		hPulse->Write(ssName.str().c_str());
	
		ssName.str("");
		ssName.clear();
		ssName << hCharg->GetName();
		ssName << "_" << trg;
		hCharg->Write(ssName.str().c_str());
	
		ssName.str("");
		ssName.clear();
		ssName << hRWMon->GetName();
		ssName << "_" << trg;
		hRWMon->Write(ssName.str().c_str());
	}
	
	ssName.str("");
	ssName.clear();
	ssName << hEner->GetName();
	ssName << "_" << trg << "_" << evt;
	if (verb > 2)
	{
		std::cout << "hEner " << "max " << MaxM(mI);
		std::cout << " min " << MinM(mI) << std::endl;
	}	
	hEner->GetZaxis()->SetRangeUser(MinM(mI), MaxM(mI));
	hEner->Write(ssName.str().c_str());

	ssName.str("");
	ssName.clear();
	ssName << hTime->GetName();
	ssName << "_" << trg << "_" << evt;
	if (verb > 2)
	{	
		std::cout << "hTime " << "max " << MaxM(mT);
		std::cout << " min " << MinM(mT) << std::endl;
	}
	hTime->GetZaxis()->SetRangeUser(MinM(mT)-bw, MaxM(mT)+bw);
	hTime->Write(ssName.str().c_str());
}

void PulseFinder::Print1DStats()
{
//	fout << "Trigger: " << trg << std::endl;
	double tt = 0.000001*ntrg*bw*wl;
	fout << std::endl << std::endl;
	fout << "Event above " << thr_evt << ":\t";
        fout << hEvent->Integral(thr_evt, 60) << std::endl;
	fout << "Signal:\t" << hSignl->GetMean() << " \\pm ";
	fout << hSignl->GetMeanError() << ",\t";
	fout << "Entries\t" << hSignl->GetEntries();
	fout << " and rate of " << hSignl->GetEntries()/tt << " Hz\n";
	fout << hSignl->GetEntries() << std::endl;
	fout << "Noise:\t" << hNoise->GetMean() << " \\pm ";
	fout << hNoise->GetMeanError() << ",\t";
	fout << "Entries\t" << hNoise->GetEntries() <<std::endl;
	fout << "Rate total   \t" << hNoise->GetEntries()/tt << " Hz\n";
	fout << "Rate per PMT \t" << hNoise->GetEntries()/(60*tt) << " Hz\n";
	for (int i = 1; i < 61; i++)
		fout << "PMT " << i << "  :\t" << mDR[i] << " Hz\n";
}

void PulseFinder::Print2DStats(int trg, int evt)
{
	fout << "Trigger: " << trg << "\tEvent: " << evt << std::endl;
	fout << "Energy:\t\tMax\t";
	fout << hEner->GetMaximum() << "\t";
	fout << "Min\t";
	fout << hEner->GetMinimum() << std::endl;

	fout << "Time:\t\tMax\t";
	fout << hTime->GetMaximum() << "\t";
	fout << "Min\t";
	fout << hTime->GetMinimum() << std::endl;
}

void PulseFinder::ResetHist()
{
	hEner->Reset();
	hTime->Reset();
	hPulse->Reset();
	hCharg->Reset();
	hRWMon->Reset();
}

void PulseFinder::CleanHist()
{
	hPulse->Delete();
	hCharg->Delete();
	hPtot->Delete();
	hCtot->Delete();
	hEvent->Delete();
	hEner->Delete();
	hTime->Delete();
	hSNtot->Delete();
	hSignl->Delete();
	hNoise->Delete();
	hDark->Delete();
	nEvent->Delete();
}

void PulseFinder::Close()
{
	oldF->Close();
	newF->Close();
	fout.close();
}

bool PulseFinder::Exist(std::string &fileName)
{
	std::ifstream infile(fileName.c_str());
	if (!debug) return infile.good();		//commented for debuggin
	else return false;
}

std::string PulseFinder::GetBaseName(std::string fn)
{
	fn.erase(fn.begin()+fn.find('.'), fn.end());
	std::size_t fnd = fn.find('/');
	while (fnd != std::string::npos)
	{
		fn.erase(fn.begin(), fn.begin()+fn.find('/')+1);
		fnd = fn.find('/');
	}
	if (verb) std::cout << "Root file ready: " << fn << std::endl;
	return fn;
}

void PulseFinder::SetConfig(std::string cfn)
{
	ConfigFile = cfn;
	std::ifstream fin(cfn.c_str());
	std::string Line, var;
	double val;
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
			if (var == "debug") SetDebug(val);
			if (var == "graph") SetGraph(val);
			if (var == "verb") SetVerbosity(val);
			if (var == "thr_pek") SetThrPeak(val); 
			if (var == "r_hys") SetThrHyst(val); 
			if (var == "thr_evt") SetThrEvent(val);
			if (var == "thr_sig") SetThrSignal(val);
			if (var == "shape") SetShapingTime(val);
			if (var == "sample") SetSample(val);
			if (var == "binwid") SetBW(val);
			if (var == "winlength") SetWinLen(val);
		}
	}
	std::cout << "Verbosity is " << verb << std::endl;
	if (graph)
		std::cout << "Individual pulse analysis will be made" << std::endl;
	if (debug)
		std::cout << "Debugging mode on\nWARNING: plots will be overwritten" << std::endl;
	else std::cout << "Debugging mode off\nNew plots will be created" << std::endl;
	fin.close();
	std::cout << "bw " << bw << std::endl;
	std::cout << "wl " << wl << std::endl;
}

void PulseFinder::SetThrPeak(double thr)
{
	thr_pek = thr;
}

void PulseFinder::SetThrHyst(double thr)
{
	r_hys = thr;
}

void PulseFinder::SetThrEvent(int thr)
{
	thr_evt = thr;
}

void PulseFinder::SetThrSignal(int thr)
{
	thr_sig = thr;
}

void PulseFinder::SetShapingTime(int sha)
{
	shape = sha;
}

void PulseFinder::SetBW(double binw)
{
	bw = GetSample()*binw;
}

void PulseFinder::SetWinLen(int thr)
{
	wl = thr/GetSample();
}

void PulseFinder::SetSample(int thr)
{
	sample = thr;
}

double PulseFinder::GetThrPeak()
{
	return thr_pek;
}

double PulseFinder::GetThrHyst()
{
	return r_hys;
}

int PulseFinder::GetThrEvent()
{
	return thr_evt;
}

int PulseFinder::GetThrSignal()
{
	return thr_sig;
}

int PulseFinder::GetShapingTime()
{
	return shape;
}

double PulseFinder::GetBW()
{
	return bw;
}

int PulseFinder::GetWinLen()
{
	return wl;
}

int PulseFinder::GetSample()
{
	return sample;
}

int PulseFinder::GetNTrg()
{
	return ntrg;
}

void PulseFinder::SetDebug(int on)
{
	debug = on;
}

void PulseFinder::SetGraph(int on)
{
	graph = on;
}

void PulseFinder::SetVerbosity(int on)
{
	verb = on;
}

template<typename T>
T PulseFinder::MaxV(std::vector<T> &V)
{
	T ret = V.at(0);
	for (int i = 1; i < V.size(); i++)
		if (V.at(i) > ret)
			ret = V.at(i);
	return ret;
}

template<typename T>
T PulseFinder::MinV(std::vector<T> &V)
{
	T ret = V.at(0);
	for (int i = 1; i < V.size(); i++)
		if (V.at(i) < ret)
			ret = V.at(i);
	return ret;
}

template<typename K, typename V>
V PulseFinder::MaxM(std::map<K, V> const &M)
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
	std::cout << "ID of the max is " << max << std::endl;
	return ret;
}

template<typename K, typename V>
V PulseFinder::MinM(std::map<K, V> const &M)
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
	std::cout << "ID of the min is " << min << std::endl;
	return ret;
}
