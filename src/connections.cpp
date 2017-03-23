#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"
#include "../include/adst_mva.h"
#include "../include/combine.h"

#include <stdio.h>
#include <stdlib.h>

int retTemp;

// Adst plotter controls ------------------------------------
void TGAppMainFrame::UpdateAdstFile(int opt)
{
   if(DBGSIG > 1)
   {
      printf("UpdateHistogram(): Clearing the TList\n");
      gDirectory->GetList()->Delete();
      gObjectTable->Print();
   }

   // Do not do normal histogram update if we have multiple files selected
   if( (opt == 0) && (adstMultiSelect->widgetChBox[0]->IsDown()) )
   {
      printf("UpdateHistogram(): To preview changes done to a histogram, please deselect the \"Multiple files select\" option.");
      return;
   }

   unsigned int nrfiles = adstList->GetNumberOfEntries();
   TCanvas *gCanvas;
   char exportname[512];
   char cTemp[512];
   
   if(opt == 1)
      gCanvas = adstCanvas->GetCanvas();
   
   if(nrfiles > 0)
   {
      TList *files;
      files = new TList();
      adstList->GetSelectedEntries(files);

      nrfiles = files->GetSize();
      
      if( (files) && (nrfiles > 0) )
      {
         for(int i = 0; i < (int)nrfiles; i++)
         {
            if(files->At(i))
            {
               if(DBGSIG)
                  printf("UpdateAdstFile(): Filename: %s\n", files->At(i)->GetTitle());
     	       DisplayAdstPlot( (char*)(files->At(i)->GetTitle()), opt);
            }
         }
      }
      else
         cout << "Error! At least 1 file must be selected." << endl;

      delete files;
   }
   else
      cout << "Error! At least 1 file must be listed." << endl;

   if(DBGSIG > 1)
   {
      printf("UpdateHistogram(): After drawing histograms (connections)\n");
      gObjectTable->Print();
   }
}

void TGAppMainFrame::GetAugerEvent()
{
   if(oldadstvalue != (double)adstSelectEvent->widgetNE[0]->GetNumber())
   {
      oldadstvalue = (double)adstSelectEvent->widgetNE[0]->GetNumber();
      UpdateAdstFile(1);
   }
}
// Adst plotter controls ------------------------------------

// Adst file selection controls -----------------------------

// File browser for opening histograms
void TGAppMainFrame::SelectDirectory()
{
   int i = adstList->GetNumberOfEntries();

   TGFileInfo file_info;
   const char *filetypes[] = {"ADST files",histextall,0,0};
   file_info.fFileTypes = filetypes;
   file_info.fIniDir = StrDup(currentViewDir);
   file_info.fMultipleSelection = kTRUE;
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);

   TList *files = file_info.fFileNamesList;
   if(files)
   {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while(file=(TSystemFile*)next())
      {
         fname = file->GetName();
	 if(CheckFormat(fname) > -1)
	 {
	    remove_from_last((char*)fname.Data(), '/', currentViewDir);
            adstList->AddEntry(fname.Data(), i);
	    i++;
	 }
      }
   }
   adstList->Layout();
}

// Toggle multiple selection in filelist or delete all entries
void TGAppMainFrame::ListMultiSelect(int opt)
{
   // Enable multiselect
   if(opt == 0)
   {
      adstList->SetMultipleSelections((adstMultiSelect->widgetChBox[0]->IsOn()));

      if(adstMultiSelect->widgetChBox[1]->IsDown())
         adstMultiSelect->widgetChBox[1]->SetState(kButtonUp);

      ToggleAdstPlotType(0);
      if(adstMultiSelect->widgetChBox[0]->IsDown())
         adstPlotSingle->widgetChBox[0]->SetState(kButtonDisabled);
      else
         adstPlotSingle->widgetChBox[0]->SetState(kButtonUp);
   }
   else if(opt == 1)
   {
      if(adstMultiSelect->widgetChBox[1]->IsDown())
      {
         adstMultiSelect->widgetChBox[0]->SetState(kButtonDown);
         adstList->SetMultipleSelections((adstMultiSelect->widgetChBox[0]->IsOn()));
         ToggleAdstPlotType(0);
         adstPlotSingle->widgetChBox[0]->SetState(kButtonDisabled);
         for(int i = 0; i < adstList->GetNumberOfEntries(); i++) 
            adstList->Select(i,kTRUE);
      }
      else if(!adstMultiSelect->widgetChBox[1]->IsDown())
      {
         adstMultiSelect->widgetChBox[0]->SetState(kButtonUp);
         adstList->SetMultipleSelections((adstMultiSelect->widgetChBox[0]->IsOn()));
         ToggleAdstPlotType(0);
         adstPlotSingle->widgetChBox[0]->SetState(kButtonUp);
         for(int i = 0; i < adstList->GetNumberOfEntries(); i++) 
            adstList->Select(i,kFALSE);
      }
   }
}

// Navigation buttons for the filelist (<<, >>) and double click
void TGAppMainFrame::FileListNavigation(int opt)
{
   unsigned int nrfiles = adstList->GetNumberOfEntries();
   int curSel;
   TList *files;
   if( nrfiles > 0 )
   {
      if(opt < -1)
      {
    	 // turn off multiple selection and select first file on list
         if(adstMultiSelect->widgetChBox[0]->IsOn())
         {
            adstList->SetMultipleSelections(kFALSE);
            adstMultiSelect->widgetChBox[0]->SetState(kButtonUp);
	    adstMultiSelect->widgetChBox[1]->SetState(kButtonUp);

            adstList->Select(0,kTRUE);
         }
         else
         {
            // if nothing is selected, curSel will be -1
            curSel = adstList->GetSelected();
            // go to next file on list
            if(opt == -3)
            {
               if( (curSel == (int)(nrfiles-1)) || (curSel == -1) )
   	          adstList->Select(0);
	       else
	          adstList->Select(curSel+1);
            }
            // go to previous file on list
            else if(opt == -2)
            {
               if( (curSel == 0) || (curSel == -1) )
   	          adstList->Select(nrfiles-1);
	       else
	          adstList->Select(curSel-1);
            }
         }
      }
   }

   UpdateAdstFile(0);
}

// Clear the histogram file selection list and dark run analysis selection
void TGAppMainFrame::ClearHistogramList()
{
   adstList->RemoveAll();
   adstMultiSelect->widgetChBox[0]->SetState(kButtonUp);
   adstMultiSelect->widgetChBox[1]->SetState(kButtonUp);
   adstList->SetMultipleSelections(kFALSE);
}

// Disable some options when plotting distribution or single
void TGAppMainFrame::ToggleAdstPlotType(int opt)
{
   // Setting distribution plot
   if(opt == 0)
   {
      adstPlotSingle->widgetChBox[0]->SetState(kButtonUp);
      adstPlotSingleSelect->widgetCB->SetEnabled(kFALSE);
      adstPlotDistSelect->widgetCB->SetEnabled(kTRUE);

      adstPlotDist->widgetChBox[0]->SetState(kButtonDown);

      adstSelectEvent->widgetNE[0]->SetState(kFALSE);
      adstSelectSdId->widgetCB->SetEnabled(kFALSE);
      adstSelectFdId->widgetCB->SetEnabled(kFALSE);
   }
   // Setting single plot
   else if(opt == 1)
   {
      adstPlotDist->widgetChBox[0]->SetState(kButtonUp);
      adstPlotDistSelect->widgetCB->SetEnabled(kFALSE);
      adstPlotSingleSelect->widgetCB->SetEnabled(kTRUE);

      adstPlotSingle->widgetChBox[0]->SetState(kButtonDown);

      adstSelectEvent->widgetNE[0]->SetState(kTRUE);
      adstSelectSdId->widgetCB->SetEnabled(kTRUE);
      adstSelectFdId->widgetCB->SetEnabled(kTRUE);
   }
}

// Replot the selected ADST info or reset plotting options to default
void TGAppMainFrame::AdstPlottingButton(int opt)
{
   // Produce a plot
   if(opt == 0)
   {
      cout << "This will produce a plot!" << endl;
      UpdateAdstFile(1);
   }
   // Reset plotting options to default
   else if(opt == 1)
   {
      adstPlotDist->widgetChBox[0]->SetState(kButtonDown);
      adstPlotDistSelect->widgetCB->Select(14);
      adstPlotSingle->widgetChBox[0]->SetState(kButtonUp);
      adstPlotSingleSelect->widgetCB->Select(12);
      adstSetZenith->widgetNE[0]->SetNumber(0.);
      adstSetZenith->widgetNE[1]->SetNumber(60.);
      adstSetZenith->widgetChBox[0]->SetState(kButtonUp);
      adstSetEnergy->widgetNE[0]->SetNumber(1.e+17);
      adstSetEnergy->widgetNE[1]->SetNumber(1.e+21);
      adstSetEnergy->widgetChBox[0]->SetState(kButtonUp);
      adstSetEnergyErr->widgetNE[0]->SetNumber(0.18);
      adstSetEnergyErr->widgetChBox[0]->SetState(kButtonUp);
      adstSetXmaxErr->widgetNE[0]->SetNumber(40.);
      adstSetXmaxErr->widgetChBox[0]->SetState(kButtonUp);
      adstSetShfoot->widgetNE[0]->SetNumber(10.);
      adstSetShfoot->widgetChBox[0]->SetState(kButtonUp);
      ToggleAdstPlotType(0);
   }
}

// Adst file selection controls -----------------------------

// Mva file selection controls -----------------------------

// File browser for opening histograms
void TGAppMainFrame::SelectMvaDir()
{
   int i = mvaList[0]->GetNumberOfEntries();
   int j = mvaList[1]->GetNumberOfEntries();

   TGFileInfo file_info;
   const char *filetypes[] = {"MVA files",histextall,0,0};
   file_info.fFileTypes = filetypes;
   file_info.fIniDir = StrDup(currentMvaDir);
   file_info.fMultipleSelection = kTRUE;
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);

   TList *files = file_info.fFileNamesList;
   if(files)
   {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while(file=(TSystemFile*)next())
      {
         fname = file->GetName();
	 if(CheckMvaFormat(fname) == 1)
	 {
	    remove_from_last((char*)fname.Data(), '/', currentMvaDir);
            mvaList[0]->AddEntry(fname.Data(), i);
	    i++;
	 }
	 else if(CheckMvaFormat(fname) == 2)
	 {
	    remove_from_last((char*)fname.Data(), '/', currentMvaDir);
            mvaList[1]->AddEntry(fname.Data(), j);
	    j++;
	 }
	 else if(CheckMvaFormat(fname) == 3)
	 {
	    remove_from_last((char*)fname.Data(), '/', currentMvaDir);
            mvaList[2]->AddEntry(fname.Data(), j);
	    j++;
	 }
      }
   }
   mvaList[0]->Layout();
   mvaList[1]->Layout();
   mvaList[2]->Layout();
}

// File browser for selecting the save file for rewritten ADST
void TGAppMainFrame::SelectRewrite(int opt)
{
   TGFileInfo file_info;
   const char *filetypes[] = {"Rewritten ADST",histextall,0,0};
   file_info.fFileTypes = filetypes;
   file_info.fIniDir = StrDup(currentRewriteDir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);

   if(file_info.fFilename != NULL)
   {
//      fileName->widgetTE->SetText(file_info.fFilename);
      remove_from_last(file_info.fFilename, '/', currentRewriteDir);
      StartRewrite(file_info.fFilename, opt);
   }
}

// Start rewriting the ADST file
void TGAppMainFrame::StartRewrite(char *outfile, int opt)
{
   unsigned int nrfiles = mvaList[0]->GetNumberOfEntries();
   char exportname[512];
   char cTemp[512];
   
   if(nrfiles > 0)
   {
      TList *files;
      files = new TList();
      mvaList[0]->GetSelectedEntries(files);

      nrfiles = files->GetSize();
      
      if( (files) && (nrfiles > 0) )
      {
         if( (opt == 1) && (nrfiles > 1) ) return;

         AdstMva *mvatool = new AdstMva();
	 mvatool->outname = string(outfile);

	 Observables *obssig[3];
	 Observables *obsall[3];

	 for (int i = 0; i < 3; i++)
	 {
	    obssig[i] = new Observables();
	    obsall[i] = new Observables();
	 }

         mvatool->outfile = TFile::Open((mvatool->outname).c_str(),"RECREATE");
// NEW
         mvatool->all_tree = new TTree("TreeA", "Background tree with all events, including signal events.");
/*         mvatool->all_tree[0] = new TTree("TreeA_mean", "Background tree with all events, including signal events (mean value).");
         mvatool->all_tree[1] = new TTree("TreeA_neg", "Background tree with all events, including signal events (mean value + negative error).");
         mvatool->all_tree[2] = new TTree("TreeA_pos", "Background tree with all events, including signal events (mean value + positive error).");
*/
         for(int i = 0; i < (int)nrfiles; i++)
         {
            if(files->At(i))
            {
               if(DBGSIG)
                  printf("StartRewrite(): Filename: %s\n", files->At(i)->GetTitle());
	       mvatool->inname = string(files->At(i)->GetTitle());
	       if(opt == 0)
	          mvatool->RewriteObservables(nrfiles, i, obssig, obsall, -1);
	       else if(opt == 1)
	          mvatool->RewriteObservables(nrfiles, i, obssig, obsall, sampleRewriting->widgetNE[0]->GetNumber());
               mvatool->PrepareOtherTrees(nrfiles, i);
//     	       DisplayAdstPlot( (char*)(files->At(i)->GetTitle()), opt);
            }
         }
// NEW
         mvatool->all_tree->Write();
/*	 mvatool->all_tree[0]->Write();
	 mvatool->all_tree[1]->Write();
	 mvatool->all_tree[2]->Write();*/

         // Close open files
	 mvatool->outfile->Close();

	 for (int i = 0; i < 3; i++)
	 {
	    delete obssig[i];
	    delete obsall[i];
	 }
	 delete mvatool;
      }
      else
         cout << "Error! At least 1 file must be selected." << endl;

      delete files;
   }
   else
      cout << "Error! At least 1 file must be listed." << endl;
}

// File browser for selecting the save file for combining rewritten ADST
void TGAppMainFrame::SelectCombine()
{
   TGFileInfo file_info;
   const char *filetypes[] = {"Combined rewritten ADST",histextall,0,0};
   file_info.fFileTypes = filetypes;
   file_info.fIniDir = StrDup(currentRewriteDir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);

   if(file_info.fFilename != NULL)
   {
//      fileName->widgetTE->SetText(file_info.fFilename);
      remove_from_last(file_info.fFilename, '/', currentRewriteDir);
      StartCombine(file_info.fFilename);
   }
}


// Start combining rewritten ADST files
void TGAppMainFrame::StartCombine(char *outfile)
{
   unsigned int nrfiles = mvaList[1]->GetNumberOfEntries();
   char exportname[512];
   char cTemp[512];
   int ret;
   
   if(nrfiles > 1)
   {
      TList *files;
      files = new TList();
      mvaList[1]->GetSelectedEntries(files);

      nrfiles = files->GetSize();
      
      if( (files) && (nrfiles > 1) )
      {
         hadd(nrfiles, files, outfile);

	 sprintf(cTemp, "Would you like to add the newly combined file %s to the MVA file list?", outfile);
	 new TGMsgBox(gClient->GetRoot(), fMain, "Replacing files", cTemp, kMBIconQuestion, kMBYes | kMBNo, &ret);

	 if(ret == kMBYes)
	 {
            mvaList[2]->AddEntry(outfile, mvaList[0]->GetNumberOfEntries());
	    cout << "Yes was clicked!" << endl;
	 }
	 else if(ret == kMBNo)
	    cout << "No was clicked!" << endl;
      }
      else
         cout << "Error! At least 2 files must be selected." << endl;

      delete files;
   }
   else
      cout << "Error! At least 2 files must be listed." << endl;
}

// File browser for selecting the save file for combining rewritten ADST
void TGAppMainFrame::SelectMerge()
{
   TGFileInfo file_info;
   const char *filetypes[] = {"Merged rewritten ADST",histextall,0,0};
   file_info.fFileTypes = filetypes;
   file_info.fIniDir = StrDup(currentRewriteDir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);

   if(file_info.fFilename != NULL)
   {
//      fileName->widgetTE->SetText(file_info.fFilename);
      remove_from_last(file_info.fFilename, '/', currentRewriteDir);
      StartMerge(file_info.fFilename);
   }
}


// Start combining rewritten ADST files
void TGAppMainFrame::StartMerge(char *outfile)
{
   unsigned int nrfiles = mvaList[1]->GetNumberOfEntries();
   char exportname[512];
   char cTemp[512];
   int ret;
   
   if(nrfiles > 0)
   {
      TList *files;
      files = new TList();
      mvaList[1]->GetSelectedEntries(files);

      nrfiles = files->GetSize();
      
      if( (files) && (nrfiles > 0) )
         hmerge(nrfiles, files, outfile);
      else
         cout << "Error! At least 1 file must be selected." << endl;

      delete files;
   }
   else
      cout << "Error! At least 1 file must be listed." << endl;
}

void TGAppMainFrame::UpdateSigTreeSelection(int selid)
{
   if(oldselect[0] != signalSelect->widgetCB->GetSelected())
   {
      outputMvaName = "empty";
      startMvaAnalysis->widgetTB[1]->SetState(kButtonDisabled);
   }

   oldselect[0] = signalSelect->widgetCB->GetSelected();
}

void TGAppMainFrame::UpdateBackTreeSelection(int selid)
{
   if(oldselect[1] != backgroundSelect->widgetCB->GetSelected())
   {
      outputMvaName = "empty";
      startMvaAnalysis->widgetTB[1]->SetState(kButtonDisabled);
   }

   oldselect[1] = backgroundSelect->widgetCB->GetSelected();
}

void TGAppMainFrame::UpdateEnergyBinSelect()
{
   char ctemp[1024];
   double erange;
   double emin = cutEnergy->widgetNE[0]->GetNumber();
   double emax = cutEnergy->widgetNE[1]->GetNumber();

   erange = (emax - emin)/((double)cutEnergyBins->widgetNE[0]->GetNumber());

   if(!ecutBins.empty())
      ecutBins.erase(ecutBins.begin(), ecutBins.end());

   cutEnergyBinSelect->widgetCB->RemoveAll();
   for(int i = 0; i <= (int)cutEnergyBins->widgetNE[0]->GetNumber(); i++)
   {
      ecutBins.push_back((double)pow(10,i*erange+emin));
      if(DBGSIG) cout << "Bin " << i << " = " << ecutBins[i] << endl;
   }

   for(int i = 0; i < (int)cutEnergyBins->widgetNE[0]->GetNumber(); i++)
   {
      if(DBGSIG) cout << "Bin " << i << " = " << ecutBins[i] << ", " << ecutBins[i+1] << " (" << TMath::Log10(ecutBins[i]) << ", " << TMath::Log10(ecutBins[i+1]) << ")" << endl;
      if( i < 9)
         sprintf(ctemp, "0%d (%.1le - %.1le)", i+1, ecutBins[i], ecutBins[i+1]);
      else
         sprintf(ctemp, "%d (%.1le - %.1le)", i+1, ecutBins[i], ecutBins[i+1]);
      cutEnergyBinSelect->widgetCB->NewEntry(ctemp);
   }

   cutEnergyBinSelect->widgetCB->Select(1);
}

void TGAppMainFrame::UpdateZenithBinSelect()
{
   char ctemp[1024];
   double arange;
   double asinmin = SinSquare(cutZenith->widgetNE[0]->GetNumber(), true);
   double asinmax = SinSquare(cutZenith->widgetNE[1]->GetNumber(), true);

   arange = (asinmax - asinmin)/((double)cutZenithBins->widgetNE[0]->GetNumber());

   if(!zcutBins.empty())
      zcutBins.erase(zcutBins.begin(), zcutBins.end());

   cutZenithBinSelect->widgetCB->RemoveAll();
   for(int i = 0; i <= (int)cutZenithBins->widgetNE[0]->GetNumber(); i++)
   {
      zcutBins.push_back((double)i*arange+asinmin);
      if(DBGSIG) cout << "Bin " << i << " = " << zcutBins[i] << endl;
   }

   for(int i = 0; i < (int)cutZenithBins->widgetNE[0]->GetNumber(); i++)
   {
      if(DBGSIG) cout << "Bin " << i << " = " << zcutBins[i] << ", " << zcutBins[i+1] << " (" << AsinSqrt(zcutBins[i],true) << ", " << AsinSqrt(zcutBins[i+1],true) << ")" << endl;
      if( i < 9)
         sprintf(ctemp, "0%d (%.1lf - %.1lf)", i+1, AsinSqrt(zcutBins[i],true), AsinSqrt(zcutBins[i+1],true));
      else
         sprintf(ctemp, "%d (%.1lf - %.1lf)", i+1, AsinSqrt(zcutBins[i],true), AsinSqrt(zcutBins[i+1],true));
      cutZenithBinSelect->widgetCB->NewEntry(ctemp);
   }

   cutZenithBinSelect->widgetCB->Select(1);
}

void TGAppMainFrame::CheckEnergyBin()
{
   // Open the file for MVA analysis input
   if(string(selectedMva->widgetTE->GetText()) == "")
   {
      cout << "Error! No MVA input file selected." << endl;
      return;
   }

   string stemp;
   char ctemp[1024];
   float ftemp;
   int evtcount = 0;
  
   TFile *ifile = TFile::Open(selectedMva->widgetTE->GetText(),"READ");

   int selectedBin = (int)cutEnergyBinSelect->widgetCB->GetSelected();
   if( (cutEnergyBins->widgetNE[0]->GetNumber() == 1) && (selectedBin == 0) )
      selectedBin = 1;
   cout << "Selected entry = " << selectedBin << endl;

   int nrkeys = ifile->GetNkeys();
   cout << "Number of keys = " << nrkeys << endl;
   
   for(int i = 1; i <= nrkeys; i++)
   {
      if(i < nrkeys)
         stemp = "TreeS" + IntToStr(i);
      else
         stemp = "TreeA";

      TTree *tempTree = (TTree*)ifile->Get(stemp.c_str());
      cout << "Number of total entries for " << stemp << " = " << tempTree->GetEntries() << endl;

      tempTree->SetBranchAddress("fdenergy", &ftemp);

/*      sprintf(ctemp, "(fdenergy>%lf)&&(fdenergy<=%lf)", ecutBins[selectedBin-1], ecutBins[selectedBin]);
//      cout << "Selection: " << ctemp << endl;
      cout << "Number of entries inside energy bin for " << stemp << " = " << tempTree->GetEntries(ctemp) << endl << endl;*/

      evtcount = 0;

      for(int j = 0; j < tempTree->GetEntries(); j++)
      {
         tempTree->GetEntry(j);

/*	 if(ftemp <= ecutBins[selectedBin-1])
	    cout << "Event " << j+1 << " is below energy limit (" << ecutBins[selectedBin-1] << "), with energy = " << ftemp << endl;
	 else if(ftemp > ecutBins[selectedBin])
	    cout << "Event " << j+1 << " is above energy limit (" << ecutBins[selectedBin] << "), with energy = " << ftemp << endl;*/
	 
	 if((ftemp > ecutBins[selectedBin-1]) && (ftemp <= ecutBins[selectedBin]))
	    evtcount++;
      }

      cout << "Number of entries inside energy bin for " << stemp << " = " << evtcount << " (" << evtcount - tempTree->GetEntries() << ")" << endl << endl;
   }
}

