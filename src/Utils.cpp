#include "Utils.h"
/*
double Utils::fThrPeak = 0.02;
double Utils::fHysRatio = 0;
int Utils::iThrEvent = 10;
int Utils::iThrSignal = 5;
int Utils::iShape = 4;
int Utils::iVerbosity = 0;
bool Utils::bDebug = 1;
bool Utils::bGraph = 1;
double Utils::fBinWidth = 0.002;
double Utils::fPercent = 20;
int Utils::iBuffer = 8000;
int Utils::iSample = 4;
int Utils::iEvtLength = 500;
unsigned int Utils::iNumTrg = 1;
*/
Utils *Utils::in_Utils = NULL;

Utils *Utils::GetUtils()
{
	if (!in_Utils)
		in_Utils = new Utils;
	return in_Utils;
}

void Utils::SetDir(std::string dir)
{
	DirName = dir;
	std::size_t fnd = DirName.find('/');
	if (DirName.at(DirName.length()-1) == '/')
		DirName.erase(DirName.end()-1, DirName.end());
	if (GetVerbosity() > 2) std::cout << "Directory " << DirName << std::endl;
}

//Maths stuff
int Utils::LocMaximum(float *data, int j, double &max)
{
	max = data[j];
	int time = j;
	for (int i = j-1; i < j+GetShapingTime()+1; i++)
	{
		if (GetVerbosity() > 6)
			std::cout << i << "\t" << data[i] << std::endl;
		if ((i < 0) || (i > GetBuffer())) continue;
		if (data[i] > max)
		{
			max = data[i];
			time = i;
		}
	}
	return time;
}

double Utils::Integrate(float *data, int j, double bw)		//Charge like
{
	double sum = 0;
//	for (int i = j-shape/2; i < j+1+shape/2; i++)
	for (int i = j-2; i < j+3; i++)
	{
		if ((i < 0) || (i > GetBuffer())) continue;
		sum += data[i];
	}
	return sum > 0 ? bw*sum : 0;
//	return bw*sum;
}

double Utils::CI(float *data, int x2, double fmax, double tau)
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

//Files handler
bool Utils::OpenIns(std::string fname)
{
	InFile = new TFile(fname.c_str(), "READ");		//open file -> should be a loop on vector
	if (InFile->IsZombie()) 
	{
		std::cout << "ZOMBIE!\n";
		Close();
		return false;
	}
	else
	{
		GetTree();
		return true;
	}
}

bool Utils::OpenOuts(std::string fname)
{
	std::string noExt = GetBaseName(fname);		//Create result output file
	std::string outName = DirName + "/result/d" + noExt + ".dat";

	if (!Exist(outName))
	{
		if (fout.is_open()) fout.close();
		fout.open(outName.c_str(), std::fstream::out);
		std::cout << "Results are saved in: " << outName << std::endl;
		outName = DirName + "/result/r" + noExt + ".root";
		std::cout << "Root is saved in: " << outName << std::endl;
		OutFile = new TFile(outName.c_str(), "RECREATE");	//Open output root file
		if (OutFile->IsZombie()) 
		{
			std::cout << "Not able to open ROOT file. Skipping...\n\n";
			Close();
			return false;
		}
		else
		{
			OutFile->mkdir("2D");
			OutFile->mkdir("GR");
			return true;
		}
	}
	else 
	{
		std::cout << "File already analysed. Skipping...\n\n";
		return false;
	}

}

void Utils::GetTree()
{
	bool IsPMTData = false;
	InFile->cd();
	TIter nextkey(InFile->GetListOfKeys());		//Get first tree on each file
	TKey *key;
	TTree *TreeData;
	while ((key = (TKey*)nextkey()))
	{
		TObject *obj = key->ReadObj();
		if (obj->IsA()->InheritsFrom(TTree::Class()))
		{
			TreeData = (TTree*)obj;		//assign found tree to TreeData
			std::string TreeName = "PMTData";
			if (TreeData->GetName() == TreeName)
				IsPMTData = true;
			break;
		}
	}

	//Create object PMT
	if (IsPMTData) PMT = new PMTData(TreeData);
	else PMT = 0;
}

bool Utils::Exist(std::string &fileName)
{
	std::ifstream infile(fileName.c_str());
	if (!GetDebug()) return infile.good();		//commented for debuggin
	else return false;
}

std::string Utils::GetBaseName(std::string fn)
{
	fn.erase(fn.begin()+fn.find('.'), fn.end());
	std::size_t fnd = fn.find('/');
	while (fnd != std::string::npos)
	{
		fn.erase(fn.begin(), fn.begin()+fn.find('/')+1);
		fnd = fn.find('/');
	}
	if (GetVerbosity()) std::cout << "Root file ready: " << fn << std::endl;
	return fn;
}

