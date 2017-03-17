#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"
#include "../include/substructure.h"
#if OFFVER == 0
  #include "../include/OfflineIncludeOld.h"
#elif OFFVER == 1
  #include "../include/OfflineIncludeNew.h"
#endif
#include "../include/adst_mva.h"
#include "../include/combine.h"

#include <stdio.h>
#include <stdlib.h>

// Main window constructor (+layout) ---------------------------------

TGAppMainFrame::TGAppMainFrame(const TGWindow *p, int w, int h)
{
   TGCompositeFrame *fT1;
   idtotal = 0;

   char *cTemp;

   // Prepare the default open directory (rootdir)
   currentViewDir = new char[1024];
   sprintf(currentViewDir, "%s/results", rootdir);
   currentMvaDir = new char[1024];
   sprintf(currentMvaDir, "%s/results", rootdir);
   currentRewriteDir = new char[1024];
   sprintf(currentRewriteDir, "%s/results", rootdir);
   currentAnalysisDir = new char[1024];
   sprintf(currentAnalysisDir, "%s/results", rootdir);
   currentCutsDir = new char[1024];
   sprintf(currentCutsDir, "%s/results", rootdir);
   currentCutsInputDir = new char[1024];
   sprintf(currentCutsInputDir, "%s/results", rootdir);

   // Observables
   observables.erase(observables.begin(), observables.end());
   observablesCheck.erase(observablesCheck.begin(), observablesCheck.end());
   observables.push_back("xmax");            observablesCheck.push_back(1);
   observables.push_back("x0");              observablesCheck.push_back(0);
   observables.push_back("lambda");          observablesCheck.push_back(0);
   observables.push_back("fdenergy");        observablesCheck.push_back(0);
   observables.push_back("shfoot");          observablesCheck.push_back(1);
   observables.push_back("shwsize");         observablesCheck.push_back(1);
   observables.push_back("nrmu");            observablesCheck.push_back(0);
   observables.push_back("curvature");       observablesCheck.push_back(0);
   observables.push_back("risetime");        observablesCheck.push_back(0);
   observables.push_back("risetimerecalc");  observablesCheck.push_back(1);
   observables.push_back("aop");             observablesCheck.push_back(0);
   observables.push_back("zenith");          observablesCheck.push_back(0);
   nrobs = observables.size();

   // Populate the config, sdcut and fdcut names vectors
   // configure options
   configNames.erase(configNames.begin(), configNames.end());
   configNames.push_back("SDCutFile");		// 1
   configNames.push_back("FDCutFile");		// 2
   configNames.push_back("SDBadPeriodsFile");	// 3
   // sd cut options
   sdcutNames.erase(sdcutNames.begin(), sdcutNames.end());
   sdcutNames.push_back("saturatedCandidates");	// 1
   sdcutNames.push_back("LDFStatus");		// 2
   sdcutNames.push_back("T4Trigger");		// 3
   sdcutNames.push_back("T5Trigger");		// 4
   sdcutNames.push_back("minZenithSD");		// 5
   sdcutNames.push_back("maxZenithSD");		// 6
   sdcutNames.push_back("minRecLevel");		// 7
   sdcutNames.push_back("minCandidateStations");	// 8
   sdcutNames.push_back("maxCandidateStations");	// 9
   sdcutNames.push_back("badPeriodsRejection");	// 10
   sdcutNames.push_back("badPeriodsRejectionFromFile");	// 11
   sdcutNames.push_back("selectSaturation");	// 12
   sdcutNames.push_back("minLgEnergySD");	// 13
   sdcutNames.push_back("maxLgEnergySD");	// 14
   sdcutNames.push_back("timeInterval");	// 15
   sdcutNames.push_back("heraldSelection");	// 16
   sdcutNames.push_back("fixBeta");		// 17
   sdcutNames.push_back("sdId");		// 18
   sdcutNames.push_back("augerId");		// 19
   sdcutNames.push_back("temperature");		// 20
   sdcutNames.push_back("idsFromFile");		// 21
   sdcutNames.push_back("lightning");		// 22
   sdcutNames.push_back("maxRelativeShowerSizeError");	// 23
   sdcutNames.push_back("badSilentStations");	// 24
   sdcutNames.push_back("hasSilents");		// 25
   // fd cut options
   fdcutNames.erase(fdcutNames.begin(), fdcutNames.end());
   fdcutNames.push_back("xMaxInFOV");		// 1
   fdcutNames.push_back("xMaxError");		// 2

   // Define main window and menubar
   fMain = new TGMainFrame(p, w, h, kVerticalFrame);
   
   // Initialize the menu
   fMenuBar = new TGMenuBar(fMain, 200, 30);
   InitMenu();
   fMain->AddFrame(fMenuBar, new TGLayoutHints(kLHintsExpandX | kLHintsTop));

   // Prepare the tabbed structure
   fTab = new TGTab(fMain, 500, 500);

   // Read the layout we wish to use
   int frmWidth[adstwin+mvawin+cutswin], frmHeight[adstwin+mvawin+cutswin];
   LayoutRead(adstwin+mvawin+cutswin, frmWidth, frmHeight);

   // Structure the ADST viewer layout window
   int *vert, *hor, *wPane, *hPane;

   vert = new int[2]; hor = new int;
   *hor = 1;
   vert[0] = 1; vert[1] = 0;

   const char *adstFrmTit[] = {"ADST plotter", "ADST plotter controls", "Settings"};
   wPane = new int[adstwin]; hPane = new int[adstwin];
   for(int i = 0; i < adstwin; i++)
   {
      wPane[i] = frmWidth[i];
      hPane[i] = frmHeight[i];
   }

   fT1 = fTab->AddTab("ADST viewer");
   TGSplitter(fT1, "horizontal", hor, vert, adstFrmTit, adstLayout, wPane, hPane);
//   TGSplitter(fT1, "vertical", hor, vert, adstFrmTit, adstLayout, wPane, hPane);
   fT1->AddFrame(fLayout[idtotal], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   idtotal++;
   delete[] vert; delete hor; delete[] wPane; delete[] hPane;

   // Structure the MVA layout window
   vert = new int[2]; hor = new int;
   *hor = 1;
   vert[0] = 1; vert[1] = 0;

   const char *mvaFrmTit[] = {"Filelist", "Results", "MVA analysis"};
   wPane = new int[mvawin]; hPane = new int[mvawin];
   for(int i = 0; i < mvawin; i++)
   {
      wPane[i] = frmWidth[i+adstwin];
      hPane[i] = frmHeight[i+adstwin];
   }

   fT1 = fTab->AddTab("MVA analysis");
   TGSplitter(fT1, "horizontal", hor, vert, mvaFrmTit, mvaLayout, wPane, hPane);
   fT1->AddFrame(fLayout[idtotal], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   idtotal++;
   delete[] vert; delete hor; delete[] wPane; delete[] hPane;

   // Structure the Cuts layout window
   vert = new int[2]; hor = new int;
   *hor = 1;
   vert[0] = 1; vert[1] = 0;

   const char *cutsFrmTit[] = {"Options", "Selection cuts"};
   wPane = new int[cutswin]; hPane = new int[cutswin];
   for(int i = 0; i < cutswin; i++)
   {
      wPane[i] = frmWidth[i+adstwin+mvawin];
      hPane[i] = frmHeight[i+adstwin+mvawin];
   }

   fT1 = fTab->AddTab("Selection cuts");
   TGSplitter(fT1, "horizontal", hor, vert, cutsFrmTit, cutsLayout, wPane, hPane);
   fT1->AddFrame(fLayout[idtotal], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   idtotal++;
   delete[] vert; delete hor; delete[] wPane; delete[] hPane;

   // Structure the help layout window
   fT1 = fTab->AddTab("Help");
   TGTextView *helpdesc;
   const TGFont *tfont = gClient->GetFont(HELPFONT);
   FontStruct_t helpFont = tfont->GetFontStruct();
   helpdesc = new TGTextView(fT1,100,100);
   helpdesc->SetFont(helpFont);
   helpdesc->SetForegroundColor(0x000000);
   fT1->AddFrame(helpdesc, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   
   cTemp = new char[1024];
   sprintf(cTemp, "%s/doc/README", rootdir);
   if(helpdesc->LoadFile(cTemp))
   {
      if(DBGSIG) printf("TGAppMainFrame(): Help file correctly loaded.\n");
   }
   else
      printf("Error! Help file not loaded.\n");
   delete[] cTemp;
   helpdesc->AddLine("");

   fMain->AddFrame(fTab, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

   // Set the inner layout of each frame
   AppLayout();

   fMain->SetWindowName(WINDOW_NAME);
   fMain->MapSubwindows();
   fMain->MapWindow();
   fMain->Layout();

   // Prepare initial settings
   gSystem->Load("libRecEventKG.so");
   gSystem->Load("libAnalysisKG.so");

   ToggleAdstPlotType(0);
   oldadstvalue = 1;

//   cutMva[1]->widgetNE[0]->SetState(kFALSE);
//   cutMva[2]->widgetNE[0]->SetState(kFALSE);

   oldselect[0] = 1;
   oldselect[1] = 2;

   outputMvaName = "empty";

   UpdateEnergyBinSelect();
   UpdateZenithBinSelect();

//   RisetimeSelect(1);

/*   EnableScan(0);	//. 
   EnableScan(1);	// Grey out scan
   EnableScan(2);	// options by default
   EnableScan(3);	//.
   EnableLiveUpdate();	//. Disable the live histogram update at beginning
   VoltOut(1);		//. Get the output voltage save in file
   HistogramOptions(1);	//. Enable clean plots by default
   plotType->widgetTB[0]->SetDown(kTRUE);			//.
   fMenuHisttype->CheckEntry(M_ANALYSIS_HISTTYPE_1DADC);	// Set the ADC histogram
   fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_1DTDC);	// to show by default
   fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_2D);		//.
   acqStarted = false;	//. At program start, the acquisition is stopped
   ToolTipSet();	//. Turn on tooltips
   PositionSet(1);	//. Get starting table position
   RotationSet(1);	//. Get starting rotation*/

   if(DBGSIG > 1)
   {
      printf("TGAppMainFrame(): At end of constructor\n");
      gObjectTable->Print();
   }
}

TGAppMainFrame::~TGAppMainFrame()
{
   delete currentViewDir;
   delete currentMvaDir;
   delete currentRewriteDir;
   delete currentAnalysisDir;
   delete currentCutsDir;
   delete fMenuBar;
   delete fTab;
   fMain->Cleanup();
   delete fMain;
}

// -------------------------------------------------------------------

// Dialog window constructor (+layout) -------------------------------
TGDialogFrame::TGDialogFrame(const TGWindow *p, const TGWindow *main, int w, int h, TSubStructure *cutVal, int *opt)
{
   contAnalysis = opt;
   tempStruct = cutVal;

   fDialogMain = new TGTransientFrame(p, main, w, h, kVerticalFrame);
   fDialogMain->Connect("CloseWindow()", "TGDialogFrame", this, "DoClose()");
   fDialogMain->DontCallClose();

   fDialogMain->SetCleanup(kDeepCleanup);

   fFrame = new TGCompositeFrame(fDialogMain, w, h, kVerticalFrame);;

   TGLabel *lab;
   double numform[6];
   TGLayoutHints *f0centerX = new TGLayoutHints(kLHintsCenterX,2,2,2,2);
   TGLayoutHints *f0leftX = new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2);
   TGLayoutHints *f0leftXnoleft = new TGLayoutHints(kLHintsLeft | kLHintsTop,0,2,2,2);
   TGLayoutHints *f0leftXpad = new TGLayoutHints(kLHintsLeft | kLHintsTop,12,12,2,2);
   TGLayoutHints *f0rightX = new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2);
   TGLayoutHints *f0rightXpad = new TGLayoutHints(kLHintsRight | kLHintsTop,12,12,2,2);
   TGLayoutHints *f0centerY = new TGLayoutHints(kLHintsCenterY,2,2,2,2);
   TGLayoutHints *f0center2d = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,2,2,2,2);
   TGLayoutHints *f1expandX = new TGLayoutHints(kLHintsExpandX,2,2,2,2);
   TGLayoutHints *f1expand2d = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2);
   TGLayoutHints *f1expandXpad = new TGLayoutHints(kLHintsExpandX,12,12,2,2);
   vector<string> selnames;

   // Select MVA cut
   if(DBGSIG > 1) printf("TGDialogFrame(): Creating TSubStructure *cutDialogEntry -> Number entry for selecting MVA cut\n");
   cutDialogEntry = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 8; numform[1] = 4;
   if(cutDialogEntry->TGLabelNEntry(fFrame, (fFrame->GetWidth())-10, 30, "Select MVA cut:", 0.00, numform, "center"))
      fFrame->AddFrame(cutDialogEntry->outsidebox, f0centerX);

   // Buttons for setting MVA or canceling the analysis
   if(DBGSIG > 1) printf("TGDialogFrame(): Creating TSubStructure *cutDialogButtons -> Multiple buttons for setting the MVA cut or canceling the analysis\n");
   cutDialogButtons = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("OK");
   selnames.push_back("Cancel");
   if(cutDialogButtons->TGMultiButton(fFrame, (fFrame->GetWidth())-10, 30, selnames.size(), selnames, "center"))
      fFrame->AddFrame(cutDialogButtons->outsidebox, f0centerX);

   // Actions
   cutDialogButtons->widgetTB[0]->Connect("Clicked()", "TGDialogFrame", this, "SetMvaCut(=0)");
   cutDialogButtons->widgetTB[1]->Connect("Clicked()", "TGDialogFrame", this, "SetMvaCut(=1)");

   fDialogMain->AddFrame(fFrame, f1expand2d);

   fDialogMain->MapSubwindows();
   fDialogMain->Resize();
   fDialogMain->CenterOnParent();
   fDialogMain->SetWindowName("Cut for MVA analysis");
   fDialogMain->MapWindow();

   gClient->WaitFor(fDialogMain);
}

TGDialogFrame::~TGDialogFrame()
{
   fDialogMain->DeleteWindow();
}

void TGDialogFrame::SetMvaCut(int opt)
{
   if(DBGSIG > 1) printf("TGDialogFrame::SetMvaCut(): Setting the MVA cut (opt = %d)\n", opt);

   if(opt == 0)
      tempStruct->widgetNE[0]->SetNumber(cutDialogEntry->widgetNE[0]->GetNumber());

   *contAnalysis = opt;
   CloseWindow();
}

void TGDialogFrame::DoClose()
{
   if(DBGSIG > 1) printf("TGDialogFrame::DoClose(): Terminating dialog window\n");

   CloseWindow();
}

void TGDialogFrame::CloseWindow()
{
   delete this;
}
// -------------------------------------------------------------------

// Event handler for menubar actions ---------------------------------

void TGAppMainFrame::HandleMenu(Int_t id)
{
   int ret = 0;
   char cmd[256];

   switch(id)
   {
      case M_FILE_SET_LAYOUT:
	 LayoutSet();
	 break;

      case M_FILE_SAVE_LAYOUT:
	 LayoutSave();
	 break;

      case M_FILE_EXIT:
         CloseWindow();
	 break;

      case M_HELP_ABOUT:
         About();
	 break;

/*      case M_FILE_SAVE_MSETTINGS:
	 SaveMeasSettings();
	 break;

      case M_FILE_SAVE_ASETTINGS:
	 SaveAnalSettings();
	 break;

      case M_FILE_DEF_MSETTINGS:
         printf("Reseting measurement settings to default.\n");
         sprintf(cmd, "cp %s/settings/default_measurement_settings.txt %s/settings/measurement_settings.txt", rootdir, rootdir);
         ret = system(cmd);
	 break;

      case M_FILE_DEF_ASETTINGS:
         printf("Reseting analysis settings to default.\n");
         sprintf(cmd, "cp %s/settings/default_analysis_settings.txt %s/settings/analysis_settings.txt", rootdir, rootdir);
         ret = system(cmd);
	 break;

      case M_FILE_DEF_SETTINGS:
         printf("Reseting all settings to default.\n");
         sprintf(cmd, "cp %s/settings/default_measurement_settings.txt %s/settings/measurement_settings.txt", rootdir, rootdir);
         ret = system(cmd);
         sprintf(cmd, "cp %s/settings/default_analysis_settings.txt %s/settings/analysis_settings.txt", rootdir, rootdir);
         ret = system(cmd);
	 break;

      case M_FILE_CHECK_WIDTH:
	 printf("\nSettings window: %dx%d\n", adstLayout[0]->GetWidth(), adstLayout[0]->GetHeight());
	 printf("Histogram window: %dx%d\n", adstLayout[1]->GetWidth(), adstLayout[1]->GetHeight());
	 printf("Main measurement window: %dx%d\n", adstLayout[2]->GetWidth(), adstLayout[2]->GetHeight());
	 printf("Histogram file window: %dx%d\n", mvaLayout[0]->GetWidth(), mvaLayout[0]->GetHeight());
	 printf("Analysis window: %dx%d\n", mvaLayout[1]->GetWidth(), mvaLayout[1]->GetHeight());
	 printf("Histogram window: %dx%d\n", mvaLayout[2]->GetWidth(), mvaLayout[2]->GetHeight());
	 printf("Histogram controls window: %dx%d\n", mvaLayout[3]->GetWidth(), mvaLayout[3]->GetHeight());

	 printf("Main window: %dx%d\n", fMain->GetWidth(), fMain->GetHeight());
	 printf("Menu bar: %dx%d\n", fMenuBar->GetWidth(), fMenuBar->GetHeight());
	 printf("Tab window: %dx%d\n", fTab->GetWidth(), fTab->GetHeight());
         break;

      case M_FILE_EXIT:
         CloseWindow();
	 break;

      case M_ANALYSIS_HISTTYPE_1DADC:
         ChangeHisttype(0);
	 break;

      case M_ANALYSIS_HISTTYPE_1DTDC:
         ChangeHisttype(1);
	 break;

      case M_ANALYSIS_HISTTYPE_2D:
         ChangeHisttype(2);
	 break;

      case M_ANALYSIS_INTEG:
	 fTab->SetTab(1);
	 analTab->SetTab(0);
	 for(int i = 0; i < 3; i++)
	 {
	    if(intSpect->widgetChBox[i]->IsDown())
               intSpect->widgetChBox[i]->SetState(kButtonUp);
	 }
	 break;

      case M_ANALYSIS_INTEGX:
	 fTab->SetTab(1);
	 analTab->SetTab(0);
	 for(int i = 0; i < 3; i++)
	 {
            if(i == 0)
	       intSpect->widgetChBox[i]->SetState(kButtonDown);
	    else
	    {
	       if(intSpect->widgetChBox[i]->IsDown())
                  intSpect->widgetChBox[i]->SetState(kButtonUp);
	    }
	 }
	 break;

      case M_ANALYSIS_INTEGY:
	 fTab->SetTab(1);
	 analTab->SetTab(0);
	 for(int i = 0; i < 3; i++)
	 {
            if(i == 1)
	       intSpect->widgetChBox[i]->SetState(kButtonDown);
	    else
	    {
	       if(intSpect->widgetChBox[i]->IsDown())
                  intSpect->widgetChBox[i]->SetState(kButtonUp);
	    }
	 }
	 break;

      case M_ANALYSIS_PHOTMU:
	 fTab->SetTab(1);
	 analTab->SetTab(1);
	 for(int i = 0; i < 3; i++)
	 {
            if(i == 2)
	       intSpect->widgetChBox[i]->SetState(kButtonDown);
	    else
	    {
	       if(intSpect->widgetChBox[i]->IsDown())
                  intSpect->widgetChBox[i]->SetState(kButtonUp);
	    }
	 }
	 break;

      case M_ANALYSIS_BREAKDOWN:
	 fTab->SetTab(1);
	 analTab->SetTab(2);
	 break;

      case M_ANALYSIS_SURFSCAN:
	 fTab->SetTab(1);
	 analTab->SetTab(3);
	 break;

      case M_ANALYSIS_TIMING:
	 fTab->SetTab(1);
	 analTab->SetTab(4);
	 break;

      case M_HELP_WEBHELP:
         printf("TGAppMainFrame::HandleMenu(): Opening %s/doc/documentation.html in a web browser.\n", rootdir);
	 sprintf(cmd, "xdg-open %s/doc/documentation.html &", rootdir);
	 system(cmd);
	 break;

      case M_HELP_ABOUT:
         About();
	 break;
*/
      default:
	 break;
   }
}

// -------------------------------------------------------------------

// Initialize the main window menu -----------------------------------

void TGAppMainFrame::InitMenu()
{
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

   // Popup menu in menubar for File controls
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry(new TGHotString("Set &user layout"), M_FILE_SET_LAYOUT);
   fMenuFile->AddEntry(new TGHotString("Save &current layout"), M_FILE_SAVE_LAYOUT);
   fMenuFile->AddSeparator();
/*   fMenuFile->AddEntry(new TGHotString("Save current &measurement settings"), M_FILE_SAVE_MSETTINGS);
   fMenuFile->AddEntry(new TGHotString("Save current &analysis settings"), M_FILE_SAVE_ASETTINGS);
   fMenuFile->AddEntry(new TGHotString("Set default measurement settings"), M_FILE_DEF_MSETTINGS);
   fMenuFile->AddEntry(new TGHotString("Set default analysis settings"), M_FILE_DEF_ASETTINGS);
   fMenuFile->AddEntry(new TGHotString("Set default settings"), M_FILE_DEF_SETTINGS);
   fMenuFile->AddEntry(new TGHotString("&Check frame width (Testing)"), M_FILE_CHECK_WIDTH);
   fMenuFile->AddSeparator();*/
   fMenuFile->AddEntry(new TGHotString("E&xit"), M_FILE_EXIT);

/*   // Popup menu in menubar for Analysis controls
   fMenuHisttype = new TGPopupMenu(gClient->GetRoot()); // adds a cascade menu that will be incorporated into analysis controls
   fMenuHisttype->AddEntry(new TGHotString("1D &ADC histogram"), M_ANALYSIS_HISTTYPE_1DADC);
   fMenuHisttype->AddEntry(new TGHotString("1D &TDC histogram"), M_ANALYSIS_HISTTYPE_1DTDC);
   fMenuHisttype->AddEntry(new TGHotString("&2D ADC vs. TDC histogram"), M_ANALYSIS_HISTTYPE_2D);

   fMenuAnalysis = new TGPopupMenu(gClient->GetRoot());	// adds a new popup menu to the menubar
   fMenuAnalysis->AddPopup(new TGHotString("&Histogram type"), fMenuHisttype);
   fMenuAnalysis->AddEntry(new TGHotString("&Integrate spectrum"), M_ANALYSIS_INTEG);
   fMenuAnalysis->AddEntry(new TGHotString("Integrate spectrum (&X direction)"), M_ANALYSIS_INTEGX);
   fMenuAnalysis->AddEntry(new TGHotString("Integrate spectrum (&Y direction)"), M_ANALYSIS_INTEGY);
   fMenuAnalysis->AddEntry(new TGHotString("&Relative PDE"), M_ANALYSIS_PHOTMU);
   fMenuAnalysis->AddEntry(new TGHotString("&Breakdown voltage"), M_ANALYSIS_BREAKDOWN);
   fMenuAnalysis->AddEntry(new TGHotString("Surface 2&D scan"), M_ANALYSIS_SURFSCAN);
   fMenuAnalysis->AddEntry(new TGHotString("&Timing analysis"), M_ANALYSIS_TIMING);
*/
   // Popup menu in menubar for File controls
   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry(new TGHotString("Open &help in web browser"), M_HELP_WEBHELP);
   fMenuHelp->AddEntry(new TGHotString("&About"), M_HELP_ABOUT);

   // Connect all menu items with actions
   fMenuFile->Connect("Activated(Int_t)", "TGAppMainFrame", this, "HandleMenu(Int_t)");
//   fMenuAnalysis->Connect("Activated(Int_t)", "TGAppMainFrame", this, "HandleMenu(Int_t)");
   fMenuHelp->Connect("Activated(Int_t)", "TGAppMainFrame", this, "HandleMenu(Int_t)");

   // Draw the created popup menus on the menubar
   fMenuBar->AddPopup(new TGHotString("&File"), fMenuFile, fMenuBarItemLayout);
//   fMenuBar->AddPopup(new TGHotString("&Analysis"),fMenuAnalysis,fMenuBarItemLayout);
   fMenuBar->AddPopup(new TGHotString("&Help"), fMenuHelp, fMenuBarItemLayout);
}

// -------------------------------------------------------------------

// Setting the application subwindow layout --------------------------

void TGAppMainFrame::AppLayout()
{
   double numform[6], numform2[6];
   char selected[256];
   int subgroup[2];
   TGCompositeFrame *fH1, *fV1, *fH2, *fT1;
   TGGroupFrame *fG1;
   TGLabel *lab;

   TGLayoutHints *f0centerX = new TGLayoutHints(kLHintsCenterX,2,2,2,2);
   TGLayoutHints *f0leftX = new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2);
   TGLayoutHints *f0leftXnoleft = new TGLayoutHints(kLHintsLeft | kLHintsTop,0,2,2,2);
   TGLayoutHints *f0leftXpad = new TGLayoutHints(kLHintsLeft | kLHintsTop,12,12,2,2);
   TGLayoutHints *f0rightX = new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2);
   TGLayoutHints *f0rightXpad = new TGLayoutHints(kLHintsRight | kLHintsTop,12,12,2,2);
   TGLayoutHints *f0centerY = new TGLayoutHints(kLHintsCenterY,2,2,2,2);
   TGLayoutHints *f0center2d = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,2,2,2,2);
   TGLayoutHints *f1expandX = new TGLayoutHints(kLHintsExpandX,2,2,2,2);
   TGLayoutHints *f1expand2d = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2);
   TGLayoutHints *f1expandXpad = new TGLayoutHints(kLHintsExpandX,12,12,2,2);
   vector<string> selnames;
   vector<string> butnames;
   vector<int> checksel;