void TGAppMainFrame::CheckZenithBin()
{
   // Open the file for MVA analysis input
   if(string(selectedMva->widgetTE->GetText()) == "")
   {
      cout << "Error! No MVA input file selected." << endl;
      return;
   }

   string stemp;
   char ctemp[1024];
   float ftemp;
   int evtcount = 0;
  
   TFile *ifile = TFile::Open(selectedMva->widgetTE->GetText(),"READ");

   int selectedBin = (int)cutZenithBinSelect->widgetCB->GetSelected();
   if( (cutZenithBins->widgetNE[0]->GetNumber() == 1) && (selectedBin == 0) )
      selectedBin = 1;
   cout << "Selected entry = " << selectedBin << endl;

   int nrkeys = ifile->GetNkeys();
   cout << "Number of keys = " << nrkeys << endl;
   
   for(int i = 1; i <= nrkeys; i++)
   {
      if(i < nrkeys)
         stemp = "TreeS" + IntToStr(i);
      else
         stemp = "TreeA";

      TTree *tempTree = (TTree*)ifile->Get(stemp.c_str());
      cout << "Number of total entries for " << stemp << " = " << tempTree->GetEntries() << endl;

      tempTree->SetBranchAddress("zenith", &ftemp);

/*      sprintf(ctemp, "(zenith>%lf)&&(zenith<=%lf)", AsinSqrt(zcutBins[selectedBin-1],true), AsinSqrt(zcutBins[selectedBin],true));
      cout << "Selection: " << ctemp << endl;
      cout << "Number of entries inside zenith bin for " << stemp << " = " << tempTree->GetEntries(ctemp) << endl << endl;*/

      evtcount = 0;

      for(int j = 0; j < tempTree->GetEntries(); j++)
      {
         tempTree->GetEntry(j);

/*	 if(ftemp <= zcutBins[selectedBin-1])
	    cout << "Event " << j+1 << " is below zenith limit (" << zcutBins[selectedBin-1] << "), with zenith = " << ftemp << endl;
	 else if(ftemp > ecutBins[selectedBin])
	    cout << "Event " << j+1 << " is above zenith limit (" << zcutBins[selectedBin] << "), with zenith = " << ftemp << endl;*/
	 
	 if((ftemp > AsinSqrt(zcutBins[selectedBin-1],false)) && (ftemp <= AsinSqrt(zcutBins[selectedBin],false)))
	    evtcount++;
      }

      cout << "Number of entries inside zenith bin for " << stemp << " = " << evtcount << " (" << evtcount - tempTree->GetEntries() << ")" << endl << endl;
   }
}

void TGAppMainFrame::CheckBothBins()
{
   // Open the file for MVA analysis input
   if(string(selectedMva->widgetTE->GetText()) == "")
   {
      cout << "Error! No MVA input file selected." << endl;
      return;
   }
   
   if( (!cutEnergy->widgetChBox[0]->IsDown()) && (!cutZenith->widgetChBox[0]->IsDown()) )
   {
      cout << "Both binning options are disabled." << endl;
      return;
   }

   string stemp;
   char ctemp[1024];
   float ftemp[2];
   int evtcount[5] = {0,0,0,0,0};
   bool deleted[2] = {false,false};
  
   TFile *ifile = TFile::Open(selectedMva->widgetTE->GetText(),"READ");

   int selectedBin[2];
   selectedBin[0] = (int)cutEnergyBinSelect->widgetCB->GetSelected();
   selectedBin[1] = (int)cutZenithBinSelect->widgetCB->GetSelected();
   if( (cutEnergyBins->widgetNE[0]->GetNumber() == 1) && (selectedBin[0] == 0) )
      selectedBin[0] = 1;
   if( (cutZenithBins->widgetNE[0]->GetNumber() == 1) && (selectedBin[1] == 0) )
      selectedBin[1] = 1;
   cout << "Selected entry = " << selectedBin[0] << ", " << selectedBin[1] << endl;

   int nrkeys = ifile->GetNkeys();
   cout << "Number of keys = " << nrkeys << endl;

   for(int i = 1; i <= nrkeys; i++)
   {
      if(i < nrkeys)
         stemp = "TreeS" + IntToStr(i);
      else
         stemp = "TreeA";

      TTree *tempTree = (TTree*)ifile->Get(stemp.c_str());
      cout << "Number of total entries for " << stemp << " = " << tempTree->GetEntries() << endl;

      tempTree->SetBranchAddress("fdenergy", &ftemp[0]);
      tempTree->SetBranchAddress("zenith", &ftemp[1]);

      evtcount[0] = 0;
      evtcount[1] = 0;
      evtcount[2] = 0;
      evtcount[3] = 0;
      evtcount[4] = 0;

      for(int j = 0; j < tempTree->GetEntries(); j++)
      {
         tempTree->GetEntry(j);

	 deleted[0] = false;
	 deleted[1] = false;

	 if(cutEnergy->widgetChBox[0]->IsDown())
	 {
	    if(ftemp[0] <= ecutBins[selectedBin[0]-1])
	    {
	       deleted[0] = true;
	       evtcount[1]++;
	    }
	    else if(ftemp[0] > ecutBins[selectedBin[0]])
	    {
	       deleted[0] = true;
	       evtcount[2]++;
	    }
	 }

	 if(cutZenith->widgetChBox[0]->IsDown())
	 {
	    if(ftemp[1] <= AsinSqrt(zcutBins[selectedBin[1]-1],false))
	    {
	       deleted[1] = true;
	       evtcount[3]++;
	    }
	    else if(ftemp[1] > AsinSqrt(zcutBins[selectedBin[1]],false))
	    {
	       deleted[1] = true;
	       evtcount[4]++;
	    }
	 }

         if(!(deleted[0] | deleted[1]))
	    evtcount[0]++;

//	 cout << j << ": Deleted = " << (int)deleted[0] << "|" << (int)deleted[1] << "|" << (int)(deleted[0] | deleted[1]) << ", Counter = " << evtcount << ", Energy = " << ftemp[0] << ", Angle = " << ftemp[1]*180./TMath::Pi() << endl;
      }

      cout << "Cut results for " << stemp << ":" << endl;
      cout << "- Number of total entries = " << tempTree->GetEntries() << endl;
      cout << "- Number of selected entries = " << evtcount[0] << endl;
      cout << "- Total dropped entries = " << tempTree->GetEntries() - evtcount[0] << endl;
      if(cutEnergy->widgetChBox[0]->IsDown())
         cout << "- Number of entries ouside energy bin = " << evtcount[1] << ", " << evtcount[2] << endl;
      if(cutZenith->widgetChBox[0]->IsDown())
         cout << "- Number of entries ouside zenith bin = " << evtcount[3] << ", " << evtcount[4] << endl << endl;
   }
}

// Copy the currently selected MVA file for MVA analysis
void TGAppMainFrame::EnableMvaFile(int opt)
{
   unsigned int nrfiles = mvaList[2]->GetNumberOfEntries();
   string signalName;
   int cnt = 0;
   char ctemp[1024];
   char ctemp2[1024];
   
   if(nrfiles > 0)
   {
      TList *files;
      files = new TList();
      mvaList[2]->GetSelectedEntries(files);

      nrfiles = files->GetSize();
      
      if( (files) && (nrfiles > 0) )
      {
         for(int i = 0; i < (int)nrfiles; i++)
         {
            if(files->At(i))
            {
	       cnt = 0;

     	       selectedMva->widgetTE->SetText(files->At(i)->GetTitle());
	       TFile *ifile = TFile::Open(files->At(i)->GetTitle());

               signalSelect->widgetCB->RemoveAll();
               backgroundSelect->widgetCB->RemoveAll();

// NEW	       
               for(int j = 1; j <= (ifile->GetNkeys()-1); j++)
               {
                  signalName = "TreeS" + IntToStr(j);

                  remove_before_last((char*)ifile->GetKey(signalName.c_str())->GetTitle(), '/', ctemp2);
                  remove_from_last(ctemp2, '(', ctemp);
		  signalSelect->widgetCB->NewEntry(ctemp);
		  backgroundSelect->widgetCB->NewEntry(ctemp);
		  cnt++;
               }

	       if(oldselect[0] > cnt)
	       {
	          signalSelect->widgetCB->Select(1);
		  oldselect[0] = 1;
	       }
	       else
	          signalSelect->widgetCB->Select(oldselect[0]);

	       if(oldselect[1] > cnt)
	       {
	          backgroundSelect->widgetCB->Select(2);
		  oldselect[1] = 2;
	       }
	       else
	          backgroundSelect->widgetCB->Select(oldselect[1]);

	       ifile->Close();
            }
         }

	 outputMvaName = "empty";
         startMvaAnalysis->widgetTB[1]->SetState(kButtonDisabled);
      }
      else
         cout << "Error! At least 1 file must be selected." << endl;

      delete files;
   }
   else
      cout << "Error! At least 1 file must be listed." << endl;
}

void TGAppMainFrame::ScrollObservables(int pos)
{
   for(int i = 0; i < 3; i++)
   {
      if( (oldpos[i] != pos) && (selectObservables[i]->GetVScrollbar()->GetPosition() != pos) )
      {
//         cout << "Moved " << i << " to position = " << pos << " (" << oldpos[i] << ")" << endl;
         selectObservables[i]->GetVScrollbar()->SetPosition(pos);
	 oldpos[i] = pos;
      }
   }
}

void TGAppMainFrame::PickObservables(int pos)
{
//   cout << "Selected value = " << pos << " (old selection = " << observablesCheck[pos] << ")" << endl;

//   cout << "Old observable values:" << endl;
//   for(int i = 0; i < observablesCheck.size(); i++)
//      cout << "Position " << i << " = " << observablesCheck[i] << endl;

   if(observablesCheck[pos] == 0)
   {
      for(int i = 0; i < 3; i++)
      {
         if(selectObservables[i]->GetEntry(pos)->IsActive())
            observablesCheck[pos] = i+1;
      }
   }
   else
   {
      if( (!selectObservables[0]->GetEntry(pos)->IsActive()) && (!selectObservables[1]->GetEntry(pos)->IsActive()) && (!selectObservables[2]->GetEntry(pos)->IsActive()) )
         observablesCheck[pos] = 0;
      else
      {
         if(observablesCheck[pos] == 1)
         {
            for(int i = 0; i < 3; i++)
	    {
               if( (i != 0) && (selectObservables[i]->GetEntry(pos)->IsActive()) )
	       {
		  selectObservables[0]->Select(pos,kFALSE);
		  observablesCheck[pos] = i+1;
		  break;
	       }
	    }
	 }
         else if(observablesCheck[pos] == 2)
         {
            for(int i = 0; i < 3; i++)
	    {
               if( (i != 1) && (selectObservables[i]->GetEntry(pos)->IsActive()) )
	       {
		  selectObservables[1]->Select(pos,kFALSE);
		  observablesCheck[pos] = i+1;
		  break;
	       }
	    }
	 }
         else if(observablesCheck[pos] == 3)
         {
            for(int i = 0; i < 3; i++)
	    {
               if( (i != 2) && (selectObservables[i]->GetEntry(pos)->IsActive()) )
	       {
		  selectObservables[2]->Select(pos,kFALSE);
		  observablesCheck[pos] = i+1;
		  break;
	       }
	    }
	 }
      }
   }

   outputMvaName = "empty";
   startMvaAnalysis->widgetTB[1]->SetState(kButtonDisabled);

//   cout << "New observable values:" << endl;
//   for(int i = 0; i < observablesCheck.size(); i++)
//      cout << "Position " << i << " = " << observablesCheck[i] << endl;
}

