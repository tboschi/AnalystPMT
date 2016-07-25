#include "EventReco.h"

EventReco::EventReco(int ent, int bin, int rwm) :
	fBaseLine(0),
	fPeak(0),
	fValley(0),
	fCharge(0),
	fEnergy(0),
	ftCFD(0),
	fZeroC(0),
	fTOF(0)
{
	Utl = Utils::GetUtils();
	fBW = Utl->GetBinWidth();
	iEL = Utl->GetEvtLength();
	iEnt = ent;			//tree entry
	iPeak = bin;			//bin is position of peak in bins
	iRWM = rwm;			//rwm is RWM  pos

	PMT = Utl->GetPMT();
	PMT->GetEntry(GetEntry());

	Init(iEL);
//	PMT->GetEntry(ent);
	for (int i = 0; i < iEL; i++)
	{
		if (i+GettPeak()-NewP < 0)
			Pulse[i] = PMT->Data[i + GettPeak() - NewP + iEL];	//i == np -> tPeak
		else if (i+GettPeak()-NewP >= Utl->GetBuffer())
			Pulse[i] = PMT->Data[i + GettPeak() - NewP - iEL];
		else Pulse[i] = PMT->Data[i+GettPeak()-NewP];
	}
//	if (Utl->GetPrintGraph()) gPulse = new TGraph(iEL);
//	LoadParam();
}

EventReco::EventReco(TGraph* iG) 
{
	Utl = Utils::GetUtils();
	fBW = Utl->GetBinWidth();
	iEL = iG->GetN();
	iPeak = 0;
	iRWM = 0;

	Init(iEL);
	double x, y;
	for (int i = 0; i < iG->GetN(); i++)
	{
		iG->GetPoint(i, x, y);
		Pulse[i] = y;
	}
	NewP = Utl->LocMaximum(Pulse, NewP, fPeak); 
	iPeak = NewP;
}

EventReco::~EventReco()
{
	delete Pulse;
	delete gPulse;
}

void EventReco::Init(int length)
{
	fBaseLine = 0;
	fPeak = 0;
	fValley = 0;
	fCharge = 0;
	fEnergy = 0;
	fArea = 0;
	ftCFD = 0;
	fZeroC = 0;
	fTOF = 0;

	NewP = int(length*Utl->GetPercent()*0.01);	//np is the Pulse position of peak
	Pulse = new float[length];
	gPulse = new TGraph(length);
}

TGraph *EventReco::LoadGraph()
{
	for (int i = 0; i < iEL; i++)
//		gPulse->SetPoint(i, bw*(i-NewP), Pulse[i]);
		gPulse->SetPoint(i, fBW*(i-NewP+GettPeak())-GettCFD(), Pulse[i]);
	return gPulse;
}

TGraph *EventReco::LoadDeriv()
{
	gDeriv = new TGraph(iEL);
	gDeriv->SetPoint(0, *(gPulse->GetX()), *(gPulse->GetY()));
	for (int i = 1; i < iEL; i++)
		gDeriv->SetPoint(i, fBW*(i-NewP), Pulse[i]-Pulse[i-1]);
	return gDeriv;
}

float *EventReco::GetPulse()
{
	return Pulse;
}

void EventReco::LoadPulse(float *ext)
{
	for (int i = 0; i < iEL; i++)
		ext[i] = Pulse[i];
}

/*
void EventReco::FillN(TNtuple *nT)
{
	nPulse = nT;
	nPulse->Fill(GetBL(), GetPK(), bw*(GettVL()-NewP), GetCH(), GetEN(), GetCFD(), GetZC(), GetTOF());	
}
*/

void EventReco::LoadParam()
{			//order is vital!
	SetBaseLine();
	SetPeak();
	SetValley();
	SettCFD();
	SetZeroC();
	SetCharge();
	SetEnergy();
	SetArea();
	SetTOF();

	if (Utl->GetVerbosity() > 4) Print();
//	Print();
}

void EventReco::Print()
{
	std::cout << "Baseline \t" << GetBaseLine() << std::endl;
	std::cout << "Peak     \t" << GetPeak() << std::endl;
	std::cout << "Peak time\t" << fBW*GettPeak() << std::endl;
	std::cout << "Valley   \t" << GetValley() << std::endl;
	std::cout << "Valley t \t" << fBW*GettValley() << std::endl;
	std::cout << "tCFD     \t" << GettCFD() << std::endl;
	std::cout << "Zero Cr  \t" << GetZeroC() << std::endl;
	std::cout << "Time O F \t" << GetTOF() << std::endl;
	std::cout << "Charge   \t" << GetCharge() << std::endl;
	std::cout << "Energy   \t" << GetEnergy() << std::endl;
	std::cout << "Area     \t" << GetArea() << std::endl;
}