/*
void Utils::Print1DStats()
{
//	fout << "Trigger: " << trg << std::endl;
	double tt = 0.000001*GetNumTrg()*GetBinWidth()*GetBuffer();
	fout << std::endl << std::endl;
	fout << "Event above " << GetThrEvent() << ":\t";
        fout << hEvent->Integral(GetThrEvent(), 60) << std::endl;
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

void Utils::Print2DStats(int trg, int evt)
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
*/

//Cleanse

void Utils::Close()
{
	InFile->Close();
	OutFile->Close();
	fout.close();
//	delete GetPMT();
}

//Variables input
void Utils::SetConfig(std::string cfn)
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
			if (var == "debug") SetDebug(val);
			if (var == "verbosity") SetVerbosity(val);
			if (var == "hysratio") SetHysRatio(val);
			if (var == "thrpeak") SetThrPeak(val); 
			if (var == "trhevent") SetThrEvent(val);
			if (var == "thrsignal") SetThrSignal(val); 
			if (var == "shapingtime") SetShapingTime(val);
			if (var == "sample") SetSample(val);
			if (var == "binwidth") SetBinWidth(val);
			if (var == "buffer") SetBuffer(val);
			if (var == "printgraph") SetPrintGraph(val);
			if (var == "percent") SetPercent(val);
			if (var == "eventlength") SetEvtLength(val);
		}
	}
	std::cout << "Verbosity is " << GetVerbosity() << std::endl;
	if (GetPrintGraph())
		std::cout << "A individual pulse graph will be printed out of every " << GetPrintGraph() << std::endl;
	if (GetDebug())
		std::cout << "Debugging mode on\nWARNING: plots will be overwritten" << std::endl;
	else std::cout << "Debugging mode off\nNew plots will be created" << std::endl;
	fin.close();
}

//Set Functions

void Utils::SetDebug(bool emp)
{
	Utils::bDebug = emp;
}

void Utils::SetVerbosity(int emp)
{
	Utils::iVerbosity = emp;
}

void Utils::SetHysRatio(double emp)
{
	fHysRatio = emp;
}

void Utils::SetThrPeak(double emp)
{
	fThrPeak = emp;
}

void Utils::SetThrEvent(int emp)
{
	iThrEvent = emp;
}

void Utils::SetThrSignal(double emp)
{
	fThrSignal = emp;
}

void Utils::SetShapingTime(int emp)
{
	iShape = emp;
}

void Utils::SetSample(int emp)
{
	iSample = emp;
}

void Utils::SetBinWidth(double emp)
{
	fBinWidth = GetSample()*emp;
}

void Utils::SetBuffer(int emp)
{
	iBuffer = emp/GetSample();
}

void Utils::SetPrintGraph(int emp)
{
	iGraph = emp;
}

void Utils::SetPercent(double emp)
{
	fPercent = emp;
}

void Utils::SetEvtLength(int emp)
{
	iEvtLength = emp;
}

void Utils::SetNumTrg(int emp)
{
	iNumTrg = emp;
}

//Get Functions
bool Utils::GetDebug()
{
	return bDebug;
}

int Utils::GetVerbosity()
{
	return iVerbosity;
}

double Utils::GetHysRatio()
{
	return fHysRatio;
}

double Utils::GetThrPeak()
{
	return fThrPeak;
}

int Utils::GetThrEvent()
{
	return iThrEvent;
}

double Utils::GetThrSignal()
{
	return fThrSignal;
}

int Utils::GetShapingTime()
{
	return iShape;
}

int Utils::GetSample()
{
	return iSample;
}

double Utils::GetBinWidth()
{
	return fBinWidth;
}

int Utils::GetBuffer()
{
	return iBuffer;
}

int Utils::GetPrintGraph()
{
	return iGraph;
}

double Utils::GetPercent()
{
	return fPercent;
}

int Utils::GetEvtLength()
{
	return iEvtLength;
}

unsigned int Utils::GetNumTrg()
{
	return iNumTrg;
}

PMTData *Utils::GetPMT()
{
	return PMT;
}

