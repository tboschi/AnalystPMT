#include <iostream>
#include <string>

#include "PulseFinder.h"

int main(int argc, char** argv)
{
	std::string fold = ".";
	if (argc > 1) fold = argv[1];
	std::stringstream ssL;
	std::string Line, Input;
	std::vector<std::string> RF_In;

	ssL.str("");
	ssL.clear();
	ssL << "mkdir -p ";
	ssL << fold << "/result";
	system(ssL.str().c_str());

	ssL.str("");
	ssL.clear();
	ssL << "find ";
	ssL << fold;
       	ssL << " -maxdepth 1 -name \"*.root*\" > rootfiles.list";
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

	PulseFinder *PF;

	for (int i = 0; i < RF_In.size(); i++)
	{
		PF = new PulseFinder(fold);
		
		PF->SetConfig("config");
		PF->OpenIns(RF_In.at(i));
		PF->OpenOuts(RF_In.at(i));

		PF->GetTree();
		PF->LoopTrg();

		delete PF;
	}

	return 0;
}