// Start the MVA analysis
void TGAppMainFrame::StartMvaAnalysis(int opt)
{
   char ctemp[1024];
   string stemp, stemp2;
   int ret = -1;

   // Start MVA analysis
   if(opt == 0)
   {
      TGFileInfo file_info;
      const char *filetypes[] = {"MVA save file",histextall,0,0};
      file_info.fFileTypes = filetypes;
      file_info.fIniDir = StrDup(currentAnalysisDir);
      new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);

      if(file_info.fFilename != NULL)
      {
//         type = selectMvaType->widgetCB->GetSelected();
         outputMvaName = string(file_info.fFilename);

         remove_from_last(file_info.fFilename, '/', currentAnalysisDir);

	 // Open the file for MVA analysis input
	 if(string(selectedMva->widgetTE->GetText()) == "")
	 {
	    cout << "Error! No MVA input file selected." << endl;
            return;
	 }

 	 TFile *ifile = TFile::Open(selectedMva->widgetTE->GetText(),"READ");

         // Open the file to write out to
	 TFile *ofile = TFile::Open(file_info.fFilename,"RECREATE");
	 // Prepare the MVA Factory
         // Factory usage:
         // - user-defined job name, reappearing in names of weight files for training results ("TMVAClassification")
         // - pointer to an output file (ofile)
         // - options
         // Factory has the following options:
         //        V = verbose
         //        Silent = batch mode
         //        Color = colored screen output
         //        DrawProgressBar = progress bar display during training and testing
         //        Transformations = the transformations to make (identity, decorrelation, PCA, uniform, gaussian, gaussian decorrelation)
         //        AnalysisType = setting the analysis type (Classification, Regression, Multiclass, Auto)
         // Default values = !V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Auto
         TMVA::Factory *factory = new TMVA::Factory("TMVAClassification",ofile,"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification");

	 (TMVA::gConfig().GetIONames()).fWeightFileDir = string(currentAnalysisDir) + "/weights";
	 cout << "Weights directory after = " << (TMVA::gConfig().GetIONames()).fWeightFileDir << endl;

	 // Add the observables to the MVA factory
	 for(int i = 0; i < nrobs; i++)
	 {
            if(observablesCheck[i] > 0)
	    {
	       if(observablesCheck[i] == 1)		// mean
	       {
	          factory->AddVariable(observables[i], 'F');
	          cout << "Adding variable: " << observables[i] << " (" << observablesCheck[i] << ")" << endl;
	       }
	       else if(observablesCheck[i] == 2)	// mean + neg error
	       {
	          factory->AddVariable((observables[i] + "_neg"), 'F');
	          cout << "Adding variable: " << observables[i]+"_neg" << " (" << observablesCheck[i] << ")" << endl;
	       }
	       else if(observablesCheck[i] == 3)	// mean + pos error
	       {
	          factory->AddVariable((observables[i] + "_pos"), 'F');
	          cout << "Adding variable: " << observables[i]+"_pos" << " (" << observablesCheck[i] << ")" << endl;
	       }
//	       factory->AddVariable(observables[i], 'F');
//	       cout << "Adding variable: " << observables[i] << " (" << observablesCheck[i] << ")" << endl;
	    }
	 }

         int selectedBin[2];
         selectedBin[0] = (int)cutEnergyBinSelect->widgetCB->GetSelected();
         selectedBin[1] = (int)cutZenithBinSelect->widgetCB->GetSelected();

         TTree *signalTree;
         for(int j = 1; j <=(ifile->GetNkeys()-1); j++)
         {
            stemp = "TreeS" + IntToStr(j);
            remove_before_last((char*)ifile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
            if( string(signalSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
            {
	       if( !cutEnergy->widgetChBox[0]->IsDown() && !cutZenith->widgetChBox[0]->IsDown() )
	       {
	          cout << "Both energy and zenith cuts are disabled (signal)." << endl;
	          signalTree = (TTree*)ifile->Get(stemp.c_str());
	       }
	       else if( cutEnergy->widgetChBox[0]->IsDown() && cutZenith->widgetChBox[0]->IsDown() )
	       {
	          cout << "Both energy and zenith cuts are enabled (signal): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << "), Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	          stemp2 = "((zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + "))&&((fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + "))";
	          cout << "Cut setting: " << stemp2 << endl;
	          signalTree = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
               }
	       else if(cutEnergy->widgetChBox[0]->IsDown())
	       {
	          cout << "Only energy cuts are enabled (signal): Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	          stemp2 = "(fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + ")";
	          cout << "Cut setting: " << stemp2 << endl;
	          signalTree = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	       }
	       else if(cutZenith->widgetChBox[0]->IsDown())
	       {
	          cout << "Only zenith cuts are enabled (signal): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << ")." << endl;
	          stemp2 = "(zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + ")";
	          cout << "Cut setting: " << stemp2 << endl;
	          signalTree = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	       }
	       else
	          return;

	       break;
	    }
	 }

	 nrTreeEvents[0] = signalTree->GetEntries();
	 cout << "Number of entries in signal tree = " << nrTreeEvents[0] << endl;

         TTree *backgroundTree;
         for(int j = 1; j <=(ifile->GetNkeys()-1); j++)
         {
            stemp = "TreeS" + IntToStr(j);
            remove_before_last((char*)ifile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
            if( string(backgroundSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
            {
	       if( !cutEnergy->widgetChBox[0]->IsDown() && !cutZenith->widgetChBox[0]->IsDown() )
	       {
	          cout << "Both energy and zenith cuts are disabled (background)." << endl;
	          backgroundTree = (TTree*)ifile->Get(stemp.c_str());
	       }
	       else if( cutEnergy->widgetChBox[0]->IsDown() && cutZenith->widgetChBox[0]->IsDown() )
	       {
	          cout << "Both energy and zenith cuts are enabled (background): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << "), Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	          stemp2 = "((zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + "))&&((fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + "))";
	          cout << "Cut setting: " << stemp2 << endl;
	          backgroundTree = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
               }
	       else if(cutEnergy->widgetChBox[0]->IsDown())
	       {
	          cout << "Only energy cuts are enabled (background): Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	          stemp2 = "(fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + ")";
	          cout << "Cut setting: " << stemp2 << endl;
	          backgroundTree = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	       }
	       else if(cutZenith->widgetChBox[0]->IsDown())
	       {
	          cout << "Only zenith cuts are enabled (background): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << ")." << endl;
	          stemp2 = "(zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + ")";
	          cout << "Cut setting: " << stemp2 << endl;
	          backgroundTree = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	       }
	       else
	          return;

	       break;
	    }
	 }

	 nrTreeEvents[1] = backgroundTree->GetEntries();
	 cout << "Number of entries in background tree = " << nrTreeEvents[1] << endl;

/*         if(type == 0)
	    stemp = GetTree(0, "mean", ifile);
         else if(type == 1)
	    stemp = GetTree(0, "neg", ifile);
         else if(type == 2)
	    stemp = GetTree(0, "pos", ifile);
	 else
	    stemp == NULL;

	 if( stemp == NULL )
	 {
	    cout << "No signal trees found for MVA analysis." << endl;
            ifile->Close();
            delete factory;
            ofile->Close();
	    return;
	 }
	 TTree *signalTree = (TTree*)ifile->Get(stemp.c_str());
	 nrTreeEvents[0] = signalTree->GetEntries();

         if(type == 0)
	    stemp = GetTree(1, "mean", ifile);
         else if(type == 1)
	    stemp = GetTree(1, "neg", ifile);
         else if(type == 2)
	    stemp = GetTree(1, "pos", ifile);
	 else
	    stemp == NULL;

	 if( stemp == NULL )
	 {
	    cout << "No background trees found for MVA analysis." << endl;
            ifile->Close();
            delete factory;
            ofile->Close();
	    return;
	 }
	 TTree *backgroundTree = (TTree*)ifile->Get(stemp.c_str());
	 nrTreeEvents[1] = backgroundTree->GetEntries();*/
         factory->AddSignalTree(signalTree, 1.0);
         factory->AddBackgroundTree(backgroundTree, 1.0);

         // Preparing and training from the trees:
         // - preselection cuts make cuts on input variables, before even starting the MVA
         // - options
         // These are the possible options:
         //        nTrain_Signal = number of training events of class Signal (0 takes all)
         //        nTrain_Background = number of training events of class Background (0 takes all)
         //        nTest_Signal = number of test events of class Signal (0 takes all)
         //        nTest_Background = number of test events of class Background (0 takes all)
         //        SplitMode = method of choosing training and testing events (Random, Alternate, Block)
         //        NormMode = renormalisation of event-by-event weights for training (NumEvents: average weight of 1 per event for signal and background, EqualNumEvents: average weight of 1 per event for signal and sum of weights for background equal to sum of weights for signal, None)
         //        V = verbose
         //        MixMode = method of mixing events of different classes into one dataset (SameAsSplitMode, Random, Alternate, Block)
         //        SplitSeed = seed for random event shuffling (default = 100)
         //        VerboseLevel = level of verbose (Debug, Verbose, Info)
         factory->PrepareTrainingAndTestTree("", "", "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

         // Booking MVA methods:
         // - type of MVA method to be used (all defined in src/Types.h)
         // - the unique name for the MVA method suplied by the user
         // - options
         // The possible options for each method are defined here: http://tmva.sourceforge.net/optionRef.html
         // For example:
         //        H = print method-specific help message
         //        V = verbose
         //        NeuronType = neuron activation function type (default = sigmoid)
         //        VarTransform = list of variable transformations to do before training (D_Background,P_Signal,G,N_AllClasses -> N = Normalization for all classes)
         //        NCycles = number of training cycles
         //        HiddenLayers = hidden layer architecture (default = N,N-1)
         //        TestRate = test for overtraining at each #th epoch (default = 10)
         //        TrainingMethod = train with back propagation (BP), BFGS algorithm (BFGS) or generic algorithm (GA)
         //        UseRegulator = use regulator to avoid overtraining
         BookTheMethod(factory);

         // Train the selected methods and save them to the weights folder
         factory->TrainAllMethods();
         // Test the selected methods by applying the trained data to the test data set -> outputs saved to TestTree output file and then to the output ROOT file
         factory->TestAllMethods();
         // Evaluation of methods printed to stdout
         factory->EvaluateAllMethods();

         // Close the open files
         ifile->Close();
         delete factory;
         ofile->Close();

	 startMvaAnalysis->widgetTB[1]->SetState(kButtonUp);

         sigBackLabel->SetText(("MVA analysis results for all trees (" + IntToStr(nrTreeEvents[0]) + " signal vs. " + IntToStr(nrTreeEvents[1]) + " background):                                              ").c_str());
         sigBackLabel->SetWrapLength(-1);

	 // Open the MVA GUI to review the training and testing procedure
         if(specialMva->widgetChBox[0]->IsDown())
	 {
	    sprintf(ctemp, "Running analysis. Currently the cut is set to: %.2lf\nMake sure that the MVA cut is correct. Check for correct cuts by plotting \"(5a) Classifier Cut Efficiency\" at S/B values of: [%d/%d].\n", cutMva->widgetNE[0]->GetNumber(), nrTreeEvents[0], nrTreeEvents[1]);
	    new TGMsgBox(gClient->GetRoot(), fMain, "Setting MVA cuts", ctemp, kMBIconExclamation, kMBOk, &ret);

	    sprintf(ctemp, "%s/bin/tmvagui %s", rootdir, file_info.fFilename);
            system(ctemp);

	    ret = 1;
	    cutDialog = new TGDialogFrame(gClient->GetRoot(), fMain, 400, 200, cutMva, &ret);

/*            if( ret == kMBOk )
	    {
	       specialMva->widgetChBox[0]->SetState(kButtonUp);
	       return;
	    }
	    else if(ret == kMBCancel)
	       return;*/

	    if(ret != 0)
	       return;
	 }

	 ret = 0;
	 for(int i = 0; i < nrmethods; i++)
	 {
	    if(methodsSelect->widgetChBox[i]->IsDown())
	    {
	       applymva = methods[i];
cout << "Selected MVA method: " << applymva << endl;
	       ret++;
	    }
	 }

	 if(ret > 1)
	 {
	    cout << "More than 1 MVA method selected. Please only select one method for applying the MVA cuts and rerun the analysis." << endl;
	    return;
	 }

	 // Apply MVA cuts on all signal trees of the input file
         // Open a reader and add variables (must be the same as for the training)
         TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

         float obsvars[3*observables.size()];

	 // Add the observables to the MVA reader
	 for(int i = 0; i < nrobs; i++)
	 {
            if(observablesCheck[i] > 0)
	    {
	       if(observablesCheck[i] == 1)		// mean
	       {
cout << "Selected observable: " << observables[i] << endl;
	          reader->AddVariable(observables[i], &obsvars[i]);
	       }
	       else if(observablesCheck[i] == 2)	// mean + neg error
	       {
cout << "Selected observable: " << observables[i]+"_neg" << endl;
	          reader->AddVariable((observables[i] + "_neg"), &obsvars[i]);
	       }
	       else if(observablesCheck[i] == 3)	// mean + pos error
	       {
cout << "Selected observable: " << observables[i]+"_pos" << endl;
	          reader->AddVariable((observables[i] + "_pos"), &obsvars[i]);
	       }
	    }
	 }

         // Book the MVA with the produced weights file
         stemp = string(currentAnalysisDir) + "/weights/TMVAClassification_" + applymva + ".weights.xml";
cout << "Weights file: " << stemp << endl;
         string mvamethod = (string)(applymva + " method");
cout << "MVA method: " << mvamethod << endl;
         reader->BookMVA(mvamethod, stemp);

         // Open the input file and prepare the TTree
	 ifile = TFile::Open(selectedMva->widgetTE->GetText(),"READ");

         for(int j = 1; j <= (ifile->GetNkeys()-1); j++)
         {
            stemp = "TreeS" + IntToStr(j);
            cout << endl << stemp << " has been selected for evaluation." << endl;

	    TTree *signalapp;

	    if( !cutEnergy->widgetChBox[0]->IsDown() && !cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are disabled (application)." << endl;
	       signalapp = (TTree*)ifile->Get(stemp.c_str());
	    }
	    else if( cutEnergy->widgetChBox[0]->IsDown() && cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are enabled (application): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << "), Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "((zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + "))&&((fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + "))";
	       cout << "Cut setting: " << stemp2 << endl;
	       signalapp = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
            }
	    else if(cutEnergy->widgetChBox[0]->IsDown())
	    {
	       cout << "Only energy cuts are enabled (application): Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "(fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       signalapp = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	    }
	    else if(cutZenith->widgetChBox[0]->IsDown())
	    {
	       cout << "Only zenith cuts are enabled (application): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << ")." << endl;
	       stemp2 = "(zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       signalapp = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	    }
	    else
	       return;

	    for(int i = 0; i < nrobs; i++)
	    {
	       if(observablesCheck[i] == 1)		// mean
	          signalapp->SetBranchAddress((observables[i]).c_str(), &obsvars[i]);
	       else if(observablesCheck[i] == 2)	// mean + neg error
	          signalapp->SetBranchAddress((observables[i] + "_neg").c_str(), &obsvars[i]);
	       else if(observablesCheck[i] == 3)	// mean + pos error
	          signalapp->SetBranchAddress((observables[i] + "_pos").c_str(), &obsvars[i]);
	    }
            
            CreateMVAPlots(signalapp, reader, mvamethod, obsvars, stemp, j);
         }

	 for(int j = ifile->GetNkeys(); j < 6; j++)
            displayResult[j]->widgetTE->SetState(kFALSE);

         ifile->Close();
         delete reader;

         // TODO: Check for correlation of variables and the line profile (to determine the errors)
      }
   }
   // Apply the MVA cut
   else if(opt == 1)
   {
      if(outputMvaName != "empty")
      {
	 // Open the MVA GUI to review the training and testing procedure
         if(specialMva->widgetChBox[0]->IsDown())
	 {
	    sprintf(ctemp, "%s/bin/tmvagui %s", rootdir, outputMvaName.c_str());
            system(ctemp);

	    ret = 1;
	    cutDialog = new TGDialogFrame(gClient->GetRoot(), fMain, 400, 200, cutMva, &ret);

/*            if( ret == kMBOk )
	    {
	       specialMva->widgetChBox[0]->SetState(kButtonUp);
	       return;
	    }
	    else if(ret == kMBCancel)
	       return;*/

	    if(ret != 0)
	       return;
	 }

cout << "Selected MVA method: " << applymva << endl;
	 // Apply MVA cuts on all signal trees of the input file
         // Open a reader and add variables (must be the same as for the training)
         TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

         float obsvars[3*observables.size()];

	 // Add the observables to the MVA reader
	 for(int i = 0; i < nrobs; i++)
	 {
            if(observablesCheck[i] > 0)
	    {
	       if(observablesCheck[i] == 1)		// mean
	       {
cout << "Selected observable: " << observables[i] << endl;
	          reader->AddVariable(observables[i], &obsvars[i]);
	       }
	       else if(observablesCheck[i] == 2)	// mean + neg error
	       {
cout << "Selected observable: " << observables[i]+"_neg" << endl;
	          reader->AddVariable((observables[i] + "_neg"), &obsvars[i]);
	       }
	       else if(observablesCheck[i] == 3)	// mean + pos error
	       {
cout << "Selected observable: " << observables[i]+"_pos" << endl;
	          reader->AddVariable((observables[i] + "_pos"), &obsvars[i]);
	       }
	    }
	 }

         int selectedBin[2];
         selectedBin[0] = (int)cutEnergyBinSelect->widgetCB->GetSelected();
         selectedBin[1] = (int)cutZenithBinSelect->widgetCB->GetSelected();

         // Book the MVA with the produced weights file
         stemp = string(currentAnalysisDir) + "/weights/TMVAClassification_" + applymva + ".weights.xml";
cout << "Weights file: " << stemp << endl;
         string mvamethod = (string)(applymva + " method");
cout << "MVA method: " << mvamethod << endl;
         reader->BookMVA(mvamethod, stemp);

         // Open the input file and prepare the TTree
 	 TFile *ifile = TFile::Open(selectedMva->widgetTE->GetText(),"READ");

         for(int j = 1; j <= (ifile->GetNkeys()-1); j++)
         {
            stemp = "TreeS" + IntToStr(j);
            cout << endl << stemp << " has been selected for evaluation." << endl;

	    TTree *signalapp;

	    if( !cutEnergy->widgetChBox[0]->IsDown() && !cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are disabled (application)." << endl;
	       signalapp = (TTree*)ifile->Get(stemp.c_str());
	    }
	    else if( cutEnergy->widgetChBox[0]->IsDown() && cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are enabled (application): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << "), Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "((zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + "))&&((fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + "))";
	       cout << "Cut setting: " << stemp2 << endl;
	       signalapp = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
            }
	    else if(cutEnergy->widgetChBox[0]->IsDown())
	    {
	       cout << "Only energy cuts are enabled (application): Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "(fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       signalapp = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	    }
	    else if(cutZenith->widgetChBox[0]->IsDown())
	    {
	       cout << "Only zenith cuts are enabled (application): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << ")." << endl;
	       stemp2 = "(zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       signalapp = ((TTree*)ifile->Get(stemp.c_str()))->CopyTree(stemp2.c_str());
	    }
	    else
	       return;

	    for(int i = 0; i < nrobs; i++)
	    {
	       if(observablesCheck[i] == 1)		// mean
	          signalapp->SetBranchAddress((observables[i]).c_str(), &obsvars[i]);
	       else if(observablesCheck[i] == 2)	// mean + neg error
	          signalapp->SetBranchAddress((observables[i] + "_neg").c_str(), &obsvars[i]);
	       else if(observablesCheck[i] == 3)	// mean + pos error
	          signalapp->SetBranchAddress((observables[i] + "_pos").c_str(), &obsvars[i]);
	    }
            
            CreateMVAPlots(signalapp, reader, mvamethod, obsvars, stemp, j);
         }

	 for(int j = ifile->GetNkeys(); j < 6; j++)
            displayResult[j]->widgetTE->SetState(kFALSE);

         ifile->Close();
         delete reader;
      }
   }
   // Check both bins if they are enabled (zenith angle and energy)
   else if(opt == 2)
   {
      CheckBothBins();
   }
   // Reset MVA analysis defaults
   else if(opt == 3)
   {
      observablesCheck.erase(observablesCheck.begin(), observablesCheck.end());
      observablesCheck.push_back(1);
      observablesCheck.push_back(0);
      observablesCheck.push_back(0);
      observablesCheck.push_back(0);
      observablesCheck.push_back(1);
      observablesCheck.push_back(1);
      observablesCheck.push_back(0);
      observablesCheck.push_back(0);
      observablesCheck.push_back(0);
      observablesCheck.push_back(1);
      observablesCheck.push_back(0);

      for(int i = 0; i < nrobs; i++)
      {
         if(observablesCheck[i] == 0)
	 {
            selectObservables[0]->Select(i, kFALSE);
            selectObservables[1]->Select(i, kFALSE);
            selectObservables[2]->Select(i, kFALSE);
	 }
	 else if(observablesCheck[i] == 1)
	 {
            selectObservables[0]->Select(i, kTRUE);
            selectObservables[1]->Select(i, kFALSE);
            selectObservables[2]->Select(i, kFALSE);
	 }
	 else if(observablesCheck[i] == 2)
	 {
            selectObservables[0]->Select(i, kFALSE);
            selectObservables[1]->Select(i, kTRUE);
            selectObservables[2]->Select(i, kFALSE);
	 }
	 else if(observablesCheck[i] == 3)
	 {
            selectObservables[0]->Select(i, kFALSE);
            selectObservables[1]->Select(i, kFALSE);
            selectObservables[2]->Select(i, kTRUE);
	 }
      }

      methodsSelect->widgetChBox[0]->SetState(kButtonUp);
      methodsSelect->widgetChBox[1]->SetState(kButtonUp);
      methodsSelect->widgetChBox[2]->SetState(kButtonUp);
      methodsSelect->widgetChBox[3]->SetState(kButtonUp);
      methodsSelect->widgetChBox[4]->SetState(kButtonUp);
      methodsSelect->widgetChBox[5]->SetState(kButtonUp);
      methodsSelect->widgetChBox[6]->SetState(kButtonUp);
      methodsSelect->widgetChBox[7]->SetState(kButtonUp);
      methodsSelect->widgetChBox[8]->SetState(kButtonDown);
      methodsSelect->widgetChBox[9]->SetState(kButtonUp);
      methodsSelect->widgetChBox[10]->SetState(kButtonUp);
      methodsSelect->widgetChBox[11]->SetState(kButtonUp);

      specialMva->widgetChBox[0]->SetState(kButtonDown);

      cutEnergy->widgetChBox[0]->SetState(kButtonUp);
      cutEnergy->widgetNE[0]->SetNumber(pow(10,17.));
      cutEnergy->widgetNE[1]->SetNumber(pow(10,21.));
      cutEnergyBins->widgetNE[0]->SetNumber(1);
      UpdateEnergyBinSelect();

      cutZenith->widgetChBox[0]->SetState(kButtonUp);
      cutZenith->widgetNE[0]->SetNumber(0.);
      cutZenith->widgetNE[1]->SetNumber(60.);
      cutZenithBins->widgetNE[0]->SetNumber(1);
      UpdateZenithBinSelect();

      plotSetXmax->widgetNE[0]->SetNumber(400. );
      plotSetXmax->widgetNE[1]->SetNumber(1200.);
      plotSetX0->widgetNE[0]->SetNumber(-550.);
      plotSetX0->widgetNE[1]->SetNumber(450. );
      plotSetLambda->widgetNE[0]->SetNumber(35. );
      plotSetLambda->widgetNE[1]->SetNumber(135.);
      plotSetFdenergy->widgetNE[0]->SetNumber(17.);
      plotSetFdenergy->widgetNE[1]->SetNumber(20.);
      plotSetShfoot->widgetNE[0]->SetNumber(300.);
      plotSetShfoot->widgetNE[1]->SetNumber(900.);
      plotSetShwsize->widgetNE[0]->SetNumber(0.  );
      plotSetShwsize->widgetNE[1]->SetNumber(170.);
      plotSetNrmu->widgetNE[0]->SetNumber(0.    );
      plotSetNrmu->widgetNE[1]->SetNumber(90.e+6);
      plotSetCurvature->widgetNE[0]->SetNumber(1.e-4  );
      plotSetCurvature->widgetNE[1]->SetNumber(0.25e-3);
      plotSetRisetime->widgetNE[0]->SetNumber(0.  );
      plotSetRisetime->widgetNE[1]->SetNumber(650.);
      plotSetMva->widgetNE[0]->SetNumber(-2.);
      plotSetMva->widgetNE[1]->SetNumber(2. );
   }
}

/*string TGAppMainFrame::GetTree(int sigback, TFile *infile, TTree *outTree)
{
   string stemp, stemp2;
   char ctemp[1024];

   int selectedBin[2];
   selectedBin[0] = (int)cutEnergyBinSelect->widgetCB->GetSelected();
   selectedBin[1] = (int)cutZenithBinSelect->widgetCB->GetSelected();
   if( (cutEnergyBins->widgetNE[0]->GetNumber() == 1) && (selectedBin[0] == 0) )
      selectedBin[0] = 1;
   if( (cutZenithBins->widgetNE[0]->GetNumber() == 1) && (selectedBin[1] == 0) )
      selectedBin[1] = 1;

   // Get the signal tree
   if(sigback == 0)
   {
      for(int j = 1; j <=(infile->GetNkeys()-1); j++)
      {
         stemp = "TreeS" + IntToStr(j);
         remove_before_last((char*)infile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
	 if( string(signalSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
	 {
	    TTree *oldtree = (TTree*)infile->Get(stemp.c_str());
	    oldtree->ls();
	    if( !cutEnergy->widgetChBox[0]->IsDown() && !cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are disabled (signal)." << endl;
	       outTree = (TTree*)oldtree->CloneTree();
	    }
	    else if( cutEnergy->widgetChBox[0]->IsDown() && cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are enabled (signal): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << "), Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "((zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + "))&&((fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + "))";
	       cout << "Cut setting: " << stemp2 << endl;
	       outTree = (TTree*)oldtree->CopyTree(stemp2.c_str());
	    }
	    else if(cutEnergy->widgetChBox[0]->IsDown())
	    {
	       cout << "Only energy cuts are enabled (signal): Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "(fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       outTree = (TTree*)oldtree->CopyTree(stemp2.c_str());
	    }
	    else if(cutZenith->widgetChBox[0]->IsDown())
	    {
	       cout << "Only zenith cuts are enabled (signal): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << ")." << endl;
	       stemp2 = "(zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       outTree = (TTree*)oldtree->CopyTree(stemp2.c_str());
	    }

	    cout << "GetTree(): Number of entries in signal tree = " << outTree->GetEntries() << endl;
	    
            return stemp;
	 }
      }
   }
   // Get the background tree
   else if(sigback == 1)
   {
      for(int j = 1; j <=(infile->GetNkeys()-1); j++)
      {
         stemp = "TreeS" + IntToStr(j);
         remove_before_last((char*)infile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
	 if( string(backgroundSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
	 {
	    TTree *oldtree = (TTree*)infile->Get(stemp.c_str());
	    oldtree->ls();
	    if( !cutEnergy->widgetChBox[0]->IsDown() && !cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are disabled (signal)." << endl;
	       outTree = (TTree*)oldtree->CloneTree();
	    }
	    else if( cutEnergy->widgetChBox[0]->IsDown() && cutZenith->widgetChBox[0]->IsDown() )
	    {
	       cout << "Both energy and zenith cuts are enabled (signal): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << "), Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "((zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + "))&&((fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + "))";
	       cout << "Cut setting: " << stemp2 << endl;
	       outTree = (TTree*)oldtree->CopyTree(stemp2.c_str());
	    }
	    else if(cutEnergy->widgetChBox[0]->IsDown())
	    {
	       cout << "Only energy cuts are enabled (signal): Energy = (" << DblToStr(ecutBins[selectedBin[0]-1],6) << "," << DblToStr(ecutBins[selectedBin[0]],6) << ")." << endl;
	       stemp2 = "(fdenergy>" + DblToStr(ecutBins[selectedBin[0]-1],6) + ")&&(fdenergy<=" + DblToStr(ecutBins[selectedBin[0]],6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       outTree = (TTree*)oldtree->CopyTree(stemp2.c_str());
	    }
	    else if(cutZenith->widgetChBox[0]->IsDown())
	    {
	       cout << "Only zenith cuts are enabled (signal): Zenith = (" << DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) << "," << DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) << ")." << endl;
	       stemp2 = "(zenith>" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]-1],false),6) + ")&&(zenith<=" + DblToStr(AsinSqrt(zcutBins[selectedBin[1]],false),6) + ")";
	       cout << "Cut setting: " << stemp2 << endl;
	       outTree = (TTree*)oldtree->CopyTree(stemp2.c_str());
	    }

	    cout << "GetTree(): Number of entries in background tree = " << outTree->GetEntries() << endl;
	    
            return stemp;
	 }
      }
   }

   return "";*/

/*   string stemp;
   char ctemp[1024];
   char ctemp2[1024];
   float *tempobs;
   int newnrobs = 0;
   int itemp = 0;
   int nrevts = 0;
   string seltree;
   TTree *tempTree[3];

   // Determine the number of used observables
   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] > 0)
         newnrobs++;
   }

   tempobs = new float[newnrobs];

   // Create a new tree from the observables we select
   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] > 0)
      {
	 sprintf(ctemp, "%s/F", observables[i].c_str());
         outTree->Branch(observables[i].c_str(), &tempobs[itemp], ctemp);
         itemp++;
      }
   }

   cout << "Nr keys = " << (infile->GetNkeys()-3)/3 << endl;
   // Determine which signal or background tree is selected
   for(int j = 1; j <= ((infile->GetNkeys()-3)/3); j++)
   {
      stemp = "TreeS" + IntToStr(j);
      remove_before_last((char*)infile->GetKey((stemp+"_mean").c_str())->GetTitle(), '/', ctemp2);
      remove_from_last(ctemp2, '(', ctemp);
      if( (sigback == 0) && (string(signalSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp)) )
      {
	 seltree = stemp;
         cout << "Signal tree is " << seltree << ", title is = " << ctemp << endl;
      }
      else if( (sigback == 1) && (string(backgroundSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp)) )
      {
	 seltree = stemp;
         cout << "Background tree is " << seltree << ", title is = " << ctemp << endl;
      }
   }

   // Save the trees
   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] > 0)
      {
	 if(observablesCheck[i] == 1)
            tempTree[0] = ((TTree*)infile->Get((seltree+GetType(observablesCheck[i])).c_str()));
         else if(observablesCheck[i] == 2)
            tempTree[1] = ((TTree*)infile->Get((seltree+GetType(observablesCheck[i])).c_str()));
	 else if(observablesCheck[i] == 3)
            tempTree[2] = ((TTree*)infile->Get((seltree+GetType(observablesCheck[i])).c_str()));
      }
   }

   // Determine the largest number of events in a tree
   itemp = 0;
   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] > 0)
      {
	 cout << "Selecting the tree = " << (seltree+GetType(observablesCheck[i])).c_str() << ", nrevts = " << tempTree[observablesCheck[i]-1]->GetEntries() << endl;
	 if( tempTree[observablesCheck[i]-1]->GetEntries() > nrevts )
	    nrevts = tempTree[observablesCheck[i]-1]->GetEntries();
         itemp++;
	 if(itemp == newnrobs)
            break;
      }
   }

   cout << "Number of events in input trees = " << nrevts << endl;

   // Prepare trees to read from
   float *obsTree1,*obsTree2,*obsTree3;
   obsTree1 = new float[nrobs];
   obsTree2 = new float[nrobs];
   obsTree3 = new float[nrobs];

   itemp = 0;
   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] > 0)
      {
	 if(observablesCheck[i] == 1)
            tempTree[observablesCheck[i]-1]->SetBranchAddress(observables[i].c_str(), &obsTree1[i]);
         else if(observablesCheck[i] == 2)
            tempTree[observablesCheck[i]-1]->SetBranchAddress(observables[i].c_str(), &obsTree2[i]);
         else if(observablesCheck[i] == 3)
            tempTree[observablesCheck[i]-1]->SetBranchAddress(observables[i].c_str(), &obsTree3[i]);

	 itemp++;
	 if(itemp == newnrobs)
            break;
      }
   }
   
   itemp = 0;
   // Copy events from input to output tree
   for(int j = 0; j < nrevts; j++)
   {
      for(int i = 0; i < nrobs; i++)
      {
         if(observablesCheck[i] > 0)
         {
            if(observablesCheck[i] == 1)
            {
               tempTree[0]->GetEntry(j);
//               cout << "Entry " << j << ", tree = " << seltree+GetType(observablesCheck[i]) << ": " << observables[i] << " = " << obsTree1[i] << endl;
               tempobs[itemp] = obsTree1[i];
	       itemp++;
            }
            else if(observablesCheck[i] == 2)
            {
               tempTree[1]->GetEntry(j);
//               cout << "Entry " << j << ", tree = " << seltree+GetType(observablesCheck[i]) << ": " << observables[i] << " = " << obsTree2[i] << endl;
               tempobs[itemp] = obsTree2[i];
	       itemp++;
            }
            else if(observablesCheck[i] == 3)
            {
               tempTree[2]->GetEntry(j);
//               cout << "Entry " << j << ", tree = " << seltree+GetType(observablesCheck[i]) << ": " << observables[i] << " = " << obsTree3[i] << endl;
               tempobs[itemp] = obsTree3[i];
	       itemp++;
            }
         }
      }

      outTree->Fill();
   }

   cout << "Number of events in output tree = " << outTree->GetEntries() << endl;
   if(outTree->GetEntries() > 0)
      return "good";
   else
      return "";

   delete[] obsTree3;
   delete[] obsTree2;
   delete[] obsTree1;

   delete[] tempobs;*/

/*   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] == 1)	// mean tree
      {
         stemp = "TreeS" + 
      }
      else if(observablesCheck[i] == 2)	// mean + neg tree
      {
      }
      else if(observablesCheck[i] == 3)	// mean + pos tree
      {
      }
   }


   for(int j = 1; j <= ((infile->GetNkeys()-3)/3); j++)
   {
      stemp = "TreeS" + IntToStr(j) + "_" + type;
      remove_before_last((char*)infile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
      cout << "Tree is " << stemp << ", title is = " << ctemp << endl;*/
/*      if( string(signalSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
         return stemp;
   }*/

/*   if(sigback == 0)
   {
      return "sigtest";
   }
   else if(sigback == 1)
   {
      return "backtest";
   }
   else
      return NULL;*/

/*   string stemp;
   char ctemp[1024];

   if( (type == "mean") || (type == "pos") || (type == "neg") )
   {
      // Get the signal tree
      if(sigback == 0)
      {
         for(int j = 1; j <= ((infile->GetNkeys()-3)/3); j++)
         {
            stemp = "TreeS" + IntToStr(j) + "_" + type;
            remove_before_last((char*)infile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
	    if( string(signalSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
	       return stemp;
         }
      }
      // Get the background tree
      else if(sigback == 1)
      {
         for(int j = 1; j <= ((infile->GetNkeys()-3)/3); j++)
         {
            stemp = "TreeS" + IntToStr(j) + "_" + type;
            remove_before_last((char*)infile->GetKey(stemp.c_str())->GetTitle(), '/', ctemp);
	    if( string(backgroundSelect->widgetCB->GetSelectedEntry()->GetTitle()) == string(ctemp) )
	       return stemp;
         }
      }
      else
         return NULL;
   }
   else
      return NULL;
}*/

void TGAppMainFrame::BookTheMethod(TMVA::Factory *factory)
{
   for(int i = 0; i < nrmethods; i++)
   {
      if(methodsSelect->widgetChBox[i]->IsDown())
      {
         if(methods[i] == "Cuts")
            factory->BookMethod(TMVA::Types::kCuts, methods[i], "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart");

         if(methods[i] == "CutsD")
            factory->BookMethod(TMVA::Types::kCuts, methods[i], "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate");

         if(methods[i] == "CutsPCA")
            factory->BookMethod(TMVA::Types::kCuts, methods[i], "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA");

         if(methods[i] == "CutsGA")
            factory->BookMethod(TMVA::Types::kCuts, methods[i], "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95");

         if(methods[i] == "CutsSA")
            factory->BookMethod(TMVA::Types::kCuts, methods[i], "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale");

         if(methods[i] == "Likelihood")
            factory->BookMethod(TMVA::Types::kLikelihood, methods[i], "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50");

         if(methods[i] == "LikelihoodD")
            factory->BookMethod(TMVA::Types::kLikelihood, methods[i], "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate");

         if(methods[i] == "LikelihoodPCA")
            factory->BookMethod(TMVA::Types::kLikelihood, methods[i], "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA"); 

         if(methods[i] == "LikelihoodKDE")
            factory->BookMethod(TMVA::Types::kLikelihood, methods[i], "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50"); 

         if(methods[i] == "LikelihoodMIX")
            factory->BookMethod(TMVA::Types::kLikelihood, methods[i], "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50"); 

         if(methods[i] == "PDERS")
            factory->BookMethod(TMVA::Types::kPDERS, methods[i], "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600");

         if(methods[i] == "PDERSD")
            factory->BookMethod(TMVA::Types::kPDERS, methods[i], "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate");

         if(methods[i] == "PDERSPCA")
            factory->BookMethod(TMVA::Types::kPDERS, methods[i], "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA");

         if(methods[i] == "PDEFoam")
            factory->BookMethod(TMVA::Types::kPDEFoam, methods[i], "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T");

         if(methods[i] == "PDEFoamBoost")
            factory->BookMethod(TMVA::Types::kPDEFoam, methods[i], "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T");

         if(methods[i] == "KNN")
            factory->BookMethod(TMVA::Types::kKNN, methods[i], "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim");

         if(methods[i] == "LD")
            factory->BookMethod(TMVA::Types::kLD, methods[i], "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10");

         if(methods[i] == "Fisher")
            factory->BookMethod(TMVA::Types::kFisher, methods[i], "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10");

         if(methods[i] == "FisherG")
            factory->BookMethod(TMVA::Types::kFisher, methods[i], "H:!V:VarTransform=Gauss");

         if(methods[i] == "BoostedFisher")
            factory->BookMethod(TMVA::Types::kFisher, methods[i], "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring");

         if(methods[i] == "HMatrix")
            factory->BookMethod(TMVA::Types::kHMatrix, methods[i], "!H:!V:VarTransform=None");

         if(methods[i] == "FDA_GA")
            factory->BookMethod(TMVA::Types::kFDA, methods[i], "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1");

         if(methods[i] == "FDA_SA")
            factory->BookMethod(TMVA::Types::kFDA, methods[i], "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale");

         if(methods[i] == "FDA_MC")
            factory->BookMethod(TMVA::Types::kFDA, methods[i], "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1");

         if(methods[i] == "FDA_MT")
            factory->BookMethod(TMVA::Types::kFDA, methods[i], "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch");

         if(methods[i] == "FDA_GAMT")
            factory->BookMethod(TMVA::Types::kFDA, methods[i], "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim");

         if(methods[i] == "FDA_MCMT")
            factory->BookMethod(TMVA::Types::kFDA, methods[i], "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20");

         if(methods[i] == "MLP")
            factory->BookMethod(TMVA::Types::kMLP, methods[i], "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator");

         if(methods[i] == "MLPBFGS")
            factory->BookMethod(TMVA::Types::kMLP, methods[i], "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator");

         if(methods[i] == "MLPBNN")
            factory->BookMethod(TMVA::Types::kMLP, methods[i], "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator");

         if(methods[i] == "CFMlpANN")
            factory->BookMethod(TMVA::Types::kCFMlpANN, methods[i], "!H:!V:NCycles=2000:HiddenLayers=N+1,N");

         if(methods[i] == "TMlpANN")
            factory->BookMethod(TMVA::Types::kTMlpANN, methods[i], "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3");

         if(methods[i] == "SVM")
            factory->BookMethod(TMVA::Types::kSVM, methods[i], "Gamma=0.25:Tol=0.001:VarTransform=Norm");

         if(methods[i] == "BDT")
            factory->BookMethod(TMVA::Types::kBDT, methods[i], "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");

         if(methods[i] == "BDTG")
            factory->BookMethod(TMVA::Types::kBDT, methods[i], "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");

         if(methods[i] == "BDTB")
            factory->BookMethod(TMVA::Types::kBDT, methods[i], "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20");

         if(methods[i] == "BDTD")
            factory->BookMethod(TMVA::Types::kBDT, methods[i], "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate");

         if(methods[i] == "BDTF")
            factory->BookMethod(TMVA::Types::kBDT, "BDTMitFisher", "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");

         if(methods[i] == "RuleFit")
            factory->BookMethod(TMVA::Types::kRuleFit, methods[i], "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02");
      }
   }
}

void TGAppMainFrame::CreateMVAPlots(TTree *app, TMVA::Reader *reader, string mvamethod, float *obsvars, string signalName, int curtree)
{
//cutmva, observables, signalapp, reader, mvamethod, obsvars, signalName
   // Prepare colors for signal, background and MVA cut line
   static Int_t c_SignalLine     = TColor::GetColor("#0000ee");
   static Int_t c_SignalFill     = TColor::GetColor("#7d99d1");
   static Int_t c_AllLine        = TColor::GetColor("#ff0000");
   static Int_t c_AllFill        = TColor::GetColor("#ff0000");
   static Int_t c_MvaCut         = TColor::GetColor("#ffff66");

   vector<string> obs;
   for(int i = 0; i < nrobs; i++)
   {
      if(observablesCheck[i] == 1)		// mean
         obs.push_back(observables[i]);
      else if(observablesCheck[i] == 2)	// mean + neg error
         obs.push_back(observables[i] + "_neg");
      else if(observablesCheck[i] == 3)	// mean + pos error
         obs.push_back(observables[i] + "_pos");
   }

cout << "Number of events in the tree = " << app->GetEntries() << endl;
   
   // All additional things we need for plotting
   TLegend *legend;
   TLine *line;
   TH1F *basesig[obs.size()+1];
   TH1F *baseback[obs.size()+1];

   string stemp;

   int legendFill = 1001;
   float yhistlimit[2];
   int cnt;

   float *max;
   int *sigcount, *backcount;
   max = new float[obs.size()+1];
   sigcount = new int[obs.size()+1];
   backcount = new int[obs.size()+1];

   gStyle->SetOptStat(0);

   TCanvas *c1 = new TCanvas("c1","",1200,900);
   c1->SetGrid();
   c1->SetRightMargin(0.05);
   c1->SetTopMargin(0.05);

   // Prepare the histograms
   for(int i = 0; i <= obs.size(); i++)
   {
      if(i < obs.size())
      {
         SetupBinning(obs[i], yhistlimit);
         stemp = "basesig" + IntToStr(i);
         basesig[i] = new TH1F(stemp.c_str(), obs[i].c_str(), 100, yhistlimit[0], yhistlimit[1]);
         basesig[i]->SetBit(TH1::kCanRebin);
         stemp = "baseback" + IntToStr(i);
         baseback[i] = new TH1F(stemp.c_str(), obs[i].c_str(), 100, yhistlimit[0], yhistlimit[1]);
         baseback[i]->SetBit(TH1::kCanRebin);
      }
      else
      {
         SetupBinning("MVA", yhistlimit);
         stemp = "basesig" + IntToStr(i);
         basesig[i] = new TH1F(stemp.c_str(), "mva", 100, yhistlimit[0], yhistlimit[1]);
         basesig[i]->SetBit(TH1::kCanRebin);
         stemp = "baseback" + IntToStr(i);
         baseback[i] = new TH1F(stemp.c_str(), "mva", 100, yhistlimit[0], yhistlimit[1]);
         baseback[i]->SetBit(TH1::kCanRebin);
      }

      max[i] = 0.0;
      sigcount[i] = 0;
      backcount[i] = 0;
   }

cout << "Current tree = " << curtree << ", Observables size = " << obs.size() << endl;
   for(int ievt = 0; ievt < app->GetEntries(); ievt++)
   {
      app->GetEntry(ievt);
      cnt = 0;

      for(int i = 0; i <= nrobs; i++)
      {
         if(cnt < obs.size())
         {
	    if( (observables[i] == obs[cnt]) || ((observables[i]+"_neg") == obs[cnt]) || ((observables[i]+"_pos") == obs[cnt]) )
	    {
//cout << "  Event = " << ievt << ": value = " << obsvars[i] << ", observable = " << observables[i] << " (" << obs[cnt] << ")" << endl;
               if(reader->EvaluateMVA(mvamethod) >= cutMva->widgetNE[0]->GetNumber())
               {
                  basesig[cnt]->Fill(obsvars[i]);
                  sigcount[cnt]++;
               }
               else
               {
                  baseback[cnt]->Fill(obsvars[i]);
                  backcount[cnt]++;
               }

               cnt++;
	    }
         }
         else if(cnt == obs.size())
         {
//cout << "  MVA Event = " << ievt << ": value = " << reader->EvaluateMVA(mvamethod) << ", observable = " << "MVA" << endl;
            if(reader->EvaluateMVA(mvamethod) >= cutMva->widgetNE[0]->GetNumber())
            {
               basesig[cnt]->Fill(reader->EvaluateMVA(mvamethod));
               sigcount[cnt]++;
            }
            else
            {
               baseback[cnt]->Fill(reader->EvaluateMVA(mvamethod));
               backcount[cnt]++;
            }

            cnt++;
         }
	 else
	    break;
      }
   }

   cout << "Signal vs. background (" << app->GetTitle() << "):" << endl;
   for(int i = 0; i <= obs.size(); i++)
   {
      if(i < obs.size())
         cout << " - " << obs[i] << " = " << sigcount[i] << " vs. " << backcount[i] << endl;
      else
         cout << " - MVA = " << sigcount[i] << " vs. " << backcount[i] << endl;
   }

   displayResult[curtree-1]->widgetTE->SetText((IntToStr(sigcount[obs.size()]) + " signal (" + DblToStr(100.*(double)sigcount[obs.size()]/((double)app->GetEntries()), 3) + "%) vs. " + IntToStr(backcount[obs.size()]) + " background (" + DblToStr(100.*(double)backcount[obs.size()]/((double)app->GetEntries()), 3) + "%)").c_str());
   displayResult[curtree-1]->widgetTE->SetState(kTRUE);

   cout << endl;

   string outname;

   for(int i = 0; i <= obs.size(); i++)
   {
      if(basesig[i]->GetMaximum() > max[i]) max[i] = basesig[i]->GetMaximum();
      if(baseback[i]->GetMaximum() > max[i]) max[i] = baseback[i]->GetMaximum();

      cout << "Maximum = " << max[i] << endl;

      basesig[i]->SetLineColor(c_SignalLine);
      basesig[i]->SetLineWidth(2);
      basesig[i]->SetFillColor(c_SignalFill);
      basesig[i]->SetFillStyle(1001);
      baseback[i]->SetLineColor(c_AllLine);
      baseback[i]->SetLineWidth(2);
      baseback[i]->SetFillColor(c_AllFill);
      baseback[i]->SetFillStyle(3554);

      basesig[i]->Draw();
      baseback[i]->Draw("same");

      basesig[i]->GetYaxis()->SetRangeUser(0.,max[i]*1.2);
      basesig[i]->SetMaximum(max[i]*1.2);

      if(i < obs.size())
         SetupAxis(basesig[i], obs[i]);
      else
      {
         SetupAxis(basesig[i], "MVA");
         line = new TLine(cutMva->widgetNE[0]->GetNumber(), 0., cutMva->widgetNE[0]->GetNumber(), basesig[i]->GetMaximum());
         line->SetLineWidth(2);
         line->SetLineStyle(7);
         line->SetLineColor(kOrange+2);
         line->Draw("same");
      }

      // Draw legend
      TLegend *legend= new TLegend(gPad->GetLeftMargin(), 1-gPad->GetTopMargin()-.10, gPad->GetLeftMargin()+.25, 1-gPad->GetTopMargin());
      legend->SetFillStyle(legendFill);
      legend->SetFillColor(c_MvaCut);
      stemp = "MVA cut iron events (" + IntToStr(backcount[i]) + ")";
      legend->AddEntry(baseback[i],stemp.c_str(),"f");
      stemp = "MVA cut proton events (" + IntToStr(sigcount[i]) + ")";
      legend->AddEntry(basesig[i],stemp.c_str(),"f");
      legend->SetBorderSize(1);
      legend->SetMargin(0.3);
      legend->Draw("same");

      if(i < obs.size())
         stemp = string(currentAnalysisDir) + "/mva_analysis_" + signalName + "_" + obs[i] + ".pdf";
      else
         stemp = string(currentAnalysisDir) + "/mva_analysis_" + signalName + "_MVA.pdf";
      c1->SaveAs(stemp.c_str());
   }

   delete[] max;
   delete[] sigcount;
   delete[] backcount;
   for(int i = 0; i <= obs.size(); i++)
   {
      delete basesig[i];
      delete baseback[i];
   }
   delete c1;
}

void TGAppMainFrame::SetupBinning(std::string obs, float *limit)
{
   if( (obs == "xmax") || (obs == "xmax_neg") || (obs == "xmax_pos") )
   {
      limit[0] = plotSetXmax->widgetNE[0]->GetNumber();//400.;
      limit[1] = plotSetXmax->widgetNE[1]->GetNumber();//1200.;
   }
   else if( (obs == "x0") || (obs == "x0_neg") || (obs == "x0_pos") )
   {
      limit[0] = plotSetX0->widgetNE[0]->GetNumber();//-550.;
      limit[1] = plotSetX0->widgetNE[1]->GetNumber();//450.;
   }
   else if( (obs == "lambda") || (obs == "lambda_neg") || (obs == "lambda_pos") )
   {
      limit[0] = plotSetLambda->widgetNE[0]->GetNumber();//35.;
      limit[1] = plotSetLambda->widgetNE[1]->GetNumber();//135.;
   }
   else if( (obs == "fdenergy") || (obs == "fdenergy_neg") || (obs == "fdenergy_pos") )
   {
      limit[0] = TMath::Power(10,plotSetFdenergy->widgetNE[0]->GetNumber());//1.e+17;
      limit[1] = TMath::Power(10,plotSetFdenergy->widgetNE[1]->GetNumber());//1.e+20;
   }
   else if( (obs == "shfoot") || (obs == "shfoot_neg") || (obs == "shfoot_pos") )
   {
      limit[0] = plotSetShfoot->widgetNE[0]->GetNumber();//300.;
      limit[1] = plotSetShfoot->widgetNE[1]->GetNumber();//900.;
   }
   else if( (obs == "shwsize") || (obs == "shwsize_neg") || (obs == "shwsize_pos") )
   {
      limit[0] = plotSetShwsize->widgetNE[0]->GetNumber();//0.;
      limit[1] = plotSetShwsize->widgetNE[1]->GetNumber();//170.;
   }
/*   else if(obs == "ldfbeta")
   {
      limit[0] = -2.5;
      limit[1] = -1.5;
   }*/
   else if( (obs == "nrmu") || (obs == "nrmu_neg") || (obs == "nrmu_pos") )
   {
      limit[0] = plotSetNrmu->widgetNE[0]->GetNumber();//0.;
      limit[1] = plotSetNrmu->widgetNE[1]->GetNumber();//90.e+6;
   }
   else if( (obs == "curvature") || (obs == "curvature_neg") || (obs == "curvature_pos") )
   {
      limit[0] = plotSetCurvature->widgetNE[0]->GetNumber();//1.e-4;
      limit[1] = plotSetCurvature->widgetNE[1]->GetNumber();//0.25e-3;
   }
   else if( (obs == "risetime") || (obs == "risetime_neg") || (obs == "risetime_pos") )
   {
      limit[0] = plotSetRisetime->widgetNE[0]->GetNumber();//0.;
      limit[1] = plotSetRisetime->widgetNE[1]->GetNumber();//650.;
   }
   else if( (obs == "risetimerecalc") || (obs == "risetimerecalc_neg") || (obs == "risetimerecalc_pos") )
   {
      limit[0] = plotSetRisetime->widgetNE[0]->GetNumber();//0.;
      limit[1] = plotSetRisetime->widgetNE[1]->GetNumber();//650.;
   }
   else if( (obs == "aop") || (obs == "aop_neg") || (obs == "aop_pos") )
   { // TODO
      limit[0] = plotSetRisetime->widgetNE[0]->GetNumber();//0.;
      limit[1] = plotSetRisetime->widgetNE[1]->GetNumber();//650.;
   }
   else if( (obs == "zenith") || (obs == "zenith_neg") || (obs == "zenith_pos") )
   {
      limit[0] = cutZenith->widgetNE[0]->GetNumber();//0.;
      limit[1] = cutZenith->widgetNE[1]->GetNumber();//60.;
   }
   else if(obs == "MVA")
   {
      limit[0] = plotSetMva->widgetNE[0]->GetNumber();//-2.;
      limit[1] = plotSetMva->widgetNE[1]->GetNumber();//2.;
   }
   else
      return;
}

void TGAppMainFrame::SetupAxis(TH1F *hist, string obs)
{
   string obsdesc;
   // Determine which observable we have
   if( (obs == "xmax") || (obs == "xmax_neg") || (obs == "xmax_pos") )
      obsdesc = "X_{max} (g/cm^{2})";
   else if( (obs == "x0") || (obs == "x0_neg") || (obs == "x0_pos") )
      obsdesc = "GH parameter X_{0} (g/cm^{2})";
   else if( (obs == "lambda") || (obs == "lambda_neg") || (obs == "lambda_pos") )
      obsdesc = "GH parameter lambda (g/cm^{2})";
   else if( (obs == "fdenergy") || (obs == "fdenergy_neg") || (obs == "fdenergy_pos") )
      obsdesc = "FD reconstructed energy (eV)";
   else if( (obs == "shfoot") || (obs == "shfoot_neg") || (obs == "shfoot_pos") )
      obsdesc = "Shower foot (g/cm^{2})";
   else if( (obs == "shwsize") || (obs == "shwsize_neg") || (obs == "shwsize_pos") )
      obsdesc = "SD signal at 1000m from axis (VEM)";
/*   else if(obs == "ldfbeta")
      obsdesc = "LDF beta parameter";*/
   else if( (obs == "curvature") || (obs == "curvature_neg") || (obs == "curvature_pos") )
      obsdesc = "Curvature of shower plane (m^{-1})";
   else if( (obs == "nrmu") || (obs == "nrmu_neg") || (obs == "nrmu_pos") )
      obsdesc = "Number of muons at ground level";
   else if( (obs == "risetime") || (obs == "risetime_neg") || (obs == "risetime_pos") )
      obsdesc = "SD tank risetime (ns)";
   else if( (obs == "risetimerecalc") || (obs == "risetimerecalc_neg") || (obs == "risetimerecalc_pos") )
      obsdesc = "SD tank risetime (ns)";
   else if( (obs == "aop") || (obs == "aop_neg") || (obs == "aop_pos") )
      obsdesc = "Area-over-Peak (VEM)";
   else if( (obs == "zenith") || (obs == "zenith_neg") || (obs == "zenith_pos") )
      obsdesc = "Zenith angle (degree)";
   else if(obs == "MVA")
      obsdesc = "MVA observable";
   else
      return;

   hist->GetYaxis()->SetTitle("Number of events");
   hist->GetXaxis()->SetTitle(obsdesc.c_str());

   hist->GetXaxis()->SetTitleOffset(1.2);
   hist->GetXaxis()->CenterTitle(kTRUE);
   hist->GetXaxis()->SetLabelSize(0.028);
   hist->GetXaxis()->SetLabelOffset(0.015);
   hist->GetYaxis()->SetTitleOffset(1.3);
   hist->GetYaxis()->CenterTitle(kTRUE);
   hist->GetYaxis()->SetLabelSize(0.028);
   hist->GetYaxis()->SetLabelOffset(0.015);

   hist->SetTitle("");
}

// Mva file selection controls -----------------------------

// Additional functions -------------------------------------

// Display the currently selected histogram in the file list
void TGAppMainFrame::DisplayAdstPlot(char *histfile, int opt)
{
   char histtime[256];
   char ctemp[512];
   int nrevents;
   int selevent;
   bool validfd = true;

   if(DBGSIG)
      printf("DisplayHistogram(): Selected file: %s\n", histfile);

   TCanvas *gCanvas = adstCanvas->GetCanvas();

   AdstMva *mvatool = new AdstMva();
   mvatool->fFile = new RecEventFile(histfile, RecEventFile::eRead);
   (mvatool->fFile)->SetBuffers(&(mvatool->fRecEvent));
   (mvatool->fFile)->ReadDetectorGeometry(*(mvatool->fDetGeo));

   nrevents = (mvatool->fFile)->GetNEvents();
   selevent = adstSelectEvent->widgetNE[0]->GetNumber();
   cout << "Number of events: " << nrevents << endl;
   adstSelectEvent->widgetNE[0]->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, nrevents);

   // Plot the distribution
   if((adstPlotDist->widgetChBox[0]->IsDown()) && (!adstPlotSingle->widgetChBox[0]->IsDown()))
   {
      cout << "Plotting distribution!" << endl;

   }

   // Plot the single file information
   if((!adstPlotDist->widgetChBox[0]->IsDown()) && (adstPlotSingle->widgetChBox[0]->IsDown()))
   {
      cout << "Plotting single file!" << endl;

      if(selevent > nrevents)
      {
         adstSelectEvent->widgetNE[0]->SetNumber(1);
	 oldadstvalue = 1;
         selevent = 1;
      }

      // Read the selected event
      (mvatool->fFile)->ReadEvent(selevent);
      adstShowEvent->SetText(((mvatool->fRecEvent)->GetEventId()).c_str());
      adstShowEvent->SetWrapLength(-1);

      // Get a list of active tanks
      mvatool->GetActiveTanks();
      if(mvatool->nrtanks == 0)
      {
         adstSelectSdId->widgetCB->RemoveAll();
	 adstSelectSdId->widgetCB->NewEntry("Select SD tank ID...");
	 adstSelectSdId->widgetCB->Select(1);
      }
      else
      {
         adstSelectSdId->widgetCB->RemoveAll();
         for(int i = 0; i < mvatool->nrtanks; i++)
	 {
	    sprintf( ctemp, "%d", (mvatool->acttanks[i]).GetId() );
	    adstSelectSdId->widgetCB->NewEntry(ctemp);
	 }
	 adstSelectSdId->widgetCB->Select(1);
      }

      // Get a list of active eyes
      mvatool->GetActiveEyes();
      if(mvatool->nreyes == 0)
      {
         adstSelectFdId->widgetCB->RemoveAll();
	 adstSelectFdId->widgetCB->NewEntry("Select FD tank ID...");
	 adstSelectFdId->widgetCB->Select(1);
      }
      else
      {
         adstSelectFdId->widgetCB->RemoveAll();
         for(int i = 0; i < mvatool->nreyes; i++)
	 {
	    if((mvatool->acteyes[i]).GetEyeId() == 1)
	       adstSelectFdId->widgetCB->NewEntry("Los Leones");
	    else if((mvatool->acteyes[i]).GetEyeId() == 2)
	       adstSelectFdId->widgetCB->NewEntry("Los Morados");
	    else if((mvatool->acteyes[i]).GetEyeId() == 3)
	       adstSelectFdId->widgetCB->NewEntry("Loma Amarilla");
	    else if((mvatool->acteyes[i]).GetEyeId() == 4)
	       adstSelectFdId->widgetCB->NewEntry("Coihueco");
	    else if((mvatool->acteyes[i]).GetEyeId() == 5)
	       adstSelectFdId->widgetCB->NewEntry("HEAT");
	    else
	    {
	       cout << "Eye selection error!" << endl;
	       validfd = false;
	    }
	 }
	 adstSelectFdId->widgetCB->Select(1);
      }

      // Set what kind of plot we will have
      if( strcmp(adstPlotSingleSelect->widgetCB->GetSelectedEntry()->GetTitle(),"LDF") == 0 )
         mvatool->PlotLDF(gCanvas);
      else if( strcmp(adstPlotSingleSelect->widgetCB->GetSelectedEntry()->GetTitle(),"VEM trace") == 0 )
         mvatool->PlotVEM(gCanvas, string(adstSelectSdId->widgetCB->GetSelectedEntry()->GetTitle()) );
      else if( strcmp(adstPlotSingleSelect->widgetCB->GetSelectedEntry()->GetTitle(),"Longitudinal profile") == 0 )
      {
         if(validfd)
            mvatool->PlotLongProf(gCanvas, string(adstSelectFdId->widgetCB->GetSelectedEntry()->GetTitle()) );
	 else
	    cout << "Plotting is invalid (no available eye)." << endl;
      }
      else
         cout << "Error while plotting (single)!" << endl;
   }

   delete mvatool;

/*   inroot = TFile::Open(histfile, "READ");

   TTree *header_data, *meas_data;
   inroot->GetObject("header_data", header_data);
   inroot->GetObject("meas_data", meas_data);

   // Reading the header
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

   // Change timestamp to local time
   GetTime(evtheader.timestamp, histtime);

   // Displaying header information debug
   if(DBGSIG)
   {
      printf("DisplayHistogram(): Opened file header information:\n");
      printf("- Number of channels (ADC and TDC are considered as separate channels): %d\n", evtheader.nrch);
      printf("- Timestamp: %d (%s)\n", evtheader.timestamp, histtime);
      printf("- Bias voltage: %lf\n", evtheader.biasvolt);
      printf("- Table position (X,Y,Z): %d, %d, %d\n", evtheader.xpos, evtheader.ypos, evtheader.zpos);
      if(evtheader.temperature)
         printf("- Temperature: %lf\n", evtheader.temperature);
      if( header_data->FindBranch("angle") )
         printf("- Incidence angle: %lf\n", evtheader.angle);
      else
         printf("- Incidence angle: No angle information!\n");
      printf("- Laser and filter settings: %s\n", evtheader.laserinfo);
   }

   // Displaying header information on the GUI
   dispTime->widgetTE->SetText(histtime);
   dispBias->widgetNE[0]->SetNumber(evtheader.biasvolt);
   sprintf(ctemp, "%d, %d, %d", evtheader.xpos, evtheader.ypos, evtheader.zpos);
   dispPos->widgetTE->SetText(ctemp);
   if(evtheader.temperature)
      dispTemp->widgetNE[0]->SetNumber(evtheader.temperature);
   else
      dispTemp->widgetNE[0]->SetNumber(0.0);
   if( header_data->FindBranch("angle") )
      dispAngle->widgetNE[0]->SetNumber(evtheader.angle);
   else
      dispAngle->widgetNE[0]->SetNumber(0.0);
   dispLaser->widgetTE->SetText(evtheader.laserinfo);

   selectCh->widgetNE[0]->SetLimitValues(0, (evtheader.nrch/2)-1);

   // Redraw the histograms
   int j;
   char rdc[256];
   char rdcsel[256];

   j = selectCh->widgetNE[0]->GetNumber();

   printf("Found %d data points.\n", (int)meas_data->GetEntries());

   gCanvas->cd();
   double range[4];
   range[0] = adcRange->widgetNE[0]->GetNumber();
   range[1] = adcRange->widgetNE[1]->GetNumber();
   range[2] = tdcRange->widgetNE[0]->GetNumber();
   range[3] = tdcRange->widgetNE[1]->GetNumber();

   // ADC histogram
   if(histtype == 0)
   {
      if( range[0] == range[1] )
         sprintf(rdc, "ADC%d>>%s", j, histname);
      else
         sprintf(rdc, "ADC%d>>%s(%d,%lf,%lf)", j, histname, (int)(range[1]-range[0]), range[0]-0.5, range[1]-0.5);

      sprintf(rdcsel, "(TDC%d>%lf)&&(TDC%d<%lf)", j, range[2]*tdctimeconversion, j, range[3]*tdctimeconversion);
      meas_data->Draw(rdc, rdcsel);

      sprintf(rdc, "ADC%d, Vbias=%.3lf, TDC=(%.2lf,%.2lf);ADC;", j, evtheader.biasvolt, range[2], range[3]);
      TH1F *histtemp = (TH1F*)gCanvas->GetPrimitive(histname);
      if(!cleanPlots)
         histtemp->SetTitle(rdc);
      else
         histtemp->SetTitle(";ADC;");
      histtemp->GetXaxis()->SetLabelSize(0.025);
      histtemp->GetXaxis()->CenterTitle(kTRUE);
      histtemp->GetYaxis()->SetLabelSize(0.025);
      if(cleanPlots)
      {
         TGaxis *yax = (TGaxis*)histtemp->GetYaxis();
         yax->SetMaxDigits(4);
      }

      gCanvas->Modified();
      gCanvas->Update();

      if( yRange->widgetNE[0]->GetNumber() != yRange->widgetNE[1]->GetNumber() )
      {
         if( (histOpt->widgetChBox[0]->IsDown()) && (yRange->widgetNE[0]->GetNumber() <= 0) )
	 {
            histtemp->GetYaxis()->SetRangeUser(0.5, yRange->widgetNE[1]->GetNumber());
	    yRange->widgetNE[0]->SetNumber(0.5);
	    logChange = 1;
	 }
	 else
	 {
            gCanvas->SetLogy(kFALSE);
	    if(logChange == 1)
	    {
               yRange->widgetNE[0]->SetNumber(0.0);
	       logChange = 0;
	    }
            histtemp->GetYaxis()->SetRangeUser(yRange->widgetNE[0]->GetNumber(), yRange->widgetNE[1]->GetNumber());
	 }
      }

      TPaveStats *stats = (TPaveStats*)histtemp->FindObject("stats");
      if(!cleanPlots)
      {
         stats->SetX1NDC(0.84); stats->SetX2NDC(0.97);
         stats->SetY1NDC(0.87); stats->SetY2NDC(0.97);
      }
      else
      {
         stats->SetX1NDC(1.1); stats->SetX2NDC(1.3);
         stats->SetY1NDC(1.1); stats->SetY2NDC(1.3);
      }
   }
   // TDC histogram
   else if(histtype == 1)
   {
      if( range[0] == range[1] )
         sprintf(rdc, "(TDC%d/%lf)>>%s", j, tdctimeconversion, histname);
      else
         sprintf(rdc, "(TDC%d/%lf)>>%s(%d,%lf,%lf)", j, tdctimeconversion, histname, (int)((range[3]-range[2])*tdctimeconversion), range[2], range[3]);

      sprintf(rdcsel, "(TDC%d>%lf)&&(TDC%d<%lf)", j, range[2]*tdctimeconversion, j, range[3]*tdctimeconversion);
      meas_data->Draw(rdc, rdcsel);

      sprintf(rdc, "TDC%d, Vbias=%.3lf, TDC=(%.2lf,%.2lf);Time (TDC channel) [ns];", j, evtheader.biasvolt, range[2], range[3]);
      TH1F *histtemp = (TH1F*)gCanvas->GetPrimitive(histname);
      if(!cleanPlots)
         histtemp->SetTitle(rdc);
      else
         histtemp->SetTitle(";Time (TDC channel) [ns];");
      histtemp->GetXaxis()->SetLabelSize(0.025);
      histtemp->GetXaxis()->CenterTitle(kTRUE);
      histtemp->GetYaxis()->SetLabelSize(0.025);
      if(cleanPlots)
      {
         TGaxis *yax = (TGaxis*)histtemp->GetYaxis();
         yax->SetMaxDigits(4);
      }

      gCanvas->Modified();
      gCanvas->Update();

      if( yRange->widgetNE[0]->GetNumber() != yRange->widgetNE[1]->GetNumber() )
      {
         if( (histOpt->widgetChBox[0]->IsDown()) && (yRange->widgetNE[0]->GetNumber() <= 0) )
	 {
            histtemp->GetYaxis()->SetRangeUser(0.5, yRange->widgetNE[1]->GetNumber());
	    yRange->widgetNE[0]->SetNumber(0.5);
	    logChange = 1;
	 }
	 else
	 {
            gCanvas->SetLogy(kFALSE);
	    if(logChange == 1)
	    {
               yRange->widgetNE[0]->SetNumber(0.0);
	       logChange = 0;
	    }
            histtemp->GetYaxis()->SetRangeUser(yRange->widgetNE[0]->GetNumber(), yRange->widgetNE[1]->GetNumber());
	 }
      }

      TPaveStats *stats = (TPaveStats*)histtemp->FindObject("stats");
      if(!cleanPlots)
      {
         stats->SetX1NDC(0.84); stats->SetX2NDC(0.97);
         stats->SetY1NDC(0.87); stats->SetY2NDC(0.97);
      }
      else
      {
         stats->SetX1NDC(1.1); stats->SetX2NDC(1.3);
         stats->SetY1NDC(1.1); stats->SetY2NDC(1.3);
      }
   }
   // ADC vs. TDC histogram
   else if(histtype == 2)
   {
      if( ((range[0] == range[1]) && (range[2] == range[3])) || (range[2] == range[3]) || (range[0] == range[1]) )
         sprintf(rdc, "(TDC%d/%lf):ADC%d>>%s", j, tdctimeconversion, j, histname);
      else
         sprintf(rdc, "(TDC%d/%lf):ADC%d>>%s(%d,%lf,%lf,%d,%lf,%lf)", j, tdctimeconversion, j, histname, (int)(range[1]-range[0])/2, range[0]-0.5, range[1]-0.5, (int)((range[3]-range[2])*tdctimeconversion)/2, range[2], range[3]);
      meas_data->Draw(rdc,"","COLZ");

      sprintf(rdc, "ADC/TDC%d, Vbias=%.3lf, TDC=(%.2lf,%.2lf);ADC;TDC", j, evtheader.biasvolt, range[2], range[3]);
      TH2F *histtemp = (TH2F*)gCanvas->GetPrimitive(histname);
      if(!cleanPlots)
         histtemp->SetTitle(rdc);
      else
         histtemp->SetTitle(";ADC;Time (TDC channel) [ns]");
      histtemp->GetXaxis()->SetLabelSize(0.025);
      histtemp->GetXaxis()->CenterTitle(kTRUE);
      histtemp->GetYaxis()->SetLabelSize(0.025);
      histtemp->GetYaxis()->CenterTitle(kTRUE);
      histtemp->GetYaxis()->SetTitleOffset(1.35);
      if(cleanPlots)
      {
         TGaxis *yax = (TGaxis*)histtemp->GetYaxis();
         yax->SetMaxDigits(4);
      }

      gCanvas->Modified();
      gCanvas->Update();

      TPaveStats *stats = (TPaveStats*)histtemp->FindObject("stats");
      stats->SetX1NDC(1.1); stats->SetX2NDC(1.3);
      stats->SetY1NDC(1.1); stats->SetY2NDC(1.3);

      TPaletteAxis *gpalette = (TPaletteAxis*)histtemp->GetListOfFunctions()->FindObject("palette");
      gpalette->SetLabelSize(0.022);
   }

   if(histtype < 2)
   {
      if( histOpt->widgetChBox[0]->IsDown() )
         gCanvas->SetLogy(kTRUE);
      else if( !histOpt->widgetChBox[0]->IsDown() )
         gCanvas->SetLogy(kFALSE);
   }
   else
      gCanvas->SetLogy(kFALSE);

   gCanvas->Modified();
   gCanvas->Update();

   delete header_data;
   delete meas_data;

   // Delete the opened file when we just display it in the analysis canvas (otherwise wait for histogram save)
   if(opt != 1)
      delete inroot;

   // If you close the opened file (delete inroot), the data can not be accessed by other functions (any time we wish to use the data directly from histogram, we need to call the DisplayHistogram function -> using different opt to determine what we need to do)*/
}

// Open the cuts file to be used
void TGAppMainFrame::CutsOpen(int opt)
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
   file_info.fMultipleSelection = kFALSE;
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);

   if(file_info.fFilename != NULL)
   {
      remove_from_last(file_info.fFilename, '/', currentCutsDir);
      cout << "Opened file: " << file_info.fFilename << endl;
      if(opt == 0)
         configFile->widgetTE->SetText(file_info.fFilename);
      else if(opt == 1)
         sdcutsFile->widgetTE->SetText(file_info.fFilename);
      else if(opt == 2)
         fdcutsFile->widgetTE->SetText(file_info.fFilename);
   }
}

