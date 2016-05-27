//------------ Funzioni per l'analisi dei segnali -----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//---------------------------------copia di funzionicomuni.cpp-----------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

 //======================== BL Subtraction ================================

double BLS (double *sig, int nsamples, int s1, int s0=0){
	if (s1<s0){int sa=s1; s1=s0; s0=sa;}
	s0=abs(s0);
	s1=abs(s1);
	double avg=0.;  
	for (int i=s0; i<s1; i++) avg+=sig[i];
	avg= avg/(s1-s0);
	for (int i=0; i<nsamples; i++)sig[i]-=avg;
	return (avg);
}

double BL (double *sig, int nsamples, int s1, int s0=0){
	if (s1<s0){int sa=s1; s1=s0; s0=sa;}
	double avg=0.;  
	for (int i=s0; i<s1; i++) avg+=sig[i];
	avg= avg/(s1-s0);
	return (avg);
}

double stdevBL (double *sig, int nsamples, int s1, int s0=0)
{
	if (s1<s0){int sa=s1; s1=s0; s0=sa;}
	double avg=0.; 
	double stdev=0; 
	for (int i=s0; i<s1; i++) avg+=sig[i];
	avg= avg/(s1-s0);
   
	for (int i =s0; i<s1; i++){ stdev=pow((sig[i]-avg),2);}
	stdev = sqrt(stdev/(s1-s0));
	return (stdev);
}

 //======================== Integrate ====================================

double Integrate (double*sig, int s0, int s1, int ns, double tauclk=0.)
{
	double integ=0.;
	if (s0<0) {s0=0;/* printf("Fn Integrate: !! Andavo Negativo");*/}
	if (s1<s0)
	{
		int ap=s0; s0=s1;
		s1=ap;
		printf("Fn Integrate: Swap s0,s1  -  OCCHIO!!\n");
	}
	if (s1>=ns)
	{
		s1=ns-1;
		/*printf("Fn Integrate: !! settato s1=ns\n");*/
	}
	for (int i=s0; i<=s1; i++)
	{
		integ+=sig[i];
	}
	return (integ*tauclk);
}

 //========================  FIND MAX ====================================

int FindNegMaxX(double *sig, int s1, int s0=0){
   // TThread::Printf("s0=%d - s1=%d",s0,s1);
	 double max=sig[s0];
	int xm=s0;
	for (int i=s0+1; i<s1; i++)
	{
		if (sig[i]<max)
		{
			max=sig[i];
			xm=i;
		}
	}
//	TThread::Printf("xm=%d",xm);
	return xm;
}

int FindFirstNegMaxX(double *sig, int s1, int s0=0)
{
   // TThread::Printf("s0=%d - s1=%d",s0,s1);
	double max=sig[s0];
	int flin=0;
	double TRSH = -20.0;
	int xm=s0;
	for (int i=s0+1; i<s1; i++)
	{ 
		TThread::Printf("i=%d   ;  sig[i]=%f\n",i, sig[i]);
		if (sig[i]<TRSH){flin=1;}
		if (sig[i]<max && flin==1) {max=sig[i]; xm=i;}
		if (flin==1 && sig[i]>TRSH)
		{
			TThread::Printf("xm=%d",xm);
			return xm;
		}
	}
	return sqrt(-1);
}

int FindFirstPosMaxX(double *sig, int s1, int s0=0)
{
   // TThread::Printf("s0=%d - s1=%d",s0,s1);
	double max=sig[s0];
	int flin=0;
	double TRSH = 5.0;
	int xm=s0;
	for (int i=s0+1; i<s1; i++)
	{ 
		TThread::Printf("i=%d   ;  sig[i]=%f\n",i, sig[i]);
		if (sig[i]>TRSH) flin=1;
		if (sig[i]>max && flin==1)
		{
			max=sig[i];
			xm=i;
		}
		if (flin==1 && sig[i]<TRSH)
		{
			TThread::Printf("xm=%d",xm);
			return xm;
		}
	}
	return sqrt(-1);
}


