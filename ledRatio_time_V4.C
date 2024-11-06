TCanvas *c1;

int NID[2]={748,260};
int NROW[2]={34,20};
int NCOL[2]={22,13};
char* nameDET[6]={"Ecal_N","Ecal_S","Hcal_N","Hcal_S","Pres_N","Pres_S"};
char* nameEHP[3]={"Ecal","Hcal","Pres"};
char* nameNS[2]={"N","S"};

Int_t   mNRun=0, mRefe=0, mRefh=0, mRunnum=0 , totalluminrun=0, totalfindluminrun = 0;
int period=-1;
//int   mRun[300];
//float mPed[300][2][748];
//float mPedRms[300][2][748];
float mLed[300][2][748];
float mLedRatio[300][2][748];
float mLedRatioErr[300][2][748];
float mLedRms[300][2][748];
float mRatio[300][2][748];
float mSlope[10][2][748];
float mIntercept[10][2][748];
float gaincorr047[2][748];
float Periodfactor[10][2][748];
float periodendlumin[10];
int   mFlag[300][2][748];
int   mMaxDay=0, mMinDay=23000;
int   runlist[500];
int   luminrunlist[20000];
int   allluminrunlist[20000];
int   alllumin[20000];
int depchtoid[2][24][32];
int mRefeRun[10] ;
int   startrun = 22359013, endrun = 23108024;
double luminSum=0, lumin_goodgain = 0;
Double_t lumin[20000];
int plotx_runnumber[5000];
double plotx_lumin[5000];
double plotperiodx_lumin[5000];
int goodgain_index=0;

//blue->green for 0->0.5 and green->red for 0.5->1.0
//int getColor(float v, int n=128){ 
//  const float PI=3.141592654;
//  static int first=0;
//  if(first==0){
//    for(int i=0; i<n; i++){
//      int id=5000+i;
//      TColor *color = gROOT->GetColor(id);
//      if(!color) color=new TColor(id,0,0,0);    
//      float r=0.0,g=0.0,b=0.0;
//      float vv = float(i+0.5)/float(n);
//      if     (vv<=0.25){b=1.0;                       g=sin(vv/0.25*PI/2.0);}
//      else if(vv<=0.50){b=sin(vv/0.25*PI/2.0);       g=1.0;          }
//      else if(vv<=0.75){g=1.0;                       r=sin((vv-0.5)/0.25*PI/2.0);}
//      else if(vv<=1.00){g=sin((vv-0.5)/0.25*PI/2.0); r=1.0;          }
//      color->SetRGB(r,g,b);
//    }
//    first=1;
//  } 
//  if(v>=1.0) return 5000+n-1;
//  if(v<0.0) return 5000;
//  return (int)(v*n + 5000);
//}

void readtxt(int thisrun, int irun, int thisperiod){
  int day=thisrun/1000;
  if(mMaxDay<day) mMaxDay=day;
  // if(mMinDay>day) mMinDay=day;
  char line[300],file[100],name[100],dummy[100];
  sprintf(file,"led_files2/ped.%d.txt",thisrun);
  printf("Opening %s\n",file);
  FILE* F=fopen(file,"r");
  int run,flag;
  float ped,prms,led,lederr,lrms,ratio;
  mRun[irun]=thisrun;
  fgets(line,300,F);
  //printf("%s",line);
  while(fgets(line,300,F) != NULL){
    sscanf(line,"%d %s %f %f %f %f %f %d\n",
	   &run,name,&ped,&prms,&led,&lederr,&lrms,&ratio,&flag);
    int det=-1,id;
    char cdet[3];
    sscanf(name,"%2s%03d",cdet,&id);
    if(cdet[0]=='E'){
      if(cdet[1]=='N') det=0;
      if(cdet[1]=='S') det=1;
    } else {det = -1;}
    //printf("%s cdet=%s det=%d id=%d\n",name,cdet,det,id);
    if(det>=0){
      //mPed[irun][det][id]=ped;
      //mPedRms[irun][det][id]=prms;
      if ((led == 0)&& (irun > 0)) {led=mLed[irun-1][det][id];}
      mLed[irun][det][id]=led;
      if (period >= 0) {mLedRatio[irun][det][id] = led * 1.0 / mLed[mRefeRun[thisperiod]][det][id]; mLedRatioErr[irun][det][id] = lederr * 1.0 / mLed[mRefeRun[thisperiod]][det][id];}
	if ((det == 0) && (id == 1)) cout << "run:" << run << "period:" << thisperiod << "LED ratio:" << mLedRatio[irun][det][id] << "LED error:" << mLedRatioErr[irun][det][id] << endl;
	//mLedRatio[irun][det][id] = 1.0/ led;
      //mLedRms[irun][det][id]=lrms;
      //mRatio[irun][det][id]=run;
      //mFlag[irun][det][id]=flag;
      //printf("%d %s %d %03d %8.2f %8.2f %10.2f %8.2f %8.4f %3d\n",
      //     run,name,det,id,ped,prms,led,lrms,ratio,flag);
    }
  }	  
}

