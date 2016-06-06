#include "EventReco.h"

EventReco::EventReco(std::string cfn, PMTData *PMT, int ent, int bin, int rwm, int trg, int id, int evt)
{
	SetConfig(cfn);
	RWM = rwm;			//rwm is RWM  pos
	TRG = trg;			//trg is trigger number
	EVT = evt;			//evt is event number
	ID = id;			//id is pmt id
	tPeak = bin;			//bin is position of peak in bins
	np = int(el*pc*0.01);		//np is the Pulse position of peak
	Pulse = new double[el];
	PMT->GetEntry(ent);
	for (int i = 0; i < el; i++)
	{
		if (i+tPeak-np < 0)
			Pulse[i] = PMT->Data[i+tPeak-np+el];		//i == np -> tPeak
		else if (i+tPeak-np >= 40000)
			Pulse[i] = PMT->Data[i+tPeak-np-el];		//i == np -> tPeak
		else Pulse[i] = PMT->Data[i+tPeak-np];		//i == np -> tPeak
	}
}

void EventReco::LoadGraph()
{
	gPulse = new TGraph(el);
	for (int i = 0; i < el; i++)
		gPulse->SetPoint(i, bw*(i-np), Pulse[i]);
}

void EventReco::LoadDeriv()
{
	gDeriv = new TGraph(el);
	gDeriv->SetPoint(0, *(gPulse->GetX()), *(gPulse->GetY()));
	for (int i = 1; i < el; i++)
		gDeriv->SetPoint(i, bw*(i-np), Pulse[i]-Pulse[i-1]);
}

void EventReco::FillN(TNtuple *nT)
{
	nPulse = nT;
	nPulse->Fill(GetBL(), GetPK(), bw*(GettVL()-np), GetCH(), GetEN(), GetCFD(), GetZC(), GetTOF());	
}

void EventReco::LoadN()
{			//ordering is vital!
	SetBL();
	SetPK();
	SetVL();
	SetCFD();
	SetZC();
	SetCH();
	SetEN();
	SetTOF();
}

void EventReco::Print()
{
	std::cout << "Trigger\t" << GetTRG();
	std::cout << "\tID\t" << GetID();
	std::cout << "\tEvent\t" << GetEVT() << std::endl;
	std::cout << "Baseline \t" << GetBL() << std::endl;
	std::cout << "Peak     \t" << GetPK() << std::endl;
	std::cout << "Peak time\t" << bw*GettPK() << std::endl;
	std::cout << "Valley   \t" << GetVL() << std::endl;
	std::cout << "Valley t \t" << bw*GettVL() << std::endl;
	std::cout << "tCFD     \t" << GetCFD() << std::endl;
	std::cout << "Zero Cr  \t" << GetZC() << std::endl;
	std::cout << "Time O F \t" << GetTOF() << std::endl;
	std::cout << "Charge   \t" << GetCH() << std::endl;
	std::cout << "Energy   \t" << GetEN() << std::endl;
}

TGraph *EventReco::GetGraph()
{
	return gPulse;
}

TGraph *EventReco::GetDeriv()
{
	LoadDeriv();
	return gDeriv;
}

void EventReco::SetBL()		//Set to zero the avg of the first 20 points
{
	double x, y, sum = 0;
	for (int i = 0; i < 20; i++)
		sum += Pulse[i];

	Baseline = sum/20.0;

	for (int i = 0; i < gPulse->GetN(); i++)
	{
		gPulse->GetPoint(i, x, y);
		gPulse->SetPoint(i, x, y-Baseline);
	}
}

void EventReco::SetPK()
{
/*	double max = Pulse[0];
	int p = 0;
	int o = int(el*pc*0.01);
	for (int i = o-20; i < o+20; i++)
	{
		std::cout << "i " << i << std::endl;
		if (Pulse[i] > max)
		{
			max = Pulse[i];
			p = i;
		}
	}
	Peak = max;
	tPeak = p;	*/
	Peak = Pulse[np];
}

void EventReco::SetVL()
{
	double min = Pulse[0];
	int p = 0;
	for (int i = 0; i < el; i++)
	{
		if (Pulse[i] < min)
		{
			min = Pulse[i];
			p = i;
		}
	}
	Valley = min;
	tValley = p;
}

