#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"

#include <stdio.h>
#include <stdlib.h>

// Layout hints
TGLayoutHints *f0centerX = new TGLayoutHints(kLHintsCenterX,2,2,2,2);
TGLayoutHints *f0leftX = new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2);
TGLayoutHints *f0leftXnoleft = new TGLayoutHints(kLHintsLeft | kLHintsTop,0,2,2,2);
TGLayoutHints *f0leftXnopad = new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0);
TGLayoutHints *f0leftXpad = new TGLayoutHints(kLHintsLeft | kLHintsTop,12,12,2,2);
TGLayoutHints *f0rightX = new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2);
TGLayoutHints *f0rightXpad = new TGLayoutHints(kLHintsRight | kLHintsTop,12,12,2,2);
TGLayoutHints *f0centerY = new TGLayoutHints(kLHintsCenterY,2,2,2,2);
TGLayoutHints *f0center2d = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,2,2,2,2);
TGLayoutHints *f1expandX = new TGLayoutHints(kLHintsExpandX,2,2,2,2);
TGLayoutHints *f1expand2d = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2);
TGLayoutHints *f1expandXpad = new TGLayoutHints(kLHintsExpandX,12,12,2,2);

// Save the cuts file
void TGAppMainFrame::CutsSave(int opt, int savetype)
{
   bool saved = false;

   if(savetype == 0)
   {
      if(opt == 0)
         cutsout->SaveFile(configFile->widgetTE->GetText(), kFALSE);
      else if(opt == 1)
         cutsout->SaveFile(sdcutsFile->widgetTE->GetText(), kFALSE);
      else if(opt == 2)
         cutsout->SaveFile(fdcutsFile->widgetTE->GetText(), kFALSE);
   }
   else if(savetype == 1)
   {
      TGFileInfo file_info;
      char *filetypes[] = {(char*)"Configure file",(char*)"*.cfg",(char*)0,(char*)0};
      if(opt != 0)
      {
         if(opt == 1)
            filetypes[0] = (char*)"SD cuts file";
         else if(opt == 2)
            filetypes[0] = (char*)"FD cuts file";
         filetypes[1] = (char*)"*.cuts";
      }
      file_info.fFileTypes = (const char**)filetypes;
      file_info.fIniDir = StrDup(currentCutsDir);
      new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);
      if(file_info.fFilename != NULL)
      {
         saved = cutsout->SaveFile(file_info.fFilename, kTRUE);
	 if(saved)
	 {
	    if(opt == 0)
               configFile->widgetTE->SetText(file_info.fFilename);
	    else if(opt == 1)
               sdcutsFile->widgetTE->SetText(file_info.fFilename);
	    else if(opt == 2)
               fdcutsFile->widgetTE->SetText(file_info.fFilename);
	 }
      }
   }
}

// Cfg/Cuts file editor -------------------------------------------------
void TGAppMainFrame::CutsEditor(TGTab *mainTab, bool create, int *tabid, char *infile, int opt)
{
   if(create)
   {
      TGCompositeFrame *fH1, *fV1;
      TGHorizontalFrame *fTitle;
      TGGroupFrame *fG1;
      TGLabel *lab;

      char selected[256];
      char *ctemp;
      ctemp = new char[1024];
    
      int startTab = mainTab->GetCurrent();
      int newTab = mainTab->GetNumberOfTabs();
      if(DBGSIG > 1) printf("CutsEditor(): Current tab = %d, Nr. of tabs = %d\n", startTab, newTab );

      double numform[6];
      vector<string> selnames;
      
      int subgroup[2];
      subgroup[0] = mainTab->GetWidth()-10;
      subgroup[1] = mainTab->GetHeight()-10;
    
      TGCompositeFrame *fT1;
      fT1 = fTab->AddTab("Cuts file editor");

      // Title label
      fTitle = new TGHorizontalFrame(fT1, 100, 25, kFixedHeight | kSunkenFrame);
      TGTitleLabel(fT1, fTitle, "Cuts file editor", (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
      fT1->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop));

      //TODO
      fH1 = new TGCompositeFrame(fT1, subgroup[0], 30, kHorizontalFrame);
      // Label for the openend file
      lab = new TGLabel(fH1, "Opened cuts file:");
      fH1->AddFrame(lab, f0leftX);
      fT1->AddFrame(fH1, f1expandXpad);

      const TGFont *tfont = gClient->GetFont(HELPFONT);
      FontStruct_t cutsFont = tfont->GetFontStruct();
      cutsout = new TGTextEdit(fT1,100,subgroup[1]/2);
      cutsout->SetFont(cutsFont);
      cutsout->SetForegroundColor(0x000000);
      fT1->AddFrame(cutsout, new TGLayoutHints(kLHintsExpandX | kLHintsTop));

      if(cutsout->LoadFile(infile))
      {
         if(DBGSIG) printf("TGAppMainFrame(): Cuts file correctly loaded.\n");
      }
      else
         printf("Error! Cuts file not loaded.\n");

      // Select a cut or configure option to add to the file
      if(DBGSIG > 1) printf("AppLayout(): Creating TSubStructure *insertCut -> Dropdown menu for selecting a cut or configure option to add\n");
      insertCut = new TSubStructure();
      selnames.erase(selnames.begin(), selnames.end());
      selnames.push_back("Select option...");
      if(opt == 0)	// configure options
      {
	 for(int i = 0; i < configNames.size(); i++)
	    selnames.push_back(configNames[i]);
      }
      else if(opt == 1)	// sd cut options
      {
	 for(int i = 0; i < sdcutNames.size(); i++)
	    selnames.push_back(sdcutNames[i]);
      }
      else if(opt == 2)	// fd cut options
      {
	 for(int i = 0; i < fdcutNames.size(); i++)
	    selnames.push_back(fdcutNames[i]);
      }
      sprintf(selected, "Select option...");
      if(insertCut->TGLabelDrop(fT1, subgroup[0], 30, "Select an option to add to the file:", selnames.size(), selnames, selected))
         fT1->AddFrame(insertCut->outsidebox, f0leftX);

      // Save, Save As and Close buttons
      if(DBGSIG > 1) printf("CutsEditor(): Creating TSubStructure *cutsEditorButtons -> Buttons to save progress on cuts file or close\n");
      cutsEditorButtons = new TSubStructure();
      selnames.erase(selnames.begin(), selnames.end());
      selnames.push_back("Save");
      selnames.push_back("Save As");
      selnames.push_back("Close");
      if(cutsEditorButtons->TGMultiButton(fT1, subgroup[0], 30, selnames.size(), selnames, "center"))
         fT1->AddFrame(cutsEditorButtons->outsidebox, f0leftXpad);

      // Actions for things in the cuts editor
      insertCut->widgetCB->Connect("Selected(Int_t)", "TGAppMainFrame", this, "InsertCutOption(Int_t)");
      sprintf(ctemp, "CutsSave(=%d,%d)", opt, 0);
      cutsEditorButtons->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, ctemp);
      sprintf(ctemp, "CutsSave(=%d,%d)", opt, 1);
      cutsEditorButtons->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, ctemp);
      sprintf(ctemp, "CloseCutsEditor(=%d)", newTab*100+startTab);
      cutsEditorButtons->widgetTB[2]->Connect("Clicked()", "TGAppMainFrame", this, ctemp);

      fMain->MapSubwindows();
      fMain->MapWindow();
      fMain->Layout();

      delete[] ctemp;

      // Switch to new tab
      fTab->SetTab(newTab);

      if(DBGSIG > 1)
      {
         printf("HeaderEditTab(): New tab objects (Edit Header)\n");
         gObjectTable->Print();
      }
   }
   else
   {
      // Switch to new tab
      fTab->SetTab(*tabid);
   }
}