void ini_readdir(){

  //gSystem->Exec("rm ledruns.txt; ls /star/u/akio/fcs2022/www/led2/*/ped.*.txt > ledruns.txt");
  FILE* F=fopen("ledruns.txt","r");
  char line[200];
  int run;
  period = -1;
  //cout << runnum <<"	"<< ref<< "	" << refh<< endl;
  while(fgets(line,200,F) != NULL){
    sscanf(line,"led_files2/ped.%d.txt",&run);
    if (run < startrun) continue;
    runlist[mNRun] = run;
    if (run > endrun) continue;
    
    if ((run >= 22359013) && (run < 23005043) && (period == -1)) {period = 0; mRefeRun[period] = mNRun;}
    if ((run >= 23005043) && (run < 23066056) && (period == 0)) {period = 1; mRefeRun[period] = mNRun;}
    //if ((run >= 23048036) && (run < 23066056) && (period == 1)) {period = 2; mRefeRun[period] = mNRun;}
    if ((run >= 23066056) && (run < 23073059) && (period ==  1)) {period = 2; mRefeRun[period] = mNRun;}
    if ((run >= 23073059) && (run < 23080057) && (period ==  2)) {period = 3; mRefeRun[period] = mNRun;}
    if ((run >= 23080057) && (run < 23087057) && (period ==  3)) {period = 4; mRefeRun[period] = mNRun;}
    if ((run >= 23087057) && (run < 23094050) && (period ==  4)) {period = 5; mRefeRun[period] = mNRun;}
    if ((run >= 23094050) && (run < 23101043) && (period ==  5)) {period = 6; mRefeRun[period] = mNRun;}
    if ((run >= 23101043) && (period ==  6)) {period = 7; mRefeRun[period] = mNRun;}
    readtxt(run,mNRun,period);
    //if(run==ref) mRefe=mNRun;
    mNRun++;
  }
  //printf("Found %d runs, refrun=%d is at %d \n",mNRun,ref,mRefe);
}


void readlumin(int good_gain)
{

	FILE* F=fopen("integralLumi.txt","r");
	char line[2000];
	int index, runid,userunindex;
	float bbcW, bbcEW, zdcW, zdcEW;
	userunindex = 0;
	index = 0;
	ofstream runluminFile;
	runluminFile.open("run_lumin.txt");
	runluminFile << "run number	sum (time * BBCC rate)" << endl;
	fgets(line,500,F);
	while((fgets(line,200,F) != NULL))
	{

		sscanf(line,"%d %f %f %f %f", &runid, &bbcW, &bbcEW , &zdcW, &zdcEW);
		allluminrunlist[index] = runid;
		if (good_gain == runid) {lumin_goodgain = zdcEW;}
		alllumin[index] = zdcEW;
		//if (good_gain == runid) {lumin_goodgain = zdcW;}
		//alllumin[index] = zdcW;
		index = index +1;
		
		

		if ((runid >= startrun)&&(runid < endrun)) 
		//if ((runid >= startrun)) 
		{

			luminrunlist[userunindex] = runid;
			lumin[userunindex] = zdcEW;
			runluminFile << runid <<"	" << zdcEW << endl;
			//lumin[userunindex] = zdcW;
			//runluminFile << runid <<"	" << zdcW << endl;
			userunindex = userunindex + 1;

		}
		else 
		{

			runluminFile << runid <<"       " << zdcEW << endl;
			//runluminFile << runid <<"       " << zdcW << endl;
		}

	}
	totalluminrun = userunindex;
	totalfindluminrun = index;

}