// Edit the cuts file
void TGAppMainFrame::CutsEdit(int opt)
{
   bool createTab = true;
   int tabid = -1;
   int ret;

   for(int i = 0; i < fTab->GetNumberOfTabs(); i++)
   {
      if(strcmp("Cuts file editor", fTab->GetTabTab(i)->GetString() ) == 0)
      {
         createTab = false;
	 tabid = i;
      }
      
      if(DBGSIG > 1) printf("HeaderEdit(): Name of tab = %s\n", fTab->GetTabTab(i)->GetString() );
   }

   if(opt == 0)
   {
      if(string(configFile->widgetTE->GetText()) != string("No file selected!"))
         CutsEditor(fTab, createTab, &tabid, (char*)configFile->widgetTE->GetText(), opt);
      else
	 new TGMsgBox(gClient->GetRoot(), fMain, "Error editing file", "Please open a file before trying to edit it.", kMBIconExclamation, kMBOk, &ret);
   }
   else if(opt == 1)
   {
      if(string(sdcutsFile->widgetTE->GetText()) != string("No file selected!"))
         CutsEditor(fTab, createTab, &tabid, (char*)sdcutsFile->widgetTE->GetText(), opt);
      else
	 new TGMsgBox(gClient->GetRoot(), fMain, "Error editing file", "Please open a file before trying to edit it.", kMBIconExclamation, kMBOk, &ret);
   }
   else if(opt == 2)
   {
      if(string(fdcutsFile->widgetTE->GetText()) != string("No file selected!"))
         CutsEditor(fTab, createTab, &tabid, (char*)fdcutsFile->widgetTE->GetText(), opt);
      else
	 new TGMsgBox(gClient->GetRoot(), fMain, "Error editing file", "Please open a file before trying to edit it.", kMBIconExclamation, kMBOk, &ret);
   }
}