//Mapper. Leave at bottom
Coord Utils::Mapper(int ID)
{
	Coord Pos;
	switch (ID)
	{
		case 1:
			Pos.x = 0;
			Pos.z = 3;
			break;
		case 2:
			Pos.x = 0;
			Pos.z = 2;
			break;
		case 3:
			Pos.x = 0;
			Pos.z = 1;
			break;
		case 4:
			Pos.x = 1;
			Pos.z = 0;
			break;
		case 5:
			Pos.x = 2;
			Pos.z = 0;
			break;
		case 6:
			Pos.x = 3;
			Pos.z = 0;
			break;
		case 7:
			Pos.x = 4;
			Pos.z = 0;
			break;
		case 8:
			Pos.x = 5;
			Pos.z = 0;
			break;
		case 9:
			Pos.x = 6;
			Pos.z = 0;
			break;
		case 10:
			Pos.x = 7;
			Pos.z = 1;
			break;
		case 11:
			Pos.x = 7;
			Pos.z = 2;
			break;
		case 12:
			Pos.x = 7;
			Pos.z = 3;
			break;
		case 13:
			Pos.x = 1;
			Pos.z = 1;
			break;
		case 14:
			Pos.x = 2;
			Pos.z = 1;
			break;
		case 15:
			Pos.x = 2;
			Pos.z = 2;
			break;
		case 16:
			Pos.x = 1;
			Pos.z = 2;
			break;
		case 17:
			Pos.x = 3;
			Pos.z = 1;
			break;
		case 18:
			Pos.x = 4;
			Pos.z = 1;
			break;
		case 19:
			Pos.x = 4;
			Pos.z = 2;
			break;
		case 20:
			Pos.x = 3;
			Pos.z = 2;
			break;
		case 21:
			Pos.x = 5;
			Pos.z = 1;
			break;
		case 22:
			Pos.x = 6;
			Pos.z = 1;
			break;
		case 23:
			Pos.x = 6;
			Pos.z = 2;
			break;
		case 24:
			Pos.x = 5;
			Pos.z = 2;
			break;
		case 25:
			Pos.x = 1;
			Pos.z = 3;
			break;
		case 26:
			Pos.x = 2;
			Pos.z = 3;
			break;
		case 27:
			Pos.x = 2;
			Pos.z = 4;
			break;
		case 28:
			Pos.x = 1;
			Pos.z = 4;
			break;
		case 29:
			Pos.x = 3;
			Pos.z = 3;
			break;
		case 30:
			Pos.x = 4;
			Pos.z = 3;
			break;
		case 31:
			Pos.x = 4;
			Pos.z = 4;
			break;
		case 32:
			Pos.x = 3;
			Pos.z = 4;
			break;
		case 33:
			Pos.x = 5;
			Pos.z = 3;
			break;
		case 34:
			Pos.x = 6;
			Pos.z = 3;
			break;
		case 35:
			Pos.x = 6;
			Pos.z = 4;
			break;
		case 36:
			Pos.x = 5;
			Pos.z = 4;
			break;
		case 37:
			Pos.x = 1;
			Pos.z = 5;
			break;
		case 38:
			Pos.x = 2;
			Pos.z = 5;
			break;
		case 39:
			Pos.x = 2;
			Pos.z = 6;
			break;
		case 40:
			Pos.x = 1;
			Pos.z = 6;
			break;
		case 41:
			Pos.x = 3;
			Pos.z = 5;
			break;
		case 42:
			Pos.x = 4;
			Pos.z = 5;
			break;
		case 43:
			Pos.x = 4;
			Pos.z = 6;
			break;
		case 44:
			Pos.x = 3;
			Pos.z = 6;
			break;
		case 45:
			Pos.x = 5;
			Pos.z = 5;
			break;
		case 46:
			Pos.x = 6;
			Pos.z = 5;
			break;
		case 47:
			Pos.x = 6;
			Pos.z = 6;
			break;
		case 48:
			Pos.x = 5;
			Pos.z = 6;
			break;
		case 49:
			Pos.x = 0;
			Pos.z = 4;
			break;
		case 50:
			Pos.x = 0;
			Pos.z = 5;
			break;
		case 51:
			Pos.x = 0;
			Pos.z = 6;
			break;
		case 52:
			Pos.x = 1;
			Pos.z = 7;
			break;
		case 53:
			Pos.x = 2;
			Pos.z = 7;
			break;
		case 54:
			Pos.x = 3;
			Pos.z = 7;
			break;
		case 55:
			Pos.x = 4;
			Pos.z = 7;
			break;
		case 56:
			Pos.x = 5;
			Pos.z = 7;
			break;
		case 57:
			Pos.x = 6;
			Pos.z = 7;
			break;
		case 58:
			Pos.x = 7;
			Pos.z = 6;
			break;
		case 59:
			Pos.x = 7;
			Pos.z = 5;
			break;
		case 60:
			Pos.x = 7;
			Pos.z = 4;
			break;
		default:
			Pos.x = -10;
			Pos.z = -10;
			break;
	}
	return Pos;
}