int FindPosMaxX(double *sig, int s1, int s0=0)
{
	double max=sig[s0];
	int xm=s0;
	for (int i=s0+1; i<s1; i++)
	{ 
		if (sig[i]>max)
		{
			max=sig[i];
			xm=i;
		}
	}
	return xm;
}

int FindPosMinX(double *sig, int s1, int s0=0)
{
	double min=sig[s0];
	int xm=s0;
	for (int i=s0+1; i<s1; i++)
	{ 
		if (sig[i]<min)
		{
			min=sig[i];
			xm=i;
		}
	}
	return xm;
}


//====================  Find Pickup ======================
int FindiPkup(double *sig, int s1, int s0=0){
	//find max and min value
	int imax=sig[s0];
	int imin=sig[s0];
	for (int i=s0+1; i<s1; i++)
	{
		if (sig[i]>sig[imax]) imax=i;
			if (sig[i]<sig[imin]) imin=i;
	}
	double mean = (sig[imax]+sig[imin])/2;
	int flag = 0;
	for (int i=s0+1; i<s1; i++)
	{ 
		if (sig[s0]<mean && sig[i]>mean) flag=1;
		if (sig[s0]<mean && sig[i]<mean && flag==1) return(i);
		if (sig[s0]>mean && sig[i]<mean) return(i);
		
	}
	return (0);
}

//===================================================================================================================
//===============================Importo le fn di Luigi==============================================================
//===================================================================================================================


//! L.Bardelli, 2009-09
//!
/*************************************************************/
// funzione "privata"
void ApplyRecursiveFilter(double a0, int N, double *a, double *b, int NSamples, double *datax, double *datay){
// signal will be: y[n]=a0*x[n]+sum a[k] x[k] + sum b[k] y[k]
/*----------------------------------------------*/
  int i=0, k=0;
  for(i=0;i<N;i++) // primo loop su Npunti	 
	{
	 datay[i]=a0*datax[i];
	 for(k=0;k<i;k++)
	datay[i] += a[k]*datax[i-k-1]+b[k]*datay[i-k-1];
	}
  for(i=N;i<NSamples;i++) //secondo loop. cambia l'indice interno.
	{
	 datay[i]=a0*datax[i];
	 for(k=0;k<N;k++)
	datay[i] += a[k]*datax[i-k-1]+b[k]*datay[i-k-1];
	}
}

#define DUEPI 6.28318530717958623

void ApplyRCLowPass(double rc, double tau_clk, int Nsamples, double *data_in, double *data_output){
  double f=1./(DUEPI*rc)*tau_clk;
  double x=exp(-DUEPI*f);
  double a0=1-x;
  double b=x;
  double a=0;
  ApplyRecursiveFilter(a0, 1, &a, &b, Nsamples, data_in, data_output);
}
void ApplyRCHighPass(double rc, double tau_clk, int Nsamples, double *data_in, double *data_output){
  double f=1./(DUEPI*rc)*tau_clk;
  double x=exp(-DUEPI*f);
  double a0=(1+x)/2.;
  double a1=-(1+x)/2.;
  double b1=x;
  ApplyRecursiveFilter(a0, 1, &a1, &b1, Nsamples, data_in, data_output);
}



//! L.Bardelli, 2009-09
//! see NIM-A, 2004
/*************************************************************/
// questo interpola cubico qualsiasi cosa...
double CubicInterpolation(double *data, int x2, double fmax,double tau_clk , int Nrecurr=3){
 /*
	NOTA: tutti i conti fatti con i tempi in "canali". aggiungero' il *tau_clk
	solo nel return.
  */
  /**** 2) normal CFD ***************************************************/
  double xi0=(fmax-data[x2])/(data[x2+1]-data[x2]);
  if(Nrecurr<=0) return tau_clk*(x2+xi0);
  
  /**** 3) approx successive. *******************************************/
  // scrivo il polinomio come a3*xi**3+a2*xi**2+a1*xi+a0
  // dove xi=tcfd-x2 (ovvero 0<xi<1)
  //								 3
  //   (1/2 y2 - 1/6 y1 + 1/6 y4 - 1/2 y3) xi
  //
  //							   2
  //		+ (-y2 + 1/2 y3 + 1/2 y1) xi
  //
  //		+ (- 1/2 y2 - 1/6 y4 + y3 - 1/3 y1) xi + y2

  double a3=0.5*data[x2]-(1./6.)*data[x2-1]+(1./6.)*data[x2+2]-0.5*data[x2+1];
  double a2=(-data[x2] + 0.5*data[x2+1] + 0.5*data[x2-1]);
  double a1=(- 0.5* data[x2] - 1./6. *data[x2+2]+ data[x2+1] - 1./3.* data[x2-1]);
  double a0=data[x2];
  double xi=xi0;
  for(int rec=0;rec<Nrecurr;rec++)
	{
	 xi += (fmax-a0-a1*xi-a2*xi*xi-a3*xi*xi*xi)/(a1+2*a2*xi+3*a3*xi*xi);
	}
  return tau_clk*(x2+xi);
}