// File browser for opening input files for selection cuts
void TGAppMainFrame::SelectCutsDirectory()
{
   int i = cutsInputList->GetNumberOfEntries();

   TGFileInfo file_info;
   const char *filetypes[] = {"ADST files for cuts",histextall,0,0};
   file_info.fFileTypes = filetypes;
   file_info.fIniDir = StrDup(currentCutsInputDir);
   file_info.fMultipleSelection = kTRUE;
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);

   TList *files = file_info.fFileNamesList;
   if(files)
   {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while(file=(TSystemFile*)next())
      {
         fname = file->GetName();
	 if(CheckFormat(fname) > -1)
	 {
	    remove_from_last((char*)fname.Data(), '/', currentCutsInputDir);
            cutsInputList->AddEntry(fname.Data(), i);
	    i++;
	 }
      }
   }
   cutsInputList->Layout();

   for(int i = 0; i < cutsInputList->GetNumberOfEntries(); i++) 
      cutsInputList->Select(i,kTRUE);
}

// Apply the cuts to files
void TGAppMainFrame::CutsApply(int opt)
{
   if(opt == 0)
   {
      unsigned int nrfiles = cutsInputList->GetNumberOfEntries();
      int ret;

      if(nrfiles > 0)
      {
         TGFileInfo file_info;
         const char *filetypes[] = {"ADST output (applied cuts)",histextall,0,0};
         file_info.fFileTypes = filetypes;
         file_info.fIniDir = StrDup(currentCutsDir);
         new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);

         if(file_info.fFilename != NULL)
         {
            remove_from_last(file_info.fFilename, '/', currentCutsDir);

            AdstMva *mvatool = new AdstMva();
cout << "TODO 1" << endl;
            mvatool->outname = string(file_info.fFilename);
            vector<string> dataFileNames;
cout << "TODO 2" << endl;

            TList *files;
            files = new TList();
            cutsInputList->GetSelectedEntries(files);
            nrfiles = files->GetSize();
cout << "TODO 3" << endl;
         
            if( (files) && (nrfiles > 0) )
            {
               for(int i = 0; i < (int)nrfiles; i++)
               {
                  if(files->At(i))
                     dataFileNames.push_back( string(files->At(i)->GetTitle()) );
               }
cout << "TODO 4" << endl;

//               mvatool->cutinFile(dataFileNames);
cout << "TODO 5" << endl;
               
               if(string(configFile->widgetTE->GetText()) != string("No file selected!"))
                  mvatool->SelectEvents( string(configFile->widgetTE->GetText()), dataFileNames);
               else
                  new TGMsgBox(gClient->GetRoot(), fMain, "Error opening configure file", "Please select a configure file to be used for this cut selection.", kMBIconExclamation, kMBOk, &ret);
cout << "TODO 6" << endl;

//             RecEventFile* outputFile = new RecEventFile((mvatool->outname).c_str(), RecEventFile::eWrite);

               delete mvatool;
            }
         }
      }
   }
   else if(opt == 1)
   {
      configFile->widgetTE->SetText("No file selected!");
      sdcutsFile->widgetTE->SetText("No file selected!");
      fdcutsFile->widgetTE->SetText("No file selected!");
      cutsInputList->RemoveAll();
   }
}

