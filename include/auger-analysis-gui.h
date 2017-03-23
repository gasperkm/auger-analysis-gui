#ifndef __ROOT_APP_H__
#define __ROOT_APP_H__

//#define winWidth 1240
//#define winHeight 800
#define WINDOW_NAME "Auger analysis software"
#define adstwin 3
#define mvawin 3
#define cutswin 2
#define BSIZE 10000

#include "root_include.h"
#include "substructure.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <vector>
#include <string>

enum EMenuIds {
   M_FILE_SET_LAYOUT,
   M_FILE_SAVE_LAYOUT,
   M_FILE_EXIT,

   M_HELP_WEBHELP,
   M_HELP_ABOUT

/*   M_FILE_SET_LAYOUT,
   M_FILE_SAVE_LAYOUT,
   M_FILE_SAVE_MSETTINGS,
   M_FILE_SAVE_ASETTINGS,
   M_FILE_DEF_MSETTINGS,
   M_FILE_DEF_ASETTINGS,
   M_FILE_DEF_SETTINGS,
   M_FILE_CHECK_WIDTH,
   M_FILE_EXIT,

   M_ANALYSIS_HISTTYPE_1DADC,
   M_ANALYSIS_HISTTYPE_1DTDC,
   M_ANALYSIS_HISTTYPE_2D,
   M_ANALYSIS_INTEG,
   M_ANALYSIS_INTEGX,
   M_ANALYSIS_INTEGY,
   M_ANALYSIS_PHOTMU,
   M_ANALYSIS_BREAKDOWN,
   M_ANALYSIS_SURFSCAN,
   M_ANALYSIS_TIMING,

   M_HELP_WEBHELP,
   M_HELP_ABOUT*/
};

// Window classes ----------------------------------------------------
//class TGWindow;
//class TGMainFrame;

// Dialog window class
class TGDialogFrame
{
RQ_OBJECT("TGDialogFrame")

protected:
   TGTransientFrame *fDialogMain;
   TGCompositeFrame *fFrame;

public:
   TGDialogFrame(const TGWindow *p, const TGWindow *main, int w, int h, TSubStructure *cutVal, int *opt);
   virtual ~TGDialogFrame();

   TSubStructure *cutDialogEntry;
   TSubStructure *cutDialogButtons;

   void SetMvaCut(int opt);
   void DoClose();
   void CloseWindow();

   int *contAnalysis;
   TSubStructure *tempStruct;
};

// Main window class
class TGAppMainFrame
{
RQ_OBJECT("TGAppMainFrame")

   friend class TGDialogFrame;

protected:
   TGMainFrame *fMain;
   TGMenuBar *fMenuBar;
   TGLayoutHints *fMenuBarItemLayout;
   TGPopupMenu *fMenuFile, *fMenuAnalysis, *fMenuHelp, *fMenuHisttype;
   TGTab *fTab, *analTab;
   TGDialogFrame *cutDialog;
   int winWidth;
   int winHeight;

   // Variables for frame layouts and titles
   TGCompositeFrame *fLayout[3];
   TGHorizontalFrame *fTitle;

   void InitMenu();
   void AppLayout();
   void OpenWindow(int winid);
   void CloseWindow();
   Bool_t About();

   int idtotal;
   double oldadstvalue;
   
   vector<string> configNames;
   vector<string> sdcutNames;
   vector<string> fdcutNames;
/*   bool cleanPlots;
   int logChange;
   int selChannel;
   bool acqStarted;
   unsigned int gBuf[BSIZE];
   clock_t clkt0;
   time_t timet0;
   bool liveUpdate;
   int npeaks;*/

public:
   TGAppMainFrame(const TGWindow *p, int w, int h);
   virtual ~TGAppMainFrame();

   void HandleMenu(Int_t id);

   // Splitter to a set number of frames
   bool TGSplitter(TGWindow *parent, const char *majorSplit, int *vertSplits, int *horSplits, const char *frameTitles[512], TGCompositeFrame **frames, int *frameWidth, int *frameHeight);
   void TGTitleLabel(TGWindow *parent, TGHorizontalFrame *fTitle, const char *title, Pixel_t foreColor, Pixel_t backColor, const char *font);

