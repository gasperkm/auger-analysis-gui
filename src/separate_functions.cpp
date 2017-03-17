#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"
#if OFFVER == 0
  #include "../include/OfflineIncludeOld.h"
#elif OFFVER == 1
  #include "../include/OfflineIncludeNew.h"
#endif

#include <stdio.h>
#include <stdlib.h>

// Separate functions -----------------------------------------
void GetTime(int intime, char *outtime)
{
   time_t rawtime;
   struct tm * timeinfo;
   if(intime < 0)
      time(&rawtime);
   else
      rawtime = (time_t)intime;
   timeinfo = localtime(&rawtime);
   sprintf(outtime, "%s", asctime(timeinfo));
   int len = strlen(outtime);
   if(len) outtime[len-1] = 0;
}

void remove_ext(char *inname, char *outname)
{
  char ctemp[256];
  for(int i = 0; i < (int)strlen(inname); i++)
  {
    if( (inname[i] == '.') && (i > (int)(strlen(inname)-6)) )
    {
      ctemp[i] = '\0';
      sprintf(outname, "%s", ctemp);
      break;
    }
    else
      ctemp[i] = inname[i];
  }

  if(DBGSIG)
    printf("remove_ext(): Outfile = %s\n", outname);
}

void remove_from_last(char *inname, char search, char *outname)
{
   char ctemp[256];
   int searchpos = -1;
   for(int i = (int)strlen(inname); i >= 0; i--)
   {
      if(inname[i] == search)
      {
         searchpos = i;
	 break;
      }
   }

   for(int i = 0; i < searchpos; i++)
      ctemp[i] = inname[i];

   ctemp[searchpos] = '\0';
   sprintf(outname, "%s", ctemp);

   if(DBGSIG)
      printf("remove_from_last(): Outfile = %s\n", outname);
}

void remove_before_last(char *inname, char search, char *outname)
{
   char ctemp[256];
   int searchpos = -1;
   for(int i = (int)strlen(inname); i >= 0; i--)
   {
      if(inname[i] == search)
      {
         searchpos = i;
	 break;
      }
   }

   int k = 0;
   for(int i = searchpos+1; i < (int)strlen(inname); i++)
   {
      ctemp[k] = inname[i];
      k++;
   }

   ctemp[k] = '\0';
   sprintf(outname, "%s", ctemp);

   if(DBGSIG)
      printf("remove_before_last(): Outfile = %s\n", outname);
}

string remove_from_first(string input, string toremove, string replace)
{
   int size = toremove.size();
   int pos = input.find(toremove);

   if(pos >= 0)
   {
      return input.replace(pos, size, replace);
   }
   else
      return input.replace(0, size, replace);
}

void SeqNumber(int innum, int maxnum, char *outstr)
{
   int zeronum = 5;

   // Check how many zeroes we need to add to get sequential numbers
   if( (maxnum > 0) && (maxnum < 1000) )
      zeronum = 2;
   else if( (maxnum >= 1000) && (maxnum < 10000) )
      zeronum = 3;
   else if( (maxnum >= 10000) && (maxnum < 100000) )
      zeronum = 4;
   else if( (maxnum >= 100000) && (maxnum < 1000000) )
      zeronum = 5;

   // Make the sequence number depending on the number of zeroes
   if(zeronum == 2)
   {
      if(innum < 10)
         sprintf(outstr, "00%d", innum);
      else if( (innum >= 10) && (innum < 100) )
         sprintf(outstr, "0%d", innum);
      else if( (innum >= 100) && (innum < 1000) )
         sprintf(outstr, "%d", innum);
   }
   else if(zeronum == 3)
   {
      if(innum < 10)
         sprintf(outstr, "000%d", innum);
      else if( (innum >= 10) && (innum < 100) )
         sprintf(outstr, "00%d", innum);
      else if( (innum >= 100) && (innum < 1000) )
         sprintf(outstr, "0%d", innum);
      else if( (innum >= 1000) && (innum < 10000) )
         sprintf(outstr, "%d", innum);
   }
   else if(zeronum == 4)
   {
      if(innum < 10)
         sprintf(outstr, "0000%d", innum);
      else if( (innum >= 10) && (innum < 100) )
         sprintf(outstr, "000%d", innum);
      else if( (innum >= 100) && (innum < 1000) )
         sprintf(outstr, "00%d", innum);
      else if( (innum >= 1000) && (innum < 10000) )
         sprintf(outstr, "0%d", innum);
      else if( (innum >= 10000) && (innum < 100000) )
         sprintf(outstr, "%d", innum);
   }
   else if(zeronum == 5)
   {
      if(innum < 10)
         sprintf(outstr, "00000%d", innum);
      else if( (innum >= 10) && (innum < 100) )
         sprintf(outstr, "0000%d", innum);
      else if( (innum >= 100) && (innum < 1000) )
         sprintf(outstr, "000%d", innum);
      else if( (innum >= 1000) && (innum < 10000) )
         sprintf(outstr, "00%d", innum);
      else if( (innum >= 10000) && (innum < 100000) )
         sprintf(outstr, "0%d", innum);
      else if( (innum >= 100000) && (innum < 1000000) )
         sprintf(outstr, "%d", innum);
   }
}