// ADST plotter pane (ADST layout) ---------------------------------------------------------
   subgroup[0] = (adstLayout[0]->GetWidth())-10;
   subgroup[1] = (adstLayout[0]->GetHeight())-10;

   adstCanvas = new TRootEmbeddedCanvas("adstCanvas",adstLayout[0],subgroup[0],3*subgroup[1]/4);
   adstLayout[0]->AddFrame(adstCanvas, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Selection of event
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSelectEvent -> Select the event in the current file to display (only if using single plotting)\n");
   adstSelectEvent = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 5; numform[1] = 0; numform[2] = 1;
   if(adstSelectEvent->TGLabelNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Select the event to display:", 1, numform, "center"))
      fH1->AddFrame(adstSelectEvent->outsidebox, f0centerX);

   adstShowEvent = new TGLabel(fH1, "The selected Auger event ID...                             ");
   adstShowEvent->SetWidth(subgroup[0]*(1./2.));
   adstShowEvent->SetHeight(30);
   adstShowEvent->SetWrapLength(-1);
   fH1->AddFrame(adstShowEvent, f0centerX);

   adstLayout[0]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Select the SD tank ID you wish to plot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSelecdSdId -> Dropdown menu for selecting the SD tank ID\n");
   adstSelectSdId = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Select SD tank ID...");
   sprintf(selected, "Select SD tank ID...");
   if(adstSelectSdId->TGLabelDrop(fH1, subgroup[0]*(1./4.+1./6.), 30, "SD tank ID:", selnames.size(), selnames, selected))
      fH1->AddFrame(adstSelectSdId->outsidebox, f0centerX);

   // Select the SD tank ID you wish to plot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSelecdFdId -> Dropdown menu for selecting the FD eye\n");
   adstSelectFdId = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Select FD eye...");
   sprintf(selected, "Select FD eye...");
   if(adstSelectFdId->TGLabelDrop(fH1, subgroup[0]*(1./4.+1./6.), 30, "FD eye:", selnames.size(), selnames, selected))
      fH1->AddFrame(adstSelectFdId->outsidebox, f0centerX);

   adstLayout[0]->AddFrame(fH1, f1expandXpad);

   // Actions for ADST plotter in the ADST layout subwindow
   adstSelectEvent->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "GetAugerEvent()"); // VoltageLimit
   (adstSelectEvent->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "GetAugerEvent()");
//   adstSelectSdId->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "SetSdId(Int_t)");
//   adstSelectFdId->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "SetFdId(Int_t)");
// ADST plotter pane (ADST layout) ---------------------------------------------------------

// Settings pane (ADST layout) -------------------------------------------------------------
   subgroup[0] = (adstLayout[2]->GetWidth())-4;

   // Open browser for file selection 
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *selectAdst -> Button to open ADST files\n");
   selectAdst = new TSubStructure();
   if(selectAdst->TGLabelButton(adstLayout[2], 3*subgroup[0]/4, 30, "File selection:", "...", "left"))
      adstLayout[2]->AddFrame(selectAdst->outsidebox, f0leftXpad);

   // List view of the opened files
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *adstList -> List box for opened histograms\n");
   adstList = new TGListBox(adstLayout[2],1);
   adstList->GetVScrollbar();
   adstList->Resize(300, (1*adstLayout[2]->GetHeight()/3)-10 );
   adstLayout[2]->AddFrame(adstList, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Multiple file selection toggle
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstMultiSelect -> 2 check buttons (enable multiple select, select everything)\n");
   adstMultiSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   checksel.erase(checksel.begin(), checksel.end());
   selnames.push_back("Multiple file select"); checksel.push_back(0);
   selnames.push_back("Select all listed files"); checksel.push_back(0);
   if(adstMultiSelect->TGCheckList(fH1, subgroup[0]/2, 30, selnames.size(), selnames, checksel, "horizontal", "left"))
      fH1->AddFrame(adstMultiSelect->outsidebox, f0leftX);
   
   // Previous/next controls, clear list and edit header
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstListCtrl -> Multiple buttons for controlling and displaying the histograms in list box\n");
   adstListCtrl = new TSubStructure();
   butnames.erase(butnames.begin(), butnames.end());
   butnames.push_back("<<");
   butnames.push_back(">>");
   butnames.push_back("Clear list");
   if(adstListCtrl->TGMultiButton(fH1, subgroup[0]/3, 30, butnames.size(), butnames, "right"))
      fH1->AddFrame(adstListCtrl->outsidebox, f0rightX);

   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for plot type
   lab = new TGLabel(fH1, "Plot type:");
   fH1->AddFrame(lab, f0leftX);
   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Plotting a distribution
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstPlotDist -> 1 check button (plot a distribution)\n");
   adstPlotDist = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   checksel.erase(checksel.begin(), checksel.end());
   selnames.push_back("Plot observable distribution"); checksel.push_back(1);
   if(adstPlotDist->TGCheckList(fH1, subgroup[0]/4, 30, selnames.size(), selnames, checksel, "horizontal", "left"))
      fH1->AddFrame(adstPlotDist->outsidebox, f0leftX);

   // Select the observable we wish to plot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstPlotDistSelect -> Dropdown menu for selecting the observable to plot\n");
   adstPlotDistSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Number of active stations");
   selnames.push_back("Total VEM signal (average)");
   selnames.push_back("Total VEM signal (sum)");
   selnames.push_back("Risetime");
   selnames.push_back("SD reconstructed energy");
   selnames.push_back("SD reconstructed zenith angle");
   selnames.push_back("S1000");
   selnames.push_back("Curvature");
   selnames.push_back("Distance from shower axis (average)");
   selnames.push_back("Number of simulated muons (sum)");
   selnames.push_back("Number of active eyes");
   selnames.push_back("FD reconstructed energy (average)");
   selnames.push_back("FD reconstructed energy (best)");
   selnames.push_back("FD reconstructed zenith angle");
   selnames.push_back("Depth of shower maximum (X_max)");
   selnames.push_back("Shower foot");
   sprintf(selected, "Depth of shower maximum (X_max)");
   if(adstPlotDistSelect->TGLabelDrop(fH1, 2*subgroup[0]/3, 30, "", selnames.size(), selnames, selected))
      fH1->AddFrame(adstPlotDistSelect->outsidebox, f0leftX);

   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Plotting a single observable
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstPlotSingle -> 1 check button (plot a single observable)\n");
   adstPlotSingle = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   checksel.erase(checksel.begin(), checksel.end());
   selnames.push_back("Plot single observable"); checksel.push_back(0);
   if(adstPlotSingle->TGCheckList(fH1, subgroup[0]/4, 30, selnames.size(), selnames, checksel, "horizontal", "left"))
      fH1->AddFrame(adstPlotSingle->outsidebox, f0leftX);

   // Select the observable we wish to plot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstPlotSingleSelect -> Dropdown menu for selecting the observable to plot\n");
   adstPlotSingleSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Station ID");
   selnames.push_back("Total VEM signal");
   selnames.push_back("Risetime");
   selnames.push_back("Nanosecond start time");
   selnames.push_back("Distance from shower axis");
   selnames.push_back("Number of simulated muons");
   selnames.push_back("Eye ID");
   selnames.push_back("FD reconstructed energy (total)");
   selnames.push_back("FD reconstructed energy (EM)");
   selnames.push_back("Depth of shower maximum (X_max)");
   selnames.push_back("LDF");
   selnames.push_back("VEM trace");
   selnames.push_back("Longitudinal profile");
   sprintf(selected, "Longitudinal profile");
   if(adstPlotSingleSelect->TGLabelDrop(fH1, 2*subgroup[0]/3, 30, "", selnames.size(), selnames, selected))
      fH1->AddFrame(adstPlotSingleSelect->outsidebox, f0leftX);

   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for plotting options
   lab = new TGLabel(fH1, "Plot options:");
   fH1->AddFrame(lab, f0leftX);
   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   // Selection parameters (shower foot, error on Xmax, error on energy, energy,...)
   // Values for zenith angle limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSetZenith -> Set limiting zenith angles\n");
   adstSetZenith = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 6; numform2[0] = 6; numform[1] = 2; numform2[1] = 2; numform[2] = 2; numform2[2] = 2;
   if(adstSetZenith->TGCheckDoubleNEntry(adstLayout[2], subgroup[0]/2, 30, "Zenith angle limits:", 0, 0.0, numform, 60.0, numform2, "center"))
      adstLayout[2]->AddFrame(adstSetZenith->outsidebox, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for energy limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSetEnergy -> Set limiting energies\n");
   adstSetEnergy = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 6; numform2[0] = 6; numform[1] = -1; numform2[1] = -1; numform[2] = 2; numform2[2] = 2;
   if(adstSetEnergy->TGCheckDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Energy limits:", 0, 1.e+17, numform, 1.e+21, numform2, "center"))
      fH1->AddFrame(adstSetEnergy->outsidebox, f0centerX);

   // Value for energy error
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSetEnergyErr -> Set error on energy\n");
   adstSetEnergyErr = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2; numform[3] = 2; numform[4] = 0; numform[5] = 1;
   if(adstSetEnergyErr->TGCheckNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Energy error (relative):", 0, 0.18, numform, "center"))
      fH1->AddFrame(adstSetEnergyErr->outsidebox, f0centerX);

   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Value for Xmax error
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSetXmaxErr -> Set error on Xmax\n");
   adstSetXmaxErr = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2; numform[2] = 2;
   if(adstSetXmaxErr->TGCheckNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "X_max error (g/cm^2):", 0, 40.00, numform, "center"))
      fH1->AddFrame(adstSetXmaxErr->outsidebox, f0centerX);

   // Value for shower foot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSetShfoot -> Set shower foot\n");
   adstSetShfoot = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2; numform[3] = 2; numform[4] = 0; numform[5] = 100;
   if(adstSetShfoot->TGCheckNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Shower foot (%):", 0, 10.00, numform, "center"))
      fH1->AddFrame(adstSetShfoot->outsidebox, f0centerX);

   adstLayout[2]->AddFrame(fH1, f1expandXpad);

   // Update plot button
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstUpdateBut -> Button to update the plot (also to set to defaults)\n");
   adstUpdateBut = new TSubStructure();
   butnames.erase(butnames.begin(), butnames.end());
   butnames.push_back("Update plot");
   butnames.push_back("Default options");
   if(adstUpdateBut->TGMultiButton(adstLayout[2], 3*subgroup[0]/4, 30, butnames.size(), butnames, "center"))
      adstLayout[2]->AddFrame(adstUpdateBut->outsidebox, f1expandXpad);

   // Actions for settings pane in the ADST layout subwindow
   selectAdst->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectDirectory()");
   adstMultiSelect->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "ListMultiSelect(=0)");
   adstMultiSelect->widgetChBox[1]->Connect("Clicked()", "TGAppMainFrame", this, "ListMultiSelect(=1)");
   adstList->Connect("DoubleClicked(Int_t)", "TGAppMainFrame", this, "FileListNavigation(Int_t)");
   adstListCtrl->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "FileListNavigation(=-2)");
   adstListCtrl->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "FileListNavigation(=-3)");
   adstListCtrl->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "ClearHistogramList()");
   adstPlotDist->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "ToggleAdstPlotType(=0)");
   adstPlotSingle->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "ToggleAdstPlotType(=1)");
   adstUpdateBut->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "AdstPlottingButton(=0)");	// VoltOut
   adstUpdateBut->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "AdstPlottingButton(=1)");