   // Layout setup
   void LayoutRead(int nrframes, int *w, int *h);
   void LayoutSave();
   void LayoutSet();
/*   void SaveMeasSettings();
   void SaveAnalSettings();
   void GetMeasSettings();
   void GetAnalSettings();
   void ToolTipSet();
   void ToolTipSetSurfaceScan();
   void ToolTipSetRelativePDE();
   void ToolTipSetTempAnalysis();*/

   // Subframes where we display everything
   TGCompositeFrame *adstLayout[adstwin];
   TGCompositeFrame *mvaLayout[mvawin];
   TGCompositeFrame *cutsLayout[cutswin];

   // Substructures for Settings pane (ADST layout)
   TRootEmbeddedCanvas *adstCanvas;
   TSubStructure *adstSelectEvent;
   TGLabel *adstShowEvent;
   TSubStructure *adstSelectSdId;
   TSubStructure *adstSelectFdId;

   TSubStructure *selectAdst;
   TGListBox *adstList;
   TSubStructure *adstMultiSelect;
   TSubStructure *adstListCtrl;
   TSubStructure *adstPlotDist;
   TSubStructure *adstPlotDistSelect;
   TSubStructure *adstPlotSingle;
   TSubStructure *adstPlotSingleSelect;
   TSubStructure *adstPlotSingleEvent;
   TSubStructure *adstSetZenith;
   TSubStructure *adstSetEnergy;
   TSubStructure *adstSetEnergyErr;
   TSubStructure *adstSetXmaxErr;
   TSubStructure *adstSetShfoot;

   TSubStructure *adstUpdateBut;

   // Substructures for MVA layout
   TSubStructure *selectMvafile;
   TGListBox *mvaList[3];
   TSubStructure *startRewriting;
   TSubStructure *sampleRewriting;
   TSubStructure *startCombining;

   TGLabel *sigBackLabel;
   TSubStructure *displayResult[6];

   TSubStructure *selectedMva;
   TGListBox *selectObservables[3];  // TODO: NEW
   int oldpos[3];
//   TSubStructure *selectMvaType;
//   TSubStructure *observableSelect;
   vector<string> observables;
   vector<int> observablesCheck;
   int nrobs;
   TSubStructure *signalSelect;
   TSubStructure *backgroundSelect;
   int oldselect[2];
   TSubStructure *methodsSelect;
   vector<string> methods;
   int nrmethods;
   int nrspecialMva;
   TSubStructure *cutMva;
   TSubStructure *cutEnergy;
   TSubStructure *cutEnergyBins;
   TSubStructure *cutEnergyBinSelect;
   vector<double> ecutBins;
   TSubStructure *cutZenith;
   TSubStructure *cutZenithBins;
   TSubStructure *cutZenithBinSelect;
   vector<double> zcutBins;
   TSubStructure *plotSetXmax;
   TSubStructure *plotSetX0;
   TSubStructure *plotSetLambda;
   TSubStructure *plotSetFdenergy;
   TSubStructure *plotSetShfoot;
   TSubStructure *plotSetShwsize;
   TSubStructure *plotSetNrmu;
   TSubStructure *plotSetCurvature;
   TSubStructure *plotSetRisetime;
   TSubStructure *plotSetMva;
   TSubStructure *specialMva;
   TSubStructure *startMvaAnalysis;
   int nrTreeEvents[2];
   string outputMvaName;
   string applymva;

   // Substructures for Cuts layout
   TSubStructure *configFile;
   TSubStructure *sdcutsFile;
   TSubStructure *fdcutsFile;
   TSubStructure *selectCutsInputFile;
   TGListBox *cutsInputList;
   TSubStructure *startCutsSelection;