void predict_gain()
{

	char line[100];
	int run, index , eh, ns, dep, ch, det, id, crt, slt;
	float LEDratio, oldgain, thislumin, led_goodgain, led_this, predict_gain;
	FILE* F=fopen("physicsrunlist.txt","r");
	while (fgets(line, 100, F) != NULL)
	{

		sscanf(line,"%i",&run);
		index = -1;
		bool notfound= true; 
    		if ((run >= 22359013) && (run < 23005043) ) {period = 0;}
    		if ((run >= 23005043) && (run < 23066056) ) {period = 1;}
    		//if ((run >= 23048036) && (run < 23066056) ) {period = 2;}
    		if ((run >= 23066056) && (run < 23073059) ) {period = 2;}
    		if ((run >= 23073059) && (run < 23080057) ) {period = 3;}
    		if ((run >= 23080057) && (run < 23087057) ) {period = 4;}
    		if ((run >= 23087057) && (run < 23094050) ) {period = 5;}
    		if ((run >= 23094050) && (run < 23101043) ) {period = 6;}
    		if ((run >= 23101043)) {period = 7;}

		ofstream outgaincorr;
		outgaincorr.open(Form("fcsgaincorr_%i_ini.txt",run));
		while (notfound)
		{

			index = index +1;
			if (index == totalfindluminrun) {cout << "can't find the run:"<< run << endl; notfound = false; continue;} 
			if (run == allluminrunlist[index])
			{

				notfound = false;
				thislumin = alllumin[index];
				//cout << "this lumin:" << thislumin << "  23047001 lumin:" << lumin_goodgain << endl;
				for (int det = 0; det < 2; det = det +1)
				{
					for (int dep =0; dep < 24; dep = dep +1)
					{
					for (int ch = 0; ch < 32; ch = ch +1)
					{
						if (depchtoid[det][dep][ch] == -1) continue;
						id = depchtoid[det][dep][ch];
						led_goodgain = (mSlope[4][det][id] * lumin_goodgain + mIntercept[4][det][id]);
						led_this = (mSlope[period][det][id] * thislumin + mIntercept[period][det][id]);
						led_this = led_this;
						if ((det == 1) && (id == 189)) cout << run << " led_goodgain:" << led_goodgain << "  led_this:" << led_this <<endl;
						if (led_this < 0) cout << "0  "<<det << "  " << dep <<"  " << ch << "	"<< id << "  LED ratio (23047001):" << led_goodgain << "  LED this run:" << led_this << "  slope:" << mSlope[period][det][id] <<  "old good gain (23047001):" << gaincorr047[det][id] << "  predicted gain:" << (1.0 * gaincorr047[det][id] * led_goodgain / led_this) << endl;
						predict_gain = 1.0 * gaincorr047[det][id] * led_goodgain / led_this;
						if ((predict_gain < 20)&&(predict_gain > 0)) {outgaincorr << "0  "<<det << "  " << dep <<"  " << ch << "  " << (1.0 * gaincorr047[det][id] * led_goodgain / led_this) << endl;} 
						else {outgaincorr << "0  "<<det << "  " << dep <<"  " << ch << "  " << 20.00 << endl;}

					}
					}

				}


			}


		}
	}	





}