void EventReco::SetBaseLine()		//Set to zero the avg of the first 10 points
{
	double sum = 0;
	for (int i = 0; i < 10; i++)
		sum += Pulse[i];

	fBaseLine = sum/20.0;

	for (int i = 0; i < iEL; i++)
		Pulse[i] -= fBaseLine;
}

void EventReco::SetPeak()
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

	fPeak = Pulse[NewP];
//	fPeak = Utl->LocMaximum(Pulse, NewP, itPeak);
}

void EventReco::SetValley()
{
	double min = Pulse[NewP];
	int p = NewP;
	for (int i = NewP; i < iEL; i++)
	{
		if (Pulse[i] < min)
		{
			min = Pulse[i];
			p = i;
		}
	}
	fValley = min;
	iValley = p;
}

void EventReco::SetCharge()
{
	fCharge = Utl->Integrate(Pulse, NewP, fBW);
}

void EventReco::SetEnergy()
{
	double x, y, sum = 0;
	int count;
	for (int i = int(GettCFD()/fBW)-1; i < int((GettCFD()+GetZeroC())/fBW)+1; i++)
	{
		count = i-GettPeak()+NewP;
		if (count < iEL)
			sum += Pulse[count];
	}
	fEnergy = sum*fBW;
}

void EventReco::SetArea()
{
	double x, y, sum = 0;
	for (int i = 0; i < iEL; i++)
		sum += fabs(Pulse[i]);
	fArea = sum*fBW;
}

void EventReco::SettCFD()
{
	double thr = 0.25;
	int x1;		//Run in Pulse

	double diff = fabs((1-thr)*GetPeak());

	for (x1 = NewP; x1 > 0; x1--)
		if (diff < fabs(GetPeak()-Pulse[x1])) break;

	ftCFD = Utl->CI(Pulse, ++x1, GetPeak()*thr, 1);
	ftCFD += GettPeak() - NewP;
	ftCFD *= fBW;

	double x, y;
	/*
	for (int i = 0; i < gPulse->GetN(); i++)
	{
		gPulse->GetPoint(i, x, y);
		gPulse->SetPoint(i, x+fBW*GettPeak()-GettCFD(), y);
	}*/
}

void EventReco::SetZeroC()
{
	double thr = 0.05;
	int x1;		//Run in pulse

	double diff = fabs((1-thr)*GetPeak());

	for (x1 = GettValley(); x1 > NewP; x1--)
		if (diff > fabs(GetPeak()-Pulse[x1])) break;

	fZeroC = Utl->CI(Pulse, ++x1, GetPeak()*thr, 1);
	fZeroC += GettPeak() - NewP;
	fZeroC *= fBW;
	fZeroC -= GettCFD();
}

void EventReco::SetTOF()
{
	fTOF = GettCFD() - fBW*iRWM;
}

/*
void EventReco::SetBW(double binw)
{
	bw = binw*sample;
}

void EventReco::SetEL(int evnl)
{
	el = evnl;
}

void EventReco::SetWL(int evnl)
{
	wl = evnl/sample;
}

void EventReco::SetSample(int evnl)
{
	sample = evnl;
}

void EventReco::SetPC(double perc)
{
	pc = perc;
}
*/
double EventReco::GetBaseLine()
{
	return fBaseLine;
}

double EventReco::GetPeak()
{
	return fPeak;
}

int EventReco::GettPeak()
{
	return iPeak;
}

double EventReco::GetValley()
{
	return fValley;
}

int EventReco::GettValley()
{
	return iValley;
}

double EventReco::GettP2V()
{
	return fBW*(GettValley()-NewP);
}

double EventReco::GetCharge()
{
	return fCharge;
}

double EventReco::GetEnergy()
{
	return fEnergy;
}

double EventReco::GetArea()
{
	return fArea;
}

double EventReco::GettCFD()
{
	return ftCFD;
}

double EventReco::GetZeroC()
{
	return fZeroC;
}

double EventReco::GetTOF()
{
	return fTOF;
}

int EventReco::GetEntry()
{
	return iEnt;
}