   TGTextEdit *cutsout;
   TSubStructure *insertCut;
   TSubStructure *cutsEditorButtons;

/*   // Substructures for Settings pane (measurement layout)
   TSubStructure *scansOn;
   TSubStructure *vHardlimit;
   TSubStructure *NCH;
   TSubStructure *posUnits;
   TSubStructure *rotUnits;
   TSubStructure *oscConnect;
   TSubStructure *laserInfo;
   TSubStructure *chtemp;
   TSubStructure *liveDisp;

   // Substructures for Measurement pane (measurement layout)
   TSubStructure *vOutCh;
   TSubStructure *vOut;
   TSubStructure *vOutOpt;
   TSubStructure *vOutButtons;
   TSubStructure *vOutStart;
   TSubStructure *vOutStop;
   TSubStructure *vOutStep;
   TSubStructure *xPos;
   TSubStructure *yPos;
   TSubStructure *zPos;
   TSubStructure *zPosMin;
   TSubStructure *zPosMax;
   TSubStructure *zPosStep;
   TSubStructure *posButtons;
   TSubStructure *xPosMin;
   TSubStructure *xPosMax;
   TSubStructure *xPosStep;
   TSubStructure *yPosMin;
   TSubStructure *yPosMax;
   TSubStructure *yPosStep;
   TSubStructure *rotPos;
   TSubStructure *rotButtons;
   TSubStructure *rotButEmergency;
   TSubStructure *rotPosMin;
   TSubStructure *rotPosMax;
   TSubStructure *rotPosStep;
   TSubStructure *evtNum;
   TSubStructure *timeStamp;
   TSubStructure *fileName;
   TSubStructure *measProgress;

   // Substructures for Display pane (measurement layout)
   TRootEmbeddedCanvas *measCanvas;

   // Action connections for Settings pane (measurement layout)
   void EnableScan(int type);
   void VoltageLimit();
   void ChangeUnits(int type);
   void ChangeUnitsRot(int type);
   void EnableLiveUpdate();

   // Action connections for Measurement pane (measurement layout)
   int GetChannel();
   void NegativePolarity();
   void VoltOut(int opt);
   void PositionSet(int opt);
   void RotationSet(int opt);
   void SaveFile();
   void StartAcq();

   // Substructures for Histogram selection pane (analysis layout)
   TSubStructure *selectDir;
   TGListBox *fileList;
   TSubStructure *multiSelect;
   TSubStructure *fileListCtrl;
   TSubStructure *dispTime;
   TSubStructure *dispBias;
   TSubStructure *dispPos;
   TSubStructure *dispTemp;
   TSubStructure *dispAngle;
   TSubStructure *dispLaser;

   // Substructures for Histogram pane (analysis layout)
   TRootEmbeddedCanvas *analysisCanvas;

   // Substructures for Histogram controls pane (analysis layout)
   TSubStructure *adcRange;
   TSubStructure *tdcRange;
   TSubStructure *yRange;
   TSubStructure *selectCh;
   TSubStructure *plotType;
   TSubStructure *histOpt;
   TSubStructure *exportHist;
   TSubStructure *editSelHist;
   TSubStructure *posUnitsPlot;

   // Substructures for analysis pane (analysis layout)
   TSubStructure *intSpect;
   TSubStructure *resol2d;
   TSubStructure *intSpectButtons;

   TSubStructure *relPde;
   TSubStructure *midPeak;
   TSubStructure *darkRun;
   TSubStructure *zeroAngle;
   TSubStructure *relPdeButtons;

   TSubStructure *minPeak;
   TSubStructure *peakSepCalc;
   TSubStructure *brDownButtons;

   TSubStructure *surfScanOpt;
   TSubStructure *resolSurf;
   TSubStructure *surfButtons;

   TSubStructure *fitSigma;
   TSubStructure *fitTresh;
   TSubStructure *fitInter;
   TSubStructure *adcOffset;
   TSubStructure *accError;
   TSubStructure *pedesLow;
   TSubStructure *fitChecks;
   TSubStructure *analysisProgress;
*/
   // Action connections for ADST file selection pane (ADST viewer layout)
   void SelectDirectory();
   void ListMultiSelect(int opt);
   void FileListNavigation(int opt);
   void ClearHistogramList();
   void ToggleAdstPlotType(int opt);
   void AdstPlottingButton(int opt);
   