void EventReco::SetCH()
{
	double x, y, sum = 0;
	for (int i = np-2; i < np+3; i++)
		sum += Pulse[i];
	Charge = sum;
}

void EventReco::SetEN()
{
	double x, y, sum = 0;
	for (int i = int(tCFD/bw)-10; i < int((tCFD+ZeroC)/bw)+10; i++)
		sum += Pulse[i-tPeak+np];
	Energy = sum;
}

void EventReco::SetCFD()
{
	double thr = 0.25;
	int x1, x2 = tPeak - el*pc*0.01;	//Define this better
	double diff = fabs((1-thr)*Peak);

	for (x1 = np; x1 > 0; x1--)
		if (diff < fabs(Peak-Pulse[x1])) break;

	tCFD = CI(Pulse, ++x1, Peak*thr, 1);
	tCFD += tPeak - np;
	tCFD *= bw;
/*
	double x, y;
	for (int i = 0; i < gPulse->GetN(); i++)
	{
		gPulse->GetPoint(i, x, y);
		gPulse->SetPoint(i, x+bw*tPeak-tCFD, y);
	}
*/
}

void EventReco::SetZC()
{
	double thr = 0.05;
	int x1, x2 = tPeak + el*(1-pc)*0.01;	//Define this better
	if (x2 > el-1) x2 = el-1;

	double diff = fabs((1-thr)*Peak);

	for (x1 = tValley; x1 > np; x1--)
		if (diff > fabs(Peak-Pulse[x1])) break;

	ZeroC = CI(Pulse, ++x1, Peak*thr, 1);
	ZeroC += tPeak - np;
	ZeroC *= bw;
	double x, y;
	for (int i = 0; i < gPulse->GetN(); i++)
	{
		gPulse->GetPoint(i, x, y);
		gPulse->SetPoint(i, x+bw*tPeak-ZeroC, y);
	}

	ZeroC -= tCFD;
}

void EventReco::SetTOF()
{
	TOF = tCFD - bw*RWM;
}

void EventReco::SetBW(double binw)
{
	bw = binw;
}

void EventReco::SetEL(int evnl)
{
	el = evnl;
}

void EventReco::SetPC(double perc)
{
	pc = perc;
}


double EventReco::CI(double *data, int x2, double fmax, double tau)
{
	x2 -= 1;
	double xi = (fmax - data[x2])/(data[x2+1] - data[x2]);
	double a3 = 0.5*data[x2] - (1./6.)*data[x2-1] + (1./6.)*data[x2+2] - 0.5*data[x2+1];
	double a2 = (-data[x2] + 0.5*data[x2+1] + 0.5*data[x2-1]);
	double a1 = (-0.5*data[x2] - (1./6.)*data[x2+2] + data[x2+1] - 1./3.*data[x2-1]);
	double a0 = data[x2];

	for (int rec = 0; rec < 5; rec++)
		 xi += (fmax - a0 - a1*xi - a2*xi*xi - a3*xi*xi*xi)/(a1 + 2*a2*xi + 3*a3*xi*xi);

	return tau*(x2+xi);		//change to bw?
}

void EventReco::SetConfig(std::string cfn)
{
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
			if (var == "binwid") SetBW(val);
			if (var == "evlength") SetEL(val);
			if (var == "peakpos") SetPC(val);
		}
	}
	fin.close();
}

double EventReco::GetBL()
{
	return Baseline;
}

double EventReco::GetPK()
{
	return Peak;
}

double EventReco::GettPK()
{
	return tPeak;
}

double EventReco::GetVL()
{
	return Valley;
}

double EventReco::GettVL()
{
	return tValley;
}

double EventReco::GetCH()
{
	return Charge;
}

double EventReco::GetEN()
{
	return Energy;
}

double EventReco::GetCFD()
{
	return tCFD;
}

double EventReco::GetZC()
{
	return ZeroC;
}

double EventReco::GetTOF()
{
	return TOF;
}

int EventReco::GetTRG()
{
	return TRG;
}

int EventReco::GetID()
{
	return ID;
}

int EventReco::GetEVT()
{
	return EVT;
}

double EventReco::GetBW()
{
	return bw;
}

int EventReco::GetEL()
{
	return el;
}

double EventReco::GetPC()
{
	return pc;
}