//void history(int plt=0, int runnum=22346048, int ref=22335028){    
//void history(int plt=0, int runnum=22346048, int ref=22357032){    
void ledRatio_time_V4(int good_gain=23074018, const char *inputgaincorrfile = "fcsgaincorr.txt"){    
	cout << "inputgaincorrfile:" << inputgaincorrfile << endl;

  gSystem->Load("libPhysics");
  gSystem->Load("St_base");
  gSystem->Load("StChain");
  memset(lumin,0,sizeof(lumin));
  gSystem->Load("StUtilities");
  gSystem->Load("StEvent");
  gSystem->Load("StFcsDbMaker");
  
  StFcsDbMaker* mFcsDbMkr = new StFcsDbMaker();
  StFcsDb* mFcsDb = dynamic_cast<StFcsDb*>(mFcsDbMkr->GetDataSet("fcsDb"));
  mFcsDb->Init();

  c1 = new TCanvas("c1","LED Histry",50,0,1500,1200);
  gStyle->SetLabelSize(0.06,"xy");
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  TFile* outFile = new TFile("LED_ratio4.root","recreate");
  
  //memset(mRun,0,sizeof(mRun));
  //memset(mPed,0,sizeof(mPed));
  //memset(mPedRms,0,sizeof(mPedRms));
  memset(depchtoid,-1,sizeof(depchtoid));
  memset(mLed,0,sizeof(mLed));
  memset(mLedRatio,0,sizeof(mLedRatio));
  memset(lumin,0,sizeof(lumin));
  memset(plotx_lumin,0,sizeof(plotx_lumin));
  memset(mSlope,0,sizeof(mSlope));
  memset(mIntercept,0,sizeof(mIntercept));
  memset(Periodfactor,1,sizeof(Periodfactor));
  //memset(mLedRms,0,sizeof(mLedRms));
  //memset(mRatio,0,sizeof(mRatio));
  //memset(mFlag,0,sizeof(mFlag));
  
  ini_readdir();
  mRefeRun[9] = mNRun;

  cout<<"mNRun:"<<mNRun<<endl;
 
//  readtxt(23005043,mRefeRun[0]);
//  readtxt(23066056,mRefeRun[1]);
//  readtxt(23073059,mRefeRun[2]);
//  readtxt(23080057,mRefeRun[3]);
//  readtxt(23087057,mRefeRun[4]);
//  readtxt(23094050,mRefeRun[5]);
//  readtxt(23101043,mRefeRun[6]);
// 
//  cout << "ECal ref run index:"<<mRefe<<"	example LED det = 0, id = 1:"<<
//mLed[mRefe][0][1]<< endl;
//
  readlumin(good_gain);
//
//  cout << "total lumin run index:"<<totalluminrun<<endl;
  int luminrunindex=0;
  period = 0;
  bool found;
  //mNRun = 3;
  for (int i = 0; i < mNRun; i=i+1)
  {

	//readtxt(runlist[i],i);
	found = true;
	cout<< runlist[i] << endl;
	while(found && (luminrunindex < totalluminrun))
	{

		//cout << "lumin run:" << luminrunlist[luminrunindex] << "  led run:" << runlist[i] << endl;
		if (luminrunlist[luminrunindex] < runlist[i]) 
			//{cout << "luminrun:"<<luminrunlist[luminrunindex]<<endl; luminrunindex= luminrunindex +1;}
			{luminrunindex= luminrunindex +1;}
		else
		{
			found = false;
			//if (luminrunindex == 0) {plotx_lumin[i] = lumin[luminrunindex];} else {plotx_lumin[i] = (lumin[luminrunindex-1]+lumin[luminrunindex]) * 1.0 / 2;}
			plotx_lumin[i] = lumin[luminrunindex];
			plotx_runnumber[i] = luminrunlist[luminrunindex];
			if ((period == 0) && (luminrunlist[luminrunindex] >= 23005043)) {period = 1; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 1) && (luminrunlist[luminrunindex] >= 23048036)) {period = 2; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 2) && (luminrunlist[luminrunindex] >= 23066056)) {period = 3; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 3) && (luminrunlist[luminrunindex] >= 23073059)) {period = 4; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 4) && (luminrunlist[luminrunindex] >= 23080057)) {period = 5; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 5) && (luminrunlist[luminrunindex] >= 23087057)) {period = 6; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 6) && (luminrunlist[luminrunindex] >= 23094050)) {period = 7; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 7) && (luminrunlist[luminrunindex] >= 23101043)) {period = 8; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			if ((period == 8) && (luminrunlist[luminrunindex] >= 23108024)) {period = 9; periodendlumin[period-1] = lumin[luminrunindex]; cout << "period end lumin:" << periodendlumin[period-1] << endl;}
			cout << period << "  "  << luminrunlist[luminrunindex]<< "	"<<lumin[luminrunindex]<<endl;
			luminrunindex= luminrunindex +1;
		}

	}


  }

  cout << "period 0 end lumin:" << periodendlumin[0] << endl;
  cout << "period 1 end lumin:" << periodendlumin[1] << endl;

  char line[100];
  int eh, ns, dep, ch, det, id, crt, slt;
  float oldgain;
  //FILE* ingaincorr = fopen("fcsgaincorr_047_3a.txt","r");
  //FILE* ingaincorr = fopen("fcsgaincorr_027_new23a.txt","r");
  FILE *ingaincorr = fopen(inputgaincorrfile,"r");
  //FILE *ingaincorr = fopen("fcsgaincorr.txt","r");
  while (fgets(line,100,ingaincorr) != NULL)
  {
  
  	sscanf(line, "%d %d %d %d %f", &eh, &ns, &dep, &ch, &oldgain);
  	mFcsDb->getIdfromDep(eh,ns,dep,ch,det,id,crt,slt);
  	gaincorr047[det][id] = oldgain;
  	depchtoid[ns][dep][ch] = id;
  
  }	


  TGraphErrors* LEDratio_vs_lumin[2][748];
  //TGraph* LEDratio_vs_lumin;
  TGraph* LEDratio_slopeN, LEDratio_slopeS, LEDratio_slopeDist;
  float slope_N[748], tower_id[748] , slope_S[748];
  Double_t ledRatio_tower[700];
  Double_t plotx_Err[700];
  Double_t ledRatio_tower_Err[700];
  char title[200],name[200];
  Double_t dist[1496],allslope[1496];
  int nUsedPoints;

  for (int period = 0; period < 9; period = period +1)
  //for (int period = 0; period < 2; period = period +1)
  {
  
	  memset(plotperiodx_lumin,0,sizeof(plotperiodx_lumin));
	  memset(plotx_Err,0,sizeof(plotx_Err));

	cout << "Period:" << period << endl;
	  for (int det = 0; det < 2; det = det +1)
	  {
	
	
		for (int id=0; id < 748; id = id +1)
		{
	
	  		//ofstream towerOutfile;
			//if (det == 0) towerOutfile.open(Form("towerOutFile/Ntower_%i.txt",id));
			//if (det == 1) towerOutfile.open(Form("towerOutFile/Stower_%i.txt",id));
			//towerOutfile << "run index	run number	LED ratio" << endl;
			nUsedPoints = 0;
	  		memset(ledRatio_tower,0,sizeof(ledRatio_tower));
	  		memset(ledRatio_tower_Err,0,sizeof(ledRatio_tower_Err));
	  		memset(plotperiodx_lumin,0,sizeof(plotperiodx_lumin));
			for (int runindex=mRefeRun[period]; runindex < mRefeRun[period+1]; runindex = runindex +1)
			{
	
				if ((mLedRatio[runindex][det][id] > 1.1)||(mLedRatio[runindex][det][id] < 0.02)) continue;		
				ledRatio_tower[nUsedPoints] = mLedRatio[runindex][det][id];
				ledRatio_tower_Err[nUsedPoints] = mLedRatioErr[runindex][det][id];
				plotperiodx_lumin[nUsedPoints] = plotx_lumin[runindex];
				++nUsedPoints;
				//cout << ledRatio_tower[runindex - mRefeRun[period]] << "	"<< plotperiodx_lumin[runindex - mRefeRun[period]] << endl;
				//towerOutfile << runindex << "	"<< plotx_runnumber[runindex] <<"	" << ledRatio_tower[runindex] <<endl; 
			}
	
			//towerOutfile.close();
				
		      sprintf(name,"LedRatio%d_%d_%d",period,det,id);
		      if (det == 0) {sprintf(title,"LED Ratio for North ECal tower %d for period %d; sum of ZDC E&W (*10^{9}); LED ratio",id,period+1);}
		      if (det == 1) {sprintf(title,"LED Ratio for South ECal tower %d for period %d; sum of ZDC E&W (*10^{9}); LED ratio",id,period+1);}
			LEDratio_vs_lumin[det][id] = new TGraphErrors(nUsedPoints,plotperiodx_lumin,ledRatio_tower, plotx_Err, ledRatio_tower_Err);
			LEDratio_vs_lumin[det][id]->SetName(name);	
			LEDratio_vs_lumin[det][id]->SetTitle(title);	
			TF1 *linear=new TF1("linear","[0]+x*[1]");
			LEDratio_vs_lumin[det][id]->Fit("linear");
			LEDratio_vs_lumin[det][id]->Write();
			if ((period <= 1)) {Periodfactor[period][det][id] = 1;}
//			mSlope[period][det][id] = linear->GetParameter(1) * Periodfactor[period][det][id];
//			mIntercept[period][det][id] = linear->GetParameter(0) * Periodfactor[period][det][id];
//			if (det == 0) {slope_N[id] = linear->GetParameter(1) * Periodfactor[period][det][id];}
//			if (det == 1) {slope_S[id] = linear->GetParameter(1) * Periodfactor[period][det][id];}
			mSlope[period][det][id] = linear->GetParameter(1) ;
			mIntercept[period][det][id] = linear->GetParameter(0) ;
			if (det == 0) {slope_N[id] = linear->GetParameter(1) ;}
			if (det == 1) {slope_S[id] = linear->GetParameter(1) ;}
			tower_id[id] = id;
			StThreeVectorD towerXYZ = mFcsDb->getStarXYZ(det,id);
			dist[det * 2 +id] = sqrt(pow(towerXYZ.x(),2)+pow(towerXYZ.y(),2));
			allslope[det * 2 +id] = linear->GetParameter(1);
			//if ((period >0)&&(period < 8)) 
				//{Periodfactor[period+1][det][id] = (mSlope[period][det][id] * periodendlumin[period] + mIntercept[period][det][id]) * Periodfactor[period][det][id];
				//if (Periodfactor[period+1][det][id] < 0.03) {Periodfactor[period+1][det][id] = 0.03;}
				cout<<det<< "	"<<id<<"	Slope=: "<<linear->GetParameter(1)<< "Slope after factor:"<< mSlope[period][det][id]  <<endl;
				//}	

		}
	
	  }

	cout << "Period 0 det 0 id 639 Slope:" << mSlope[0][0][639] << "   Intercept:" << mIntercept[0][0][639]  << endl;
	cout << "Period 1 det 0 id 639 Slope:" << mSlope[1][0][639] << "   Intercept:" << mIntercept[1][0][639]  << endl;
	cout << "Period 2 det 0 id 639 Slope:" << mSlope[2][0][639] << "   Intercept:" << mIntercept[2][0][639]  << endl;
	cout << "Period 3 det 0 id 639 Slope:" << mSlope[3][0][639] << "   Intercept:" << mIntercept[3][0][639]  << endl;
	cout << "Period 4 det 0 id 639 Slope:" << mSlope[4][0][639] << "   Intercept:" << mIntercept[4][0][639]  << endl;
	cout << "Period 5 det 0 id 639 Slope:" << mSlope[5][0][639] << "   Intercept:" << mIntercept[5][0][639]  << endl;
	cout << "Period 6 det 0 id 639 Slope:" << mSlope[6][0][639] << "   Intercept:" << mIntercept[6][0][639] << endl;


  LEDratio_slopeN = new TGraph(748,tower_id,slope_N);
  LEDratio_slopeN->SetName(Form("LEDratio_slopeN_period%d",period));
  LEDratio_slopeN->SetTitle(Form("LED Ratio linear fit slope (North ECal, period %d);tower id;slope",period));
  LEDratio_slopeN->Write(); 
  LEDratio_slopeS = new TGraph(748,tower_id,slope_S);
  LEDratio_slopeS->SetName(Form("LEDratio_slopeS_period%d",period));
  LEDratio_slopeS->SetTitle(Form("LED Ratio linear fit slope (South ECal, period %d);tower id;slope",period));
  LEDratio_slopeS->Write(); 
  }
//
//  LEDratio_slopeDist = new TGraph(1496,dist,allslope);
//  LEDratio_slopeDist->SetName("LEDratio_slopeDist");
//  LEDratio_slopeDist->SetTitle("LED Ratio linear fit slope vs tower distance to beam;distance [cm];slope");
//  LEDratio_slopeDist->Write();
//
  predict_gain();


}