   // Action connections for Histogram controls pane (ADST viewer layout)
   void UpdateAdstFile(int opt);
   void GetAugerEvent();
/*   void HistogramOptions(int opt);
   void ChangeHisttype(int type);
*/
   // Action connections for Mva filelist (MVA analysis layout)
   void SelectMvaDir();
   void SelectRewrite(int opt);
   void StartRewrite(char *outfile, int opt);
   void SelectCombine();
   void StartCombine(char *outfile);
   void SelectMerge();
   void StartMerge(char *outfile);
   void EnableMvaFile(int opt);
//   void RisetimeSelect(int opt);
   void ScrollObservables(int pos);
   void PickObservables(int pos);
//   void SetMvaType(int opt);
   void UpdateSigTreeSelection(int selid);
   void UpdateBackTreeSelection(int selid);
   void UpdateEnergyBinSelect();
   void UpdateZenithBinSelect();
   void CheckEnergyBin();
   void CheckZenithBin();
   void CheckBothBins();
   void StartMvaAnalysis(int opt);
   void BookTheMethod(TMVA::Factory *factory);
//   string GetTree(int sigback, TFile *infile, TTree *outtree);
   void CreateMVAPlots(TTree *app, TMVA::Reader *reader, string mvamethod, float *obsvars, string signalName, int curtree);
   void SetupBinning(std::string obs, float *limit);
   void SetupAxis(TH1F *hist, string obs);

   // Action connections for selection cuts (Selection cuts layout)
   void SelectCutsDirectory();

/*   // Action connections for analysis pane (analysis layout)
   void SelectDarkHist();
   void AnalysisDefaults();
   void AnalysisHandle(int type);
   void IntegSpectrum(TList *files, int direction, int edit);
   void PhotonMu(TList *files, int edit);
   void PlotEdgeDistribution(TList *files, int filenr, int points, double *min, double *max, double *xy, double *integAcc, int axis, bool edge2d, int edit);
   void BreakdownVolt(TList *files, int edit);
   void SurfaceScan(TList *files, int edit);

   // Substructures for Edit file header window (new tab)
   TGListBox *editList;
   TSubStructure *timeEditDisplay;
   TSubStructure *biasEdit;
   TSubStructure *xPosEdit;
   TSubStructure *yPosEdit;
   TSubStructure *zPosEdit;
   TSubStructure *tempEdit;
   TSubStructure *angleEdit;
   TSubStructure *laserEdit;
   TSubStructure *editHead;
   TSubStructure *editMulti;
   TGLabel *selectWarn;

   // Action connections for Edit file header window (new tab)
   void SetWarnings();
   void EditTickToggle(int type);
   void StartHeaderEdit();
   void ShowHeaderEdit(int id);
   void HeaderChange(char *histfile, bool *changetype);
   void CloseEditTab(int tabval);

   // Substructures for temporary analysis edit window (new tab)
   TRootEmbeddedCanvas *tempAnalysisCanvas;
   TSubStructure *runningAver;
   TSubStructure *runningOff;
   TSubStructure *secondAxis;
   TSubStructure *exportExitAnalysis;

   TSubStructure *xCrop;
   TSubStructure *yCrop;
   TSubStructure *interpolSize;
   TSubStructure *updateCrop;
   
   // Action connections for temporary analysis edit window (new tab)
   void ApplyRunningAver();
   void UpdateIntegrateSurface(int val);
   void CloseTempAnalysisTab(int tabval);
   void ExportTempAnalysisPlot();
*/
   // Additional functions
   void DisplayAdstPlot(char *histfile, int opt);
   void CutsOpen(int opt);
   void CutsEdit(int opt);
   void CutsApply(int opt);
   void CutsSave(int opt, int savetype);
   void InsertCutOption(int select);
   void BoolOption(string title, string question, string *outstring, int selected, int linewidth, int opt);
   void CutsEditor(TGTab *mainTab, bool create, int *tabid, char *infile, int opt);
   void CloseCutsEditor(int tabval);
/*   void HeaderEditTab(TGTab *mainTab, bool create, int *tabid);
   void TempAnalysisTab(TGTab *mainTab, bool create, int *tabid, int analtype);
   void RunMeas(void *ptr, int runCase, int &scanon);
   int MyTimer();*/

