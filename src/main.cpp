#include <iostream>
#include <string>

#include "Utils.h"
#include "PulseFinder.h"
#include "EventReco.h"

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
	std::cout << ssL.str() << std::endl;
	system(ssL.str().c_str());

	ssL.str("");
	ssL.clear();
	ssL << "find ";
	ssL << fold;
       	ssL << " -maxdepth 1 -name \"Data*.root*\" > rootfiles.list";
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

	Utils *Handler = Utils::GetUtils(); 
	Handler->SetDir(fold);
	Handler->SetConfig("config");

	for (int i = 0; i < RF_In.size(); i++)
	{
		
		if (Handler->OpenIns(RF_In.at(i)))
			if (Handler->OpenOuts(RF_In.at(i)))
				PulseFinder PF;
//			{
//				std::cout << "f0 " << Handler->InFile->IsOpen() << std::endl;
//				Handler->Close();
//				std::cout << "f1\n";
//			}

	}

//	ssL.str("");
//	ssL.clear();
//	ssL << "./Merge ";
//	if (fold[fold.length()-1] == '/') ssL << fold << "result/";
//	else ssL << fold << "/result/";
//	std::cout << ssL.str() << std::endl;
//	system(ssL.str().c_str());

	return 0;
}
