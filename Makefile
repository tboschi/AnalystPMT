SRC = src/
INC = inc/
PMT = PMTData

CFLAG = -pthread -m64 -I/usr/include/root -L/usr/lib64/root -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lMathCore -lThread -pthread -lm -ldl -rdynamic
#CFLAG = -pthread -m64 -I/grid/fermiapp/products/uboone/root/v5_34_04/Linux64bit-2-6-2-5/include -L/grid/fermiapp/products/uboone/root/v5_34_04/Linux64bit-2-6-2-5/lib -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

IN = $(SRC)main.cpp
OUT = Main

all: pulse merge super select

pulse: cpinclude
	g++ $(IN) $(SRC)$(PMT).C $(SRC)Utils.cpp $(SRC)EventReco.cpp $(SRC)PulseFinder.cpp -fPIC -I $(INC) $(CFLAG) -o $(OUT)

merge: cpinclude
	g++ $(SRC)$(PMT).C $(SRC)Utils.cpp $(SRC)Merge.cpp -fPIC -I $(INC) $(CFLAG) -o Merge

super: cpinclude
	g++ $(SRC)$(PMT).C $(SRC)Utils.cpp $(SRC)SuperMerge.cpp -fPIC -I $(INC) $(CFLAG) -o SuperMerge

select: cpinclude
	g++ $(SRC)Event.C $(SRC)Selection.cpp -fPIC -I $(INC) $(CFLAG) -o Select

clean:
	rm -f inc/*
	rm -f lib/*

cpinclude:
	cp $(SRC)$(PMT).h inc/
	cp $(SRC)Event.h inc/
	cp $(SRC)Utils.h inc/
	cp $(SRC)PulseFinder.h inc/
	cp $(SRC)EventReco.h inc/