   // Open directory (separate for saving measurement and for opening histograms)
   char *currentViewDir;
   char *currentMvaDir;
   char *currentRewriteDir;
   char *currentAnalysisDir;
   char *currentCutsDir;
   char *currentCutsInputDir;

/*// ROOT file variable structure -------------------------------
   struct EventHeader {
     int nrch;
     int timestamp;
     double biasvolt;
     int xpos;
     int ypos;
     int zpos;
     double temperature;
     double angle;
     char laserinfo[256];
   } evtheader;

   struct EventData {
     int adcdata[8];
     int tdcdata[8];
   } evtdata;

   struct EventMeas {
     double measdata;
   } evtmeas;

   TFile *inroot;
   TFile *outroot;
// ROOT file variable structure -------------------------------

   daq *gDaq;
   daqscope *gScopeDaq;*/
};


// -------------------------------------------------------------------*/

/*enum EMenuIds {
   M_FILE_SET_LAYOUT,
   M_FILE_SAVE_LAYOUT,
   M_FILE_SAVE_MSETTINGS,
   M_FILE_SAVE_ASETTINGS,
   M_FILE_DEF_MSETTINGS,
   M_FILE_DEF_ASETTINGS,
   M_FILE_DEF_SETTINGS,
   M_FILE_CHECK_WIDTH,
   M_FILE_EXIT,

   M_ANALYSIS_HISTTYPE_1DADC,
   M_ANALYSIS_HISTTYPE_1DTDC,
   M_ANALYSIS_HISTTYPE_2D,
   M_ANALYSIS_INTEG,
   M_ANALYSIS_INTEGX,
   M_ANALYSIS_INTEGY,
   M_ANALYSIS_PHOTMU,
   M_ANALYSIS_BREAKDOWN,
   M_ANALYSIS_SURFSCAN,
   M_ANALYSIS_TIMING,

   M_HELP_WEBHELP,
   M_HELP_ABOUT
};*/