void TGAppMainFrame::CloseCutsEditor(int tabval)
{
   int curtab = (int)TMath::Floor(tabval/100.);
   int oldtab = tabval - curtab*100;

   if(DBGSIG > 1) printf("CloseCutsEditor(): New tab = %d, old tab = %d\n", curtab, oldtab);

   fTab->RemoveTab(curtab);

/*   delete tempAnalysisCanvas;
   delete exportExitAnalysis;*/

   for(int i = 0; i < fTab->GetNumberOfTabs(); i++)
      if(DBGSIG > 1) printf("CloseCutsEditor(): Name of tab (%d) = %s\n", i, fTab->GetTabTab(i)->GetString() );

   fTab->SetTab(oldtab);
}
// Cfg/Cuts file editor -------------------------------------------------

// Ask a Yes/No question for a bool option
void TGAppMainFrame::BoolOption(string title, string question, string *outstring, int selected, int linewidth, int opt)
{
   int curwidth;
   int ret;

   new TGMsgBox(gClient->GetRoot(), fMain, title.c_str(), question.c_str(), kMBIconQuestion, kMBYes | kMBNo, &ret);
   if(ret == kMBYes)
   {
      if(opt == 1)
         outstring[0] = sdcutNames[selected-1];
      else if(opt == 2)
         outstring[0] = fdcutNames[selected-1];
      curwidth = linewidth-outstring[0].length();
   }
   else if(ret == kMBNo)
   {
      if(opt == 1)
         outstring[0] = "!" + sdcutNames[selected-1];
      else if(opt == 2)
         outstring[0] = "!" + fdcutNames[selected-1];
      curwidth = linewidth-outstring[0].length();
   }

   for(int i = 0; i < curwidth; i++)
      outstring[0] += " ";
}