//===================================================================================================================
//===================================================================================================================
//===================================================================================================================

//3/jun/2013 rev. TMGC corretto controllo estremi, aggiunto evlength, corretto fabs() per rendere polarity-independance

 double FindTimeCubicLeft(double threshold, int x0, double tau_clk, int nsamples, double *data, int Nrecurr=3, int evlength=0){
  // ricerca verso sinistra del punto x0 tale che x0 e' il precedente di tcfd
  if(x0<0 || x0>(evlength-1)) return sqrt(-1);

  int x1 = x0;

  int x2 = x0 - nsamples;
  if(x2<0) x2=0;

  double diff0 = fabs(data[x0]-threshold);

  for( ; x1>x2; x1--){
	if (diff0 < fabs(data[x0]-data[x1])) break;
  } 

  return CubicInterpolation(data, ++x1, threshold, tau_clk, Nrecurr);
 }



 double FindTimeCubicRight(double threshold, int x0, double tau_clk, int nsamples, double *data, int Nrecurr=3, int evlength=0) {
  // ricerca verso destra del punto x0 tale che x0 e' il precedente di tcfd
  if(x0<0 || x0>(evlength-1)) return sqrt(-1);

  int x1 = x0;

  int x2 = x0 + nsamples;
  if(x2>(evlength-1)) x2=evlength-1;

  double diff0 = fabs(data[x0]-threshold);

  for( ; x1<x2; x1++){
	if (diff0 < fabs(data[x0]-data[x1])) break;
  } 

  return CubicInterpolation(data, --x1, threshold, tau_clk, Nrecurr);
 }


//====== Trapezoidal filter as indicated in EBSS 2012 (ORNL) tutorial =====

 int PoleZeroCorr(double *data, int NSamples, double RC=1., double tauclk=0.) {
	double *data_corr = new double [NSamples];

	data_corr[0]=data[0];
	for(int i=1; i<NSamples; i++){
	 data_corr[i] = data_corr[i-1] + data[i] - data[i-1] * TMath::Exp(-tauclk/RC);
	}   

   for(int i=0; i<NSamples; i++){
	 data[i] = data_corr[i];
	}  

  delete [] data_corr;
  return 1;
}
											
 int TrapezoidalFilter(double *data, int NSamples, int k=0., int l=0.){
  double *S = new double[NSamples];
  for(int i=0; i<NSamples; i++) S[i]=0.;

  double sum1=0.;
  double sum2=0.;

  for (int n=0; n<NSamples; n++){
	  sum1=0.;
	  for(int i=n-k; i<n; i++){if(i>0){sum1+=data[i];}}
	  sum2=0.;
	  for(int i=n-l-k; i<n-l; i++){if(i>0){sum2+=data[i];}}
	  S[n]=(sum1-sum2)/k;  
  }

  for(int i=0; i<NSamples; i++) data[i]=S[i];
  delete [] S;

  return 1;
}