// Separate functions -----------------------------------------
void GetTime(int intime, char *outtime);
void remove_ext(char *inname, char *outname);
void remove_from_last(char *inname, char search, char *outname);
void remove_before_last(char *inname, char search, char *outname);
string remove_from_first(string input, string toremove, string replace);
void layoutMainWindow(int *w, int *h);
void SeqNumber(int innum, int maxnum, char *outstr);
int CheckFormat(const char *infile);
int CheckMvaFormat(const char *infile);
string IntToStr(int nr);
string DblToStr(double nr, int prec);
double SinSquare(double input, bool degree);
double AsinSqrt(double input, bool degree);
string GetType(int type);
/*void TimeEstimate(clock_t stopw0, time_t time0, float progress, char *retEstim, int offset);
void TimeEstimateNew(int nr, clock_t stopw0, time_t time0, int rX, int rY, int rZ, int xWait, int yWait, int zWait, char *retEstim);
void NormateSet(int file, int nrpoint, double *min, double *max, double *setCount, double *setAcc);
double PointEstimate(int nrp, double *points);
// Separate functions -----------------------------------------


// Window classes ----------------------------------------------------
//class TGWindow;
//class TGMainFrame;

// Main window class
class TGAppMainFrame
{
RQ_OBJECT("TGAppMainFrame")

protected:
   TGMainFrame *fMain;
//   TGMdiMainFrame *fMainFrame;
//   TGMdiMenuBar *fMenuBar;
   TGMenuBar *fMenuBar;
   TGLayoutHints *fMenuBarItemLayout;
   TGPopupMenu *fMenuFile, *fMenuAnalysis, *fMenuHelp, *fMenuHisttype;
   TGTab *fTab, *analTab;
   int winWidth;
   int winHeight;
//   TGMdiSubwindow *settingsPane;

   // Variables for frame layouts and titles
   TGCompositeFrame *fLayout[2];
   TGHorizontalFrame *fTitle;

   void InitMenu();
   void AppLayout();
   void OpenWindow(int winid);
   void CloseWindow();
   Bool_t About();

   int idtotal;
   bool cleanPlots;
   int logChange;
   int selChannel;
   bool acqStarted;
   unsigned int gBuf[BSIZE];
   clock_t clkt0;
   time_t timet0;
   bool liveUpdate;
   int npeaks;

public:
   TGAppMainFrame(const TGWindow *p, int w, int h);
   virtual ~TGAppMainFrame();

   void HandleMenu(Int_t id);

   // Splitter to a set number of frames
   bool TGSplitter(TGWindow *parent, const char *majorSplit, int *vertSplits, int *horSplits, const char *frameTitles[512], TGCompositeFrame **frames, int *frameWidth, int *frameHeight);
   void TGTitleLabel(TGWindow *parent, TGHorizontalFrame *fTitle, const char *title, Pixel_t foreColor, Pixel_t backColor, const char *font);

   // Layout setup
   void LayoutRead(int nrframes, int *w, int *h);
   void LayoutSave();
   void LayoutSet();
   void SaveMeasSettings();
   void SaveAnalSettings();
   void GetMeasSettings();
   void GetAnalSettings();
   void ToolTipSet();
   void ToolTipSetSurfaceScan();
   void ToolTipSetRelativePDE();
   void ToolTipSetTempAnalysis();

   // Subframes where we display everything
   TGCompositeFrame *measLayout[measwin];
   TGCompositeFrame *analysisLayout[mvawin];

   // Substructures for Settings pane (measurement layout)
   TSubStructure *scansOn;
   TSubStructure *vHardlimit;
   TSubStructure *NCH;
   TSubStructure *posUnits;
   TSubStructure *rotUnits;
   TSubStructure *oscConnect;
   TSubStructure *laserInfo;
   TSubStructure *chtemp;
   TSubStructure *liveDisp;

   // Substructures for Measurement pane (measurement layout)
   TSubStructure *vOutCh;
   TSubStructure *vOut;
   TSubStructure *vOutOpt;
   TSubStructure *vOutButtons;
   TSubStructure *vOutStart;
   TSubStructure *vOutStop;
   TSubStructure *vOutStep;
   TSubStructure *xPos;
   TSubStructure *yPos;
   TSubStructure *zPos;
   TSubStructure *zPosMin;
   TSubStructure *zPosMax;
   TSubStructure *zPosStep;
   TSubStructure *posButtons;
   TSubStructure *xPosMin;
   TSubStructure *xPosMax;
   TSubStructure *xPosStep;
   TSubStructure *yPosMin;
   TSubStructure *yPosMax;
   TSubStructure *yPosStep;
   TSubStructure *rotPos;
   TSubStructure *rotButtons;
   TSubStructure *rotButEmergency;
   TSubStructure *rotPosMin;
   TSubStructure *rotPosMax;
   TSubStructure *rotPosStep;
   TSubStructure *evtNum;
   TSubStructure *timeStamp;
   TSubStructure *fileName;
   TSubStructure *measProgress;

   // Substructures for Display pane (measurement layout)
   TRootEmbeddedCanvas *measCanvas;

   // Action connections for Settings pane (measurement layout)
   void EnableScan(int type);
   void VoltageLimit();
   void ChangeUnits(int type);
   void ChangeUnitsRot(int type);
   void EnableLiveUpdate();

   // Action connections for Measurement pane (measurement layout)
   int GetChannel();
   void NegativePolarity();
   void VoltOut(int opt);
   void PositionSet(int opt);
   void RotationSet(int opt);
   void SaveFile();
   void StartAcq();

   // Substructures for Histogram selection pane (analysis layout)
   TSubStructure *selectDir;
   TGListBox *fileList;
   TSubStructure *multiSelect;
   TSubStructure *fileListCtrl;
   TSubStructure *dispTime;
   TSubStructure *dispBias;
   TSubStructure *dispPos;
   TSubStructure *dispTemp;
   TSubStructure *dispAngle;
   TSubStructure *dispLaser;

   // Substructures for Histogram pane (analysis layout)
   TRootEmbeddedCanvas *analysisCanvas;

   // Substructures for Histogram controls pane (analysis layout)
   TSubStructure *adcRange;
   TSubStructure *tdcRange;
   TSubStructure *yRange;
   TSubStructure *selectCh;
   TSubStructure *plotType;
   TSubStructure *histOpt;
   TSubStructure *exportHist;
   TSubStructure *editSelHist;
   TSubStructure *posUnitsPlot;

   // Substructures for analysis pane (analysis layout)
   TSubStructure *intSpect;
   TSubStructure *resol2d;
   TSubStructure *intSpectButtons;

   TSubStructure *relPde;
   TSubStructure *midPeak;
   TSubStructure *darkRun;
   TSubStructure *zeroAngle;
   TSubStructure *relPdeButtons;

   TSubStructure *minPeak;
   TSubStructure *peakSepCalc;
   TSubStructure *brDownButtons;

   TSubStructure *surfScanOpt;
   TSubStructure *resolSurf;
   TSubStructure *surfButtons;

   TSubStructure *fitSigma;
   TSubStructure *fitTresh;
   TSubStructure *fitInter;
   TSubStructure *adcOffset;
   TSubStructure *accError;
   TSubStructure *pedesLow;
   TSubStructure *fitChecks;
   TSubStructure *analysisProgress;

   // Action connections for Histogram file selection pane (analysis layout)
   void SelectDirectory();
   void ListMultiSelect(int opt);
   void FileListNavigation(int opt);
   void HeaderEdit();
   void ClearHistogramList();
   
   // Action connections for Histogram controls pane (analysis layout)
   void UpdateHistogram(int opt);
   void HistogramOptions(int opt);
   void ChangeHisttype(int type);

   // Action connections for analysis pane (analysis layout)
   void SelectDarkHist();
   void AnalysisDefaults();
   void AnalysisHandle(int type);
   void IntegSpectrum(TList *files, int direction, int edit);
   void PhotonMu(TList *files, int edit);
   void PlotEdgeDistribution(TList *files, int filenr, int points, double *min, double *max, double *xy, double *integAcc, int axis, bool edge2d, int edit);
   void BreakdownVolt(TList *files, int edit);
   void SurfaceScan(TList *files, int edit);

   // Substructures for Edit file header window (new tab)
   TGListBox *editList;
   TSubStructure *timeEditDisplay;
   TSubStructure *biasEdit;
   TSubStructure *xPosEdit;
   TSubStructure *yPosEdit;
   TSubStructure *zPosEdit;
   TSubStructure *tempEdit;
   TSubStructure *angleEdit;
   TSubStructure *laserEdit;
   TSubStructure *editHead;
   TSubStructure *editMulti;
   TGLabel *selectWarn;

   // Action connections for Edit file header window (new tab)
   void SetWarnings();
   void EditTickToggle(int type);
   void StartHeaderEdit();
   void ShowHeaderEdit(int id);
   void HeaderChange(char *histfile, bool *changetype);
   void CloseEditTab(int tabval);

   // Substructures for temporary analysis edit window (new tab)
   TRootEmbeddedCanvas *tempAnalysisCanvas;
   TSubStructure *runningAver;
   TSubStructure *runningOff;
   TSubStructure *secondAxis;
   TSubStructure *exportExitAnalysis;

   TSubStructure *xCrop;
   TSubStructure *yCrop;
   TSubStructure *interpolSize;
   TSubStructure *updateCrop;
   
   // Action connections for temporary analysis edit window (new tab)
   void ApplyRunningAver();
   void UpdateIntegrateSurface(int val);
   void CloseTempAnalysisTab(int tabval);
   void ExportTempAnalysisPlot();

   // Additional functions
   void DisplayHistogram(char *histfile, int histtype, int opt);
   void HeaderEditTab(TGTab *mainTab, bool create, int *tabid);
   void TempAnalysisTab(TGTab *mainTab, bool create, int *tabid, int analtype);
   void RunMeas(void *ptr, int runCase, int &scanon);
   int MyTimer();

   // Open directory (separate for saving measurement and for opening histograms)
   char *currentMeasDir;
   char *currentAnalDir;

// ROOT file variable structure -------------------------------
   struct EventHeader {
     int nrch;
     int timestamp;
     double biasvolt;
     int xpos;
     int ypos;
     int zpos;
     double temperature;
     double angle;
     char laserinfo[256];
   } evtheader;

   struct EventData {
     int adcdata[8];
     int tdcdata[8];
   } evtdata;

   struct EventMeas {
     double measdata;
   } evtmeas;

   TFile *inroot;
   TFile *outroot;
// ROOT file variable structure -------------------------------

   daq *gDaq;
   daqscope *gScopeDaq;
};

// -------------------------------------------------------------------*/

#endif