// Settings pane (ADST layout) -------------------------------------------------------------

// Filelist pane (MVA layout) ---------------------------------------------------------
   subgroup[0] = (mvaLayout[0]->GetWidth())-10;
   subgroup[1] = (mvaLayout[0]->GetHeight())-10;

   // Open browser for file selection 
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *selectMvafile -> Button to open files for MVA\n");
   selectMvafile = new TSubStructure();
   if(selectMvafile->TGLabelButton(mvaLayout[0], subgroup[0], 30, "File selection:", "...", "left"))
      mvaLayout[0]->AddFrame(selectMvafile->outsidebox, f0leftXpad);

   fH1 = new TGCompositeFrame(mvaLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Label for file list
   lab = new TGLabel(fH1, "ADST files:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[0]->AddFrame(fH1, f1expandXpad);

   // List view of the opened files (ADST files)
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *mvaList[0] -> List box for opened ADST files\n");
   mvaList[0] = new TGListBox(mvaLayout[0],1);
   mvaList[0]->GetVScrollbar();
   mvaList[0]->Resize(300, (1*mvaLayout[0]->GetHeight()/6) );
   mvaList[0]->SetMultipleSelections(kTRUE);
   mvaLayout[0]->AddFrame(mvaList[0], f1expandXpad);

   // Start rewriting of MVA observables from ADST 
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *startRewriting -> Button to start rewriting MVA observables from ADST\n");
   startRewriting = new TSubStructure();
   if(startRewriting->TGLabelButton(mvaLayout[0], subgroup[0], 30, "Select ADST files above to rewrite into a single rewritten ADST file (first list):", "Rewrite", "left"))
      mvaLayout[0]->AddFrame(startRewriting->outsidebox, f0leftXpad);

   // Only rewrite a sample of events from ADST 
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *sampleRewriting -> Button and number entry to start rewriting a sample of MVA observables from ADST\n");
   sampleRewriting = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2; numform[3] = 2; numform[4] = 0; numform[5] = 1;
   if(sampleRewriting->TGLabelNEntryButton(mvaLayout[0], subgroup[0], 30, "Select one ADST file above and a sample fraction to rewrite into a single rewritten ADST file (first list):", 0.1, numform, "Sample rewrite", "oneline"))
      mvaLayout[0]->AddFrame(sampleRewriting->outsidebox, f0leftXpad);

   fH1 = new TGCompositeFrame(mvaLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Label for file list
   lab = new TGLabel(fH1, "Rewritten ADST files:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[0]->AddFrame(fH1, f1expandXpad);

   // List view of the opened files (Rewritten ADST files)
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *mvaList[1] -> List box for opened rewritten files files\n");
   mvaList[1] = new TGListBox(mvaLayout[0],1);
   mvaList[1]->GetVScrollbar();
   mvaList[1]->Resize(300, (1*mvaLayout[0]->GetHeight()/6) );
   mvaList[1]->SetMultipleSelections(kTRUE);
   mvaLayout[0]->AddFrame(mvaList[1], f1expandXpad);

   // Start combining rewritten ADST files
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *startCombining -> Button to start combining the rewritten ADST files\n");
   startCombining = new TSubStructure();
   butnames.erase(butnames.begin(), butnames.end());
   butnames.push_back("Combine");
   butnames.push_back("Merge");
   if(startCombining->TGLabelMultiButton(mvaLayout[0], subgroup[0], 30, "Select rewritten ADST files above to combine or merge into one rewritten ADST file (second list):", butnames.size(), butnames, "left"))
      mvaLayout[0]->AddFrame(startCombining->outsidebox, f0leftXpad);

   fH1 = new TGCompositeFrame(mvaLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Label for file list
   lab = new TGLabel(fH1, "Files for MVA analysis:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[0]->AddFrame(fH1, f1expandXpad);

   // List view of the opened files (Rewritten ADST files for analyzing)
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *mvaList[2] -> List box for opened rewritten files that will be used in the MVA analysis\n");
   mvaList[2] = new TGListBox(mvaLayout[0],1);
   mvaList[2]->GetVScrollbar();
   mvaList[2]->Resize(300, (1*mvaLayout[0]->GetHeight()/6) );
   mvaList[2]->SetMultipleSelections(kFALSE);
   mvaLayout[0]->AddFrame(mvaList[2], f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Label for file list
   lab = new TGLabel(fH1, "Double click a file from the above list (third list) to use it in the MVA analysis.");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[0]->AddFrame(fH1, f1expandXpad);

   // Actions for filelist pane in the MVA layout subwindow
   selectMvafile->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectMvaDir()");
   startRewriting->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectRewrite(=0)");
   sampleRewriting->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectRewrite(=1)");
   startCombining->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectCombine()");
   startCombining->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "SelectMerge()");
   mvaList[2]->Connect("DoubleClicked(Int_t)", "TGAppMainFrame", this, "EnableMvaFile(Int_t)");
// Filelist pane (MVA layout) ---------------------------------------------------------

// Results pane (MVA layout) ---------------------------------------------------------
   subgroup[0] = (mvaLayout[1]->GetWidth())-10;
   subgroup[1] = (mvaLayout[1]->GetHeight())-10;

   // Label for results
   sigBackLabel = new TGLabel(mvaLayout[1], "MVA analysis results for all trees (______ signal vs. ______ background):                                               ");
   sigBackLabel->SetWidth(subgroup[0]);
   sigBackLabel->SetHeight(30);
   sigBackLabel->SetWrapLength(-1);
   mvaLayout[1]->AddFrame(sigBackLabel, f0centerX);

   fV1 = new TGCompositeFrame(mvaLayout[1], subgroup[0], 30, kVerticalFrame);
   for(int i = 0; i < 6; i++)
   {
      displayResult[i] = new TSubStructure();
      if(displayResult[i]->TGLabelTEntry(fV1, ((mvaLayout[1]->GetWidth())-10), 30, ("Results for TreeS" + IntToStr(i+1) + ":").c_str(), "", "oneline"))
      {
         displayResult[i]->widgetTE->SetState(kFALSE);
         fV1->AddFrame(displayResult[i]->outsidebox, f1expandXpad);
      }
   }
   mvaLayout[1]->AddFrame(fV1, f0leftX);

// Results pane (MVA layout) ---------------------------------------------------------

// MVA analysis pane (MVA layout) ---------------------------------------------------------
   subgroup[0] = (mvaLayout[2]->GetWidth())-10;
   subgroup[1] = (mvaLayout[2]->GetHeight())-10;

   // Selected file for MVA
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *selectedMva -> Text entry for currently selected MVA file\n");
   selectedMva = new TSubStructure();
   if(selectedMva->TGLabelTEntry(mvaLayout[2], subgroup[0], 30, "Selected file for MVA analysis:", "", "oneline"))
   {
      selectedMva->widgetTE->SetState(kFALSE);
      mvaLayout[2]->AddFrame(selectedMva->outsidebox, f1expandXpad);
   }
/* TODO */

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for plotting options
   lab = new TGLabel(fH1, "Observables to use in the MVA analysis:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // List view for selecting the mean value observable
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *selectObservables[0] -> List box for selecting mean value observables\n");
   selectObservables[0] = new TGListBox(fH1,1);
   selectObservables[0]->GetVScrollbar();
   selectObservables[0]->Resize((subgroup[0]/3)-10, 100 );
   selectObservables[0]->SetMultipleSelections(kTRUE);
   fH1->AddFrame(selectObservables[0], f0centerX);
   for(int i = 0; i < nrobs; i++)
   {
      selectObservables[0]->AddEntry((observables[i] + " (mean)").c_str(), i);
      if(observablesCheck[i] == 1)
         selectObservables[0]->Select(i, kTRUE);
   }
   selectObservables[0]->Layout();
   oldpos[0] = selectObservables[0]->GetVScrollbar()->GetPosition();

   // List view for selecting the mean value + negative error observable
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *selectObservables[1] -> List box for selecting mean value + negative error observables\n");
   selectObservables[1] = new TGListBox(fH1,1);
   selectObservables[1]->GetVScrollbar();
   selectObservables[1]->Resize((subgroup[0]/3)-10, 100 );
   selectObservables[1]->SetMultipleSelections(kTRUE);
   fH1->AddFrame(selectObservables[1], f0centerX);
   for(int i = 0; i < nrobs; i++)
   {
      selectObservables[1]->AddEntry((observables[i] + " (mean+neg)").c_str(), i);
      if(observablesCheck[i] == 2)
         selectObservables[1]->Select(i, kTRUE);
   }
   selectObservables[1]->Layout();
   oldpos[1] = selectObservables[1]->GetVScrollbar()->GetPosition();

   // List view for selecting the mean value + positive error observable
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *selectObservables[2] -> List box for selecting mean value + positive error observables\n");
   selectObservables[2] = new TGListBox(fH1,1);
   selectObservables[2]->GetVScrollbar();
   selectObservables[2]->Resize((subgroup[0]/3)-10, 100 );
   selectObservables[2]->SetMultipleSelections(kTRUE);
   fH1->AddFrame(selectObservables[2], f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);
   for(int i = 0; i < nrobs; i++)
   {
      selectObservables[2]->AddEntry((observables[i] + " (mean+pos)").c_str(), i);
      if(observablesCheck[i] == 3)
         selectObservables[2]->Select(i, kTRUE);
   }
   selectObservables[2]->Layout();
   oldpos[2] = selectObservables[2]->GetVScrollbar()->GetPosition();

   if(DBGSIG > 1) cout << "Current position = " << selectObservables[0]->GetVScrollbar()->GetPosition() << ", Range = " << selectObservables[0]->GetVScrollbar()->GetPosition() << endl;

   // Select the signal tree
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *signalSelect -> Dropdown menu for selecting the 'signal' tree\n");
   signalSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Select signal...");
   sprintf(selected, "Select signal...");
   if(signalSelect->TGLabelDrop(mvaLayout[2], subgroup[0]-10, 30, "Select 'signal' tree:", selnames.size(), selnames, selected))
      mvaLayout[2]->AddFrame(signalSelect->outsidebox, f0centerX);

   // Select the background tree
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *backgroundSelect -> Dropdown menu for selecting the 'background' tree\n");
   backgroundSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Select background...");
   sprintf(selected, "Select background...");
   if(backgroundSelect->TGLabelDrop(mvaLayout[2], subgroup[0]-10, 30, "Select 'background' tree:", selnames.size(), selnames, selected))
      mvaLayout[2]->AddFrame(backgroundSelect->outsidebox, f0centerX);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for MVA methods
   lab = new TGLabel(fH1, "Choose MVA analysis methods (choose only one to apply MVA cuts):");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Methods for MVA selection
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *methodsSelect -> Check buttons for selecting the methods for MVA\n");
   methodsSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   checksel.erase(checksel.begin(), checksel.end());
   selnames.push_back("Cut optimization (Cuts)"); checksel.push_back(0);
   selnames.push_back("Cut optimization (CutsD)"); checksel.push_back(0);
   selnames.push_back("1D likelihood (Likelihood)"); checksel.push_back(0);
   selnames.push_back("1D likelihood (LikelihoodPCA)"); checksel.push_back(0);
   selnames.push_back("Multidimensional likelihood (PDERS)"); checksel.push_back(0);
   selnames.push_back("Nearest neighbours (KNN)"); checksel.push_back(0);
   selnames.push_back("Linear discriminant (LD)"); checksel.push_back(0);
   selnames.push_back("Functional discriminant (FDA_GA)"); checksel.push_back(0);
   selnames.push_back("Neural network (MLPBNN)"); checksel.push_back(1);
   selnames.push_back("Support vector machine (SVM)"); checksel.push_back(0);
   selnames.push_back("Boosted decision trees (BDT)"); checksel.push_back(0);
   selnames.push_back("Friedman's rulefit (RuleFit)"); checksel.push_back(0);
   nrmethods = selnames.size();
   if(methodsSelect->TGCheckList(fH1, subgroup[0], 30, selnames.size(), selnames, checksel, "fourline", "center"))
      fH1->AddFrame(methodsSelect->outsidebox, f0leftX);

   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   // Save methods names
   for(int i = 0; i < nrmethods; i++)
   {
      if(selnames[i] == "Cut optimization (Cuts)")
         methods.push_back("Cuts");
      else if(selnames[i] == "Cut optimization (CutsD)")
         methods.push_back("CutsD");
      else if(selnames[i] == "1D likelihood (Likelihood)")
         methods.push_back("Likelihood");
      else if(selnames[i] == "1D likelihood (LikelihoodPCA)")
         methods.push_back("LikelihoodPCA");
      else if(selnames[i] == "Multidimensional likelihood (PDERS)")
         methods.push_back("PDERS");
      else if(selnames[i] == "Nearest neighbours (KNN)")
         methods.push_back("KNN");
      else if(selnames[i] == "Linear discriminant (LD)")
         methods.push_back("LD");
      else if(selnames[i] == "Functional discriminant (FDA_GA)")
         methods.push_back("FDA_GA");
      else if(selnames[i] == "Neural network (MLPBNN)")
         methods.push_back("MLPBNN");
      else if(selnames[i] == "Support vector machine (SVM)")
         methods.push_back("SVM");
      else if(selnames[i] == "Boosted decision trees (BDT)")
         methods.push_back("BDT");
      else if(selnames[i] == "Friedman's rulefit (RuleFit)")
         methods.push_back("RuleFit");
   }

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for MVA cuts
   lab = new TGLabel(fH1, "Choose MVA cut for the analysis:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Cut on MVA values
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutMva -> Set cut on MVA values\n");
   cutMva = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 8; numform[1] = 4;
   if(cutMva->TGLabelNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Select MVA cut value:", 0.00, numform, "center"))
      fH1->AddFrame(cutMva->outsidebox, f0centerX);

   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for performing data energy cuts
   lab = new TGLabel(fH1, "Cut on energy and/or zenith angle:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for energy limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutEnergy -> Set limiting energies for data\n");
   cutEnergy = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 6; numform2[0] = 6; numform[1] = 2; numform2[1] = 2; numform[2] = 2; numform2[2] = 2;
   if(cutEnergy->TGCheckDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Energy limits:", 0, 17., numform, 21., numform2, "center"))
      fH1->AddFrame(cutEnergy->outsidebox, f0centerX);

   // Values for zenith angle limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutZenith -> Set limiting zenith angle for data\n");
   cutZenith = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 6; numform2[0] = 6; numform[1] = 2; numform2[1] = 2; numform[2] = 2; numform2[2] = 2;
   if(cutZenith->TGCheckDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Zenith angle limits:", 0, 0., numform, 60., numform2, "center"))
      fH1->AddFrame(cutZenith->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Value for the number of zenith angle bins
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutEnergyBins -> Set number of zenith angle bins\n");
   cutEnergyBins = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 5; numform[2] = 1;
   if(cutEnergyBins->TGLabelNEntry(fH1, subgroup[0]*(1./3.), 30, "Number of energy bins:", 1, numform, "center"))
      fH1->AddFrame(cutEnergyBins->outsidebox, f0centerX);

   // Dropdown to select the zenith angle bin
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutEnergyBinSelect -> Dropdown menu for selecting the zenith angle bin\n");
   cutEnergyBinSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   sprintf(selected, "01 (%.1le - %.1le)", pow(10, cutEnergy->widgetNE[0]->GetNumber()), pow(10, cutEnergy->widgetNE[1]->GetNumber()));
   selnames.push_back(string(selected));
   butnames.erase(butnames.begin(), butnames.end());
   butnames.push_back("Check bin");
   if(cutEnergyBinSelect->TGLabelDropMultiButton(fH1, subgroup[0]*(1./2.), 30, "Select energy bin:", selnames.size(), selnames, selected, butnames.size(), butnames, "center"))
      fH1->AddFrame(cutEnergyBinSelect->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Value for the number of zenith angle bins
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutZenithBins -> Set number of zenith angle bins\n");
   cutZenithBins = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 5; numform[2] = 1;
   if(cutZenithBins->TGLabelNEntry(fH1, subgroup[0]*(1./3.), 30, "Number of zenith angle bins:", 1, numform, "center"))
      fH1->AddFrame(cutZenithBins->outsidebox, f0centerX);

   // Dropdown to select the zenith angle bin
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *cutZenithBinSelect -> Dropdown menu for selecting the zenith angle bin\n");
   cutZenithBinSelect = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   sprintf(selected, "01 (%.1lf - %.1lf)", cutZenith->widgetNE[0]->GetNumber(), cutZenith->widgetNE[1]->GetNumber());
   selnames.push_back(string(selected));
   butnames.erase(butnames.begin(), butnames.end());
   butnames.push_back("Check bin");
   if(cutZenithBinSelect->TGLabelDropMultiButton(fH1, subgroup[0]*(1./2.), 30, "Select zenith angle bin:", selnames.size(), selnames, selected, butnames.size(), butnames, "center"))
      fH1->AddFrame(cutZenithBinSelect->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   // Setup plot limits for observables
   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Label for plot limits
   lab = new TGLabel(fH1, "Plot limits for available observables:");
   fH1->AddFrame(lab, f0leftX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for xmax plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetXmax -> Set xmax plotting limits\n");
   plotSetXmax = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetXmax->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Xmax plot limits:", 400., numform, 1200., numform2, "center"))
      fH1->AddFrame(plotSetXmax->outsidebox, f0centerX);

   // Value for x0 plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetX0 -> Set x0 plotting limits\n");
   plotSetX0 = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetX0->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "X0 plot limits:", -550., numform, 450., numform2, "center"))
      fH1->AddFrame(plotSetX0->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for lambda plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetLambda -> Set lambda plotting limits\n");
   plotSetLambda = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetLambda->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Lambda plot limits:", 35., numform, 135., numform2, "center"))
      fH1->AddFrame(plotSetLambda->outsidebox, f0centerX);

   // Value for FD energy plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetFdenergy -> Set FD energy plotting limits\n");
   plotSetFdenergy = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetFdenergy->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "FD energy plot limtis:", 17., numform, 20., numform2, "center"))
      fH1->AddFrame(plotSetFdenergy->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for shfoot plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetShfoot -> Set shfoot plotting limits\n");
   plotSetShfoot = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetShfoot->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Shower foot plot limits:", 300., numform, 900., numform2, "center"))
      fH1->AddFrame(plotSetShfoot->outsidebox, f0centerX);

   // Value for S1000 plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetShwsize -> Set S1000 plotting limits\n");
   plotSetShwsize = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetShwsize->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "S1000 plot limtis:", 0., numform, 170., numform2, "center"))
      fH1->AddFrame(plotSetShwsize->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for nrmu plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetNrmu -> Set nrmu plotting limits\n");
   plotSetNrmu = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 8; numform2[0] = 8; numform[1] = -1; numform2[1] = -1;
   if(plotSetNrmu->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Nr. muons plot limits:", 0., numform, 90000000., numform2, "center"))
      fH1->AddFrame(plotSetNrmu->outsidebox, f0centerX);

   // Value for curvature plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetCurvature -> Set curvature plotting limits\n");
   plotSetCurvature = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 8; numform2[0] = 8; numform[1] = -1; numform2[1] = -1;
   if(plotSetCurvature->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Curvature plot limtis:", 1.e-4, numform, 0.25e-3, numform2, "center"))
      fH1->AddFrame(plotSetCurvature->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Values for risetime plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetRisetime -> Set risetime plotting limits\n");
   plotSetRisetime = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetRisetime->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Risetime plot limits:", 0., numform, 650., numform2, "center"))
      fH1->AddFrame(plotSetRisetime->outsidebox, f0centerX);

   // Value for MVA plotting limits
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotSetCurvature -> Set MVA plotting limits\n");
   plotSetMva = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform2[0] = 7; numform[1] = 2; numform2[1] = 2;
   if(plotSetMva->TGLabelDoubleNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "MVA plot limtis:", -2, numform, 2., numform2, "center"))
      fH1->AddFrame(plotSetMva->outsidebox, f0centerX);
   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[2], subgroup[0], 30, kHorizontalFrame);
   // Enabling special calculations
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *mvaCalculations -> Check buttons for enabling special calculations\n");
   specialMva = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   checksel.erase(checksel.begin(), checksel.end());
   selnames.push_back("Open MVA graphical interface after training and testing"); checksel.push_back(1);
   nrspecialMva = selnames.size();
   if(specialMva->TGCheckList(fH1, subgroup[0], 30, selnames.size(), selnames, checksel, "vertical", "left"))
      fH1->AddFrame(specialMva->outsidebox, f0leftX);

   mvaLayout[2]->AddFrame(fH1, f1expandXpad);

   // Start MVA analysis button
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *startMvaAnalysis -> Button to update the plot (also to set to defaults)\n");
   startMvaAnalysis = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Start MVA analysis");
   selnames.push_back("Apply MVA cut");
   selnames.push_back("Check bins");
   selnames.push_back("Default options");
   if(startMvaAnalysis->TGMultiButton(mvaLayout[2], 3*subgroup[0]/4, 30, selnames.size(), selnames, "center"))
   {
      startMvaAnalysis->widgetTB[1]->SetState(kButtonDisabled);
      mvaLayout[2]->AddFrame(startMvaAnalysis->outsidebox, f1expandXpad);
   }

   // Actions for MVA analysis pane in the MVA layout subwindow
   selectObservables[0]->GetVScrollbar()->Connect("PositionChanged(Int_t)", "TGAppMainFrame", this, "ScrollObservables(Int_t)");
   selectObservables[0]->Connect("Selected(Int_t)", "TGAppMainFrame", this, "PickObservables(Int_t)");
   selectObservables[1]->GetVScrollbar()->Connect("PositionChanged(Int_t)", "TGAppMainFrame", this, "ScrollObservables(Int_t)");
   selectObservables[1]->Connect("Selected(Int_t)", "TGAppMainFrame", this, "PickObservables(Int_t)");
   selectObservables[2]->GetVScrollbar()->Connect("PositionChanged(Int_t)", "TGAppMainFrame", this, "ScrollObservables(Int_t)");
   selectObservables[2]->Connect("Selected(Int_t)", "TGAppMainFrame", this, "PickObservables(Int_t)");
   signalSelect->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "UpdateSigTreeSelection(Int_t)");
   backgroundSelect->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "UpdateBackTreeSelection(Int_t)");
   cutEnergy->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateEnergyBinSelect()");
   (cutEnergy->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateEnergyBinSelect()");
   cutEnergy->widgetNE[1]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateEnergyBinSelect()");
   (cutEnergy->widgetNE[1]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateEnergyBinSelect()");
   cutEnergyBins->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateEnergyBinSelect()");
   (cutEnergyBins->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateEnergyBinSelect()");
   cutEnergyBinSelect->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "CheckEnergyBin()");
   cutZenith->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateZenithBinSelect()");
   (cutZenith->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateZenithBinSelect()");
   cutZenith->widgetNE[1]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateZenithBinSelect()");
   (cutZenith->widgetNE[1]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateZenithBinSelect()");
   cutZenithBins->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateZenithBinSelect()");
   (cutZenithBins->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateZenithBinSelect()");
   cutZenithBinSelect->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "CheckZenithBin()");
   startMvaAnalysis->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "StartMvaAnalysis(=0)");
   startMvaAnalysis->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "StartMvaAnalysis(=1)");
   startMvaAnalysis->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "StartMvaAnalysis(=2)");
   startMvaAnalysis->widgetTB[3]->Connect("Clicked()", "TGAppMainFrame", this, "StartMvaAnalysis(=3)");

// MVA analysis pane (MVA layout) ---------------------------------------------------------

// Options pane (Cuts layout) ---------------------------------------------------------
   subgroup[0] = (cutsLayout[0]->GetWidth())-10;
   subgroup[1] = (cutsLayout[0]->GetHeight())-10;

   // Selection of configure file
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *configFile -> Button to select or edit the cuts files\n");
   configFile = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Open");
   selnames.push_back("Edit");
   if(configFile->TGLabelTEntryMultiButton(cutsLayout[0], subgroup[0], 30, "Select the configuration file:", "No file selected!", selnames.size(), selnames, "center"))
   {
      cutsLayout[0]->AddFrame(configFile->outsidebox, f1expandXpad);
      configFile->widgetTE->SetState(kFALSE);
   }

   // Selection of SD cuts file
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *sdcutsFile -> Button to select or edit the cuts files\n");
   sdcutsFile = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Open");
   selnames.push_back("Edit");
   if(sdcutsFile->TGLabelTEntryMultiButton(cutsLayout[0], subgroup[0], 30, "Select the SD cuts file:", "No file selected!", selnames.size(), selnames, "center"))
   {
      cutsLayout[0]->AddFrame(sdcutsFile->outsidebox, f1expandXpad);
      sdcutsFile->widgetTE->SetState(kFALSE);
   }

   // Selection of FD cuts file
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fdcutsFile -> Button to select or edit the cuts files\n");
   fdcutsFile = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Open");
   selnames.push_back("Edit");
   if(fdcutsFile->TGLabelTEntryMultiButton(cutsLayout[0], subgroup[0], 30, "Select the FD cuts file:", "No file selected!", selnames.size(), selnames, "center"))
   {
      cutsLayout[0]->AddFrame(fdcutsFile->outsidebox, f1expandXpad);
      fdcutsFile->widgetTE->SetState(kFALSE);
   }

   // Open browser for file selection
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *selectCutsInputFile -> Button to open files for selection cut analysis\n");
   selectCutsInputFile = new TSubStructure();
   if(selectCutsInputFile->TGLabelButton(cutsLayout[0], 3*subgroup[0]/4, 30, "File selection:", "...", "left"))
      cutsLayout[0]->AddFrame(selectCutsInputFile->outsidebox, f0leftXpad);

   // List view of the opened files for cuts selection
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *cutsInputList -> List box for opened files for cuts selection\n");
   cutsInputList = new TGListBox(cutsLayout[0],1);
   cutsInputList->GetVScrollbar();
   cutsInputList->Resize(300, (1*cutsLayout[0]->GetHeight()/3)-10 );
   cutsInputList->SetMultipleSelections(kTRUE);
   cutsLayout[0]->AddFrame(cutsInputList, f1expandXpad);
   
   // Start the selection
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *startCutsSelection -> Buttons to start the cuts selection\n");
   startCutsSelection = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Start applying cuts");
   selnames.push_back("Clear");
   if(startCutsSelection->TGMultiButton(cutsLayout[0], subgroup[0], 30, selnames.size(), selnames, "center"))
      cutsLayout[0]->AddFrame(startCutsSelection->outsidebox, f1expandXpad);

   // Actions for options in the Cuts layout subwindow
   configFile->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "CutsOpen(=0)");
   configFile->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "CutsEdit(=0)");
   sdcutsFile->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "CutsOpen(=1)");
   sdcutsFile->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "CutsEdit(=1)");
   fdcutsFile->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "CutsOpen(=2)");
   fdcutsFile->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "CutsEdit(=2)");
   selectCutsInputFile->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectCutsDirectory()");
   startCutsSelection->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "CutsApply(=0)");
   startCutsSelection->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "CutsApply(=1)");
// Options pane (Cuts layout) ---------------------------------------------------------

/*   adstCanvas = new TRootEmbeddedCanvas("adstCanvas",adstLayout[0],subgroup[0],5*subgroup[1]/6);
   adstLayout[0]->AddFrame(adstCanvas, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Selection of event
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSelectEvent -> Select the event in the current file to display (only if using single plotting)\n");
   adstSelectEvent = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 5; numform[1] = 0; numform[2] = 1;
   if(adstSelectEvent->TGLabelNEntry(fH1, subgroup[0]*(1./4.+1./6.), 30, "Select the event to display:", 1, numform, "center"))
      fH1->AddFrame(adstSelectEvent->outsidebox, f0centerX);

   adstShowEvent = new TGLabel(fH1, "The selected Auger event ID...                             ");
   adstShowEvent->SetWidth(subgroup[0]*(1./2.));
   adstShowEvent->SetHeight(30);
   adstShowEvent->SetWrapLength(-1);
   fH1->AddFrame(adstShowEvent, f0centerX);

   adstLayout[0]->AddFrame(fH1, f1expandXpad);

   fH1 = new TGCompositeFrame(adstLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Select the SD tank ID you wish to plot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSelecdSdId -> Dropdown menu for selecting the SD tank ID\n");
   adstSelectSdId = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Select SD tank ID...");
   sprintf(selected, "Select SD tank ID...");
   if(adstSelectSdId->TGLabelDrop(fH1, subgroup[0]*(1./4.+1./6.), 30, "SD tank ID:", selnames.size(), selnames, selected))
      fH1->AddFrame(adstSelectSdId->outsidebox, f0centerX);

   // Select the SD tank ID you wish to plot
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adstSelecdFdId -> Dropdown menu for selecting the FD eye\n");
   adstSelectFdId = new TSubStructure();
   selnames.erase(selnames.begin(), selnames.end());
   selnames.push_back("Select FD eye...");
   sprintf(selected, "Select FD eye...");
   if(adstSelectFdId->TGLabelDrop(fH1, subgroup[0]*(1./4.+1./6.), 30, "FD eye:", selnames.size(), selnames, selected))
      fH1->AddFrame(adstSelectFdId->outsidebox, f0centerX);

   adstLayout[0]->AddFrame(fH1, f1expandXpad);

   // Actions for ADST plotter in the ADST layout subwindow
   adstSelectEvent->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "GetAugerEvent()"); // VoltageLimit
   (adstSelectEvent->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "GetAugerEvent()");
   adstSelectSdId->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "SetSdId(Int_t)"); // ChangeUnits
   adstSelectFdId->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "SetFdId(Int_t)");*/
// Filelist pane (ADST layout) ---------------------------------------------------------

}

// -------------------------------------------------------------------

// Closing main window and checking about information ----------------

void TGAppMainFrame::CloseWindow()
{
   gApplication->Terminate(0);
}

Bool_t TGAppMainFrame::About()
{
   int ret = 0;

   new TGMsgBox(gClient->GetRoot(), fMain, fMain->GetWindowName(), "Software for analysis of CORSIKA\nsimulations and reconstructions with Offline. Enables multivariate\nanalysis of mass composition sensitive observables.\n\nCreated by Gasper Kukec Mezek (gasper.kukec@ung.si),\nUpdated on July 17th 2015", kMBIconQuestion, kMBClose, &ret);

   return kFALSE;
}

// -------------------------------------------------------------------

// Main function -----------------------------------------------------
void root_app()
{
   if(DBGSIG > 1)
   {
      printf("root_app(): Starting objects\n");
      gObjectTable->Print();
   }

   int winWidth = 1240;
   int winHeight = 800;
   layoutMainWindow(&winWidth, &winHeight);

   new TGAppMainFrame(gClient->GetRoot(), winWidth, winHeight);
}

int main(int argc, char **argv)
{
   if(DBGSIG > 1)
   {
      printf("main(): Starting objects\n");
      gObjectTable->Print();
   }

   TApplication theApp("MdiTest", &argc, argv);
   root_app();
   theApp.Run();

   return 0;
}

// -------------------------------------------------------------------

/*TGAppMainFrame::TGAppMainFrame(const TGWindow *p, int w, int h)
{
   TGCompositeFrame *fT1;
   idtotal = 0;

   char *cTemp;

   // Prepare the default open directory (basedir)
   currentMeasDir = new char[1024];
   sprintf(currentMeasDir, "%s/results", rootdir);
   currentAnalDir = new char[1024];
   sprintf(currentAnalDir, "%s/results", rootdir);

   // CAMAC and Scope objects
   gDaq = new daq();
   gScopeDaq = new daqscope();

   // Define main window and menubar
   fMain = new TGMainFrame(p, w, h, kVerticalFrame);
   
   // Initialize the menu
   fMenuBar = new TGMenuBar(fMain, 200, 30);
   InitMenu();
   fMain->AddFrame(fMenuBar, new TGLayoutHints(kLHintsExpandX | kLHintsTop));

   // Prepare the tabbed structure
   fTab = new TGTab(fMain, 500, 500);

   // Read the layout we wish to use
   int frmWidth[measwin+mvawin], frmHeight[measwin+mvawin];
   LayoutRead(measwin+mvawin, frmWidth, frmHeight);

   // Structure the measurement layout window
   int *vert, *hor, *wPane, *hPane;

   vert = new int[2]; hor = new int;
   *hor = 1;
   vert[0] = 1; vert[1] = 0;

   const char *measFrmTit[] = {"Settings pane", "Display", "Main measurement window"};
   wPane = new int[measwin]; hPane = new int[measwin];
   for(int i = 0; i < measwin; i++)
   {
      wPane[i] = frmWidth[i];
      hPane[i] = frmHeight[i];
   }

   fT1 = fTab->AddTab("Measurement");
   TGSplitter(fT1, "horizontal", hor, vert, measFrmTit, adstLayout, wPane, hPane);
   fT1->AddFrame(fLayout[idtotal], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   idtotal++;
   delete[] vert; delete hor; delete[] wPane; delete[] hPane;

   // Structure the analysis layout window
   vert = new int[2]; hor = new int;
   *hor = 1;
   vert[0] = 1; vert[1] = 1;

   const char *analysisFrmTit[] = {"Histogram file selection", "Analysis", "Histogram", "Histogram controls"};
   wPane = new int[mvawin]; hPane = new int[mvawin];
   for(int i = 0; i < mvawin; i++)
   {
      wPane[i] = frmWidth[i+measwin];
      hPane[i] = frmHeight[i+measwin];
   }

   fT1 = fTab->AddTab("Analysis");
   TGSplitter(fT1, "horizontal", hor, vert, analysisFrmTit, mvaLayout, wPane, hPane);
   fT1->AddFrame(fLayout[idtotal], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   idtotal++;
   delete[] vert; delete hor; delete[] wPane; delete[] hPane;

   // Structure the monitoring layout window (Fieldpoint) //TODO
   fT1 = fTab->AddTab("Monitoring");
   fTab->SetEnabled(2,kFALSE); //TODO

   // Structure the help layout window
   fT1 = fTab->AddTab("Help");
   TGTextView *helpdesc;
   const TGFont *tfont = gClient->GetFont(HELPFONT);
   FontStruct_t helpFont = tfont->GetFontStruct();
   helpdesc = new TGTextView(fT1,100,100);
   helpdesc->SetFont(helpFont);
   helpdesc->SetForegroundColor(0x000000);
   fT1->AddFrame(helpdesc, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   
   cTemp = new char[1024];
   sprintf(cTemp, "%s/doc/README", rootdir);
   if(helpdesc->LoadFile(cTemp))
   {
      if(DBGSIG) printf("TGAppMainFrame(): Help file correctly loaded.\n");
   }
   else
      printf("Error! Help file not loaded.\n");
   delete[] cTemp;
   helpdesc->AddLine("");

   fMain->AddFrame(fTab, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

   // Set the inner layout of each frame
   AppLayout();

   fMain->SetWindowName(WINDOW_NAME);
   fMain->MapSubwindows();
   fMain->MapWindow();
   fMain->Layout();

   // Prepare initial settings
   EnableScan(0);	//. 
   EnableScan(1);	// Grey out scan
   EnableScan(2);	// options by default
   EnableScan(3);	//.
   EnableLiveUpdate();	//. Disable the live histogram update at beginning
   VoltOut(1);		//. Get the output voltage save in file
   HistogramOptions(1);	//. Enable clean plots by default
   plotType->widgetTB[0]->SetDown(kTRUE);			//.
   fMenuHisttype->CheckEntry(M_ANALYSIS_HISTTYPE_1DADC);	// Set the ADC histogram
   fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_1DTDC);	// to show by default
   fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_2D);		//.
   acqStarted = false;	//. At program start, the acquisition is stopped
   ToolTipSet();	//. Turn on tooltips
   PositionSet(1);	//. Get starting table position
   RotationSet(1);	//. Get starting rotation

   if(DBGSIG > 1)
   {
      printf("TGAppMainFrame(): At end of constructor\n");
      gObjectTable->Print();
   }
}

TGAppMainFrame::~TGAppMainFrame()
{
   fMain->Cleanup();
   delete fMain;
}

// -------------------------------------------------------------------

// Event handler for menubar actions ---------------------------------

void TGAppMainFrame::HandleMenu(Int_t id)
{
   int ret = 0;
   char cmd[256];

   switch(id)
   {
      case M_FILE_SET_LAYOUT:
	 LayoutSet();
	 break;

      case M_FILE_SAVE_LAYOUT:
	 LayoutSave();
	 break;

      case M_FILE_SAVE_MSETTINGS:
	 SaveMeasSettings();
	 break;

      case M_FILE_SAVE_ASETTINGS:
	 SaveAnalSettings();
	 break;

      case M_FILE_DEF_MSETTINGS:
         printf("Reseting measurement settings to default.\n");
         sprintf(cmd, "cp %s/settings/default_measurement_settings.txt %s/settings/measurement_settings.txt", rootdir, rootdir);
         ret = system(cmd);
	 break;

      case M_FILE_DEF_ASETTINGS:
         printf("Reseting analysis settings to default.\n");
         sprintf(cmd, "cp %s/settings/default_analysis_settings.txt %s/settings/analysis_settings.txt", rootdir, rootdir);
         ret = system(cmd);
	 break;

      case M_FILE_DEF_SETTINGS:
         printf("Reseting all settings to default.\n");
         sprintf(cmd, "cp %s/settings/default_measurement_settings.txt %s/settings/measurement_settings.txt", rootdir, rootdir);
         ret = system(cmd);
         sprintf(cmd, "cp %s/settings/default_analysis_settings.txt %s/settings/analysis_settings.txt", rootdir, rootdir);
         ret = system(cmd);
	 break;

      case M_FILE_CHECK_WIDTH:
	 printf("\nSettings window: %dx%d\n", adstLayout[0]->GetWidth(), adstLayout[0]->GetHeight());
	 printf("Histogram window: %dx%d\n", adstLayout[1]->GetWidth(), adstLayout[1]->GetHeight());
	 printf("Main measurement window: %dx%d\n", adstLayout[2]->GetWidth(), adstLayout[2]->GetHeight());
	 printf("Histogram file window: %dx%d\n", mvaLayout[0]->GetWidth(), mvaLayout[0]->GetHeight());
	 printf("Analysis window: %dx%d\n", mvaLayout[1]->GetWidth(), mvaLayout[1]->GetHeight());
	 printf("Histogram window: %dx%d\n", mvaLayout[2]->GetWidth(), mvaLayout[2]->GetHeight());
	 printf("Histogram controls window: %dx%d\n", mvaLayout[3]->GetWidth(), mvaLayout[3]->GetHeight());

	 printf("Main window: %dx%d\n", fMain->GetWidth(), fMain->GetHeight());
	 printf("Menu bar: %dx%d\n", fMenuBar->GetWidth(), fMenuBar->GetHeight());
	 printf("Tab window: %dx%d\n", fTab->GetWidth(), fTab->GetHeight());
         break;

      case M_FILE_EXIT:
         CloseWindow();
	 break;

      case M_ANALYSIS_HISTTYPE_1DADC:
         ChangeHisttype(0);
	 break;

      case M_ANALYSIS_HISTTYPE_1DTDC:
         ChangeHisttype(1);
	 break;

      case M_ANALYSIS_HISTTYPE_2D:
         ChangeHisttype(2);
	 break;

      case M_ANALYSIS_INTEG:
	 fTab->SetTab(1);
	 analTab->SetTab(0);
	 for(int i = 0; i < 3; i++)
	 {
	    if(intSpect->widgetChBox[i]->IsDown())
               intSpect->widgetChBox[i]->SetState(kButtonUp);
	 }
	 break;

      case M_ANALYSIS_INTEGX:
	 fTab->SetTab(1);
	 analTab->SetTab(0);
	 for(int i = 0; i < 3; i++)
	 {
            if(i == 0)
	       intSpect->widgetChBox[i]->SetState(kButtonDown);
	    else
	    {
	       if(intSpect->widgetChBox[i]->IsDown())
                  intSpect->widgetChBox[i]->SetState(kButtonUp);
	    }
	 }
	 break;

      case M_ANALYSIS_INTEGY:
	 fTab->SetTab(1);
	 analTab->SetTab(0);
	 for(int i = 0; i < 3; i++)
	 {
            if(i == 1)
	       intSpect->widgetChBox[i]->SetState(kButtonDown);
	    else
	    {
	       if(intSpect->widgetChBox[i]->IsDown())
                  intSpect->widgetChBox[i]->SetState(kButtonUp);
	    }
	 }
	 break;

      case M_ANALYSIS_PHOTMU:
	 fTab->SetTab(1);
	 analTab->SetTab(1);
	 for(int i = 0; i < 3; i++)
	 {
            if(i == 2)
	       intSpect->widgetChBox[i]->SetState(kButtonDown);
	    else
	    {
	       if(intSpect->widgetChBox[i]->IsDown())
                  intSpect->widgetChBox[i]->SetState(kButtonUp);
	    }
	 }
	 break;

      case M_ANALYSIS_BREAKDOWN:
	 fTab->SetTab(1);
	 analTab->SetTab(2);
	 break;

      case M_ANALYSIS_SURFSCAN:
	 fTab->SetTab(1);
	 analTab->SetTab(3);
	 break;

      case M_ANALYSIS_TIMING:
	 fTab->SetTab(1);
	 analTab->SetTab(4);
	 break;

      case M_HELP_WEBHELP:
         printf("TGAppMainFrame::HandleMenu(): Opening %s/doc/documentation.html in a web browser.\n", rootdir);
	 sprintf(cmd, "xdg-open %s/doc/documentation.html &", rootdir);
	 system(cmd);
	 break;

      case M_HELP_ABOUT:
         About();
	 break;

      default:
//         fMainFrame->SetCurrent(id);
	 break;
   }
}

// -------------------------------------------------------------------

// Initialize the main window menu -----------------------------------

void TGAppMainFrame::InitMenu()
{
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

   // Popup menu in menubar for File controls
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry(new TGHotString("Set &user layout"), M_FILE_SET_LAYOUT);
   fMenuFile->AddEntry(new TGHotString("Save &current layout"), M_FILE_SAVE_LAYOUT);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry(new TGHotString("Save current &measurement settings"), M_FILE_SAVE_MSETTINGS);
   fMenuFile->AddEntry(new TGHotString("Save current &analysis settings"), M_FILE_SAVE_ASETTINGS);
   fMenuFile->AddEntry(new TGHotString("Set default measurement settings"), M_FILE_DEF_MSETTINGS);
   fMenuFile->AddEntry(new TGHotString("Set default analysis settings"), M_FILE_DEF_ASETTINGS);
   fMenuFile->AddEntry(new TGHotString("Set default settings"), M_FILE_DEF_SETTINGS);
   fMenuFile->AddEntry(new TGHotString("&Check frame width (Testing)"), M_FILE_CHECK_WIDTH);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry(new TGHotString("E&xit"), M_FILE_EXIT);

   // Popup menu in menubar for Analysis controls
   fMenuHisttype = new TGPopupMenu(gClient->GetRoot()); // adds a cascade menu that will be incorporated into analysis controls
   fMenuHisttype->AddEntry(new TGHotString("1D &ADC histogram"), M_ANALYSIS_HISTTYPE_1DADC);
   fMenuHisttype->AddEntry(new TGHotString("1D &TDC histogram"), M_ANALYSIS_HISTTYPE_1DTDC);
   fMenuHisttype->AddEntry(new TGHotString("&2D ADC vs. TDC histogram"), M_ANALYSIS_HISTTYPE_2D);

   fMenuAnalysis = new TGPopupMenu(gClient->GetRoot());	// adds a new popup menu to the menubar
   fMenuAnalysis->AddPopup(new TGHotString("&Histogram type"), fMenuHisttype);
   fMenuAnalysis->AddEntry(new TGHotString("&Integrate spectrum"), M_ANALYSIS_INTEG);
   fMenuAnalysis->AddEntry(new TGHotString("Integrate spectrum (&X direction)"), M_ANALYSIS_INTEGX);
   fMenuAnalysis->AddEntry(new TGHotString("Integrate spectrum (&Y direction)"), M_ANALYSIS_INTEGY);
   fMenuAnalysis->AddEntry(new TGHotString("&Relative PDE"), M_ANALYSIS_PHOTMU);
   fMenuAnalysis->AddEntry(new TGHotString("&Breakdown voltage"), M_ANALYSIS_BREAKDOWN);
   fMenuAnalysis->AddEntry(new TGHotString("Surface 2&D scan"), M_ANALYSIS_SURFSCAN);
   fMenuAnalysis->AddEntry(new TGHotString("&Timing analysis"), M_ANALYSIS_TIMING);

   // Popup menu in menubar for File controls
   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry(new TGHotString("Open &help in web browser"), M_HELP_WEBHELP);
   fMenuHelp->AddEntry(new TGHotString("&About"), M_HELP_ABOUT);

   // Connect all menu items with actions
   fMenuFile->Connect("Activated(Int_t)", "TGAppMainFrame", this, "HandleMenu(Int_t)");
   fMenuAnalysis->Connect("Activated(Int_t)", "TGAppMainFrame", this, "HandleMenu(Int_t)");
   fMenuHelp->Connect("Activated(Int_t)", "TGAppMainFrame", this, "HandleMenu(Int_t)");

   // Draw the created popup menus on the menubar
   fMenuBar->AddPopup(new TGHotString("&File"), fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup(new TGHotString("&Analysis"),fMenuAnalysis,fMenuBarItemLayout);
   fMenuBar->AddPopup(new TGHotString("&Help"), fMenuHelp, fMenuBarItemLayout);
}

// -------------------------------------------------------------------

// Setting the application subwindow layout --------------------------

void TGAppMainFrame::AppLayout()
{
   double numform[6], numform2[6];
   int *checksel;
   char selected[256];
   int subgroup[2];
   TGCompositeFrame *fH1, *fV1, *fH2, *fT1;
   TGGroupFrame *fG1;

   TGLayoutHints *f0centerX = new TGLayoutHints(kLHintsCenterX,2,2,2,2);
   TGLayoutHints *f0leftX = new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2);
   TGLayoutHints *f0leftXnoleft = new TGLayoutHints(kLHintsLeft | kLHintsTop,0,2,2,2);
   TGLayoutHints *f0leftXpad = new TGLayoutHints(kLHintsLeft | kLHintsTop,12,12,2,2);
   TGLayoutHints *f0rightX = new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2);
   TGLayoutHints *f0rightXpad = new TGLayoutHints(kLHintsRight | kLHintsTop,12,12,2,2);
   TGLayoutHints *f0centerY = new TGLayoutHints(kLHintsCenterY,2,2,2,2);
   TGLayoutHints *f0center2d = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,2,2,2,2);
   TGLayoutHints *f1expandX = new TGLayoutHints(kLHintsExpandX,2,2,2,2);
   TGLayoutHints *f1expand2d = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2);
   TGLayoutHints *f1expandXpad = new TGLayoutHints(kLHintsExpandX,12,12,2,2);

// Settings pane ---------------------------------------------------------------------------
   subgroup[0] = (adstLayout[0]->GetWidth())-4;

   // Check buttons to toggle voltage, surface and Z axis scans
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *scansOn -> 4 check buttons (voltage, surface, Z axis, incidence angle scans)\n");
   scansOn = new TSubStructure();
   checksel = new int[4];
   checksel[0] = 0; checksel[1] = 0; checksel[2] = 0; checksel[3] = 0;
   const char *selnames[] = {"Voltage scan ON/OFF", "Surface scan ON/OFF", "Z-axis scan ON/OFF","Rotation scan ON/OFF","0","0","0","0","0","0","0","0","0","0","0","0"};
   if(scansOn->TGCheckList(adstLayout[0], subgroup[0], 30, 4, selnames, checksel, "vertical", "center"))
      adstLayout[0]->AddFrame(scansOn->outsidebox, f1expandXpad);
   delete[] checksel;

   // Hard limit for maximum voltage we can set
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vHardlimit -> Number entry for voltage limit\n");
   vHardlimit = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2;
   if(vHardlimit->TGLabelNEntry(adstLayout[0], subgroup[0], 30, "Voltage limit:", 70.00, numform, "center"))
      adstLayout[0]->AddFrame(vHardlimit->outsidebox, f1expandXpad);

   // Number of used channels
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *NCH -> Number entry for number of channels to capture\n");
   NCH = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 3; numform[2] = 2; numform[3] = 2; numform[4] = 1; numform[5] = 8;
   if(NCH->TGLabelNEntry(adstLayout[0], subgroup[0], 30, "Nr. of channels:", 1, numform, "center"))
      adstLayout[0]->AddFrame(NCH->outsidebox, f1expandXpad);

   // Select the units to use for table positioning (micrometer, table position units)
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *posUnits -> Dropdown menu for selecting the units for table positioning\n");
   posUnits = new TSubStructure();
   selnames[0] = "table units"; selnames[1] = "micrometers";
   sprintf(selected, "table units");
   if(posUnits->TGLabelDrop(adstLayout[0], 2*subgroup[0]/3, 30, "Position units:", 2, selnames, selected))
      adstLayout[0]->AddFrame(posUnits->outsidebox, f1expandXpad);

   // Select the units to use for rotation platform (degrees, table rotation units)
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotUnits -> Dropdown menu for selecting the units for rotation\n");
   rotUnits = new TSubStructure();
   selnames[0] = "table units"; selnames[1] = "degrees";
   sprintf(selected, "degrees");
   if(rotUnits->TGLabelDrop(adstLayout[0], 2*subgroup[0]/3, 30, "Rotation units:", 2, selnames, selected))
      adstLayout[0]->AddFrame(rotUnits->outsidebox, f1expandXpad);

   // Button and textbox to enter the oscilloscope IP address
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *oscConnect -> Text Entry (oscilloscope IP address)\n");
   oscConnect = new TSubStructure();
   if(oscConnect->TGLabelTEntryButton(adstLayout[0], subgroup[0], 30, "Scope IP:", "178.172.43.157", "Connect", "twoline"))
      adstLayout[0]->AddFrame(oscConnect->outsidebox, f1expandXpad);

   // Laser settings (freq., tune, ND filter)
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *laserInfo -> Text Entry (Laser setting information)\n");
   laserInfo = new TSubStructure();
   if(laserInfo->TGLabelTEntry(adstLayout[0], subgroup[0], 30, "Laser settings:", "kHz, tune, ND", "twoline"))
      adstLayout[0]->AddFrame(laserInfo->outsidebox, f1expandXpad);

   // Chamber temperature (will only be manually set until we can get it directly from the chamber)
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *chtemp -> Number entry for chamber temperature\n");
   chtemp = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 5; numform[1] = 1; numform[3] = 2; numform[4] = -70.; numform[5] = 140.;
   if(chtemp->TGLabelNEntry(adstLayout[0], subgroup[0], 30, "Chamber temp.:", 25.0, numform, "center"))
      adstLayout[0]->AddFrame(chtemp->outsidebox, f1expandXpad);

   // Check button to toggle live update of histogram (in display canvas)
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *liveDisp -> 1 check button (live histogram update)\n");
   liveDisp = new TSubStructure();
   checksel = new int;
   *checksel = 0;
   selnames[0] = "Live histogram ON/OFF";
   if(liveDisp->TGCheckList(adstLayout[0], subgroup[0], 30, 1, selnames, checksel, "vertical", "center"))
      adstLayout[0]->AddFrame(liveDisp->outsidebox, f1expandXpad);
   delete checksel;

   // Actions for Settings pane //TODO
   scansOn->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EnableScan(=0)");
   scansOn->widgetChBox[1]->Connect("Clicked()", "TGAppMainFrame", this, "EnableScan(=1)");
   scansOn->widgetChBox[2]->Connect("Clicked()", "TGAppMainFrame", this, "EnableScan(=2)");
   scansOn->widgetChBox[3]->Connect("Clicked()", "TGAppMainFrame", this, "EnableScan(=3)");
   vHardlimit->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "VoltageLimit()");
   (vHardlimit->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "VoltageLimit()");
   posUnits->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "ChangeUnits(Int_t)");
   rotUnits->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "ChangeUnitsRot(Int_t)");
   liveDisp->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EnableLiveUpdate()");
// Settings pane ---------------------------------------------------------------------------

// Main window -----------------------------------------------------------------------------
   TGTab *setTab;

   // Voltage, position and incidence angle tab
   subgroup[0] = 2*(adstLayout[2]->GetWidth())/7-14;
   subgroup[1] = (adstLayout[2]->GetHeight())/2-4;
   setTab = new TGTab(adstLayout[2], subgroup[0], subgroup[1]);

   fT1 = setTab->AddTab("Voltage, position and incidence angle");
   fH1 = new TGCompositeFrame(fT1, adstLayout[2]->GetWidth(), subgroup[1], kFixedHeight | kHorizontalFrame);

   // Left pane (Bias voltage controls)
   fV1 = new TGCompositeFrame(fH1, subgroup[0], subgroup[1], kFixedWidth | kFixedHeight | kVerticalFrame);
   fG1 = new TGGroupFrame(fV1, "Bias voltage controls");

   // Output voltage supply channel
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOutCh -> Dropdown menu for bias voltage channel\n");
   vOutCh = new TSubStructure();
   selnames[0] = "U0"; selnames[1] = "U1"; selnames[2] = "U2"; selnames[3] = "U3"; selnames[4] = "U4"; selnames[5] = "U5"; selnames[6] = "U6"; selnames[7] = "U7"; selnames[8] = "U100"; selnames[9] = "U101"; selnames[10] = "U102"; selnames[11] = "U103"; selnames[12] = "U104"; selnames[13] = "U105"; selnames[14] = "U106"; selnames[15] = "U107"; 
   sprintf(selected, "U0");
   if(vOutCh->TGLabelDrop(fG1, 2*subgroup[0]/3, 30, "Output channel:", 16, selnames, selected))
      fG1->AddFrame(vOutCh->outsidebox, f1expandXpad);

   // Output voltage setting
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOutOpt -> 2 check buttons (negative polarity, ON/OFF toggle switch)\n");
   vOutOpt = new TSubStructure();
   checksel = new int[2];
   checksel[0] = 0; checksel[1] = 0;
   selnames[1] = "Output ON/OFF"; selnames[0] = "Negative polarity";
   if(vOutOpt->TGCheckList(fG1, 3*subgroup[0]/4, 30, 2, selnames, checksel, "vertical", "center"))
      fG1->AddFrame(vOutOpt->outsidebox, f1expandXpad);
   delete[] checksel;

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOut -> Number entry for bias voltage\n");
   vOut = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2; numform[3] = 2; numform[4] = 0.; numform[5] = vHardlimit->widgetNE[0]->GetNumber();
   if(vOut->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "Output voltage:", 0.00, numform, "center"))
      fG1->AddFrame(vOut->outsidebox, f1expandXpad);
   
   // Set, get and reset voltage buttons
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOutButtons -> 3 buttons (set bias voltage, read current bias voltage, reset voltage output when it gets interlocked)\n");
   vOutButtons = new TSubStructure();
   selnames[0] = "Set"; selnames[1] = "Get"; selnames[2] = "Reset";
   if(vOutButtons->TGMultiButton(fG1, 3*subgroup[0]/4, 30, 3, selnames, "center"))
      fG1->AddFrame(vOutButtons->outsidebox, f1expandXpad);

   // Voltage scan controls
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOutStart -> Number entry for starting bias voltage\n");
   vOutStart = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2;
   if(vOutStart->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "V (min):", 0.00, numform, "center"))
      fG1->AddFrame(vOutStart->outsidebox, f1expandXpad);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOutStop -> Number entry for starting bias voltage\n");
   vOutStop = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2;
   if(vOutStop->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "V (max):", 0.00, numform, "center"))
      fG1->AddFrame(vOutStop->outsidebox, f1expandXpad);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *vOutStep -> Number entry for starting bias voltage\n");
   vOutStep = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2;
   if(vOutStep->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "V (step):", 0.00, numform, "center"))
      fG1->AddFrame(vOutStep->outsidebox, f1expandXpad);

   fV1->AddFrame(fG1, f1expand2d);
   // Left pane (Bias voltage controls)
   fH1->AddFrame(fV1, f0leftX);

   // Center pane (Table position controls)
   subgroup[0] = 3*(adstLayout[2]->GetWidth())/7-13;
   fV1 = new TGCompositeFrame(fH1, subgroup[0], subgroup[1], kFixedWidth | kFixedHeight | kVerticalFrame);
   fG1 = new TGGroupFrame(fV1, "Table position controls");

   // X, Y and Z positions
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *xPos, *yPos, *zPos, *zPosMin, *zPosMax, *zPosStep -> Settings for position and Z axis scan\n");
   fH2 = new TGCompositeFrame(fG1, 3*subgroup[0]/4, 30, kFixedWidth | kHorizontalFrame);
   xPos = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 9; numform[3] = 2; numform[4] = -100; numform[5] = 375000; //numform[4] = -100; numform[5] = 215000;	// use commented part if Z is the longer axis and X the shorter
   if(xPos->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "X:", 0, numform, "center"))
      fH2->AddFrame(xPos->outsidebox, f0centerX);

   zPosMin = new TSubStructure();
   numform[5] = 215000; //numform[5] = 375000;	// use commented part if Z is the longer axis and X the shorter
   if(zPosMin->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Z (min):", 0, numform, "center"))
      fH2->AddFrame(zPosMin->outsidebox, f0centerX);
   fG1->AddFrame(fH2, f1expandXpad);

   fH2 = new TGCompositeFrame(fG1, 3*subgroup[0]/4, 30, kFixedWidth | kHorizontalFrame);
   yPos = new TSubStructure();
   numform[5] = 215000;
   if(yPos->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Y:", 0, numform, "center"))
      fH2->AddFrame(yPos->outsidebox, f0centerX);

   zPosMax = new TSubStructure();
   numform[5] = 215000; //numform[5] = 375000;	// use commented part if Z is the longer axis and X the shorter
   if(zPosMax->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Z (max):", 0, numform, "center"))
      fH2->AddFrame(zPosMax->outsidebox, f0centerX);
   fG1->AddFrame(fH2, f1expandXpad);
   
   fH2 = new TGCompositeFrame(fG1, 3*subgroup[0]/4, 30, kFixedWidth | kHorizontalFrame);
   zPos = new TSubStructure();
   numform[5] = 215000; //numform[5] = 375000;	// use commented part if Z is the longer axis and X the shorter
   if(zPos->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Z:", 0, numform, "center"))
      fH2->AddFrame(zPos->outsidebox, f0centerX);

   zPosStep = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 9;
   if(zPosStep->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Z (step):", 0, numform, "center"))
      fH2->AddFrame(zPosStep->outsidebox, f0centerX);
   fG1->AddFrame(fH2, f1expandXpad);
   
   // Set, get, home and reset position buttons
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *posButtons -> 5 buttons (set position, read current position, home the motor, reset all three controllers and emergency stop)\n");
   posButtons = new TSubStructure();
   selnames[0] = "Set"; selnames[1] = "Get"; selnames[2] = "Home"; selnames[3] = "Reset"; selnames[4] = "Emergency stop";
   if(posButtons->TGMultiButton(fG1, 3*subgroup[0]/4, 30, 5, selnames, "center"))
      fG1->AddFrame(posButtons->outsidebox, f1expandXpad);

   // Position scan controls
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *xPosMin, *xPosMax, *xPosStep, *yPosMin, *yPosMax, *yPosStep -> Settings for X and Y axis scans\n");
   fH2 = new TGCompositeFrame(fG1, 3*subgroup[0]/4, 30, kFixedWidth | kHorizontalFrame);
   xPosMin = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 9; numform[3] = 2; numform[4] = -100; numform[5] = 375000; //numform[5] = 215000;	// use commented part if Z is the longer axis and X the shorter
   if(xPosMin->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "X (min):", 0, numform, "center"))
      fH2->AddFrame(xPosMin->outsidebox, f0centerX);

   yPosMin = new TSubStructure();
   numform[5] = 215000;
   if(yPosMin->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Y (min):", 0, numform, "center"))
      fH2->AddFrame(yPosMin->outsidebox, f0centerX);
   fG1->AddFrame(fH2, f1expandXpad);

   fH2 = new TGCompositeFrame(fG1, 3*subgroup[0]/4, 30, kFixedWidth | kHorizontalFrame);
   xPosMax = new TSubStructure();
   numform[5] = 375000; //numform[5] = 215000;	// use commented part if Z is the longer axis and X the shorter
   if(xPosMax->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "X (max):", 0, numform, "center"))
      fH2->AddFrame(xPosMax->outsidebox, f0centerX);

   yPosMax = new TSubStructure();
   numform[5] = 215000;
   if(yPosMax->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Y (max):", 0, numform, "center"))
      fH2->AddFrame(yPosMax->outsidebox, f0centerX);
   fG1->AddFrame(fH2, f1expandXpad);

   fH2 = new TGCompositeFrame(fG1, 3*subgroup[0]/4, 30, kFixedWidth | kHorizontalFrame);
   xPosStep = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 9;
   if(xPosStep->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "X (step):", 0, numform, "center"))
      fH2->AddFrame(xPosStep->outsidebox, f0centerX);

   yPosStep = new TSubStructure();
   if(yPosStep->TGLabelNEntry(fH2, 8*subgroup[0]/16, 30, "Y (step):", 0, numform, "center"))
      fH2->AddFrame(yPosStep->outsidebox, f0centerX);
   fG1->AddFrame(fH2, f1expandXpad);

   fV1->AddFrame(fG1, f1expand2d);
   // Center pane (Table position controls)
   fH1->AddFrame(fV1, f0leftX);

   // Right pane (Incidence angle controls)
   subgroup[0] = 2*(adstLayout[2]->GetWidth())/7-14;
   fV1 = new TGCompositeFrame(fH1, subgroup[0], subgroup[1], kFixedWidth | kFixedHeight | kVerticalFrame);
   fG1 = new TGGroupFrame(fV1, "Incidence angle controls");

   // Rotation positions
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotPos -> Setting for rotation position\n");
   rotPos = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2; numform[3] = 2; numform[4] = -180.; numform[5] = 180.;
   if(rotPos->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "Incidence angle:", 0., numform, "center"))
      fG1->AddFrame(rotPos->outsidebox, f1expandXpad);
   
   // Set, get, home and reset rotation buttons
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotButtons -> 4 buttons (set rotation, read current rotation, home the motor, reset controller)\n");
   rotButtons = new TSubStructure();
   selnames[0] = "Set"; selnames[1] = "Get"; selnames[2] = "Home"; selnames[3] = "Reset";
   if(rotButtons->TGMultiButton(fG1, 3*subgroup[0]/4, 30, 4, selnames, "center"))
      fG1->AddFrame(rotButtons->outsidebox, f1expandXpad);
   
   // Emergency stop rotation buttons
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotButEmergency -> 1 button (emergency stop)\n");
   rotButEmergency = new TSubStructure();
   selnames[0] = "Emergency stop";
   if(rotButEmergency->TGMultiButton(fG1, 3*subgroup[0]/4, 30, 1, selnames, "center"))
      fG1->AddFrame(rotButEmergency->outsidebox, f1expandXpad);

   // Rotation scan controls
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotPosMin -> Number entry for starting angle\n");
   rotPosMin = new TSubStructure();
   if(rotPosMin->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "Angle (min):", 0.0, numform, "center"))
      fG1->AddFrame(rotPosMin->outsidebox, f1expandXpad);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotPosMax -> Number entry for finishing angle\n");
   rotPosMax = new TSubStructure();
   if(rotPosMax->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "Angle (max):", 0.0, numform, "center"))
      fG1->AddFrame(rotPosMax->outsidebox, f1expandXpad);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *rotPosStep -> Number entry for angle step\n");
   rotPosStep = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 1;
   if(rotPosStep->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "Angle (step):", 0.0, numform, "center"))
      fG1->AddFrame(rotPosStep->outsidebox, f1expandXpad);

   fV1->AddFrame(fG1, f1expand2d);
   // Right pane (Incidence angle controls)
   fH1->AddFrame(fV1, f0leftX);
   
   fT1->AddFrame(fH1, f1expand2d);

   // Waveform tab
   //TODO
   adstLayout[2]->AddFrame(setTab, f0leftX);

   // Bottom pane (File controls)
   subgroup[0] = adstLayout[2]->GetWidth()-19;
   subgroup[1] = (adstLayout[2]->GetHeight())/4-4;
   fH1 = new TGCompositeFrame(adstLayout[2], subgroup[0], subgroup[1], kFixedWidth | kFixedHeight | kHorizontalFrame);
   fG1 = new TGGroupFrame(fH1, "Event/Data file controls");

   // Number of events
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *evtNum -> Number entry for set number of events to acquire\n");
   evtNum = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 8; numform[2] = 1;
   if(evtNum->TGLabelNEntry(fG1, 3*subgroup[0]/4, 30, "Number of events:", 10000, numform, "left"))
      fG1->AddFrame(evtNum->outsidebox, f0leftXpad);

   // Time stamp display
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *timeStamp -> Text entry for timestamp\n");
   timeStamp = new TSubStructure();
   if(timeStamp->TGLabelTEntry(fG1, 3*subgroup[0]/4, 30, "Time stamp:", "", "oneline"))
   {
      timeStamp->widgetTE->SetState(kFALSE);
      fG1->AddFrame(timeStamp->outsidebox, f1expandXpad);
   }

   // Save to file
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fileName -> Text entry for timestamp\n");
   fileName = new TSubStructure();
   char *cTemp;
   cTemp = new char[256];
   sprintf(cTemp, "%s/results/test%s", rootdir, histext);
   if(fileName->TGLabelTEntryButton(fG1, 3*subgroup[0]/4, 30, "Save to file:", cTemp, "...", "oneline"))
   {
      fileName->widgetTE->SetState(kFALSE);
      fG1->AddFrame(fileName->outsidebox, f1expandXpad);
   }

   fH1->AddFrame(fG1, new TGLayoutHints(kLHintsExpandX,8,2,2,2));
   // Bottom pane (File controls)
   adstLayout[2]->AddFrame(fH1, f0leftX);

   // Start acquisition and progress bar
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *measProgress -> Button to start acquiring data and horizontal progress bar\n");
   measProgress = new TSubStructure();
   if(measProgress->TGButtonProgressTEntry(adstLayout[2], 3*subgroup[0]/4, 30, "Start acquisition", "Estimated end time: "))
   {
      measProgress->widgetTE->SetState(kFALSE);
      adstLayout[2]->AddFrame(measProgress->outsidebox, f1expandXpad);
   }

   Pixel_t pixel_color;
   gClient->GetColorByName("white", pixel_color);
   posButtons->widgetTB[4]->SetTextColor(pixel_color);
   rotButEmergency->widgetTB[0]->SetTextColor(pixel_color);
   gClient->GetColorByName("red", pixel_color);
   posButtons->widgetTB[4]->SetBackgroundColor(pixel_color);
   rotButEmergency->widgetTB[0]->SetBackgroundColor(pixel_color);

   // Actions for Main window //TODO
   vOutOpt->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "NegativePolarity()");
   vOutButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "VoltOut(=0)");
   vOutButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "VoltOut(=1)");
   vOutButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "VoltOut(=2)");
   posButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "PositionSet(=0)");
   posButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "PositionSet(=1)");
   posButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "PositionSet(=2)");
   posButtons->widgetTB[3]->Connect("Clicked()", "TGAppMainFrame", this, "PositionSet(=3)");
   posButtons->widgetTB[4]->Connect("Clicked()", "TGAppMainFrame", this, "PositionSet(=4)");
   rotButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "RotationSet(=0)");
   rotButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "RotationSet(=1)");
   rotButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "RotationSet(=2)");
   rotButtons->widgetTB[3]->Connect("Clicked()", "TGAppMainFrame", this, "RotationSet(=3)");
   rotButEmergency->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "RotationSet(=4)");
   fileName->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SaveFile()");
   measProgress->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "StartAcq()");
   // TODO - Save file
 
// Main window -----------------------------------------------------------------------------

// Display pane ----------------------------------------------------------------------------
   measCanvas = new TRootEmbeddedCanvas("measCanvas",adstLayout[1],300,300);
   adstLayout[1]->AddFrame(measCanvas, f1expand2d);
// Display pane ----------------------------------------------------------------------------

// Histogram file selection pane -----------------------------------------------------------
   subgroup[0] = (mvaLayout[0]->GetWidth())-4;

   // Open browser for file selection 
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *selectDir -> Button to open histogram files\n");
   selectDir = new TSubStructure();
   if(selectDir->TGLabelButton(mvaLayout[0], 3*subgroup[0]/4, 30, "File selection:", "...", "left"))
      mvaLayout[0]->AddFrame(selectDir->outsidebox, f0leftXpad);

   // List view of the opened files
   if(DBGSIG > 1) printf("AppLayout(): Creating TGListBox *fileList -> List box for opened histograms\n");
   fileList = new TGListBox(mvaLayout[0],1);
   fileList->GetVScrollbar();
   fileList->Resize(300, (3*mvaLayout[0]->GetHeight()/7)-10 );
   mvaLayout[0]->AddFrame(fileList, f1expandXpad);

   fH1 = new TGCompositeFrame(mvaLayout[0], subgroup[0], 30, kHorizontalFrame);
   // Multiple file selection toggle
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *multiSelect -> 2 check buttons (enable multiple select, select everything)\n");
   multiSelect = new TSubStructure();
   checksel = new int[2];
   checksel[0] = 0; checksel[1] = 0;
   selnames[0] = "Multiple file select"; selnames[1] = "Select all listed files";
   if(multiSelect->TGCheckList(fH1, subgroup[0]/2, 30, 2, selnames, checksel, "horizontal", "left"))
      fH1->AddFrame(multiSelect->outsidebox, f0leftX);
   delete[] checksel;
   
   // Previous/next controls, clear list and edit header
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fileListCtrl -> Multiple buttons for controlling and displaying the histograms in list box\n");
   fileListCtrl = new TSubStructure();
   selnames[0] = "<<"; selnames[1] = ">>"; selnames[2] = "Edit header"; selnames[3] = "Clear list";
   if(fileListCtrl->TGMultiButton(fH1, subgroup[0]/3, 30, 4, selnames, "right"))
      fH1->AddFrame(fileListCtrl->outsidebox, f0rightX);

   mvaLayout[0]->AddFrame(fH1, f1expandXpad);

   // Header information of opened file
   fG1 = new TGGroupFrame(mvaLayout[0], "Opened file header information");

   fH1 = new TGHorizontalFrame(fG1, subgroup[0], 30);
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *dispTime -> Display text Entry for opened histogram (time stamp)\n");
   dispTime = new TSubStructure();
   if(dispTime->TGLabelTEntry(fH1, 3*subgroup[0]/4-24, 30, "Time:", "", "oneline"))
      fH1->AddFrame(dispTime->outsidebox, f0leftXnoleft);
   dispTime->widgetTE->SetState(kFALSE);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *dispBias -> Number entry for opened histogram (bias voltage)\n");
   dispBias = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2;
   if(dispBias->TGLabelNEntry(fH1, subgroup[0]/4-24, 30, "Bias voltage:", 0.00, numform, "center"))
      fH1->AddFrame(dispBias->outsidebox, f0leftX);
   dispBias->widgetNE[0]->SetState(kFALSE);
   fG1->AddFrame(fH1, f0leftXpad);

   fH1 = new TGHorizontalFrame(fG1, subgroup[0], 30);
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *dispPos -> Display text Entry for opened histogram (table position)\n");
   dispPos = new TSubStructure();
   if(dispPos->TGLabelTEntry(fH1, subgroup[0]/2-12, 30, "Position:", "", "oneline"))
      fH1->AddFrame(dispPos->outsidebox, f0leftXnoleft);
   dispPos->widgetTE->SetState(kFALSE);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *dispTemp -> Number entry for opened histogram (temperature)\n");
   dispTemp = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 1;
   if(dispTemp->TGLabelNEntry(fH1, subgroup[0]/4-18, 30, "Temperature:", 0.0, numform, "center"))
      fH1->AddFrame(dispTemp->outsidebox, f0leftX);
   dispTemp->widgetNE[0]->SetState(kFALSE);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *dispAngle -> Number entry for opened histogram (incidence angle)\n");
   dispAngle = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 7; numform[1] = 2;
   if(dispAngle->TGLabelNEntry(fH1, subgroup[0]/4-18, 30, "Angle:", 0.00, numform, "center"))
      fH1->AddFrame(dispAngle->outsidebox, f0leftX);
   dispAngle->widgetNE[0]->SetState(kFALSE);
   fG1->AddFrame(fH1, f0leftXpad);

   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *dispLaser -> Display text Entry for opened histogram (laser and additional info)\n");
   dispLaser = new TSubStructure();
   if(dispLaser->TGLabelTEntry(fG1, 3*subgroup[0]/4, 30, "Laser settings:", "", "oneline"))
      fG1->AddFrame(dispLaser->outsidebox, f1expandXpad);
   dispLaser->widgetTE->SetState(kFALSE);

   mvaLayout[0]->AddFrame(fG1, f1expandX);

   // Actions for histogram file selection pane
   selectDir->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectDirectory()");
   multiSelect->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "ListMultiSelect(=0)");
   multiSelect->widgetChBox[1]->Connect("Clicked()", "TGAppMainFrame", this, "ListMultiSelect(=1)");
   fileList->Connect("DoubleClicked(Int_t)", "TGAppMainFrame", this, "FileListNavigation(Int_t)");
   fileListCtrl->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "FileListNavigation(=-2)");
   fileListCtrl->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "FileListNavigation(=-3)");
   fileListCtrl->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "HeaderEdit()");
   fileListCtrl->widgetTB[3]->Connect("Clicked()", "TGAppMainFrame", this, "ClearHistogramList()");

// Histogram file selection pane -----------------------------------------------------------

// Analysis pane ---------------------------------------------------------------------------
   fH1 = new TGCompositeFrame(mvaLayout[1], mvaLayout[1]->GetWidth(), 30, kHorizontalFrame);

   subgroup[0] = (mvaLayout[1]->GetWidth())/2-4;
   subgroup[1] = (mvaLayout[1]->GetHeight())-4;

   analTab = new TGTab(fH1, subgroup[0], subgroup[1]);

   // Analysis tabs
   // Integrate spectrum tab
   fT1 = analTab->AddTab("Integ. spectrum");
   fV1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kVerticalFrame | kFixedWidth | kFixedHeight);

   // Check buttons to toggle direction of integration and normalization
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *intSpect -> 3 check buttons (direction of integration, normalization)\n");
   intSpect = new TSubStructure();
   checksel = new int[3];
   checksel[0] = 0; checksel[1] = 0; checksel[2] = 1;
   selnames[0] = "X direction (for edge scans)";
   selnames[1] = "Y direction (for edge scans)";
   selnames[2] = "Normalize to number of events";
   if(intSpect->TGCheckList(fV1, subgroup[0], 30, 3, selnames, checksel, "vertical", "center"))
      fV1->AddFrame(intSpect->outsidebox, f1expandXpad);
   delete[] checksel;

   // Values for 2D plot pixel resolution
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *resol2d -> Pixel resolution for 2D plots\n");
   resol2d = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 5; numform2[0] = 5; numform[2] = 2; numform2[2] = 2;
   if(resol2d->TGLabelDoubleNEntry(fV1, subgroup[0], 30, "2D plot pixel resolution (X, Y):", 40, numform, 40, numform2, "center"))
      fV1->AddFrame(resol2d->outsidebox, f1expandX);

   // Start integrating or set its defaults
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *intSpectButtons -> 2 buttons for integrating spectra\n");
   intSpectButtons = new TSubStructure();
   selnames[0] = "Start"; selnames[1] = "Start and edit"; selnames[2] = "Default values";
   if(intSpectButtons->TGMultiButton(fV1, subgroup[0], 30, 3, selnames, "center"))
      fV1->AddFrame(intSpectButtons->outsidebox, f1expandX);

   fT1->AddFrame(fV1, f1expandX);

   // Relative photon detection efficiency tab (PDE)
   fT1 = analTab->AddTab("Relative PDE");
   fV1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kVerticalFrame | kFixedWidth | kFixedHeight);

   // Check buttons to toggle normalization and if there is no peak structure on ADC spectrum
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *relPde -> 2 check buttons (normalization, no peak structure)\n");
   relPde = new TSubStructure();
   checksel = new int[2];
   checksel[0] = 1; checksel[1] = 0;
   selnames[0] = "Normalize to number of events";
   selnames[1] = "No peak structure on ADC spectrum";
   if(relPde->TGCheckList(fV1, subgroup[0], 30, 2, selnames, checksel, "vertical", "center"))
      fV1->AddFrame(relPde->outsidebox, f1expandXpad);
   delete[] checksel;

   // Calculate relative PDE using the middle of the pedestal peak
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *midPeak -> Calculate the relative PDE, by setting the middle of the pedestal peak.\n");
   midPeak = new TSubStructure();
   checksel = new int;
   *checksel = 0;
   selnames[0] = "Pedestal entries end at middle of ped. peak";
   if(midPeak->TGCheckList(fV1, subgroup[0], 30, 1, selnames, checksel, "vertical", "center"))
      fV1->AddFrame(midPeak->outsidebox, f1expandX);
   delete checksel;

   // Select the dark run 
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *darkRun -> Button to open histogram files of a dark run\n");
   darkRun = new TSubStructure();
   if(darkRun->TGLabelTEntryButton(fV1, 3*subgroup[0]/4, 30, "Select dark run histogram:", "", "...", "oneline"))
   {
      darkRun->widgetTE->SetState(kFALSE);
      fV1->AddFrame(darkRun->outsidebox, f1expandXpad);
   }

   // Select the offset of 0 angle
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *zeroAngle -> Set the offset for 0 angle\n");
   zeroAngle = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2;
   if(zeroAngle->TGLabelNEntry(fV1, 3*subgroup[0]/4, 30, "Offset to zero angle:", 0., numform, "center"))
      fV1->AddFrame(zeroAngle->outsidebox, f0centerX);

   // Start calculating the PDE or set its defaults
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *relPdeButtons -> 2 buttons for calculating the relative PDE\n");
   relPdeButtons = new TSubStructure();
   selnames[0] = "Start"; selnames[1] = "Start and edit"; selnames[2] = "Default values";
   if(relPdeButtons->TGMultiButton(fV1, subgroup[0], 30, 3, selnames, "center"))
      fV1->AddFrame(relPdeButtons->outsidebox, f1expandX);

   fT1->AddFrame(fV1, f1expandX);

   // Breaktown voltage tab
   fT1 = analTab->AddTab("Breakdown voltage");
   fV1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kVerticalFrame | kFixedWidth | kFixedHeight);

   // Select the minumum number of peaks for fitting to be initiated
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *minPeak -> Minimum number of peaks to fit for peak fitting\n");
   minPeak = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 4; numform[2] = 1; numform[3] = 2; numform[4] = 1; numform[5] = 20;
   if(minPeak->TGLabelNEntry(fV1, subgroup[0], 30, "Min. nr. of peaks:", 2, numform, "center"))
      fV1->AddFrame(minPeak->outsidebox, f0centerX);

   // Select which separation to use
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *peakSepCalc -> Make the separation between the selected peaks\n");
   peakSepCalc = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 4; numform[2] = 1; numform[3] = 2; numform[4] = 1; numform[5] = 3;
   if(peakSepCalc->TGLabelNEntry(fV1, subgroup[0], 30, "Calculate peak separation between N pe and N+1 pe peaks:", 1, numform, "center"))
      fV1->AddFrame(peakSepCalc->outsidebox, f0centerX);

   // Start calculating the breakdown voltage or set its defaults
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *brDownButtons -> 2 buttons for calculating the breakdown voltage\n");
   brDownButtons = new TSubStructure();
   selnames[0] = "Start"; selnames[1] = "Start and edit"; selnames[2] = "Default values";
   if(brDownButtons->TGMultiButton(fV1, subgroup[0], 30, 3, selnames, "center"))
      fV1->AddFrame(brDownButtons->outsidebox, f1expandX);

   fT1->AddFrame(fV1, f1expandX);

   // Surface scan tab
   fT1 = analTab->AddTab("Surface scan");
   fV1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kVerticalFrame | kFixedWidth | kFixedHeight);

   // Check button to toggle normalization
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *surfScanOpt -> 2 check buttons (normalization, zero bottom-left corner)\n");
   surfScanOpt = new TSubStructure();
   checksel = new int[2];
   checksel[0] = 1; checksel[1] = 0;
   selnames[0] = "Normalize to number of events"; selnames[1] = "Zero the bottom left corner";
   if(surfScanOpt->TGCheckList(fV1, subgroup[0], 30, 2, selnames, checksel, "vertical", "center"))
      fV1->AddFrame(surfScanOpt->outsidebox, f1expandXpad);
   delete[] checksel;

   // Values for 2D plot pixel resolution
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *resolSurf -> Pixel resolution for surface plots\n");
   resolSurf = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 5; numform2[0] = 5; numform[2] = 2; numform2[2] = 2;
   if(resolSurf->TGLabelDoubleNEntry(fV1, subgroup[0], 30, "Surface plot pixel resolution (X, Y):", 40, numform, 40, numform2, "center"))
      fV1->AddFrame(resolSurf->outsidebox, f1expandX);

   // Start calculating the breakdown voltage or set its defaults
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *surfButtons -> 2 buttons for calculating the surface scan\n");
   surfButtons = new TSubStructure();
   selnames[0] = "Start"; selnames[1] = "Start and edit"; selnames[2] = "Default values";
   if(surfButtons->TGMultiButton(fV1, subgroup[0], 30, 3, selnames, "center"))
      fV1->AddFrame(surfButtons->outsidebox, f1expandX);

   fT1->AddFrame(fV1, f1expandX);

   // Timing tab
   fT1 = analTab->AddTab("Timing");
   fV1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kVerticalFrame | kFixedWidth | kFixedHeight);

   fT1->AddFrame(fV1, f1expandX);

   // Analysis tabs
   fH1->AddFrame(analTab, f0leftX);

   // Peak fitting settings
   fV1 = new TGCompositeFrame(fH1, subgroup[0], 30, kVerticalFrame);

   fH2 = new TGCompositeFrame(fV1, subgroup[0], 30, kHorizontalFrame);
   // Select the sigma for peak fitting
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fitSigma -> Sigma for peak fitting\n");
   fitSigma = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 3; numform[2] = 2;
   if(fitSigma->TGLabelNEntry(fH2, subgroup[0]/2, 30, "Peak sigma:", 1.2, numform, "center"))
      fH2->AddFrame(fitSigma->outsidebox, f0centerX);
   // Select the signal to noise treshold for peak fitting
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fitTresh -> S/N ratio for peak fitting\n");
   fitTresh = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 3; numform[2] = 2;
   if(fitTresh->TGLabelNEntry(fH2, subgroup[0]/2, 30, "S/N ratio:", 0.005, numform, "center"))
      fH2->AddFrame(fitTresh->outsidebox, f0centerX);
   fV1->AddFrame(fH2, f1expandXpad);

   // Select the background interpolation for peak fitting
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fitInter -> Background interpolation for peak fitting\n");
   fitInter = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 5; numform[2] = 1;
   if(fitInter->TGLabelNEntry(fV1, subgroup[0], 30, "Back. interpolation:", 7, numform, "center"))
      fV1->AddFrame(fitInter->outsidebox, f0centerX);

   // Select the ADC offset
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adcOffset -> Select the offset for all ADC spectra\n");
   adcOffset = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2;
   if(adcOffset->TGLabelNEntry(fV1, subgroup[0], 30, "ADC spectrum offset:", 0.00, numform, "center"))
      fV1->AddFrame(adcOffset->outsidebox, f0centerX);

   // Select the acceptable error for peak fitting
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *accError -> Acceptable peak fitting error for peak fitting\n");
   accError = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 2; numform[2] = 1;
   if(accError->TGLabelNEntry(fV1, subgroup[0], 30, "Max. peak fit error:", 0.15, numform, "center"))
      fV1->AddFrame(accError->outsidebox, f0centerX);

   // Select the pedestal lower limit for peak fitting
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *pedesLow -> Lower ADC limit of pedestal peak for peak fitting\n");
   pedesLow = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 6; numform[1] = 1; numform[2] = 2;
   if(pedesLow->TGLabelNEntry(fV1, subgroup[0], 30, "Pedestal low limit:", 0.0, numform, "center"))
      fV1->AddFrame(pedesLow->outsidebox, f0centerX);

   // Check buttons to toggle subtracting the background and exporting the fitted spectra
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *fitChecks -> 2 check buttons (substracting background, exporting fitted spectra)\n");
   fitChecks = new TSubStructure();
   checksel = new int[2];
   checksel[0] = 1; checksel[1] = 0;
   selnames[0] = "Backround subtraction ON/OFF";
   selnames[1] = "Export fitted spectra ON/OFF";
   if(fitChecks->TGCheckList(fV1, subgroup[0], 30, 2, selnames, checksel, "vertical", "center"))
      fV1->AddFrame(fitChecks->outsidebox, f1expandXpad);
   delete[] checksel;

   // Analysis progress bar
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *analysisProgress -> Horizontal progress bar for analysis\n");
   analysisProgress = new TSubStructure();
   if(analysisProgress->TGLabelProgress(fV1, subgroup[0], 30, "Current analysis:"))
      fV1->AddFrame(analysisProgress->outsidebox, f1expandXpad);

   fH1->AddFrame(fV1, f1expandX);

   mvaLayout[1]->AddFrame(fH1, f1expandXpad);

   // Actions for analysis pane
   intSpectButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=0)");
   intSpectButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=1)");
   intSpectButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisDefaults()");

   relPdeButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=0)");
   relPdeButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=1)");
   relPdeButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisDefaults()");
   darkRun->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "SelectDarkHist()");

   brDownButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=0)");
   brDownButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=1)");
   brDownButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisDefaults()");

   surfButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=0)");
   surfButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisHandle(=1)");
   surfButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "AnalysisDefaults()");
// Analysis pane ---------------------------------------------------------------------------

// Histogram pane --------------------------------------------------------------------------
   analysisCanvas = new TRootEmbeddedCanvas("analysisCanvas",mvaLayout[2],300,300);
   mvaLayout[2]->AddFrame(analysisCanvas, f1expand2d);
   analysisCanvas->GetCanvas()->SetGrid();
// Histogram pane --------------------------------------------------------------------------

// Histogram controls pane -----------------------------------------------------------------
   subgroup[0] = (mvaLayout[3]->GetWidth())-4;

   // Control for histogram X range
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *adcRange -> Range for ADC histogram\n");
   adcRange = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 6; numform2[0] = 6;
   if(adcRange->TGLabelDoubleNEntry(mvaLayout[3], subgroup[0], 30, "ADC range (min, max):", 0, numform, 0, numform2, "center"))
      mvaLayout[3]->AddFrame(adcRange->outsidebox, f1expandXpad);

   // TDC window for getting data
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *tdcRange -> Range for TDC histogram\n");
   tdcRange = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 8; numform[1] = 2; numform2[0] = 8; numform2[1] = 2;
   if(tdcRange->TGLabelDoubleNEntry(mvaLayout[3], subgroup[0], 30, "TDC range (min, max):", -0.5, numform, 221.8, numform2, "center"))
      mvaLayout[3]->AddFrame(tdcRange->outsidebox, f1expandXpad);

   // Y axis range settings
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *yRange -> Y axis range\n");
   yRange = new TSubStructure();
   for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
   numform[0] = 8; numform[1] = 1; numform2[0] = 8; numform2[1] = 1;
   if(yRange->TGLabelDoubleNEntry(mvaLayout[3], subgroup[0], 30, "Y range (min, max):", 0, numform, 0, numform2, "center"))
      mvaLayout[3]->AddFrame(yRange->outsidebox, f1expandXpad);

   fH1 = new TGHorizontalFrame(mvaLayout[3], subgroup[0], 30);
   // Select the channel to display
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *selectCh -> Channel to display\n");
   selectCh = new TSubStructure();
   for(int i = 0; i < 6; i++) numform[i] = 0;
   numform[0] = 3; numform[2] = 2; numform[3] = 2; numform[4] = 0; numform[5] = 7; // TODO - ko odprem file, nastavi zgornjo limito
   if(selectCh->TGLabelNEntry(fH1, subgroup[0]/2-24, 30, "Display channel:", 0, numform, "center"))
      fH1->AddFrame(selectCh->outsidebox, f0leftX);
   selChannel = -1;

   // Change plot type
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *plotType -> 3 buttons for selecting the plot type (ADC, TDC, ADC/TDC)\n");
   plotType = new TSubStructure();
   selnames[0] = "ADC"; selnames[1] = "TDC"; selnames[2] = "ADC/TDC";
   if(plotType->TGMultiButton(fH1, subgroup[0]/2-24, 30, 3, selnames, "center"))
      fH1->AddFrame(plotType->outsidebox, f1expandX);
   mvaLayout[3]->AddFrame(fH1, f1expandXpad);

   // Check button to toggle logarithmic scale
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *histOpt -> 2 check buttons (histogram logarithmic scale, clean plots)\n");
   histOpt = new TSubStructure();
   checksel = new int[2];
   checksel[0] = 0; checksel[1] = 1;
   selnames[0] = "Logarithmic scale ON/OFF";
   selnames[1] = "Clean plots ON/OFF";
   if(histOpt->TGCheckList(mvaLayout[3], subgroup[0], 30, 2, selnames, checksel, "vertical", "center"))
      mvaLayout[3]->AddFrame(histOpt->outsidebox, f1expandXpad);
   delete[] checksel;

   // Export the selected files
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *exportHist -> Button to export current histogram\n");
   exportHist = new TSubStructure();
   if(exportHist->TGLabelButton(mvaLayout[3], subgroup[0], 30, "Export selected histograms:", "Export", "center"))
      mvaLayout[3]->AddFrame(exportHist->outsidebox, f1expandXpad);

   // Edit the selected histograms
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *editSelHist -> Button to additionally edit the selected histograms (make a canvas clone in a new tab)\n");
   editSelHist = new TSubStructure();
   if(editSelHist->TGLabelButton(mvaLayout[3], subgroup[0], 30, "Edit selected histograms:", "Edit", "center"))
      mvaLayout[3]->AddFrame(editSelHist->outsidebox, f1expandXpad);

   // Select position units to use for ploting (micrometer, table position units)
   if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *posUnitsPlot -> Dropdown menu for selecting the position units for ploting\n");
   posUnitsPlot = new TSubStructure();
   selnames[0] = "table units"; selnames[1] = "micrometers";
   sprintf(selected, "table units");
   if(posUnitsPlot->TGLabelDrop(mvaLayout[3], 2.*subgroup[0]/3., 30, "Position units for plots:", 2, selnames, selected))
      mvaLayout[3]->AddFrame(posUnitsPlot->outsidebox, f0centerX);

   // Actions for histogram controls pane //TODO
   for(int i = 0; i < 2; i++)
   {
      adcRange->widgetNE[i]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateHistogram(=0)");
//      (adcRange->widgetNE[i]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateHistogram(=0)");
      tdcRange->widgetNE[i]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateHistogram(=0)");
//      (tdcRange->widgetNE[i]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateHistogram(=0)");
      yRange->widgetNE[i]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateHistogram(=0)");
//      (yRange->widgetNE[i]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateHistogram(=0)");
   }
   plotType->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "ChangeHisttype(=0)");
   plotType->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "ChangeHisttype(=1)");
   plotType->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, "ChangeHisttype(=2)");
   selectCh->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "UpdateHistogram(=2)");
   selectCh->widgetNE[0]->Connect("ValueChanged(Long_t)", "TGAppMainFrame", this, "UpdateHistogram(=2)");
   (selectCh->widgetNE[0]->GetNumberEntry())->Connect("ReturnPressed()", "TGAppMainFrame", this, "UpdateHistogram(=2)");
   histOpt->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "HistogramOptions(=0)");
   histOpt->widgetChBox[1]->Connect("Clicked()", "TGAppMainFrame", this, "HistogramOptions(=1)");
   exportHist->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "UpdateHistogram(=1)");

// Histogram controls pane -----------------------------------------------------------------

   // Setting starting settings (when opening program)
   GetMeasSettings();
   GetAnalSettings();
}

// -------------------------------------------------------------------

// Closing main window and checking about information ----------------

void TGAppMainFrame::CloseWindow()
{
   gApplication->Terminate(0);
}

Bool_t TGAppMainFrame::About()
{
   int ret = 0;

   new TGMsgBox(gClient->GetRoot(), fMain, fMain->GetWindowName(), "Software for SiPM characterization with\nCAMAC, scope, bias voltage and table position support\n\nCreated by Gasper Kukec Mezek (gasper.kukec@ung.si),\nUpdated on July 17th 2015", kMBIconQuestion, kMBClose, &ret);

   return kFALSE;
}

// -------------------------------------------------------------------

// Main function -----------------------------------------------------
void root_app()
{
   if(DBGSIG > 1)
   {
      printf("root_app(): Starting objects\n");
      gObjectTable->Print();
   }

   int winWidth = 1240;
   int winHeight = 800;
   layoutMainWindow(&winWidth, &winHeight);

   new TGAppMainFrame(gClient->GetRoot(), winWidth, winHeight);
}

int main(int argc, char **argv)
{
   if(DBGSIG > 1)
   {
      printf("main(): Starting objects\n");
      gObjectTable->Print();
   }

   TApplication theApp("MdiTest", &argc, argv);
   root_app();
   theApp.Run();

   return 0;
}

// -------------------------------------------------------------------*/