//====== Trapezoidal filter by Gab, recursive calculation =====

 double TrapezoidalFilterGab(double *data, int NSamples, int k=0., int l=0., double tauclk=0.){
  double *S = new double[NSamples];
  int n1 = k/tauclk;
  int n2 = n1+l/tauclk;
  int n3 = n1+n2;
  
  for(int i=0; i<NSamples; i++) S[i]=0.;

   for (int n=0; n<NSamples; n++){
	 double norm = 1./n1;  // without norm, the filter gains a factor of n1, due to the accumulator
	 if(n ==0)  S[n] = norm*data[n];  // zero padding
	 else if(n > 0 && n < n1)  S[n] = S[n-1] + norm*data[n];  // zero padding
	 else if(n > 0 && n < n2)  S[n] = S[n-1] + norm*(data[n] - data[n-n1]);  // zero padding
	 else if(n > 0 && n < n3)  S[n] = S[n-1] + norm*(data[n] - data[n-n1] - data[n-n2]);  // zero padding
	 else S[n] = S[n-1] + norm*(data[n] - data[n-n1] - data[n-n2] + data[n-n3]);
  }

  for(int i=0; i<NSamples; i++) data[i]=S[i];

   double Ene;
   int rawmaxX = FindPosMaxX(data, NSamples);
   Ene = data[rawmaxX];

  delete [] S;
  return Ene;
}



// Da sistemare - Non funziona--- Sistemato da Gab. c'era errore su indici ricorsivi
//
//y[i] = b1 * y[i] + a0 * data[i] + a1 * data[i-1] + data[i-1]/pz; invece che
//y[i] = b1 * y[i-1] + a0 * data[i] + a1 * data[i-1] + data[i-1]/pz;
//
//
//  z[i] = b1 * z[i] + a0 * y[i]; invece che
//   z[i] = b1 * z[i-1] + a0 * y[i];


//====== Semigaussian filter - From T.Davinsson SPES school lessons - CT 2011 =====

 double Semigaussian(double *data, int NSamples, double tau, double pz=500, int npoles=6){
   double Ene = 0.;

   //Single pole high pass filter with pole zero correction
   double b1 = TMath::Exp(-1./tau);
   double a0 = (1. + b1) / 2.;
   double a1 = - (1. + b1) / 2.;
   double *y = new double [NSamples];
   double *z = new double [NSamples];
   for (int i=1; i<NSamples; i++){y[i]=z[i]=0.;}

   double gain = 6.22575;

   y[0]=data[0];

   for (int i=1; i<NSamples; i++){
	y[i] = b1 * y[i-1] + a0 * data[i] + a1 * data[i-1] + data[i-1]/pz;
   }


   //n-pole low pass filter
   b1 = TMath::Exp(-1./tau);
   a0 = 1. - b1;
   for (int j=0; j<npoles; j++){
	for (int i=1; i<NSamples; i++) z[i] = b1 * z[i-1] + a0 * y[i];
	for (int k=1; k<NSamples; k++) y[k]=z[k];
   }

   for(int i=0; i<NSamples; i++) data[i]=z[i]*gain;

   int rawmaxX = FindPosMaxX(data, NSamples);
   Ene = data[rawmaxX];

  return Ene;
}


 double SemigaussianGab(double *data, int NSamples, double tau, double pz1=500, int npoles=6, double tauclk=0.){
   double Ene = 0.;

   //Single pole high pass filter with pole zero correction
   double pz = TMath::Exp(-tauclk/pz1);
   double b1 = TMath::Exp(-tauclk/tau);
   double a0 = b1;
   double a1 = -b1 * pz;
   double *y = new double [NSamples];
   double *z = new double [NSamples];
   for (int i=1; i<NSamples; i++){y[i]=z[i]=0.;}

   double gain = 6.22575;

   y[0]=data[0];

   for (int i=1; i<NSamples; i++){
	y[i] = b1 * y[i-1] + a0 * data[i] + a1 * data[i-1];
   }


   //n-pole low pass filter
   b1 = TMath::Exp(-tauclk/tau);
   a0 = 1. - b1;
   for (int j=0; j<npoles; j++){
	for (int i=1; i<NSamples; i++) z[i] = b1 * z[i-1] + a0 * y[i];
	for (int k=1; k<NSamples; k++) y[k]=z[k];
   }

   for(int i=0; i<NSamples; i++) data[i]=z[i]*gain;

   int rawmaxX = FindPosMaxX(data, NSamples);
   Ene = data[rawmaxX];

  return Ene;
}

//------------ FINE Funzioni per l'analisi dei segnali -----------------