// Insert the selected cfg/cuts option
void TGAppMainFrame::InsertCutOption(int select)
{
   int opt = -1;
   char *ctemp;
   int linewidth = 45;
   int curwidth;
   
   if(opt == -1)
   {
      for(int i = 0; i < configNames.size(); i++)
      {
         if(configNames[i] == string(insertCut->widgetCB->GetSelectedEntry()->GetTitle()))
         {
	    opt = 0;
	    break;
	 }
      }
   }
   
   if(opt == -1)
   {
      for(int i = 0; i < sdcutNames.size(); i++)
      {
         if(sdcutNames[i] == string(insertCut->widgetCB->GetSelectedEntry()->GetTitle()))
         {
	    opt = 1;
	    break;
	 }
      }
   }
   
   if(opt == -1)
   {
      for(int i = 0; i < fdcutNames.size(); i++)
      {
         if(fdcutNames[i] == string(insertCut->widgetCB->GetSelectedEntry()->GetTitle()))
         {
	    opt = 2;
	    break;
	 }
      }
   }

   string insertStr = "";

   if(opt == 0)
   {
      if(select == 1)	// SDCutFile
      {
         if( string(sdcutsFile->widgetTE->GetText()) != string("No file selected!") )
            insertStr = configNames[select-1] + " " + string(sdcutsFile->widgetTE->GetText());
	 else
	    insertStr = configNames[select-1] + " " + string(currentCutsDir) + "/sdcuts.cuts";

	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 2)	// FDCutFile
      {
         if( string(fdcutsFile->widgetTE->GetText()) != string("No file selected!") )
            insertStr = configNames[select-1] + " " + string(fdcutsFile->widgetTE->GetText());
	 else
	    insertStr = configNames[select-1] + " " + string(currentCutsDir) + "/fdcuts.cuts";

	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 3)	// SDBadPeriodsFile
      {
	 ctemp = new char[1024];
         TGFileInfo file_info;
         const char *filetypes[] = {"Bad periods file","*.txt",0,0};
         file_info.fFileTypes = filetypes;
	 sprintf(ctemp, "%s/input/event_selection", rootdir);
         file_info.fIniDir = StrDup(ctemp);
         file_info.fMultipleSelection = kFALSE;
         new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);

	 if(file_info.fFilename != NULL)
	 {
	    insertStr = configNames[select-1] + " " + string(file_info.fFilename);
	    cutsout->AddLine(insertStr.c_str());
	 }
	 delete[] ctemp;
      }

      insertCut->widgetCB->Select(0);
   }
   else if(opt == 1)
   {
      if(select == 1)	// saturatedCandidates
      {
	 BoolOption("Saturated candidates", "Check low-gain saturation only in candidate stations without rejection status?", &insertStr, select, linewidth, opt);
	 insertStr += "# check low-gain saturation only in candidate stations without rejection status";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 2)	// LDFStatus
      {
	 insertStr = sdcutNames[select-1] + " 4.5";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# minSdRecStatus: Bary = 0.5, PlaneFitLinear = 1, PlaneFitLinear2 = 1.1, PlaneFitLinear3d = 1.2,";
	 cutsout->AddLine(insertStr.c_str());
	 insertStr = "";
         for(int i = 0; i < linewidth; i++)
            insertStr += " ";
	 insertStr += "# LDFEstimate = 2, LDFEstimateAndCurvature = 2.5, LDF = 3, LDFAndCurvature = 3.5, LDFSilents = 4,";
	 cutsout->AddLine(insertStr.c_str());
	 insertStr = "";
         for(int i = 0; i < linewidth; i++)
	    insertStr += " ";
	 insertStr += "# LDFSilentsAndCurvature = 4.5, LDFGlobalFit = 5, LDFBeta += 0.1, LDFGamma += 0.01, LDFLowerLimit += 0.001";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 3)	// T4Trigger
      {
	 insertStr = sdcutNames[select-1] + " 2";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# required T4 flag (0: no T4, 1: FD-triggered, 2: SD-triggered, 3: FD- and SD-triggered (golden hybrids))";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 4)	// T5Trigger
      {
	 insertStr = sdcutNames[select-1] + " 2";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# required T5 flag (0: any T5, 1: 5T5 posterior, 2: 6T5 prior, 3: T5Has)";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 5)	// minZenithSD
      {
	 insertStr = sdcutNames[select-1] + " 0.";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# minimum zenith angle (deg)";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 6)	// maxZenithSD
      {
	 insertStr = sdcutNames[select-1] + " 0.";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# maximum zenith angle (deg)";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 7)	// minRecLevel
      {
	 insertStr = sdcutNames[select-1] + " 3";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# minimum reconstruction level (0: no SD event, 1: SD has triggered stations, 2: axis reconstructed, 3: LDF fit, 4: curvature)";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 8)	// minCandidateStations
      {
	 insertStr = sdcutNames[select-1] + " 5";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# minimum number of candidate stations";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 9)	// maxCandidateStations
      {
	 insertStr = sdcutNames[select-1] + " 5";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# maximum number of candidate stations";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 10)	// badPeriodRejection
      {
	 BoolOption("Bad period rejection", "Reject bad periods of SD?", &insertStr, select, linewidth, opt);
	 insertStr += "# (bool) reject bad periods";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 11)	// badPeriodRejectionFromFile
      {
	 insertStr = sdcutNames[select-1];
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# reject bad periods defined in the file SDBadPeriodsFile in config";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 12)	// selectSaturation
      {
	 BoolOption("Saturated selection", "Select saturated events?", &insertStr, select, linewidth, opt);
	 insertStr += "# (bool) select saturated events";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 13)	// minLgEnergySD
      {
	 insertStr = sdcutNames[select-1] + " 18.0";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# minimum reconstructed lg(energy/[eV])";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 14)	// maxLgEnergySD
      {
	 insertStr = sdcutNames[select-1] + " 99.0";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# maximum reconstructed lg(energy/[eV])";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 15)	// timeInterval
      {
	 insertStr = sdcutNames[select-1] + " { params: 40000 61231";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# time interval: yymmdd yymmdd";
	 cutsout->AddLine(insertStr.c_str());
         insertStr = "";
         for(int i = 0; i < sdcutNames[select-1].length(); i++)
            insertStr += " ";
	 insertStr += "   nMinusOne: 20 61231 90312 }";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 16)	// heraldSelection
      {
	 BoolOption("Selection for Herald converted to ADST files", "Select Herald events?", &insertStr, select, linewidth, opt);
	 insertStr += "# (bool) works only for Herald converted to ADST files";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 17)	// fixBeta
      {
	 BoolOption("Selection of fixed beta", "Select events with fixed beta?", &insertStr, select, linewidth, opt);
	 insertStr += "# (bool) select events with fix beta";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 18)	// sdId
      {
	 insertStr = sdcutNames[select-1] + " 0909919";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# selects SD id";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 19)	// augerId
      {
	 insertStr = sdcutNames[select-1] + " 204001810";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# selects Auger id";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 20)	// temperature
      {
	 insertStr = sdcutNames[select-1] + " { params: -100 100 }";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# temperature interval (Celsius)";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 21)	// idsFromFile
      {
	 insertStr = sdcutNames[select-1] + " 0";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# select events from a file SDIdsFile set in the config (1: Auger ID, 0: SD ID)";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 22)	// lightning
      {
	 BoolOption("Lightning selection", "Select events with at least one lightning station?", &insertStr, select, linewidth, opt);
	 insertStr += "# (bool) select events that have at least one lightning station";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 23)	// maxRelativeShowerSizeError
      {
	 insertStr = sdcutNames[select-1] + " 0.2";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# maximum relative uncertainty of shower size";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 24)	// badSilentStations
      {
	 insertStr = sdcutNames[select-1] + " { params: 500 10 }";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# parameters: maxDistance, maxLDFValue";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 25)	// hasSilents
      {
	 BoolOption("Silent station selection", "Select events with silent stations?", &insertStr, select, linewidth, opt);
	 insertStr += "# (bool) has silent stations";
	 cutsout->AddLine(insertStr.c_str());
      }

      insertCut->widgetCB->Select(0);
   }
   else if(opt == 2)
   {
      if(select == 1)	// xMaxInFOV
      {
	 insertStr = fdcutNames[select-1] + " 20.0";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# maximum distance of xMax to borders";
	 cutsout->AddLine(insertStr.c_str());
      }
      else if(select == 2)	// xMaxError
      {
	 insertStr = fdcutNames[select-1] + " 40.0";
         curwidth = linewidth-insertStr.length();
         for(int i = 0; i < curwidth; i++)
            insertStr += " ";
	 insertStr += "# maximum error on xMax [g/cm^2]";
	 cutsout->AddLine(insertStr.c_str());
      }

      insertCut->widgetCB->Select(0);
   }

   insertStr = "";
}

/*// Edit file window ---------------------------------------------------

// Open a new tab for editing datafile headers
void TGAppMainFrame::HeaderEditTab(TGTab *mainTab, bool create, int *tabid)
{
   unsigned int nrfiles;
   ULong_t rcolor, bcolor;
   gClient->GetColorByName("red", rcolor);
   gClient->GetColorByName("black", bcolor);

   if(create)
   {
      TGCompositeFrame *fH1, *fV1;
      TGHorizontalFrame *fTitle;
      TGGroupFrame *fG1;
      TGLabel *lab;
    
      int startTab = mainTab->GetCurrent();
      int newTab = mainTab->GetNumberOfTabs();
      if(DBGSIG > 1) printf("HeaderEditTab(): Current tab = %d, Nr. of tabs = %d\n", startTab, newTab );

      double numform[6];
      
      int subgroup[2];
      subgroup[0] = mainTab->GetWidth()-10;
      subgroup[1] = mainTab->GetHeight()-10;
    
      TGCompositeFrame *fT1;
      fT1 = fTab->AddTab("File header editor");

      // Title label
      fTitle = new TGHorizontalFrame(fT1, 100, 25, kFixedHeight | kSunkenFrame);
      TGTitleLabel(fT1, fTitle, "File header editor", (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
      fT1->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop));
    
      // List view of files that we will edit
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TGListBox *editList -> List box for editing files\n");
      editList = new TGListBox(fT1,1);
      editList->GetVScrollbar();
      editList->Resize(300, (3*subgroup[1]/7)-10 );
      fT1->AddFrame(editList, f1expandXpad);

      editList->SetMultipleSelections((multiSelect->widgetChBox[0]->IsOn()));
    
      // Copy the file list from the analysis tab for clearer view
      nrfiles = fileList->GetNumberOfEntries();
      printf("Nr. files = %d\n", nrfiles);
      for(int i = 0; i < nrfiles; i++)
         editList->AddEntry(fileList->GetEntry(i)->GetTitle(), i);

      fH1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kHorizontalFrame);
      fV1 = new TGCompositeFrame(fH1, subgroup[0]/2, subgroup[1], kVerticalFrame);
      // Time stamp display
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *timeEditDisplay -> Display text Entry (time stamp)\n");
      timeEditDisplay = new TSubStructure();
      if(timeEditDisplay->TGLabelTEntry(fV1, subgroup[0]/2-4, 30, "Time of measurement:", "", "oneline"))
         fV1->AddFrame(timeEditDisplay->outsidebox, f0leftXpad);
      timeEditDisplay->widgetTE->SetState(kFALSE);

      // Bias voltage edit
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *biasEdit -> Number entry for bias voltage edit\n");
      biasEdit = new TSubStructure();
      for(int i = 0; i < 6; i++) numform[i] = 0;
      numform[0] = 7; numform[1] = 2;
      if(biasEdit->TGCheckNEntry(fV1, subgroup[0]/2, 30, "Bias voltage edit:", 0, 0.00, numform, "left"))
         fV1->AddFrame(biasEdit->outsidebox, f0leftXpad);

      // Position edits
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *xPosEdit, *yPosEdit, *zPosEdit -> Number entries for position edit\n");
      xPosEdit = new TSubStructure();
      for(int i = 0; i < 6; i++) numform[i] = 0;
      numform[0] = 9; numform[3] = 2; numform[4] = -100; numform[5] = 215000;
      if(xPosEdit->TGCheckNEntry(fV1, subgroup[0]/2, 30, "X position edit:", 0, 0, numform, "left"))
         fV1->AddFrame(xPosEdit->outsidebox, f0leftXpad);

      yPosEdit = new TSubStructure();
      if(yPosEdit->TGCheckNEntry(fV1, subgroup[0]/2, 30, "Y position edit:", 0, 0, numform, "left"))
         fV1->AddFrame(yPosEdit->outsidebox, f0leftXpad);

      zPosEdit = new TSubStructure();
      numform[5] = 375000;
      if(zPosEdit->TGCheckNEntry(fV1, subgroup[0]/2, 30, "Z position edit:", 0, 0, numform, "left"))
         fV1->AddFrame(zPosEdit->outsidebox, f0leftXpad);

      // Temperature edit
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *tempEdit -> Number entry for temperature edit\n");
      tempEdit = new TSubStructure();
      for(int i = 0; i < 6; i++) numform[i] = 0;
      numform[0] = 6; numform[1] = 1;
      if(tempEdit->TGCheckNEntry(fV1, subgroup[0]/2, 30, "Temperature edit:", 0, 0.0, numform, "left"))
         fV1->AddFrame(tempEdit->outsidebox, f0leftXpad);

      // Angle edit
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *angleEdit -> Number entry for angle edit\n");
      angleEdit = new TSubStructure();
      for(int i = 0; i < 6; i++) numform[i] = 0;
      numform[0] = 7; numform[1] = 2;
      if(angleEdit->TGCheckNEntry(fV1, subgroup[0]/2, 30, "Incidence angle edit:", 0, 0.00, numform, "left"))
         fV1->AddFrame(angleEdit->outsidebox, f0leftXpad);

      // Laser settings edit
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *laserEdit -> Display text Entry for laser edit\n");
      laserEdit = new TSubStructure();
      if(laserEdit->TGCheckTEntry(fV1, subgroup[0]/2, 30, "Laser settings edit:", 0, "", "oneline"))
         fV1->AddFrame(laserEdit->outsidebox, f0leftXpad);

      // Edit and close buttons
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *editHead -> 2 buttons for either editing the head or closing the tab\n");
      editHead = new TSubStructure();
      const char *selnames[512] = {"Edit header","Close"};
      if(editHead->TGMultiButton(fV1, subgroup[0]/2, 30, 2, selnames, "center"))
         fV1->AddFrame(editHead->outsidebox, f0leftXpad);
      fH1->AddFrame(fV1, f0leftXnopad);

      fV1 = new TGCompositeFrame(fH1, subgroup[0]/2, subgroup[1], kVerticalFrame);
      // Multiple file select
      if(DBGSIG > 1) printf("HeaderEditTab(): Creating TSubStructure *editMulti -> 1 Check button to set multi select or not\n");
      editMulti = new TSubStructure();
      int *checksel;
      checksel = new int;
      *checksel = multiSelect->widgetChBox[0]->IsDown();
      selnames[0] = "Select multiple files";
      if(editMulti->TGCheckList(fV1, subgroup[0]/2, 30, 1, selnames, checksel, "vertical", "center"))
         fV1->AddFrame(editMulti->outsidebox, f0centerX);

      // Warning information
      fG1 = new TGGroupFrame(fV1, "Warnings");
      lab = new TGLabel(fG1, "Note: Tick checkbox in front of each header information you wish to change\n(to avoid any unwanted changes, they are unticked by default).");
      fG1->AddFrame(lab, f0leftXpad);
      lab = new TGLabel(fG1, "Note: When selecting files in the list, the entry fields will update accordingly\nwith information from the selected file (only for those where check\nboxes are not turned on).");
      fG1->AddFrame(lab, f0leftXpad);
      lab = new TGLabel(fG1, "Warning: Using button \"Edit header\" will edit headers in all files currently\nselected in the above selection list.");
      lab->SetTextColor(rcolor);
      fG1->AddFrame(lab, f0leftXpad);
      if((editMulti->widgetChBox[0]->IsOn()))
      {
         selectWarn = new TGLabel(fG1, "Warning: Multiple files selected!");
         selectWarn->SetTextColor(rcolor);
         fG1->AddFrame(selectWarn, f0leftXpad);
      }
      else
      {
         selectWarn = new TGLabel(fG1, "Note: Single file selected.      ");
         selectWarn->SetTextColor(bcolor);
         fG1->AddFrame(selectWarn, f0leftXpad);
      }
      fV1->AddFrame(fG1, f0centerX);
      fH1->AddFrame(fV1, f0centerX);

      // Actions for header editor
      editList->Connect("Selected(Int_t)", "TGAppMainFrame", this, "ShowHeaderEdit(Int_t)");
      biasEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=1)");
      xPosEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=2)");
      yPosEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=3)");
      zPosEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=4)");
      tempEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=5)");
      angleEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=6)");
      laserEdit->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "EditTickToggle(=7)");
      editMulti->widgetChBox[0]->Connect("Clicked()", "TGAppMainFrame", this, "SetWarnings()");
      editHead->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "StartHeaderEdit()");
      char cTemp[512];
      sprintf(cTemp, "CloseEditTab(=%d)", newTab*100+startTab);
      editHead->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, cTemp);

      fT1->AddFrame(fH1, f1expand2d);
      fMain->MapSubwindows();
      fMain->MapWindow();
      fMain->Layout();

      // Initialize the values
      for(int i = 0; i < 8; i++)
         EditTickToggle(i);

      // Switch to new tab
      fTab->SetTab(newTab);

      if(DBGSIG > 1)
      {
         printf("HeaderEditTab(): New tab objects (Edit Header)\n");
         gObjectTable->Print();
      }
   }
   else
   {
      if(multiSelect->widgetChBox[0]->IsDown())
         editMulti->widgetChBox[0]->SetState(kButtonDown);
      else
         editMulti->widgetChBox[0]->SetState(kButtonUp);

      editList->SetMultipleSelections((editMulti->widgetChBox[0]->IsDown()));

      // Recopy the file list from the analysis tab
      nrfiles = fileList->GetNumberOfEntries();
      printf("Nr. files = %d\n", nrfiles);
      for(int i = 0; i < nrfiles; i++)
         editList->AddEntry(fileList->GetEntry(i)->GetTitle(), i);

      SetWarnings();

      // Switch to new tab
      fTab->SetTab(*tabid);
   }
}

// Change the warning when selecting multiple files
void TGAppMainFrame::SetWarnings()
{
   ULong_t rcolor, bcolor;
   gClient->GetColorByName("red", rcolor);
   gClient->GetColorByName("black", bcolor);

   editList->SetMultipleSelections((editMulti->widgetChBox[0]->IsDown()));

   // Set the warnings
   if(editMulti->widgetChBox[0]->IsDown())
   {
      selectWarn->SetText("Warning: Multiple files selected!");
      selectWarn->SetTextColor(rcolor);
      selectWarn->SetWrapLength(-1);
   }
   else
   {
      selectWarn->SetText("Note: Single file selected.      ");
      selectWarn->SetTextColor(bcolor);
      selectWarn->SetWrapLength(-1);
   }
}

// Actions for editing the header
void TGAppMainFrame::EditTickToggle(int type)
{
   // Toggle the edit possibility for header entries

   // Bias voltage
   if(type == 1)
   {
      if(biasEdit->widgetChBox[0]->IsDown()) biasEdit->widgetNE[0]->SetState(kTRUE);
      else biasEdit->widgetNE[0]->SetState(kFALSE);
   }
   // X position
   else if(type == 2)
   {
      if(xPosEdit->widgetChBox[0]->IsDown()) xPosEdit->widgetNE[0]->SetState(kTRUE);
      else xPosEdit->widgetNE[0]->SetState(kFALSE);
   }
   // Y position
   else if(type == 3)
   {
      if(yPosEdit->widgetChBox[0]->IsDown()) yPosEdit->widgetNE[0]->SetState(kTRUE);
      else yPosEdit->widgetNE[0]->SetState(kFALSE);
   }
   // Z position
   else if(type == 4)
   {
      if(zPosEdit->widgetChBox[0]->IsDown()) zPosEdit->widgetNE[0]->SetState(kTRUE);
      else zPosEdit->widgetNE[0]->SetState(kFALSE);
   }
   // Temperature
   else if(type == 5)
   {
      if(tempEdit->widgetChBox[0]->IsDown()) tempEdit->widgetNE[0]->SetState(kTRUE);
      else tempEdit->widgetNE[0]->SetState(kFALSE);
   }
   // Angle
   else if(type == 6)
   {
      if(angleEdit->widgetChBox[0]->IsDown()) angleEdit->widgetNE[0]->SetState(kTRUE);
      else angleEdit->widgetNE[0]->SetState(kFALSE);
   }
   // Laser info
   else if(type == 7)
   {
      if(laserEdit->widgetChBox[0]->IsDown()) laserEdit->widgetTE->SetState(kTRUE);
      else laserEdit->widgetTE->SetState(kFALSE);
   }
}

void TGAppMainFrame::StartHeaderEdit()
{
   unsigned int nrfiles = editList->GetNumberOfEntries();
   TList *files;
   // Changelist: Bias, X, Y, Z Positions, Temperature, Angle, Laser info
   bool changelist[] = { biasEdit->widgetChBox[0]->IsDown(), xPosEdit->widgetChBox[0]->IsDown(), yPosEdit->widgetChBox[0]->IsDown(), zPosEdit->widgetChBox[0]->IsDown(), tempEdit->widgetChBox[0]->IsDown(), angleEdit->widgetChBox[0]->IsDown(), laserEdit->widgetChBox[0]->IsDown() };

   if( nrfiles > 0 )
   {
      // check the selected file/files and return its name/their names
      files = new TList();
      editList->GetSelectedEntries(files);
      if(files)
      {
         for(int i = 0; i < (int)nrfiles; i++)
         {
            if(files->At(i))
	    {
	       if(DBGSIG)
                  printf("StartHeaderEdit(): Filename: %s\n", files->At(i)->GetTitle());

	       HeaderChange( (char*)(files->At(i)->GetTitle()), changelist );
	    }
         }
      }
   }
}

void TGAppMainFrame::ShowHeaderEdit(int id)
{
   char cTemp[512];

   // Preparing input file
   inroot = TFile::Open(editList->GetEntry(id)->GetTitle(), "READ");

   // Header tree
   TTree *header_data;
   inroot->GetObject("header_data", header_data);

   // Display branches from header in the entry fields
   header_data->SetBranchAddress("timestamp", &evtheader.timestamp);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("biasvolt", &evtheader.biasvolt);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("xpos", &evtheader.xpos);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("ypos", &evtheader.ypos);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("zpos", &evtheader.zpos);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("temperature", &evtheader.temperature);
   header_data->GetEntry(0);
   if( header_data->FindBranch("angle") )
   {
      header_data->SetBranchAddress("angle", &evtheader.angle);
      header_data->GetEntry(0);
   }
   header_data->SetBranchAddress("laserinfo", &evtheader.laserinfo);
   header_data->GetEntry(0);

   GetTime(evtheader.timestamp, cTemp);

   timeEditDisplay->widgetTE->SetText(cTemp);
   if(!biasEdit->widgetChBox[0]->IsDown())
      biasEdit->widgetNE[0]->SetNumber(evtheader.biasvolt);
   if(!xPosEdit->widgetChBox[0]->IsDown())
      xPosEdit->widgetNE[0]->SetNumber(evtheader.xpos);
   if(!yPosEdit->widgetChBox[0]->IsDown())
      yPosEdit->widgetNE[0]->SetNumber(evtheader.ypos);
   if(!zPosEdit->widgetChBox[0]->IsDown())
      zPosEdit->widgetNE[0]->SetNumber(evtheader.zpos);
   if(!tempEdit->widgetChBox[0]->IsDown())
      tempEdit->widgetNE[0]->SetNumber(evtheader.temperature);
   if(!angleEdit->widgetChBox[0]->IsDown())
   {
      if( header_data->FindBranch("angle") )
         tempEdit->widgetNE[0]->SetNumber(evtheader.angle);
      else
         tempEdit->widgetNE[0]->SetNumber(0.);
   }
   if(!laserEdit->widgetChBox[0]->IsDown())
      laserEdit->widgetTE->SetText(evtheader.laserinfo);

   delete header_data;
   delete inroot;
}

void TGAppMainFrame::HeaderChange(char *histfile, bool *changetype)
{
   int scopeTemp;

   if(DBGSIG)
      printf("HeaderChange(): Selected file: %s\n", histfile);

   // Preparing input file and the temporary output file
   inroot = TFile::Open(histfile, "READ");

   scopeTemp = inroot->GetListOfKeys()->Contains("scope_data");

   char outname[256];
   sprintf(outname, "%s/results/temp.root", rootdir);
   outroot = TFile::Open(outname, "RECREATE");

   // Tree structure of input file and output file
   TTree *header_data, *meas_data, *scope_data;

   printf("%d\n", inroot->GetListOfKeys()->Contains("header_data"));
   printf("%d\n", inroot->GetListOfKeys()->Contains("meas_data"));
   printf("%d\n", scopeTemp);

   TTree *new_meas_data, *new_scope_data;
   inroot->GetObject("header_data", header_data);
   inroot->GetObject("meas_data", meas_data);
   new_meas_data = meas_data->CloneTree();
   //TTree *new_scope_data;
   if(scopeTemp)
   {
      inroot->GetObject("scope_data", scope_data);
      new_scope_data = scope_data->CloneTree();
   }
   else
      printf("No scope_data header found.\n");

   // Save branches from the old header to temporary variables
   header_data->SetBranchAddress("nrch", &evtheader.nrch);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("timestamp", &evtheader.timestamp);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("biasvolt", &evtheader.biasvolt);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("xpos", &evtheader.xpos);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("ypos", &evtheader.ypos);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("zpos", &evtheader.zpos);
   header_data->GetEntry(0);
   header_data->SetBranchAddress("temperature", &evtheader.temperature);
   header_data->GetEntry(0);
   if( header_data->FindBranch("angle") )
   {
      header_data->SetBranchAddress("angle", &evtheader.angle);
      header_data->GetEntry(0);
   }
   header_data->SetBranchAddress("laserinfo", &evtheader.laserinfo);
   header_data->GetEntry(0);

   int itemp[5] = {0,0,0,0,0};
   double dtemp[3] = {0.,0.,0.};
   char ctemp[256];

   itemp[0] = evtheader.nrch;
   itemp[1] = evtheader.timestamp;
   itemp[2] = evtheader.xpos;
   itemp[3] = evtheader.ypos;
   itemp[4] = evtheader.zpos;
   dtemp[0] = evtheader.biasvolt;
   dtemp[1] = evtheader.temperature;
   if( header_data->FindBranch("angle") )
      dtemp[2] = evtheader.angle;
   else
      dtemp[2] = 0.;
   sprintf(ctemp, "%s", evtheader.laserinfo);

   delete header_data;
   delete meas_data;
   if(scopeTemp)
      delete scope_data;
   delete inroot;
  
//printf("HeaderChange(): 6\n");
   // Prepare branches for the new header
   TTree *new_header_data = new TTree("header_data", "Header information for the measurement.");
   new_header_data->Branch("nrch", &evtheader.nrch, "nrch/I");
   new_header_data->Branch("timestamp", &evtheader.timestamp, "timestamp/I");
   new_header_data->Branch("biasvolt", &evtheader.biasvolt, "biasvolt/D");
   new_header_data->Branch("xpos", &evtheader.xpos, "xpos/I");
   new_header_data->Branch("ypos", &evtheader.ypos, "ypos/I");
   new_header_data->Branch("zpos", &evtheader.zpos, "zpos/I");
   new_header_data->Branch("temperature", &evtheader.temperature, "temperature/D");
   new_header_data->Branch("angle", &evtheader.angle, "temperature/D");
   new_header_data->Branch("laserinfo", &evtheader.laserinfo, "laserinfo/C");

//printf("HeaderChange(): 7\n");
   // Save new values (and old ones where we don't want to edit anything)
   evtheader.nrch = itemp[0];
   evtheader.timestamp = itemp[1];
   // Bias voltage
   if(changetype[0])
      evtheader.biasvolt = (double)biasEdit->widgetNE[0]->GetNumber();
   else
      evtheader.biasvolt = dtemp[0];
   // X pos
   if(changetype[1])
      evtheader.xpos = (int)xPosEdit->widgetNE[0]->GetNumber();
   else
      evtheader.xpos = itemp[2];
   // Y pos
   if(changetype[2])
      evtheader.ypos = (int)yPosEdit->widgetNE[0]->GetNumber();
   else
      evtheader.ypos = itemp[3];
   // Z pos
   if(changetype[3])
      evtheader.zpos = (int)zPosEdit->widgetNE[0]->GetNumber();
   else
      evtheader.zpos = itemp[4];
   // Temperature
   if(changetype[4])
      evtheader.temperature = (double)tempEdit->widgetNE[0]->GetNumber();
   else
      evtheader.temperature = dtemp[1];
   // Angle
   if(changetype[5])
      evtheader.angle = (double)angleEdit->widgetNE[0]->GetNumber();
   else
      evtheader.angle = dtemp[2];
   // Laser info
   if(changetype[6])
      sprintf(evtheader.laserinfo, "%s", laserEdit->widgetTE->GetText());
   else
      sprintf(evtheader.laserinfo, "%s", ctemp);

//printf("HeaderChange(): 8\n");
   new_header_data->Fill();

   // Write down the temporary output file
   new_header_data->Write();
   new_meas_data->Write();
   if(scopeTemp)
      new_scope_data->Write();

//printf("HeaderChange(): 9\n");
   delete new_header_data;
   delete new_meas_data;
   if(scopeTemp)
      delete new_scope_data;
   delete outroot;

   // Replace the original file with temporary output file (and delete temporary file)
   sprintf(outname, "cp -f %s/results/temp.root %s", rootdir, histfile);
   system(outname);
   sprintf(outname, "rm -f %s/results/temp.root", rootdir);
   system(outname);

   printf("Edited header in file: %s\n", histfile);
}

void TGAppMainFrame::CloseEditTab(int tabval)
{
   int curtab = (int)TMath::Floor(tabval/100.);
   int oldtab = tabval - curtab*100;

   if(DBGSIG > 1) printf("CloseEditTab(): New tab = %d, old tab = %d\n", curtab, oldtab);

   fTab->RemoveTab(curtab);

   delete editList;
   delete timeEditDisplay;
   delete biasEdit;
   delete xPosEdit;
   delete yPosEdit;
   delete zPosEdit;
   delete tempEdit;
   delete angleEdit;
   delete laserEdit;
   delete editHead;
   delete selectWarn;

   for(int i = 0; i < fTab->GetNumberOfTabs(); i++)
      if(DBGSIG > 1) printf("CloseEditTab(): Name of tab (%d) = %s\n", i, fTab->GetTabTab(i)->GetString() );

   fTab->SetTab(oldtab);
}

// Edit file window ---------------------------------------------------

// Temporary analysis window ------------------------------------------

// Open a new tab for editing datafile headers
void TGAppMainFrame::TempAnalysisTab(TGTab *mainTab, bool create, int *tabid, int analtype)
{
   if(create)
   {
      TGCompositeFrame *fH1, *fV1;
      TGHorizontalFrame *fTitle;
      TGGroupFrame *fG1;
      TGLabel *lab;
    
      int startTab = mainTab->GetCurrent();
      int newTab = mainTab->GetNumberOfTabs();
      if(DBGSIG > 1) printf("TempAnalysisTab(): Current tab = %d, Nr. of tabs = %d\n", startTab, newTab );

      double numform[6];
      double numform2[6];
      
      int subgroup[2];
      subgroup[0] = mainTab->GetWidth()-10;
      subgroup[1] = mainTab->GetHeight()-10;

      TGCompositeFrame *fT1;
      fT1 = fTab->AddTab("Analysis edit");

      // Title label
      fTitle = new TGHorizontalFrame(fT1, 100, 25, kFixedHeight | kSunkenFrame);
      TGTitleLabel(fT1, fTitle, "Analysis edit", (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
      fT1->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop));

      fV1 = new TGCompositeFrame(fT1, subgroup[0], subgroup[1], kVerticalFrame);
      // Temporary analysis canvas
      if( (analTab->GetCurrent() == 3) || ((analTab->GetCurrent() == 0) && (intSpect->widgetChBox[0]->IsDown() || intSpect->widgetChBox[1]->IsDown())) )
      {
         tempAnalysisCanvas = new TRootEmbeddedCanvas("tempAnalysisCanvas",fV1,subgroup[1],5*subgroup[1]/6);
         fV1->AddFrame(tempAnalysisCanvas, f0centerX);
      }
      else
      {
         tempAnalysisCanvas = new TRootEmbeddedCanvas("tempAnalysisCanvas",fV1,3*subgroup[0]/4,3*subgroup[1]/4);
         fV1->AddFrame(tempAnalysisCanvas, f1expandXpad);
      }
      tempAnalysisCanvas->GetCanvas()->SetGrid();

      // Specific options for plotting (analtype: 0 = Normal integration, 1 = Edge scans, 2 = Relative PDE, 3 = Breakdown voltage, 4 = Surface scan, 5 = Timing,...)
      // Normal integration
      if(analtype == 0)
      {
      }
      // Edge scans
      else if(analtype == 1)
      {
      }
      // Relative PDE
      else if(analtype == 2)
      {
         // Running average offset
         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *runningOff -> Set running average offset.\n");
         runningOff = new TSubStructure();
         for(int i = 0; i < 6; i++) numform[i] = 0;
         numform[0] = 5; numform[2] = 2;
         if(runningOff->TGLabelNEntry(fV1, subgroup[0]/2-24, 30, "Running average offset:", 0, numform, "center"))
            fV1->AddFrame(runningOff->outsidebox, f1expandXpad);

         // Running average setting for plot
         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *runningAver -> Produce running average of a graph.\n");
         runningAver = new TSubStructure();
         for(int i = 0; i < 6; i++) numform[i] = 0;
         numform[0] = 5; numform[2] = 2;
         if(runningAver->TGLabelNEntry(fV1, subgroup[0]/2-24, 30, "Running average type (0 to disable):", 0, numform, "center"))
            fV1->AddFrame(runningAver->outsidebox, f1expandXpad);

         // Putting a second y-axis to the plot for mean number of photons histogram
         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *secondAxis -> Create second y-axis for mean number of photons.\n");
         secondAxis = new TSubStructure();
         for(int i = 0; i < 6; i++) numform[i] = 0;
         numform[0] = 5; numform[1] = 1; numform[2] = 2;
         if(secondAxis->TGLabelNEntry(fV1, subgroup[0]/2-24, 30, "Scale second axis:", 0, numform, "center"))
            fV1->AddFrame(secondAxis->outsidebox, f1expandXpad);

         runningAver->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "ApplyRunningAver()");
         runningOff->widgetNE[0]->Connect("ValueSet(Long_t)", "TGAppMainFrame", this, "ApplyRunningAver()");

	 ToolTipSetRelativePDE();
      }
      // Breakdown voltage
      else if(analtype == 3)
      {
      }
      // Surface scan
      else if(analtype == 4)
      {
         fH1 = new TGCompositeFrame(fV1, subgroup[0], subgroup[1], kHorizontalFrame);

         // Values to create a crop of the graph
         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *xCrop -> Set how many X parts of the surface scan to crop.\n");
         xCrop = new TSubStructure();
         for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
	 if(posUnitsPlot->widgetCB->GetSelected() == 0) { numform[0] = 8; numform[2] = 2; numform2[0] = 8; numform2[2] = 2; }
	 else if(posUnitsPlot->widgetCB->GetSelected() == 1) { numform[0] = 8; numform[1] = 2; numform[2] = 2; numform2[0] = 8; numform2[1] = 2; numform2[2] = 2; }
         if(xCrop->TGLabelDoubleNEntry(fH1, subgroup[0]/4-50, 30, "X axis crop values:", 0, numform, 0, numform2, "center"))
            fH1->AddFrame(xCrop->outsidebox, f1expandXpad);

         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *yCrop -> Set how many Y parts of the surface scan to crop.\n");
         yCrop = new TSubStructure();
         for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
	 if(posUnitsPlot->widgetCB->GetSelected() == 0) { numform[0] = 8; numform[2] = 2; numform2[0] = 8; numform2[2] = 2; }
	 else if(posUnitsPlot->widgetCB->GetSelected() == 1) { numform[0] = 8; numform[1] = 2; numform[2] = 2; numform2[0] = 8; numform2[1] = 2; numform2[2] = 2; }
         if(yCrop->TGLabelDoubleNEntry(fH1, subgroup[0]/4-50, 30, "Y axis crop values:", 0, numform, 0, numform2, "center"))
            fH1->AddFrame(yCrop->outsidebox, f1expandXpad);

         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *interpolSize -> Set the interpolation step size of the 2D graph.\n");
         interpolSize = new TSubStructure();
         for(int i = 0; i < 6; i++) { numform[i] = 0; numform2[i] = 0; }
	 if(posUnitsPlot->widgetCB->GetSelected() == 0) { numform[0] = 8; numform[2] = 2; numform2[0] = 8; numform2[2] = 2; }
	 else if(posUnitsPlot->widgetCB->GetSelected() == 1) { numform[0] = 8; numform[1] = 2; numform[2] = 2; numform2[0] = 8; numform2[1] = 2; numform2[2] = 2; }
         if(interpolSize->TGLabelDoubleNEntry(fH1, subgroup[0]/4+20, 30, "Interpol. step size:", 500, numform, 500, numform2, "center"))
            fH1->AddFrame(interpolSize->outsidebox, f1expandXpad);

         if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *updateCrop -> 2 buttons for either updating the crop or making an integral\n");
         updateCrop = new TSubStructure();
         const char *selnames[512] = {"Update","Integrate surface scan"};
         if(updateCrop->TGMultiButton(fH1, subgroup[0]/4-50, 30, 2, selnames, "center"))
            fH1->AddFrame(updateCrop->outsidebox, f1expandXpad);

         fV1->AddFrame(fH1, f1expandXpad);

         updateCrop->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "UpdateIntegrateSurface(=0)");
         updateCrop->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, "UpdateIntegrateSurface(=1)");

	 ToolTipSetSurfaceScan();
      }

      // Export and close buttons
      if(DBGSIG > 1) printf("TempAnalysisTab(): Creating TSubStructure *exportExitAnalysis -> 2 buttons for either exporting the plot or closing the tab\n");
      exportExitAnalysis = new TSubStructure();
      const char *selnames[512] = {"Export","Close"};
      if(exportExitAnalysis->TGMultiButton(fV1, subgroup[0]/2, 30, 2, selnames, "center"))
         fV1->AddFrame(exportExitAnalysis->outsidebox, f1expandXpad);

      // Actions for header editor
      char cTemp[512];
      exportExitAnalysis->widgetTB[0]->Connect("Clicked()", "TGAppMainFrame", this, "ExportTempAnalysisPlot()");
      sprintf(cTemp, "CloseTempAnalysisTab(=%d)", newTab*100+startTab);
      exportExitAnalysis->widgetTB[1]->Connect("Clicked()", "TGAppMainFrame", this, cTemp);

      fT1->AddFrame(fV1, f1expand2d);

      fMain->MapSubwindows();
      fMain->MapWindow();
      fMain->Layout();

      ToolTipSetTempAnalysis();

      // Set tab ID
      *tabid = newTab;

      if(DBGSIG > 1)
      {
         printf("TempAnalysisTab(): New tab objects (Temporary Analysis Header)\n");
         gObjectTable->Print();
      }
   }
   else
   {
      // Switch to new tab
      fTab->SetTab(*tabid);
   }
}

void TGAppMainFrame::CloseTempAnalysisTab(int tabval)
{
   int curtab = (int)TMath::Floor(tabval/100.);
   int oldtab = tabval - curtab*100;

   if(DBGSIG > 1) printf("CloseTempAnalysisTab(): New tab = %d, old tab = %d\n", curtab, oldtab);

   fTab->RemoveTab(curtab);

   delete tempAnalysisCanvas;
   delete exportExitAnalysis;

   for(int i = 0; i < fTab->GetNumberOfTabs(); i++)
      if(DBGSIG > 1) printf("CloseTempAnalysisTab(): Name of tab (%d) = %s\n", i, fTab->GetTabTab(i)->GetString() );

   fTab->SetTab(oldtab);
}

void TGAppMainFrame::ApplyRunningAver()
{
   TCanvas *gCanvas = tempAnalysisCanvas->GetCanvas();
   TList *funcList = (TList*)gCanvas->GetListOfPrimitives();
   unsigned int nrfunc = funcList->GetSize();
   TGraph *baseGr;
   char funcname[512];
   int runav = runningAver->widgetNE[0]->GetNumber();
   int offx = runningOff->widgetNE[0]->GetNumber();

   if(runav == 0)	// If running average is disabled, don't update the plot
      return;

   for(int i = 0; i < nrfunc; i++)
   {
      sprintf(funcname, "%s", funcList->At(i)->GetName());
      if(DBGSIG) printf("ApplyRunningAver(): Function is: %s\n", funcname);

      if(strcmp(funcname,"runaver") == 0)
      {
         gCanvas->GetPrimitive(funcname)->Delete();
         gCanvas->Modified();
         gCanvas->Update();
      }
      else if(strcmp(funcname,"pde") == 0)
      {
         baseGr = (TGraph*)gCanvas->GetPrimitive(funcname);
	 int nrpoints = baseGr->GetN();
	 TGraph *runaver = new TGraph((int)(nrpoints-2*offx)/runav);
         runaver->SetName("runaver");
         runaver->SetFillColor(1);
         runaver->SetLineColor(kBlack);
         runaver->SetLineWidth(2);
         runaver->SetMarkerColor(kBlack);
         int nr = 0, j = 0;
         double averx = 0, avery = 0;
         double *xval, *yval;
	 xval = new double[runav];
	 yval = new double[runav];
         while(1)
         {
            if((nr == (int)nrpoints/runav) || (runav*nr+j+offx > nrpoints-offx)) break;
            baseGr->GetPoint(runav*nr+j+offx,xval[j],yval[j]);
            if(DBGSIG) printf("ApplyRunningAver(): j = %d: X = %lf, Y = %lf\n", j, xval[j], yval[j]);
            averx += xval[j];
            avery += yval[j];
            j++;
            if((j == runav) && (runav*nr+j+offx <= nrpoints-offx))
            {
               runaver->SetPoint(nr,averx/runav,avery/runav);
               if(DBGSIG) printf("ApplyRunningAver(): \t%d: averX = %lf, averY = %lf\n", nr, averx/runav, avery/runav);
               nr++;
               averx = 0;
               avery = 0;
               j = 0;
            }
         }
         gCanvas->cd();
         runaver->Draw("l same");
         gCanvas->Modified();
         gCanvas->Update();
	 delete[] xval;
	 delete[] yval;
      }
   }
}

void TGAppMainFrame::UpdateIntegrateSurface(int val)
{
   TCanvas *gCanvas = tempAnalysisCanvas->GetCanvas();
   TList *funcList = (TList*)gCanvas->GetListOfPrimitives();
   unsigned int nrfunc = funcList->GetSize();
   TGraph2D *baseGr;
   char funcname[512];

   double integralValue;
   int pointCount;
   double xStepsize, yStepsize;
   int xInterpol, yInterpol;

   for(int i = 0; i < nrfunc; i++)
   {
      sprintf(funcname, "%s", funcList->At(i)->GetName());
      if(DBGSIG) printf("UpdateIntegrateSurface(): Function is: %s\n", funcname);

      if(strcmp(funcname,"surfscan") == 0)
      {
         baseGr = (TGraph2D*)gCanvas->GetPrimitive(funcname);

         // Just set the initial things for the surface scan
	 if(val == -1)
	 {
	    xCrop->widgetNE[0]->SetNumber(baseGr->GetXaxis()->GetXmin());
	    xCrop->widgetNE[1]->SetNumber(baseGr->GetXaxis()->GetXmax());
	    yCrop->widgetNE[0]->SetNumber(baseGr->GetYaxis()->GetXmin());
	    yCrop->widgetNE[1]->SetNumber(baseGr->GetYaxis()->GetXmax());

	    xCrop->widgetNE[0]->SetLimits(TGNumberFormat::kNELLimitMinMax, baseGr->GetXaxis()->GetXmin(), baseGr->GetXaxis()->GetXmax());
	    xCrop->widgetNE[1]->SetLimits(TGNumberFormat::kNELLimitMinMax, baseGr->GetXaxis()->GetXmin(), baseGr->GetXaxis()->GetXmax());
	    yCrop->widgetNE[0]->SetLimits(TGNumberFormat::kNELLimitMinMax, baseGr->GetYaxis()->GetXmin(), baseGr->GetYaxis()->GetXmax());
	    yCrop->widgetNE[1]->SetLimits(TGNumberFormat::kNELLimitMinMax, baseGr->GetYaxis()->GetXmin(), baseGr->GetYaxis()->GetXmax());
	 }
	 // Update the cropping
	 else if(val == 0)
	 {
	    baseGr->GetXaxis()->SetRange(xCrop->widgetNE[0]->GetNumber(), xCrop->widgetNE[1]->GetNumber());
	    baseGr->GetXaxis()->SetRangeUser(xCrop->widgetNE[0]->GetNumber(), xCrop->widgetNE[1]->GetNumber());
	    baseGr->GetYaxis()->SetRange(yCrop->widgetNE[0]->GetNumber(), yCrop->widgetNE[1]->GetNumber());
	    baseGr->GetYaxis()->SetRangeUser(yCrop->widgetNE[0]->GetNumber(), yCrop->widgetNE[1]->GetNumber());

	    gCanvas->Modified();
	    gCanvas->Update();
	 }
	 // Make an integral over all histogram values in range
	 else if(val == 1)
	 {
	    integralValue = 0;
	    pointCount = 0;

	    xStepsize = interpolSize->widgetNE[0]->GetNumber();
	    yStepsize = interpolSize->widgetNE[1]->GetNumber();
            xInterpol = (int)((xCrop->widgetNE[1]->GetNumber() - xCrop->widgetNE[0]->GetNumber())/xStepsize);
            yInterpol = (int)((yCrop->widgetNE[1]->GetNumber() - yCrop->widgetNE[0]->GetNumber())/yStepsize);

	    for(int j = 0; j < xInterpol; j++)
	    {
	       for(int k = 0; k < yInterpol; k++)
	       {
	          if((xCrop->widgetNE[0]->GetNumber()+j*xStepsize+xStepsize/2. <= xCrop->widgetNE[1]->GetNumber()) && (yCrop->widgetNE[0]->GetNumber()+k*yStepsize+yStepsize/2. <= yCrop->widgetNE[1]->GetNumber()))
		  {
  	             if(DBGSIG) cout << "Interpolation at (" << xCrop->widgetNE[0]->GetNumber()+j*xStepsize+xStepsize/2. << "," << yCrop->widgetNE[0]->GetNumber()+k*yStepsize+yStepsize/2. << ") is " << baseGr->Interpolate(xCrop->widgetNE[0]->GetNumber()+j*xStepsize+xStepsize/2., yCrop->widgetNE[0]->GetNumber()+k*yStepsize+yStepsize/2.) << endl;
		     integralValue += baseGr->Interpolate(xCrop->widgetNE[0]->GetNumber()+j*xStepsize+xStepsize/2., yCrop->widgetNE[0]->GetNumber()+k*yStepsize+yStepsize/2.);
		     pointCount++;
		  }
	       }
	    }

	    cout << "Total integral value (" << pointCount << " points) = " << integralValue << endl << "Total normated integral value = " << (double)integralValue/pointCount << endl;

	 }
      }
   }
}

void TGAppMainFrame::ExportTempAnalysisPlot()
{
   TCanvas *gCanvas = tempAnalysisCanvas->GetCanvas();

   TGFileInfo file_info;
   const char *filetypes[] = {"PDF","*.pdf","Encapsulated PostScript (.eps)","*.eps", "JPG/JPEG", "*.jpg", "PNG", "*.png",0,0};
   char *cTemp;
   file_info.fFileTypes = filetypes;
   cTemp = new char[1024];
//   sprintf(cTemp, "%s/results", rootdir);
//   file_info.fIniDir = StrDup(cTemp);
   file_info.fIniDir = StrDup(currentAnalDir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);
   delete[] cTemp;

   if(file_info.fFilename != NULL)
      gCanvas->SaveAs(file_info.fFilename);
}

// Temporary analysis window ------------------------------------------*/