int CheckFormat(const char *infile)
{
   TFile *tempfile = new TFile(infile, "READ");
   if(tempfile->IsOpen())
   {
      if( (tempfile->GetListOfKeys()->Contains("recEvent")) || (tempfile->GetListOfKeys()->Contains("eventInfo")) )
      {
         if(DBGSIG > 1) cout << "Input file " << infile << " is in ADST format." << endl;
	 tempfile->Close();
	 delete tempfile;
	 return 1;
      }
      else
      {
         tempfile->Close();
	 delete tempfile;
	 return -1;
      }
   }
   else
   {
      delete tempfile;
      return -1;
   }
}

int CheckMvaFormat(const char *infile)
{
   TFile *tempfile = new TFile(infile, "READ");
   if(tempfile->IsOpen())
   {
      if( (tempfile->GetListOfKeys()->Contains("recEvent")) || (tempfile->GetListOfKeys()->Contains("eventInfo")) )
      {
         if(DBGSIG > 1) cout << "Input file " << infile << " is in ADST format." << endl;
	 tempfile->Close();
	 delete tempfile;
	 return 1;
      }
// NEW
      else if( (tempfile->GetListOfKeys()->Contains("TreeOldS1")) || (tempfile->GetListOfKeys()->Contains("TreeNewS1")) )
//      else if( (tempfile->GetListOfKeys()->Contains("TreeOldS1_mean")) || (tempfile->GetListOfKeys()->Contains("TreeNewS1_mean")) )
      {
         if(DBGSIG > 1) cout << "Input file " << infile << " is in MVA rewritten ADST format." << endl;
	 tempfile->Close();
	 delete tempfile;
	 return 2;
      }
// NEW
      else if( (tempfile->GetListOfKeys()->Contains("TreeS1")) || (tempfile->GetListOfKeys()->Contains("TreeS1")) )
//      else if( (tempfile->GetListOfKeys()->Contains("TreeS1_mean")) || (tempfile->GetListOfKeys()->Contains("TreeS1_mean")) )
      {
         if(DBGSIG > 1) cout << "Input file " << infile << " is in MVA rewritten ADST format and will be used as input for MVA analysis." << endl;
	 tempfile->Close();
	 delete tempfile;
	 return 3;
      }
      else
      {
         tempfile->Close();
	 delete tempfile;
	 return -1;
      }
   }
   else
   {
      delete tempfile;
      return -1;
   }
}

// Int to string conversion
string IntToStr(int nr)
{
   stringstream ss;
   ss << nr;
   return ss.str();
}

// Double to string conversion
string DblToStr(double nr, int prec)
{
   stringstream ss;
   ss.precision(prec);
   ss << fixed << nr;
   return ss.str();
}

// Create sin^2\theta on an angle, degree = true will take angle in degrees
double SinSquare(double input, bool degree)
{
   if(degree)
      return sin(input*TMath::Pi()/180.)*sin(input*TMath::Pi()/180.);
   else
      return sin(input)*sin(input);
}

