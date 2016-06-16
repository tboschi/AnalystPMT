SRC = src/
INC = inc/
ROOTDIR = ../ANNIEDAQ/ToolDaq/root/
ROOTFLAG := $(shell root-config --cflags)
ROOTLIBS := $(shell root-config --glibs)

#CFLAG = $(ROOTFLAG) $(ROOTLIBS)
#CFLAG = -pthread -m64 -I $(ROOTDIR)include/root -L $(ROOTDIR)lib/root -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lMathCore -lThread -pthread -lm -ldl -rdynamic
#CFLAG = -pthread -I/usr/local/include -L/usr/local/lib -lGui -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lMathCore -lThread -lMultiProc -pthread -lm -ldl -rdynamic
CFLAG = -pthread -m64 -I/usr/include/root -L/usr/lib64/root -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lMathCore -lThread -pthread -lm -ldl -rdynamic
#CFLAG = -pthread -m64 -I/grid/fermiapp/products/uboone/root/v5_34_04/Linux64bit-2-6-2-5/include -L/grid/fermiapp/products/uboone/root/v5_34_04/Linux64bit-2-6-2-5/lib -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

IN = $(SRC)main.cpp
OUT = Main

all: clean cpinclude pulse merge

pulse: clean cpinclude
	g++ $(IN) src/PMTData.C $(SRC)PulseFinder.cpp $(SRC)PulseUtils.cpp $(SRC)PulseGraph.cpp $(SRC)EventReco.cpp -fPIC -I $(INC) $(CFLAG) -o PulseFinder

merge: clean cpinclude
	g++ src/Merge.cpp -fPIC -I $(INC) $(CFLAG) -o Merge

clean:
	rm -f $(OUT)
	rm -f libPMTH*
	rm -f inc/*
	rm -f lib/*

cpinclude:
	cp $(SRC)PMTData.h inc/
	cp $(SRC)PulseFinder.h inc/
	cp $(SRC)EventReco.h inc/


