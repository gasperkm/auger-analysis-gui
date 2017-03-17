#ifndef _ADSTMVA_H_
#define _ADSTMVA_H_

#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"
#if OFFVER == 0
  #include "../include/OfflineIncludeOld.h"
#elif OFFVER == 1
  #include "../include/OfflineIncludeNew.h"
#endif

class Observables {
public:
   Observables();
   virtual ~Observables();

   float xmax;          // depth of shower maximum
   float x0;            // GH shape parameter X0 (0 is at first interaction point
   float lambda;        // GH shape parameter lambda
   float fdenergy;      // Total energy from FD
   float shfoot;        // Shower foot
   float shwsize;       // Shower size - replacement for S1000
   float ldfbeta;       // beta variable of the LDF
   float curvature;     // curvature R
   float nrmu;          // number of muons at ground level
   float risetime;      // risetime
   float risetimerecalc;      // recalculated risetime
   float aop;           // area-over-peak
   float zenith;	// zenith angle
};

class AdstMva
{
public:
   AdstMva();
   virtual ~AdstMva();

   RecEvent *fRecEvent;
   RecEventFile *fFile;
   DetectorGeometry* fDetGeo;
   SdRecShower *sdrecshw;
   GenShower *genshw;

   int nrtanks;
   vector<SdRecStation> acttanks;
   int nreyes;
   vector<FDEvent> acteyes;
   int longesttrack;

   void GetActiveTanks();
   void GetActiveEyes();

   void PrepareOtherTrees(unsigned int nrfiles, int innr);
   int GetEyeLongestTrack();

   TGraph *gr;
   TGraphErrors *grErr;
   TGraphErrors *grErrCum;

   void PlotLDF(TCanvas *c1);
   void PlotVEM(TCanvas *c1, string sdid);
   void PlotLongProf(TCanvas *c1, string fdid);

   void RewriteObservables(unsigned int nrfiles, int innr, Observables **sig, Observables **all, double frac);

   string outname;
   string inname;
   TFile *outfile;
   bool goodrec;

// NEW
   TTree *all_tree, *sig_tree;
/*   TTree *all_tree[3];
   TTree *sig_tree[3];*/

   double shfoot, shfootmin, shfootmax, shfootlimit;

   int GetShowerFoot(int longestEye, std::vector<FDEvent> fdevt);
   int GetRisetime(int event, double inRisetime, double *outRisetime);
   int GetAoP(int event, double *outAop);
   void RiseTimeFunction(double zenith, double energy, TF1 *risetimeFit);

   int SelectEvents(string config, vector<string> infiles);
   enum EAdstMode
   {
      eFull,
      eMini,
      eMicro
   };
   bool gNminusOne;
   unsigned int gMinNFDEyes;
   int gVerbosity;

   double minSignal;
   bool includeSaturated;
   int minPoints;
   double evalDistance;
   double limitTankDistance[2];
   TFormula *fRTWeights;
};

#endif