/*
// Start a measurement (acquisition from CAMAC)
void TGAppMainFrame::RunMeas(void *ptr, int runCase, int &scanon)
{
   int vscan = 0, pscan = 0, zscan = 0, ascan = 0;
   if(scansOn->widgetChBox[0]->IsDown()) vscan = 1;
   if(scansOn->widgetChBox[1]->IsDown()) pscan = 1;
   if(scansOn->widgetChBox[2]->IsDown()) zscan = 1;
   if(scansOn->widgetChBox[3]->IsDown()) ascan = 1;

   printf("Start of Run, run case %d\n", runCase);

   float progVal;

   char ctemp[256];
   char ctemp2[256];
   char fname[256];
   int itemp = 0;
   TH1F *liveHist;

   float minVoltage, maxVoltage, stepVoltage, diffVoltage;
   float minXpos, maxXpos, stepXpos, diffXpos;
   float minYpos, maxYpos, stepYpos, diffYpos;
   float minZpos, maxZpos, stepZpos, diffZpos;
   float minAlpha, maxAlpha, stepAlpha, diffAlpha;

   minVoltage = vOutStart->widgetNE[0]->GetNumber();
   maxVoltage = vOutStop->widgetNE[0]->GetNumber();
   diffVoltage = abs(maxVoltage - minVoltage);
   stepVoltage = abs(vOutStep->widgetNE[0]->GetNumber());
   minXpos = xPosMin->widgetNE[0]->GetNumber();
   maxXpos = xPosMax->widgetNE[0]->GetNumber();
   diffXpos = abs(maxXpos - minXpos);
   stepXpos = abs(xPosStep->widgetNE[0]->GetNumber());
   minYpos = yPosMin->widgetNE[0]->GetNumber();
   maxYpos = yPosMax->widgetNE[0]->GetNumber();
   diffYpos = abs(maxYpos - minYpos);
   stepYpos = abs(yPosStep->widgetNE[0]->GetNumber());
   minZpos = zPosMin->widgetNE[0]->GetNumber();
   maxZpos = zPosMax->widgetNE[0]->GetNumber();
   diffZpos = abs(maxZpos - minZpos);
   stepZpos = abs(zPosStep->widgetNE[0]->GetNumber());
   minAlpha = rotPosMin->widgetNE[0]->GetNumber();
   maxAlpha = rotPosMax->widgetNE[0]->GetNumber();
   diffAlpha = abs(maxAlpha - minAlpha);
   stepAlpha = abs(rotPosStep->widgetNE[0]->GetNumber());

   remove_ext((char*)fileName->widgetTE->GetText(), ctemp);

   // TODO - angle scan + voltage scan
   // Voltage or surface scan
   if( vscan || pscan || ascan )
   {
      // No Z scan, No angle scan
      if(!zscan && !ascan)
      {
	 // When we have a voltage scan
	 if( vscan && (stepVoltage > 0.) )
 	    SeqNumber(runCase, (int)diffVoltage/stepVoltage, ctemp2);
         // With only a surface scan
         else if(pscan)
	 {
            if( stepXpos == 0 )
	       itemp = 1;
	    else
	       itemp = (int)diffXpos/stepXpos;

	    if( stepYpos == 0 )
	       itemp *= 1;
	    else
	       itemp *= (int)diffYpos/stepYpos;
	    SeqNumber(runCase, itemp, ctemp2);
	 }
         sprintf(fname, "%s_%s%s", ctemp, ctemp2, histext);
      }
      // With Z scan, No angle scan
      else if(zscan && !ascan)
      {
         SeqNumber((int)zPos->widgetNE[0]->GetNumber(), maxZpos, ctemp2);

	 // Voltage scan is on
	 if( vscan && (stepVoltage > 0.) )
	 {
	    sprintf(fname, "%s_z%s_", ctemp, ctemp2);
	    SeqNumber(runCase, (int)diffVoltage/stepVoltage+1, ctemp2);
	    strcat(fname, ctemp2);
	    strcat(fname, histext);
	 }
	 // Surface scan is on
	 else if(pscan)
	 {
	    sprintf(fname, "%s_z%s_", ctemp, ctemp2);

            if( stepXpos == 0 )
	       itemp = 1;
	    else
	       itemp = (int)diffXpos/stepXpos+1;

	    if( stepYpos == 0 )
	       itemp *= 1;
	    else
	       itemp *= (int)diffYpos/stepYpos+1;
	    SeqNumber(runCase, itemp, ctemp2);
	    strcat(fname, ctemp2);
	    strcat(fname, histext);
	 }
	 // Just Z scan
	 else
	    sprintf(fname, "%s_z%s%s", ctemp, ctemp2, histext);
      }
      // No Z scan, With angle scan
      else if(!zscan && ascan)
      {
         SeqNumber(runCase, (int)diffAlpha/stepAlpha, ctemp2);

	 // Voltage scan is on
	 if( vscan && (stepVoltage > 0.) )
	 {
	    sprintf(fname, "%s_phi%s_", ctemp, ctemp2);
	    SeqNumber(runCase, (int)diffVoltage/stepVoltage+1, ctemp2);
	    strcat(fname, ctemp2);
	    strcat(fname, histext);
	 }
	 // Just angle scan
	 else
	    sprintf(fname, "%s_phi%s%s", ctemp, ctemp2, histext);
      }
   }
   // All the rest
   else if(!vscan && !pscan)
      sprintf(fname, "%s%s", ctemp, histext);

   // Check if set voltage is below the hard limit
   if( vOut->widgetNE[0]->GetNumber() > vHardlimit->widgetNE[0]->GetNumber() )
   {
      printf("Voltage hard limit triggered (%lf > %lf)!\n", vOut->widgetNE[0]->GetNumber(), vHardlimit->widgetNE[0]->GetNumber() );
      vOut->widgetNE[0]->SetNumber( vHardlimit->widgetNE[0]->GetNumber() );
   }

   printf("Output file is (runCase = %d): %s\n", runCase, fname);

   // Writing to output file
   outroot = TFile::Open(fname, "RECREATE");

   TTree *header_data = new TTree("header_data", "Header information for the measurement.");
   TTree *meas_data = new TTree("meas_data", "Saved ADC and TDC measurement data.");
   TTree *scope_data = new TTree("scope_data", "Saved scope measurement data.");

   // Branches for the header
   header_data->Branch("nrch", &evtheader.nrch, "nrch/I");
   header_data->Branch("timestamp", &evtheader.timestamp, "timestamp/I");
   header_data->Branch("biasvolt", &evtheader.biasvolt, "biasvolt/D");
   header_data->Branch("xpos", &evtheader.xpos, "xpos/I");
   header_data->Branch("ypos", &evtheader.ypos, "ypos/I");
   header_data->Branch("zpos", &evtheader.zpos, "zpos/I");
   header_data->Branch("temperature", &evtheader.temperature, "temperature/D");
   header_data->Branch("angle", &evtheader.angle, "angle/D");
   header_data->Branch("laserinfo", &evtheader.laserinfo, "laserinfo/C");

   evtheader.nrch = (int)NCH->widgetNE[0]->GetNumber()*2;
   evtheader.timestamp = (int)time(NULL);
   evtheader.biasvolt = (double)vOut->widgetNE[0]->GetNumber();
   if(posUnits->widgetCB->GetSelected() == 0)
   {
      evtheader.xpos = (int)xPos->widgetNE[0]->GetNumber();
      evtheader.ypos = (int)yPos->widgetNE[0]->GetNumber();
      evtheader.zpos = (int)zPos->widgetNE[0]->GetNumber();
   }
   else if(posUnits->widgetCB->GetSelected() == 1)
   {
      evtheader.xpos = (int)xPos->widgetNE[0]->GetNumber()/lenconversion;
      evtheader.ypos = (int)yPos->widgetNE[0]->GetNumber()/lenconversion;
      evtheader.zpos = (int)zPos->widgetNE[0]->GetNumber()/lenconversion;
   }
   evtheader.temperature = (double)chtemp->widgetNE[0]->GetNumber();
   if(rotUnits->widgetCB->GetSelected() == 0)
      evtheader.angle = (double)rotPos->widgetNE[0]->GetNumber()*rotconversion;
   else if(rotUnits->widgetCB->GetSelected() == 1)
      evtheader.angle = (double)rotPos->widgetNE[0]->GetNumber();
   sprintf(evtheader.laserinfo, "%s", laserInfo->widgetTE->GetText());

   char histtime[256];
   GetTime(evtheader.timestamp, histtime);

   printf("Save file header information:\n");
   printf("- Number of channels: %d\n", evtheader.nrch);
   printf("- Timestamp: %d (%s)\n", evtheader.timestamp, histtime);
   printf("- Bias voltage: %lf\n", evtheader.biasvolt);
   printf("- Table position (X,Y,Z): %d, %d, %d\n", evtheader.xpos, evtheader.ypos, evtheader.zpos);
   printf("- Temperature: %lf\n", evtheader.temperature);
   printf("- Incidence angle: %lf\n", evtheader.angle);
   printf("- Laser and filter settings: %s\n", evtheader.laserinfo);

   header_data->Fill();

   // Branches for ADC and TDC data
   for(int i = 0; i < evtheader.nrch/2; i++)
   {
      sprintf(ctemp, "ADC%d", i);
      sprintf(fname, "ADC%d/I", i);
      meas_data->Branch(ctemp, &evtdata.adcdata[i], fname);

      sprintf(ctemp, "TDC%d", i);
      sprintf(fname, "TDC%d/I", i);
      meas_data->Branch(ctemp, &evtdata.tdcdata[i], fname);
   }

   int neve  = (int) evtNum->widgetNE[0]->GetNumber();
   int allEvt, zProg;
   zProg = 1;

#if WORKSTAT == 'I'
#else
// ONLY FOR TESTING!
   TRandom *randNum = new TRandom();
   randNum->SetSeed(0);
// ONLY FOR TESTING!
#endif

   // Initialize the CAMAC
   if (gDaq)
   {
      if(scanon == 0)
      {
         gDaq->init(evtheader.nrch);
	 scanon = 1;
      }
      gDaq->fStop=0;

      // Set the stopwatch
      clock_t clkt1;

      // Prepare histogram for live histogram update
      int liven;
      TCanvas *gCanvas;
      if(liveUpdate && (!vscan && !pscan && !zscan && !ascan))
      {
         gCanvas = measCanvas->GetCanvas();
	 gCanvas->SetGrid();
	 gCanvas->cd();
	 liveHist = new TH1F(histname,"",(int)TMath::Sqrt(neve),0,0);
	 liven = 1;
      }

      // Start gathering
      gDaq->start();

      for (int n=0;n<neve && !gDaq->fStop ;)
      {
         int nb = gDaq->event(gBuf,BSIZE);

#if WORKSTAT == 'I'
#else
// ONLY FOR TESTING!
         for(int i=0; i < evtheader.nrch; i++)
	 {
            if(i == 1)
               gBuf[i] = randNum->Gaus(1500,300);
            else if(i == 0)
               gBuf[i] = randNum->Poisson(2500);
         }
// ONLY FOR TESTING!
#endif
         if (nb<=0) n--;

         int nc=0;

         while ( (nb>0) && (n<neve) )
         {
            for(int i = 0; i < evtheader.nrch; i++)
	    {
	       unsigned short adc = gBuf[i+nc]&0xFFFF;
	       if(i % 2 == 0)		// TDC
	          evtdata.tdcdata[i/2] = (int)adc;
	       else if(i % 2 == 1)	// ADC
	          evtdata.adcdata[i/2] = (int)adc;

	       // Start plotting the scope waveform
	    }
	    meas_data->Fill();
	    n++;

            // Start filling the histogram (only in normal single scan)
            if(liveUpdate && (!vscan && !pscan && !zscan && !ascan))
            {
	       liveHist->Fill(evtdata.adcdata[0]);
	       if( n == (neve*liven)/10 )
	       {
                  gCanvas->cd();
		  liveHist->Draw("");
		  gCanvas->Modified();
		  gCanvas->Update();
                  liven++;
	       }
            }
	    
            nc += evtheader.nrch;
            nb -= evtheader.nrch;
	 }

         MyTimer();
         allEvt = n;
         if (gSystem->ProcessEvents()) printf("Run Interrupted\n");

         if( acqStarted && (n == (neve*zProg)/10) && (!vscan && !pscan && !zscan && !ascan) )
         {
	    // Progress the progress bar
            progVal = (float)zProg*10;
            measProgress->widgetPB->SetPosition(progVal);

	    // Calculate the remaining time
	    TimeEstimate(clkt0, timet0, progVal, ctemp, 0);
	    printf("End time: %s\n", ctemp);
	    measProgress->widgetTE->SetText(ctemp);

	    gVirtualX->Update(1);
            zProg++;
         }
      }

      printf("Number of gathered events: %d\n", allEvt);
      measProgress->widgetTB[0]->SetText("Start acquisition");
      acqStarted = false;

      gDaq->stop();
   }

   printf("End of Run neve=%d\n",neve);

   header_data->Write();
   meas_data->Write();
//   scope_data->Write(); // TODO
   delete header_data;
   delete meas_data;
   delete scope_data;

   // Remove the histogram
   if(liveUpdate && (!vscan && !pscan && !zscan && !ascan))
      delete liveHist;


   outroot->Close();
}

int TGAppMainFrame::MyTimer()
{
   char cmd[100];
   GetTime(-1, cmd);
   if (timeStamp) timeStamp->widgetTE->SetText(cmd);
   return 0;
}

// Additional functions -------------------------------------

// Settings pane connections --------------------------------

// Enable or disable scans
void TGAppMainFrame::EnableScan(int type)
{
   // Voltage scan
   if(type == 0)
   {
      if(scansOn->widgetChBox[type]->IsOn())
      {
         vOutStart->widgetNE[0]->SetState(kTRUE);
         vOutStop->widgetNE[0]->SetState(kTRUE);
         vOutStep->widgetNE[0]->SetState(kTRUE);
      }
      else
      {
         vOutStart->widgetNE[0]->SetState(kFALSE);
         vOutStop->widgetNE[0]->SetState(kFALSE);
         vOutStep->widgetNE[0]->SetState(kFALSE);
      }
   }
   // Surface (X, Y axis) scan
   else if(type == 1)
   {
      if(scansOn->widgetChBox[type]->IsOn())
      {
         xPosMin->widgetNE[0]->SetState(kTRUE);
         xPosMax->widgetNE[0]->SetState(kTRUE);
         xPosStep->widgetNE[0]->SetState(kTRUE);
         yPosMin->widgetNE[0]->SetState(kTRUE);
         yPosMax->widgetNE[0]->SetState(kTRUE);
         yPosStep->widgetNE[0]->SetState(kTRUE);
      }
      else
      {
         xPosMin->widgetNE[0]->SetState(kFALSE);
         xPosMax->widgetNE[0]->SetState(kFALSE);
         xPosStep->widgetNE[0]->SetState(kFALSE);
         yPosMin->widgetNE[0]->SetState(kFALSE);
         yPosMax->widgetNE[0]->SetState(kFALSE);
         yPosStep->widgetNE[0]->SetState(kFALSE);
      }
   }
   // Z axis scan
   else if(type == 2)
   {
      if(scansOn->widgetChBox[type]->IsOn())
      {
         zPosMin->widgetNE[0]->SetState(kTRUE);
         zPosMax->widgetNE[0]->SetState(kTRUE);
         zPosStep->widgetNE[0]->SetState(kTRUE);
      }
      else
      {
         zPosMin->widgetNE[0]->SetState(kFALSE);
         zPosMax->widgetNE[0]->SetState(kFALSE);
         zPosStep->widgetNE[0]->SetState(kFALSE);
      }
   }
   // Incidence angle scan
   else if(type == 3)
   {
      if(scansOn->widgetChBox[type]->IsOn())
      {
         rotPosMin->widgetNE[0]->SetState(kTRUE);
         rotPosMax->widgetNE[0]->SetState(kTRUE);
         rotPosStep->widgetNE[0]->SetState(kTRUE);
      }
      else
      {
         rotPosMin->widgetNE[0]->SetState(kFALSE);
         rotPosMax->widgetNE[0]->SetState(kFALSE);
         rotPosStep->widgetNE[0]->SetState(kFALSE);
      }
   }
}

// Apply the upper voltage limit from settings pane to main window
void TGAppMainFrame::VoltageLimit()
{
   vOut->widgetNE[0]->SetLimitValues(0, vHardlimit->widgetNE[0]->GetNumber() );
}

// Select the table position units to be used (1 = 0.3595 micron)
void TGAppMainFrame::ChangeUnits(int type)
{
   int pos[12], poslim[3], chng = 0;
   double micro[12], microlim[3];

   TGNumberEntry *posEntries[12];
   posEntries[0] = (TGNumberEntry*)xPos->widgetNE[0];
   posEntries[1] = (TGNumberEntry*)yPos->widgetNE[0];
   posEntries[2] = (TGNumberEntry*)zPos->widgetNE[0];
   posEntries[3] = (TGNumberEntry*)xPosMin->widgetNE[0];
   posEntries[4] = (TGNumberEntry*)xPosMax->widgetNE[0];
   posEntries[5] = (TGNumberEntry*)xPosStep->widgetNE[0];
   posEntries[6] = (TGNumberEntry*)yPosMin->widgetNE[0];
   posEntries[7] = (TGNumberEntry*)yPosMax->widgetNE[0];
   posEntries[8] = (TGNumberEntry*)yPosStep->widgetNE[0];
   posEntries[9] = (TGNumberEntry*)zPosMin->widgetNE[0];
   posEntries[10] = (TGNumberEntry*)zPosMax->widgetNE[0];
   posEntries[11] = (TGNumberEntry*)zPosStep->widgetNE[0];

   // Table position values
   if(type == 0)
   {
      // Check if we had microns before
      if(posEntries[0]->GetNumStyle() == TGNumberFormat::kNESRealTwo)
         chng = 1;

      // Change to table position values   
      if(chng == 1)
      {
         for(int i = 0; i < 12; i++)
         {
            if(posEntries[i]->GetNumber() == 0.0)
               pos[i] = 0;
            else
               pos[i] = (int)posEntries[i]->GetNumber()/lenconversion;
         }

         poslim[0] = -100;
         poslim[1] = 215000;
         poslim[2] = 375000;

         for(int i = 0; i < 12; i++)
         {
            posEntries[i]->SetNumStyle(TGNumberFormat::kNESInteger);
            if( (i > 8) || (i == 2) ) // limits for Z axis (longer table)
               posEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, poslim[0], poslim[2]);
            else
               posEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, poslim[0], poslim[1]);

            posEntries[i]->SetNumber(pos[i]);
         }
      }
   }
   // Microns
   else if(type == 1)
   {
      // Check if we had table position values before
      if(posEntries[0]->GetNumStyle() == TGNumberFormat::kNESInteger)
         chng = 1;

      // Change to microns   
      if(chng == 1)
      {
         for(int i = 0; i < 12; i++)
         {
            if(posEntries[i]->GetNumber() == 0.0)
               micro[i] = 0.;
            else
               micro[i] = (double)posEntries[i]->GetNumber()*lenconversion;
         }
    
         microlim[0] = (double)-100*lenconversion;
         microlim[1] = (double)215000*lenconversion;
         microlim[2] = (double)375000*lenconversion;
    
         for(int i = 0; i < 12; i++)
         {
            posEntries[i]->SetNumStyle(TGNumberFormat::kNESRealTwo);
            if( (i > 8) || (i == 2) ) // limits for Z axis (longer table)
               posEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, microlim[0], microlim[2]);
            else
               posEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, microlim[0], microlim[1]);
    
            posEntries[i]->SetNumber(micro[i]);
         }
      }
   }
}

// Select the rotation units to be used (1 = 6.3281/3600 degrees)
void TGAppMainFrame::ChangeUnitsRot(int type)
{
   int rot[4], rotlim[2], chng = 0;
   double deg[4], deglim[2];

   TGNumberEntry *rotEntries[4];
   rotEntries[0] = (TGNumberEntry*)rotPos->widgetNE[0];
   rotEntries[1] = (TGNumberEntry*)rotPosMin->widgetNE[0];
   rotEntries[2] = (TGNumberEntry*)rotPosMax->widgetNE[0];
   rotEntries[3] = (TGNumberEntry*)rotPosStep->widgetNE[0];

   // Rotation values
   if(type == 0)
   {
      // Check if we had degrees before
      if(rotEntries[0]->GetNumStyle() == TGNumberFormat::kNESRealTwo)
         chng = 1;

      // Change to rotation values   
      if(chng == 1)
      {
         for(int i = 0; i < 4; i++)
         {
            if(rotEntries[i]->GetNumber() == 0.0)
               rot[i] = 0;
            else
               rot[i] = (int)rotEntries[i]->GetNumber()/rotconversion;
         }

         rotlim[0] = (int)-180/rotconversion;
         rotlim[1] = (int)180/rotconversion;

         for(int i = 0; i < 4; i++)
         {
            rotEntries[i]->SetNumStyle(TGNumberFormat::kNESInteger);
            rotEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, rotlim[0], rotlim[1]);

            rotEntries[i]->SetNumber(rot[i]);
         }
      }
   }
   // Degree
   else if(type == 1)
   {
      // Check if we had table position values before
      if(rotEntries[0]->GetNumStyle() == TGNumberFormat::kNESInteger)
         chng = 1;

      // Change to degrees   
      if(chng == 1)
      {
         for(int i = 0; i < 4; i++)
         {
            if(rotEntries[i]->GetNumber() == 0)
               deg[i] = 0.;
            else
               deg[i] = (double)rotEntries[i]->GetNumber()*rotconversion;
         }
    
         deglim[0] = -180.;
         deglim[1] = 180.;
    
         for(int i = 0; i < 4; i++)
         {
            rotEntries[i]->SetNumStyle(TGNumberFormat::kNESRealTwo);
            rotEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, deglim[0], deglim[1]);
    
            rotEntries[i]->SetNumber(deg[i]);
         }
      }
   }
}

// Enable display canvas to have a live update of histogram
void TGAppMainFrame::EnableLiveUpdate()
{
   liveUpdate = liveDisp->widgetChBox[0]->IsDown();
}

// Settings pane connections --------------------------------

// Main measurement window connections ----------------------

// Get the currently selected channel
int TGAppMainFrame::GetChannel()
{
   int selectedOutput;
   if(vOutCh->widgetCB->GetSelected() < 8) selectedOutput = (vOutCh->widgetCB->GetSelected())+1;
   else if( (vOutCh->widgetCB->GetSelected() >= 8) && (vOutCh->widgetCB->GetSelected() < 16) ) selectedOutput = (vOutCh->widgetCB->GetSelected())+93;
   else selectedOutput = 1;

   return selectedOutput;
}

// Set, get or reset the output voltage
void TGAppMainFrame::VoltOut(int opt)
{
   char cmd[256];

   // Set the selected voltage
   if(opt == 0)
   {
      int outOn;
      float outputVoltage;
      
      outputVoltage = vOut->widgetNE[0]->GetNumber();
    
      if(vOutOpt->widgetChBox[1]->IsOn()) outOn = 1;
      else outOn = 0;
     
      fflush(stdout);
      sprintf(cmd, "%s/src/mpod/mpod_voltage.sh -o %d -v %f -s %d", rootdir, GetChannel(), outputVoltage, outOn);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      fflush(stdout);
   }
   // Get current voltage
   else if(opt == 1)
   {
      fflush(stdout);
      sprintf(cmd, "%s/src/mpod/mpod_voltage.sh -o %d -g > %s/settings/curvolt.txt", rootdir, GetChannel(), rootdir);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      fflush(stdout);

#if WORKSTAT == 'I'
      FILE* fvolt;
      double dtemp;
      char ctemp[24];
      sprintf(cmd, "%s/settings/curvolt.txt", rootdir);
      fvolt = fopen(cmd, "r");
      
      if(fvolt != NULL)
      {
         sprintf(cmd, "WIENER-CRATE-MIB::outputVoltage.u%d = Opaque: Float: %s V\n", GetChannel()-1, "%lf" );
         retTemp = fscanf(fvolt, cmd, &dtemp);
         vOut->widgetNE[0]->SetNumber(dtemp);
         sprintf(cmd, "WIENER-CRATE-MIB::outputSwitch.u%d = INTEGER: %s\n", GetChannel()-1, "%s" );
         retTemp = fscanf(fvolt, cmd, ctemp);
         if( strcmp(ctemp, "On(1)") == 0 )
            vOutOpt->widgetChBox[1]->SetState(kButtonDown);
         else if( strcmp(ctemp, "Off(0)") == 0 )
            vOutOpt->widgetChBox[1]->SetState(kButtonUp);
      }
    
      fclose(fvolt);
#endif
   }
   // Reset output voltage (if stuck in interlock)
   else if(opt == 2)
   {
      vOut->widgetNE[0]->SetNumber(0.000);
      vOutOpt->widgetChBox[1]->SetState(kButtonUp);

      fflush(stdout);
      sprintf(cmd, "%s/src/mpod/mpod_voltage.sh -r %d", rootdir, GetChannel());
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      fflush(stdout);
   }
}

// Set output voltage polarity to negative
void TGAppMainFrame::NegativePolarity()
{
   double newHardlimit;
   int polar = 0;  // 0 = positive, 1 = negative

   if(vOutOpt->widgetChBox[0]->IsOn())
      polar = 1; 
   else
      polar = 0; 

   // Set hard limit to the negative version of what it was before
   if( (vHardlimit->widgetNE[0]->GetNumber() > 0.) && (polar == 1) )
      newHardlimit = -(vHardlimit->widgetNE[0]->GetNumber());
   else if( (vHardlimit->widgetNE[0]->GetNumber() < 0.) && (polar == 0) )
      newHardlimit = -(vHardlimit->widgetNE[0]->GetNumber());
   else if(vHardlimit->widgetNE[0]->GetNumber() == 0.)
      newHardlimit = 0.;
   else
      newHardlimit = vHardlimit->widgetNE[0]->GetNumber();

   // Apropriately set the limit to the output voltage number entry
   vHardlimit->widgetNE[0]->SetNumber(newHardlimit);

   if(polar == 1)
      vOut->widgetNE[0]->SetLimits(TGNumberFormat::kNELLimitMinMax, newHardlimit, 0.);
   else if(polar == 0)
      vOut->widgetNE[0]->SetLimits(TGNumberFormat::kNELLimitMinMax, 0., newHardlimit);
}

// Set, get, home or reset the table position
void TGAppMainFrame::PositionSet(int opt)
{
   char cmd[1024];

   // Set the selected table position
   if(opt == 0)
   {
      int positX, positY, positZ;
  
      if(posUnits->widgetCB->GetSelected() == 0)
      {
         positX = xPos->widgetNE[0]->GetNumber();
         positY = yPos->widgetNE[0]->GetNumber();
         positZ = zPos->widgetNE[0]->GetNumber();
      }
      else if(posUnits->widgetCB->GetSelected() == 1)
      {
         positX = (int)xPos->widgetNE[0]->GetNumber()/lenconversion;
         positY = (int)yPos->widgetNE[0]->GetNumber()/lenconversion;
         positZ = (int)zPos->widgetNE[0]->GetNumber()/lenconversion;
      }
  
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 1 -c m", rootdir, positX, rootdir);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
  
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 2 -c m", rootdir, positY, rootdir);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
  
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 3 -c m", rootdir, positZ, rootdir);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
   }
   // Get current table position
   else if(opt == 1)
   {
      fflush(stdout);
   
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -p > %s/settings/curpos.txt", rootdir, rootdir);	// X-axis
      fflush(stdout);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
   
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -p >> %s/settings/curpos.txt", rootdir, rootdir);	// Y-axis
      fflush(stdout);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
   
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -p >> %s/settings/curpos.txt", rootdir, rootdir);	// Z-axis
      fflush(stdout);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

#if WORKSTAT == 'I'
      FILE* fpos;
      int itemp;
      sprintf(cmd, "%s/settings/curpos.txt", rootdir);
      fpos = fopen(cmd, "r");
      
      if(fpos != NULL)
      {
         if(posUnits->widgetCB->GetSelected() == 0)
         {
            retTemp = fscanf(fpos, "%d\n", &itemp);
            xPos->widgetNE[0]->SetNumber(itemp);
            retTemp = fscanf(fpos, "%d\n", &itemp);
            yPos->widgetNE[0]->SetNumber(itemp);
            retTemp = fscanf(fpos, "%d\n", &itemp);
            zPos->widgetNE[0]->SetNumber(itemp);
	 }
	 else if(posUnits->widgetCB->GetSelected() == 1)
         {
            retTemp = fscanf(fpos, "%d\n", &itemp);
            xPos->widgetNE[0]->SetNumber((double)itemp*lenconversion);
            retTemp = fscanf(fpos, "%d\n", &itemp);
            yPos->widgetNE[0]->SetNumber((double)itemp*lenconversion);
            retTemp = fscanf(fpos, "%d\n", &itemp);
            zPos->widgetNE[0]->SetNumber((double)itemp*lenconversion);
	 }
      }

      fclose(fpos);
#endif
   }
   // Home the table position
   else if(opt == 2)
   {
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -h", rootdir);	// X-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -h", rootdir);	// Y-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -h", rootdir);	// Z-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      PositionSet(1);
   }
   // Reset the table position
   else if(opt == 3)
   {
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -r && sudo %s/src/MIKRO/mikro_ctrl -n 1 -i 3 && sudo %s/src/MIKRO/mikro_ctrl -n 1 -h", rootdir, rootdir, rootdir);	// X-axis
#if WORKSTAT == 'I'
      printf("Positioning table reset, initialization and homing in progress. Please wait...\n");
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -r && sudo %s/src/MIKRO/mikro_ctrl -n 2 -i 3 && sudo %s/src/MIKRO/mikro_ctrl -n 2 -h", rootdir, rootdir, rootdir);	// Y-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -r && sudo %s/src/MIKRO/mikro_ctrl -n 3 -i 3 && sudo %s/src/MIKRO/mikro_ctrl -n 3 -h", rootdir, rootdir, rootdir);	// Z-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
      printf("Positioning table reset, initialization and homing complete.\n");
#else
      printf("Cmd: %s\n",cmd);
#endif
      PositionSet(1);
   }
   // Abort any motion
   else if(opt == 4)
   {
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -c ab", rootdir);	// X-axis
#if WORKSTAT == 'I'
      printf("Emergency stop of the current movement of all linear tables.\n");
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -c ab", rootdir);	// Y-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -c ab", rootdir);	// Z-axis
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      PositionSet(1);
   }
}

// Set, get, home or reset the rotation platform
void TGAppMainFrame::RotationSet(int opt)
{
   char cmd[1024];

   // Set the selected rotation
   if(opt == 0)
   {
      int positAlpha;
  
      if(rotUnits->widgetCB->GetSelected() == 0)
         positAlpha = rotPos->widgetNE[0]->GetNumber();
      else if(rotUnits->widgetCB->GetSelected() == 1)
         positAlpha = rotPos->widgetNE[0]->GetNumber()/rotconversion;
  
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 4 -c m", rootdir, positAlpha, rootdir);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
   }
   // Get current rotation
   else if(opt == 1)
   {
      fflush(stdout);
   
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -p > %s/settings/currot.txt", rootdir, rootdir);
      fflush(stdout);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif

#if WORKSTAT == 'I'
      FILE* frot;
      int itemp;
      sprintf(cmd, "%s/settings/currot.txt", rootdir);
      frot = fopen(cmd, "r");

      if(frot != NULL)
      {
         retTemp = fscanf(frot, "%d\n", &itemp);
         if(rotUnits->widgetCB->GetSelected() == 0)
            rotPos->widgetNE[0]->SetNumber(itemp);
         else if(rotUnits->widgetCB->GetSelected() == 1)
            rotPos->widgetNE[0]->SetNumber((double)itemp*rotconversion);
      }

      fclose(frot);
#endif
   }
   // Home the rotation
   else if(opt == 2)
   {
      // TODO: For now only set back to 0, not home!
//      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -h", rootdir);
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -v 0 -s la && %s/src/MIKRO/mikro_ctrl -n 4 -c m", rootdir, rootdir);
#if WORKSTAT == 'I'
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      RotationSet(1);
   }
   // Reset the rotation
   else if(opt == 3)
   {
      // TODO: For now only set back to 0, not home!
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -r && sudo %s/src/MIKRO/mikro_ctrl -n 4 -i 2 && sudo %s/src/MIKRO/mikro_ctrl -n 4 -h", rootdir, rootdir, rootdir);
#if WORKSTAT == 'I'
      printf("Rotation platform reset, initalization and homing in progress. Please wait...\n");
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -v 0 -s la && %s/src/MIKRO/mikro_ctrl -n 4 -c m", rootdir, rootdir);
      retTemp = system(cmd);
      sleep(15);	// wait for the motor to change position from wherever to 0
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -r && sudo %s/src/MIKRO/mikro_ctrl -n 4 -i 2", rootdir, rootdir);
      retTemp = system(cmd);
      printf("Rotation platform reset, initalization and homing complete.\n");
#else
      printf("Cmd: %s\n",cmd);
#endif
      RotationSet(1);
   }
   // Abort any motion
   else if(opt == 4)
   {
      sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -c ab", rootdir);
#if WORKSTAT == 'I'
      printf("Emergency stop of the current movement of the rotation platform.\n");
      retTemp = system(cmd);
#else
      printf("Cmd: %s\n",cmd);
#endif
      RotationSet(1);
   }
}

// File browser for selecting the save file
void TGAppMainFrame::SaveFile()
{
//   char *cTemp;

   TGFileInfo file_info;
   const char *filetypes[] = {"Histograms",histextall,0,0};
   file_info.fFileTypes = filetypes;
//   cTemp = new char[1024];
//   sprintf(cTemp, "%s/results", rootdir);
//   file_info.fIniDir = StrDup(cTemp);
   file_info.fIniDir = StrDup(currentMeasDir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);
//   delete[] cTemp;

   if(file_info.fFilename != NULL)
   {
      fileName->widgetTE->SetText(file_info.fFilename);
      remove_from_last(file_info.fFilename, '/', currentMeasDir);
   }
}

// Start the acquisition
void TGAppMainFrame::StartAcq()
{
   // Save measurement settings any time we run a new measurement
   SaveMeasSettings();

   // Variable that will initialize camac only once (for scans)
   int scanon = 0;

   // Determine the type of measurement to perform
   int vscan = 0, pscan = 0, zscan = 0, ascan = 0;
   if(scansOn->widgetChBox[0]->IsDown()) vscan = 1;
   if(scansOn->widgetChBox[1]->IsDown()) pscan = 1;
   if(scansOn->widgetChBox[2]->IsDown()) zscan = 1;
   if(scansOn->widgetChBox[3]->IsDown()) ascan = 1;

   char cmd[256];
   int i, j, k;
   float progVal;
   FILE *pfin;

   // Variables for voltage scan
   float currentVoltage, minVoltage, maxVoltage, stepVoltage;
   int repetition;

   // Variables for surface scan and Z axis scan
   float minXpos, maxXpos, stepXpos;
   float minYpos, maxYpos, stepYpos;
   float minZpos, maxZpos, stepZpos;
   int repetX, repetY, repetZ;

   // Variables for angle scan
   float currentAlpha, minAlpha, maxAlpha, stepAlpha;
   int repetAlpha;

   // Only voltage scan
   if( (vscan == 1) && (pscan == 0) && (ascan == 0) )
   { // TODO - include possibility to make voltage and angle scan at same time
      // If already started, stop the acquisition
      if(acqStarted)
      {
         printf("Stopping current voltage scan...\n");
	 gROOT->SetInterrupt();
	 measProgress->widgetTB[0]->SetText("Start acquisition");
	 acqStarted = false;

         // Write information to the finish_sig.txt value
	 sprintf(cmd, "%s/dbg/finish_sig.txt", rootdir);
	 pfin = fopen(cmd,"w");
	 fprintf(pfin, "%s: Voltage scan stopped.", timeStamp->widgetTE->GetText());
	 fclose(pfin);
      }
      // If stopped, start the acquisition
      else if(!acqStarted)
      {
         printf("Running a voltage scan...\n");

	 // Check the steps
         minVoltage = vOutStart->widgetNE[0]->GetNumber();
         maxVoltage = vOutStop->widgetNE[0]->GetNumber();
         stepVoltage = vOutStep->widgetNE[0]->GetNumber();

	 if(stepVoltage == 0.)
	    repetition = 1;
	 else
	 {
	    // Example: min = 40, max = 70, step = 5 (in increasing steps)
	    if( (maxVoltage > minVoltage) && (stepVoltage > 0) )
	       repetition = ((maxVoltage - minVoltage)/stepVoltage)+1;
	    // Example: min = 70, max = 40, step = -5 (in decreasing steps)
	    else if( (maxVoltage < minVoltage) && (stepVoltage < 0) )
	       repetition = ((minVoltage - maxVoltage)/stepVoltage)-1;
	    // Example: min = 70, max = 70 (no scan)
	    else if( maxVoltage == minVoltage )
	       repetition = 1;
	    // If step is not correctly set, stop the acqusition
	    else
	    {
	       // TODO
               printf("Stopping current voltage scan...\n");
               gROOT->SetInterrupt();
               measProgress->widgetTB[0]->SetText("Start acquisition");
               acqStarted = false;
	       repetition = 0;
	    }
	 }

         if(DBGSIG) printf("StartAcq(): Voltage repetition (%lf,%lf,%lf) = %d\n", minVoltage, maxVoltage, stepVoltage, repetition);

	 i = 0;

	 // TODO - Setting button text and acqStarted do not work!
	 measProgress->widgetTB[0]->SetText("Stop acquisition");
	 acqStarted = true;
         progVal = 0.00;
         measProgress->widgetPB->SetPosition(progVal);
	 gVirtualX->Update(1);

	 clkt0 = clock();
	 timet0 = time(NULL);

         while(1)
         {
	    if( (repetition > 0) && (i == repetition) ) break;
	    else if( (repetition < 0) && (i == -repetition) ) break;
	    else if( repetition == 0 ) break;

            progVal = (float)(100.00/abs(repetition))*i;
            measProgress->widgetPB->SetPosition(progVal);

	    TimeEstimate(clkt0, timet0, progVal, cmd, singlewait*abs(repetition));
	    measProgress->widgetTE->SetText(cmd);

	    gVirtualX->Update(1);
        
            fflush(stdout);
            currentVoltage = minVoltage + stepVoltage*i;
            sprintf(cmd, "%s/src/mpod/mpod_voltage.sh -o %d -v %f -s 1", rootdir, GetChannel(), currentVoltage);
#if WORKSTAT == 'I'
            retTemp = system(cmd);
#else
            printf("Cmd: %s\n",cmd);
#endif
            fflush(stdout);
        
            printf("Waiting for voltage change...\n");
            sleep(singlewait);
            vOut->widgetNE[0]->SetNumber(currentVoltage);
	    gVirtualX->Update(1);
            printf("Continuing...\n");
        
            // Here comes function to start histogramming <<<<<<<<<<<<<<<<<<<<<<<<
            RunMeas((void*)0, i, scanon); // TODO
            fflush(stdout);

	    i++;
         }

         // Set output back to off
         fflush(stdout);
         printf("Measurement finished, returning to starting voltage...\n");
         sprintf(cmd, "%s/src/mpod/mpod_voltage.sh -o %d -v %f -s 1", rootdir, GetChannel(), minVoltage);
         vOut->widgetNE[0]->SetNumber(minVoltage);
#if WORKSTAT == 'I'
         retTemp = system(cmd);
#else
         printf("Cmd: %s\n",cmd);
#endif
         fflush(stdout);
        
         progVal = 100.00;
         measProgress->widgetPB->SetPosition(progVal);
         printf("\n");

         sprintf(cmd, "%s/dbg/finish_sig.txt", rootdir);
   	 pfin = fopen(cmd,"w");
         fprintf(pfin, "%s: Voltage scan finished.", timeStamp->widgetTE->GetText());
         fclose(pfin);

         measProgress->widgetTB[0]->SetText("Start acquisition");
         acqStarted = false;
      }
   }
   // Surface scan
   else if( (pscan == 1) && (vscan == 0) && (ascan == 0) )
   {
      // If already started, stop the acquisition
      if(acqStarted)
      {
         printf("Stopping current surface scan...\n");
	 gROOT->SetInterrupt();
	 measProgress->widgetTB[0]->SetText("Start acquisition");
	 acqStarted = false;

         // Write information to the finish_sig.txt value
	 sprintf(cmd, "%s/dbg/finish_sig.txt", rootdir);
	 pfin = fopen(cmd,"w");
	 fprintf(pfin, "%s: Surface scan stopped.", timeStamp->widgetTE->GetText());
	 fclose(pfin);
      }
      // If stopped, start the acquisition
      else if(!acqStarted)
      {
         printf("Running a surface scan...\n");

         minXpos = xPosMin->widgetNE[0]->GetNumber();
         maxXpos = xPosMax->widgetNE[0]->GetNumber();
         stepXpos = xPosStep->widgetNE[0]->GetNumber();
         minYpos = yPosMin->widgetNE[0]->GetNumber();
         maxYpos = yPosMax->widgetNE[0]->GetNumber();
         stepYpos = yPosStep->widgetNE[0]->GetNumber();
         minZpos = zPosMin->widgetNE[0]->GetNumber();
         maxZpos = zPosMax->widgetNE[0]->GetNumber();
         stepZpos = zPosStep->widgetNE[0]->GetNumber();

	 // Setting repetition for Z axis scan
         if(zscan == 1)
         {
            if(stepZpos == 0.)
	       repetZ = 1;
            else
	    {
               // Example: min = 40, max = 70, step = 5 (in increasing steps)
               if( (maxZpos > minZpos) && (stepZpos > 0) )
                  repetZ = ((maxZpos - minZpos)/stepZpos)+1;
               // Example: min = 70, max = 40, step = -5 (in decreasing steps)
               else if( (maxZpos < minZpos) && (stepZpos < 0) )
                  repetZ = ((minZpos - maxZpos)/stepZpos)-1;
               // Example: min = 70, max = 70 (no scan)
               else if( maxZpos == minZpos )
                  repetZ = 1;
               // If step is not correctly set, stop the acqusition
               else
               {
                  // TODO
                  printf("Stopping current surface scan (Z step error)...\n");
                  gROOT->SetInterrupt();
                  measProgress->widgetTB[0]->SetText("Start acquisition");
                  acqStarted = false;
                  repetZ = 0;
               }
	    }
         }
	 else
	 {
            minZpos = zPos->widgetNE[0]->GetNumber();
	    repetZ = 1;
	 }

	 // Setting repetition for X axis
	 if(stepXpos == 0.)
	    repetX = 1;
         else
	 {
            // Example: min = 40, max = 70, step = 5 (in increasing steps)
            if( (maxXpos > minXpos) && (stepXpos > 0) )
               repetX = ((maxXpos - minXpos)/stepXpos)+1;
            // Example: min = 70, max = 40, step = -5 (in decreasing steps)
            else if( (maxXpos < minXpos) && (stepXpos < 0) )
               repetX = ((minXpos - maxXpos)/stepXpos)-1;
            // Example: min = 70, max = 70 (no scan)
            else if( maxXpos == minXpos )
               repetX = 1;
            // If step is not correctly set, stop the acqusition
            else
            {
               // TODO
               printf("Stopping current surface scan (X step error)...\n");
               gROOT->SetInterrupt();
               measProgress->widgetTB[0]->SetText("Start acquisition");
               acqStarted = false;
               repetX = 0;
            }
	 }
	 // Setting repetition for Y axis
	 if(stepYpos == 0.)
	    repetY = 1;
         else
	 {
            // Example: min = 40, max = 70, step = 5 (in increasing steps)
            if( (maxYpos > minYpos) && (stepYpos > 0) )
               repetY = ((maxYpos - minYpos)/stepYpos)+1;
            // Example: min = 70, max = 40, step = -5 (in decreasing steps)
            else if( (maxYpos < minYpos) && (stepYpos < 0) )
               repetY = ((minYpos - maxYpos)/stepYpos)-1;
            // Example: min = 70, max = 70 (no scan)
            else if( maxYpos == minYpos )
               repetY = 1;
            // If step is not correctly set, stop the acqusition
            else
            {
               // TODO
               printf("Stopping current surface scan (Y step error)...\n");
               gROOT->SetInterrupt();
               measProgress->widgetTB[0]->SetText("Start acquisition");
               acqStarted = false;
               repetY = 0;
            }
	 }

         if(DBGSIG) printf("StartAcq(): X axis repetition (%lf,%lf,%lf) = %d\n", minXpos, maxXpos, stepXpos, repetX);
         if(DBGSIG) printf("StartAcq(): Y axis repetition (%lf,%lf,%lf) = %d\n", minYpos, maxYpos, stepYpos, repetY);
         if(DBGSIG) printf("StartAcq(): Z axis repetition (%lf,%lf,%lf) = %d\n", minZpos, maxZpos, stepZpos, repetZ);

	 i = 0; j = 0; k = 0;

	 // TODO - Setting button text and acqStarted do not work!
	 measProgress->widgetTB[0]->SetText("Stop acquisition");
	 acqStarted = true;
         progVal = 0.00;
         measProgress->widgetPB->SetPosition(progVal);
	 gVirtualX->Update(1);

	 int nrAverMeas = -1;

	 clkt0 = clock();
	 timet0 = time(NULL);

	 // Scan over Z axis
         while(1)
         {
	    if( (repetZ > 0) && (k == repetZ) ) break;
	    else if( (repetZ < 0) && (k == -repetZ) ) break;
	    else if( repetZ == 0 ) break;

            fflush(stdout);
            // Z-axis change
	    if( posUnits->widgetCB->GetSelected() == 0)
               sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 3 -c m", rootdir, (int)(minZpos + stepZpos*k), rootdir);
	    else if( posUnits->widgetCB->GetSelected() == 1)
               sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 3 -c m", rootdir, (int)((minZpos + stepZpos*k)/lenconversion), rootdir);
#if WORKSTAT == 'I'
            retTemp = system(cmd);
#else
            printf("Cmd: %s\n",cmd);
#endif
            fflush(stdout);
     
            printf("Next Z position...\n");
            sleep(2*doublewait);
            zPos->widgetNE[0]->SetNumber(minZpos + stepZpos*k);
            fflush(stdout);

	    // Scan over Y axis
	    while(1)
	    {
	       if( (repetY > 0) && (j == repetY) ) break;
	       else if( (repetY < 0) && (j == -repetY) ) break;
	       else if( repetY == 0 ) break;

               fflush(stdout);
               // Y-axis change
	       if( posUnits->widgetCB->GetSelected() == 0)
                  sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 2 -c m", rootdir, (int)(minYpos + stepYpos*j), rootdir);
	       else if( posUnits->widgetCB->GetSelected() == 1)
                  sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 2 -c m", rootdir, (int)((minYpos + stepYpos*j)/lenconversion), rootdir);
#if WORKSTAT == 'I'
               retTemp = system(cmd);
#else
               printf("Cmd: %s\n",cmd);
#endif
               fflush(stdout);
     
               printf("Next Y position...\n");
               sleep(2*doublewait);
               yPos->widgetNE[0]->SetNumber(minYpos + stepYpos*j);
               fflush(stdout);

	       // Scan over X axis
	       while(1)
	       {
	          if( (repetX > 0) && (i == repetX) ) break;
	          else if( (repetX < 0) && (i == -repetX) ) break;
	          else if( repetX == 0 ) break;

                  progVal = (float)(100.00/(abs(repetX)*abs(repetY)*abs(repetZ)))*(k*abs(repetX)*abs(repetY) + j*abs(repetX) + i);
                  measProgress->widgetPB->SetPosition(progVal);

//     		  TimeEstimate(clkt0, timet0, progVal, cmd, doublewait*((abs(repetX)+2)*abs(repetY)+2)*abs(repetZ));
//	    	  measProgress->widgetTE->SetText(cmd);

	          gVirtualX->Update(1);

                  // X-axis change
	          if( posUnits->widgetCB->GetSelected() == 0)
                     sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 1 -c m", rootdir, (int)(minXpos + stepXpos*i), rootdir);
	          else if( posUnits->widgetCB->GetSelected() == 1)
                     sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 1 -c m", rootdir, (int)((minXpos + stepXpos*i)/lenconversion), rootdir);
#if WORKSTAT == 'I'
                  retTemp = system(cmd);
#else
                  printf("Cmd: %s\n",cmd);
#endif
                  fflush(stdout);
        
                  printf("Next X position...\n");
                  fflush(stdout);

TimeEstimateNew(nrAverMeas, clkt0, timet0, repetX, repetY, repetZ, doublewait, 2*doublewait, 2*doublewait, cmd);
measProgress->widgetTE->SetText(cmd);
     
                  printf("Waiting for position change...\n");
                  sleep(doublewait);
                  xPos->widgetNE[0]->SetNumber(minXpos + stepXpos*i);
                  printf("Continuing...\n");
     
               // Here comes function to start histogramming <<<<<<<<<<<<<<<<<<<<<<<<
clkt0 = clock();
                  RunMeas((void*)0, (j*repetX + i), scanon);
nrAverMeas++;
                  fflush(stdout);

		  i++;
	       }

	       i = 0;
	       printf("\n");

	       j++;
	    }

	    j = 0;

	    k++;
	 }
printf("Time = %d\n", (int)time(NULL)); 

         fflush(stdout);
         printf("Measurement finished, returning to starting position...\n");
         // X-axis return
	 if( posUnits->widgetCB->GetSelected() == 0)
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 1 -c m", rootdir, (int)minXpos, rootdir);
	 else if( posUnits->widgetCB->GetSelected() == 1)
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 1 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 1 -c m", rootdir, (int)(minXpos/lenconversion), rootdir);
#if WORKSTAT == 'I'
         retTemp = system(cmd);
#else
         printf("Cmd: %s\n",cmd);
#endif
         fflush(stdout);

         // Y-axis return
	 if( posUnits->widgetCB->GetSelected() == 0)
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 2 -c m", rootdir, (int)minYpos, rootdir);
	 else if( posUnits->widgetCB->GetSelected() == 1)
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 2 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 2 -c m", rootdir, (int)(minYpos/lenconversion), rootdir);
#if WORKSTAT == 'I'
         retTemp = system(cmd);
#else
         printf("Cmd: %s\n",cmd);
#endif

         // Z-axis return
	 if( posUnits->widgetCB->GetSelected() == 0)
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 3 -c m", rootdir, (int)minZpos, rootdir);
	 else if( posUnits->widgetCB->GetSelected() == 1)
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 3 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 3 -c m", rootdir, (int)(minZpos/lenconversion), rootdir);
#if WORKSTAT == 'I'
         retTemp = system(cmd);
#else
         printf("Cmd: %s\n",cmd);
#endif
         xPos->widgetNE[0]->SetNumber(minXpos);
         yPos->widgetNE[0]->SetNumber(minYpos);
         zPos->widgetNE[0]->SetNumber(minZpos);

         progVal = 100.00;
         measProgress->widgetPB->SetPosition(progVal);
         printf("\n");

         // Write information to the finish_sig.txt value
	 sprintf(cmd, "%s/dbg/finish_sig.txt", rootdir);
	 pfin = fopen(cmd,"w");
	 fprintf(pfin, "%s: Surface scan finished.", timeStamp->widgetTE->GetText());
	 fclose(pfin);

         measProgress->widgetTB[0]->SetText("Start acquisition");
         acqStarted = false;
      }
   }
   // Only angle scan
   if( (ascan == 1) && (pscan == 0) && (vscan == 0) )
   {
      // If already started, stop the acquisition
      if(acqStarted)
      {
         printf("Stopping current angle scan...\n");
	 gROOT->SetInterrupt();
	 measProgress->widgetTB[0]->SetText("Start acquisition");
	 acqStarted = false;

         // Write information to the finish_sig.txt value
	 sprintf(cmd, "%s/dbg/finish_sig.txt", rootdir);
	 pfin = fopen(cmd,"w");
	 fprintf(pfin, "%s: Angle scan stopped.", timeStamp->widgetTE->GetText());
	 fclose(pfin);
      }
      // If stopped, start the acquisition
      else if(!acqStarted)
      {
         printf("Running an incidence angle scan...\n");

	 // Check the steps
         minAlpha = rotPosMin->widgetNE[0]->GetNumber();
         maxAlpha = rotPosMax->widgetNE[0]->GetNumber();
         stepAlpha = rotPosStep->widgetNE[0]->GetNumber();

	 if(stepAlpha == 0.)
	    repetAlpha = 1;
	 else
	 {
	    // Example: min = 40, max = 70, step = 5 (in increasing steps)
	    if( (maxAlpha > minAlpha) && (stepAlpha > 0) )
	       repetAlpha = ((maxAlpha - minAlpha)/stepAlpha)+1;
	    // Example: min = 70, max = 40, step = -5 (in decreasing steps)
	    else if( (maxAlpha < minAlpha) && (stepAlpha < 0) )
	       repetAlpha = ((minAlpha - maxAlpha)/stepAlpha)-1;
	    // Example: min = 70, max = 70 (no scan)
	    else if( maxAlpha == minAlpha )
	       repetAlpha = 1;
	    // If step is not correctly set, stop the acqusition
	    else
	    {
	       // TODO
               printf("Stopping current incidence angle scan...\n");
               gROOT->SetInterrupt();
               measProgress->widgetTB[0]->SetText("Start acquisition");
               acqStarted = false;
	       repetAlpha = 0;
	    }
	 }

         if(DBGSIG) printf("StartAcq(): Angle repetition (%lf,%lf,%lf) = %d\n", minAlpha, maxAlpha, stepAlpha, repetAlpha);

	 int angleWait = TMath::Ceil(abs(rotPos->widgetNE[0]->GetNumber()-minAlpha)*15/(rotPos->widgetNE[0]->GetNumMax()));
	 if(rotUnits->widgetCB->GetSelected() == 1)
	 {
            minAlpha = minAlpha/rotconversion;
            maxAlpha = maxAlpha/rotconversion;
            stepAlpha = stepAlpha/rotconversion;
	 }

	 i = 0;

	 // TODO - Setting button text and acqStarted do not work!
	 measProgress->widgetTB[0]->SetText("Stop acquisition");
	 acqStarted = true;
         progVal = 0.00;
         measProgress->widgetPB->SetPosition(progVal);
	 gVirtualX->Update(1);

	 clkt0 = clock();
	 timet0 = time(NULL);

	 // Setting angle to initial position
	 sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 4 -c m", rootdir, (int)minAlpha, rootdir);
	 if(rotUnits->widgetCB->GetSelected() == 0)
	    rotPos->widgetNE[0]->SetNumber(minAlpha);
	 else if(rotUnits->widgetCB->GetSelected() == 1)
	    rotPos->widgetNE[0]->SetNumber(minAlpha*rotconversion);
#if WORKSTAT == 'I'
            retTemp = system(cmd);
#else
            printf("Cmd: %s\n",cmd);
#endif
            fflush(stdout);
        
            printf("Waiting for %ds for rotation platform to move into starting position...\n", angleWait);
            sleep(angleWait);

         while(1)
         {
	    if( (repetAlpha > 0) && (i == repetAlpha) ) break;
	    else if( (repetAlpha < 0) && (i == -repetAlpha) ) break;
	    else if( repetAlpha == 0 ) break;

            progVal = (float)(100.00/abs(repetAlpha))*i;
            measProgress->widgetPB->SetPosition(progVal);

	    TimeEstimate(clkt0, timet0, progVal, cmd, singlewait*abs(repetAlpha));
	    measProgress->widgetTE->SetText(cmd);

	    gVirtualX->Update(1);
        
            fflush(stdout);
            currentAlpha = minAlpha + stepAlpha*i;
            sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 4 -c m", rootdir, (int)currentAlpha, rootdir);
#if WORKSTAT == 'I'
            retTemp = system(cmd);
#else
            printf("Cmd: %s\n",cmd);
#endif
            fflush(stdout);
        
            printf("Waiting for angle change...\n");
            sleep(singlewait);
	    if(rotUnits->widgetCB->GetSelected() == 0)
               rotPos->widgetNE[0]->SetNumber(currentAlpha);
	    else if(rotUnits->widgetCB->GetSelected() == 1)
               rotPos->widgetNE[0]->SetNumber(currentAlpha*rotconversion);
	    gVirtualX->Update(1);
            printf("Continuing...\n");
        
            // Here comes function to start histogramming <<<<<<<<<<<<<<<<<<<<<<<<
            RunMeas((void*)0, i, scanon); // TODO
            fflush(stdout);

	    i++;
         }
        
         // Set angle back to original position
         fflush(stdout);
         printf("Measurement finished, returning to starting incidence angle...\n");
         sprintf(cmd, "sudo %s/src/MIKRO/mikro_ctrl -n 4 -v %d -s la && %s/src/MIKRO/mikro_ctrl -n 4 -c m", rootdir, (int)minAlpha, rootdir);
	 if(rotUnits->widgetCB->GetSelected() == 0)
            rotPos->widgetNE[0]->SetNumber(minAlpha);
	 else if(rotUnits->widgetCB->GetSelected() == 1)
            rotPos->widgetNE[0]->SetNumber(minAlpha*rotconversion);
#if WORKSTAT == 'I'
         retTemp = system(cmd);
#else
         printf("Cmd: %s\n",cmd);
#endif
         fflush(stdout);
        
         progVal = 100.00;
         measProgress->widgetPB->SetPosition(progVal);
         printf("\n");

         sprintf(cmd, "%s/dbg/finish_sig.txt", rootdir);
   	 pfin = fopen(cmd,"w");
         fprintf(pfin, "%s: Incidence angle scan finished.", timeStamp->widgetTE->GetText());
         fclose(pfin);

         measProgress->widgetTB[0]->SetText("Start acquisition");
         acqStarted = false;
      }
   }
   // Normal single measurement
   else if( (vscan == 0) && (pscan == 0) && (ascan == 0) )
   {
      // Set the start button to stop and enable stopping of measurement
      if(acqStarted)
      {
         printf("Stopping current single scan...\n");
         gROOT->SetInterrupt();
         measProgress->widgetTB[0]->SetText("Start acquisition");
         acqStarted = false;
      }
      else if(!acqStarted)
      {
         measProgress->widgetTB[0]->SetText("Stop acquisition");
         acqStarted = true;

         printf("Running a single scan...\n");
	 clkt0 = clock();
	 timet0 = time(NULL);
         RunMeas((void*)0, 0, scanon);
         printf("Measurement finished...\n");
         printf("\n");

         measProgress->widgetTB[0]->SetText("Start acquisition");
         acqStarted = false;
      }
   }
}

// Main measurement window connections ----------------------

// Histogram file selection pane connections ----------------

// File browser for opening histograms
void TGAppMainFrame::SelectDirectory()
{
   int i = fileList->GetNumberOfEntries();
//   char *cTemp;

   TGFileInfo file_info;
   const char *filetypes[] = {"Histograms",histextall,0,0};
   file_info.fFileTypes = filetypes;
//   cTemp = new char[1024];
//   sprintf(cTemp, "%s/results", rootdir);
//   file_info.fIniDir = StrDup(cTemp);
   file_info.fIniDir = StrDup(currentAnalDir);
   file_info.fMultipleSelection = kTRUE;
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);
//   delete[] cTemp;

   TList *files = file_info.fFileNamesList;
   if(files)
   {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while(file=(TSystemFile*)next())
      {
         fname = file->GetName();
	 remove_from_last((char*)fname.Data(), '/', currentAnalDir);
         fileList->AddEntry(fname.Data(), i);
	 i++;
      }
   }
   fileList->Layout();
}

// Toggle multiple selection in filelist or delete all entries
void TGAppMainFrame::ListMultiSelect(int opt)
{
   // Enable multiselect
   if(opt == 0)
   {
      fileList->SetMultipleSelections((multiSelect->widgetChBox[0]->IsOn()));

      if(multiSelect->widgetChBox[1]->IsDown())
         multiSelect->widgetChBox[1]->SetState(kButtonUp);
   }
   else if(opt == 1)
   {
      if(multiSelect->widgetChBox[1]->IsDown())
      {
         multiSelect->widgetChBox[0]->SetState(kButtonDown);
         fileList->SetMultipleSelections((multiSelect->widgetChBox[0]->IsOn()));
         for(int i = 0; i < fileList->GetNumberOfEntries(); i++) 
            fileList->Select(i,kTRUE);
      }
      else if(!multiSelect->widgetChBox[1]->IsDown())
      {
         multiSelect->widgetChBox[0]->SetState(kButtonUp);
         fileList->SetMultipleSelections((multiSelect->widgetChBox[0]->IsOn()));
         for(int i = 0; i < fileList->GetNumberOfEntries(); i++) 
            fileList->Select(i,kFALSE);
      }
   }
}

// Navigation buttons for the filelist (<<, >>) and double click
void TGAppMainFrame::FileListNavigation(int opt)
{
   unsigned int nrfiles = fileList->GetNumberOfEntries();
   int curSel;
   TList *files;
   if( nrfiles > 0 )
   {
      if(opt < -1)
      {
    	 // turn off multiple selection and select first file on list
         if(multiSelect->widgetChBox[0]->IsOn())
         {
            fileList->SetMultipleSelections(kFALSE);
            multiSelect->widgetChBox[0]->SetState(kButtonUp);
	    multiSelect->widgetChBox[1]->SetState(kButtonUp);

            fileList->Select(0,kTRUE);
         }
         else
         {
            // if nothing is selected, curSel will be -1
            curSel = fileList->GetSelected();
            // go to next file on list
            if(opt == -3)
            {
               if( (curSel == (int)(nrfiles-1)) || (curSel == -1) )
   	          fileList->Select(0);
	       else
	          fileList->Select(curSel+1);
            }
            // go to previous file on list
            else if(opt == -2)
            {
               if( (curSel == 0) || (curSel == -1) )
   	          fileList->Select(nrfiles-1);
	       else
	          fileList->Select(curSel-1);
            }
         }
      }
   }

   UpdateHistogram(0);
}

// Open the header edit window when pressing on editHeader button
void TGAppMainFrame::HeaderEdit()
{
   bool createTab = true;
   int tabid = -1;

   for(int i = 0; i < fTab->GetNumberOfTabs(); i++)
   {
      if(strcmp("File header editor", fTab->GetTabTab(i)->GetString() ) == 0)
      {
         createTab = false;
	 tabid = i;
      }
      
      if(DBGSIG > 1) printf("HeaderEdit(): Name of tab = %s\n", fTab->GetTabTab(i)->GetString() );
   }

   unsigned int nrfiles = fileList->GetNumberOfEntries();
   if(nrfiles > 0)
      HeaderEditTab(fTab, createTab, &tabid);
}

// Clear the histogram file selection list and dark run analysis selection
void TGAppMainFrame::ClearHistogramList()
{
   fileList->RemoveAll();
   darkRun->widgetTE->Clear();
}

// Histogram file selection pane connections ----------------

// Histogram controls pane connections ----------------------

// Readjust the histogram range after changing ADC, TDC, Y range or logarithmic scale (opt: 0 = normal redraw, 1 = export, 2 = redraw when changing which channel to display) 
void TGAppMainFrame::UpdateHistogram(int opt)
{
   if(DBGSIG > 1)
   {
      printf("UpdateHistogram(): Clearing the TList\n");
      gDirectory->GetList()->Delete();
      gObjectTable->Print();
   }

   // Do not do normal histogram update if we have multiple files selected
   if( (opt == 0) && (multiSelect->widgetChBox[0]->IsDown()) )
   {
      printf("UpdateHistogram(): To preview changes done to a histogram, please deselect the \"Multiple files select\" option.");
      return;
   }

   // Do not update histogram if we are on the same channel
   if( ((opt == 2) && (selChannel != (int)selectCh->widgetNE[0]->GetNumber())) || (opt < 2) )
   {
      unsigned int nrfiles = fileList->GetNumberOfEntries();
      TCanvas *gCanvas;
      char exportname[512];
      char cTemp[512];
    
      if(opt == 1)
         gCanvas = analysisCanvas->GetCanvas();
    
      if(nrfiles > 0)
      {
         TList *files;
         files = new TList();
         fileList->GetSelectedEntries(files);
         
         if(files)
         {
            for(int i = 0; i < (int)nrfiles; i++)
            {
               if(files->At(i))
               {
                  if(DBGSIG)
                     printf("UpdateHistogram(): Filename: %s\n", files->At(i)->GetTitle());
                  if(opt == 1)
           	     remove_ext((char*)files->At(i)->GetTitle(), cTemp);
                  if( fMenuHisttype->IsEntryChecked(M_ANALYSIS_HISTTYPE_1DADC) )
                  {
                     sprintf(exportname, "%s_adc%d.pdf", cTemp, (int)selectCh->widgetNE[0]->GetNumber());
     	             DisplayHistogram( (char*)(files->At(i)->GetTitle()), 0, opt);
                  }
                  else if( fMenuHisttype->IsEntryChecked(M_ANALYSIS_HISTTYPE_1DTDC) )
                  {
                     sprintf(exportname, "%s_tdc%d.pdf", cTemp, (int)selectCh->widgetNE[0]->GetNumber());
     	             DisplayHistogram( (char*)(files->At(i)->GetTitle()), 1, opt);
                  }
                  else if( fMenuHisttype->IsEntryChecked(M_ANALYSIS_HISTTYPE_2D) )
                  {
                     sprintf(exportname, "%s_adctdc%d.pdf", cTemp, (int)selectCh->widgetNE[0]->GetNumber());
     	             DisplayHistogram( (char*)(files->At(i)->GetTitle()), 2, opt);
                  }
    
                  if(opt == 1)
		  {
                     gCanvas->SaveAs(exportname);
		     delete inroot;
		  }
               }
            }
         }
      }
      selChannel = selectCh->widgetNE[0]->GetNumber();
   }

   if(DBGSIG > 1)
   {
      printf("UpdateHistogram(): After drawing histograms (connections)\n");
      gObjectTable->Print();
   }
}

// Options for histogram (logarithmic scale, clean plots)
void TGAppMainFrame::HistogramOptions(int opt)
{
   // Logarithmic scale
   if(opt == 0)
      UpdateHistogram(0);
   // Clean plots
   else if(opt == 1)
   {
      cleanPlots = histOpt->widgetChBox[1]->IsDown();
      UpdateHistogram(0);
   }
}

// Changing the histogram type to display
void TGAppMainFrame::ChangeHisttype(int type)
{
   TGTextButton *pressedB = new TGTextButton();
   int menuID = 0;
   unsigned int nrfiles = fileList->GetNumberOfEntries();

   // ADC histogram
   if(type == 0)
   {
      pressedB = plotType->widgetTB[0];
      menuID = M_ANALYSIS_HISTTYPE_1DADC;

      plotType->widgetTB[1]->SetDown(kFALSE);
      plotType->widgetTB[2]->SetDown(kFALSE);
      fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_1DTDC);
      fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_2D);
   }
   // TDC histogram
   else if(type == 1)
   {
      pressedB = plotType->widgetTB[1];
      menuID = M_ANALYSIS_HISTTYPE_1DTDC;

      plotType->widgetTB[0]->SetDown(kFALSE);
      plotType->widgetTB[2]->SetDown(kFALSE);
      fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_1DADC);
      fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_2D);
   }
   // ADC vs. TDC histogram
   else if(type == 2)
   {
      pressedB = plotType->widgetTB[2];
      menuID = M_ANALYSIS_HISTTYPE_2D;

      plotType->widgetTB[0]->SetDown(kFALSE);
      plotType->widgetTB[1]->SetDown(kFALSE);
      fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_1DADC);
      fMenuHisttype->UnCheckEntry(M_ANALYSIS_HISTTYPE_1DTDC);
   }

   if( fMenuHisttype->IsEntryChecked(menuID) )
   {
      pressedB->SetDown(kFALSE);
      fMenuHisttype->UnCheckEntry(menuID);
   }
   else if( !fMenuHisttype->IsEntryChecked(menuID) )
   {
      pressedB->SetDown(kTRUE);
      fMenuHisttype->CheckEntry(menuID);
   }

   UpdateHistogram(0);
}

// Histogram controls pane connections ----------------------*/