// Create asin(sqrt(\theta)) on an angle, degree = true will return angle in degrees
double AsinSqrt(double input, bool degree)
{
   if(degree)
      return asin(sqrt(input))*180./TMath::Pi();
   else
      return asin(sqrt(input));
}

string GetType(int type)
{
   if(type == 1)
      return "_mean";
   else if(type == 2)
      return "_neg";
   else if(type == 3)
      return "_pos";
   else
      return "_error";
}

/*
void TimeEstimate(clock_t stopw0, time_t time0, float progress, char *retEstim, int offset)
{
   // Calculate the remaining time
   clock_t clkt1;
   char ctemp[512];

   clkt1 = clock() - stopw0;
   if(DBGSIG) printf("TimeEstimate(): Startclock = %d, Midclock (%f) = %f (%d), starttime = %d, curtime = %d\n", (int)stopw0, progress, (float)clkt1/CLOCKS_PER_SEC, (int)clkt1, (int)time0, (int)time(NULL));
   GetTime((int)(100.*((float)clkt1/CLOCKS_PER_SEC)/progress+(int)time0+offset), ctemp);
   sprintf(retEstim, "Estimated end time: %s", ctemp);
}

void TimeEstimateNew(int nr, clock_t stopw0, time_t time0, int rX, int rY, int rZ, int xWait, int yWait, int zWait, char *retEstim)
{
   clock_t clkt1;
   char ctemp[512];
   double timeSec;
   double averMeasTime;

   clkt1 = clock() - stopw0;
   if(nr == -1)
      return;
   else if(nr == 0)
      averMeasTime = (double)clkt1/CLOCKS_PER_SEC;
   else
      averMeasTime = (averMeasTime + (double)clkt1/CLOCKS_PER_SEC)/2.0;

   // calculate the time of one row
   timeSec = rX*(xWait + averMeasTime);
   // calculate the time of a surface scan
   timeSec = timeSec + rY*(timeSec + yWait);
   // calculate the time of a zscan
   if(rZ == 1)
      timeSec = timeSec + zWait;
   else
      timeSec = timeSec + rZ*(timeSec + zWait);

   GetTime((int)timeSec+(int)time0, ctemp);
   sprintf(retEstim, "Estimated end time: %s", ctemp);

   printf("TimeEstimateNew(): Average time of measurement = %lf, Measurement time = %lf, Finishing time = %s\n", averMeasTime, timeSec, ctemp);
}

void NormateSet(int file, int nrpoint, double *min, double *max, double *setCount, double *setAcc)
{
   int count = 0;

   // Find minimal value in set and subtract the offset
   *min = TMath::MinElement(nrpoint, setAcc);
   for(int i = 0; i < nrpoint; i++)
      setAcc[i] -= *min;

   // Find maximal value in set and normate to 1
   *max = TMath::MaxElement(nrpoint, setAcc);
   for(int i = 0; i < nrpoint; i++)
   {
      count = file - nrpoint + i;
      setCount[count] = setAcc[i]/(*max);
      if(DBGSIG) printf("NormateSet(): Integral check 2 (i=%d,za=%d,j=%d): %lf\t\%lf\n", count, i, nrpoint, setCount[count], setAcc[i]/(*max));
   }
}

// Estimate the next point, depending on the set of points beforehand (least squares fit) and return the error
double PointEstimate(int nrp, double *points)
{
   double accx = 0, accy = 0, accxy = 0, accx2 = 0;
   double A, B;
   double esty;

   for(int i = 0; i < nrp; i++)
   {
      accx += points[2*i];
      accy += points[2*i+1];
      accxy += points[2*i]*points[2*i+1];
      accx2 += points[2*i]*points[2*i];
   }

   A = (accx2*accy - accx*accxy)/(nrp*accx2 - accx*accx);
   B = (nrp*accxy - accx*accy)/(nrp*accx2 - accx*accx);

   esty = A + B*points[2*nrp];

   if(DBGSIG) printf("PointEstimate(): A = %lf, B = %lf, estimate = %lf, real = %lf, error = %lf\n", A, B, esty, points[2*nrp+1], abs(esty - points[2*nrp+1])/points[2*nrp+1]);

   return abs(esty - points[2*nrp+1])/points[2*nrp+1];
}*/

// Separate functions -----------------------------------------
