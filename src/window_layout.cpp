#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"

#include <stdio.h>
#include <stdlib.h>

// Title labels for each of the frame
void TGAppMainFrame::TGTitleLabel(TGWindow *parent, TGHorizontalFrame *fTitle, const char *title, Pixel_t foreColor, Pixel_t backColor, const char *font)
{
   TGLabel *lab = new TGLabel(fTitle, title);
   lab->ChangeBackground(backColor);
   lab->SetTextColor(foreColor);
   lab->SetTextFont(font);
   fTitle->AddFrame(lab, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY) );
   fTitle->ChangeBackground(backColor);
}

// Splitter to a set number of frames
bool TGAppMainFrame::TGSplitter(TGWindow *parent, const char *majorSplit, int *horSplits, int *vertSplits, const char *frameTitles[512], TGCompositeFrame **frames, int *frameWidth, int *frameHeight)
{
   // Number of Frames that are not the ones we supply
   int nrofmed = 0;
   // Number of all splits in the minor direction
   int minorSplits = 0;
//   int j, k;
   int k = 0;

   // Title frame height
   int titHeight = 25;

   // There are no splits, we just prepare the title and main frame
   if( (horSplits[0] == 0) && (vertSplits[0] == 0) )
   {
      fLayout[idtotal] = new TGCompositeFrame(parent, 300, 300, kHorizontalFrame);
      frames[k] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], frameHeight[k], kVerticalFrame | kFixedHeight | kSunkenFrame);
      fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
      TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
      frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
      fLayout[idtotal]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   }

   // The major split is in the horizontal direction (we get two vertical sides)
   if(strcmp("horizontal",majorSplit) == 0)
   {
      fLayout[idtotal] = new TGCompositeFrame(parent, 300, 300, kHorizontalFrame);
      for(int i = 0; i <= horSplits[0]; i++)
      {
         minorSplits += vertSplits[i];
         if(vertSplits[i] > 0)
	    nrofmed++;
      }

      if(DBGSIG > 1) printf("TGSplitter(): Number of intermediate frames = %d, all minor splits = %d\n", nrofmed, minorSplits);

      TGCompositeFrame *fInter[nrofmed];
      TGVSplitter *vsplit[horSplits[0]];
      TGHSplitter *hsplit[minorSplits];

      nrofmed = 0;
      k = 0;
/*      j = 0;

      // 2 + 1
      if( (horSplits[0] == 1) && (vertSplits[0] == 1) && (vertSplits[1] == 0) )
      {
         fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame | kFixedWidth);
         fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandY));

         frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kFixedHeight | kSunkenFrame);
         fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
         TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
         frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
         fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX));

         hsplit[nrofmed] = new TGHSplitter(fInter[nrofmed]);
         hsplit[nrofmed]->SetFrame(frames[k], kTRUE);
         fInter[nrofmed]->AddFrame(hsplit[nrofmed], new TGLayoutHints(kLHintsExpandX));

         k++;

         frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
         fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
         TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
         frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
         fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

         k++;

	 vsplit[j] = new TGVSplitter(fLayout[idtotal]);
         vsplit[j]->SetFrame(fInter[nrofmed], kTRUE);
	 fLayout[idtotal]->AddFrame(vsplit[nrofmed], new TGLayoutHints(kLHintsExpandY));
	 j++;
	 nrofmed++;

         frames[k] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
         fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
         TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
         frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
         fLayout[idtotal]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

         k++;
      }

      nrofmed = 0;
      j = 0; k = 0;

      // 1 + 2
      if( (horSplits[0] == 1) && (vertSplits[0] == 0) && (vertSplits[1] == 1) )
      {
         frames[k] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame | kFixedWidth);
         fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
         TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
         frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
         fLayout[idtotal]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandY));

         k++;

	 vsplit[j] = new TGVSplitter(fLayout[idtotal]);
         vsplit[j]->SetFrame(frames[nrofmed], kTRUE);
	 fLayout[idtotal]->AddFrame(vsplit[nrofmed], new TGLayoutHints(kLHintsExpandY));
	 j++;

         fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame);
         fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

         frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kFixedHeight | kSunkenFrame);
         fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
         TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
         frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
         fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX));

         hsplit[nrofmed] = new TGHSplitter(fInter[nrofmed]);
         hsplit[nrofmed]->SetFrame(frames[k], kTRUE);
         fInter[nrofmed]->AddFrame(hsplit[nrofmed], new TGLayoutHints(kLHintsExpandX));

         k++;

         frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
         fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
         TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
         frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
         fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

         k++;
      }*/

      

      for(int i = 0; i <= horSplits[0]; i++)
      {
         if(DBGSIG > 1) printf("TGSplitter(): i = %d\n",i);
         for(int j = 0; j <= vertSplits[i]; j++)
	 {
            if(DBGSIG > 1) printf("TGSplitter(): j = %d, vertSplits = %d\n",j, vertSplits[i]);
	    if( vertSplits[i] > 0 )
	    {
	       if(j == 0)
	       {
                  if(DBGSIG > 1) printf("TGSplitter(): vertSplits - j = %d (nrofmed = %d)\n", (vertSplits[i]-j), nrofmed);
		  if(i > 0)
		  {
                     fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame);
                     fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandY | kLHintsExpandX));
		  }
		  else
		  {
	             if(vertSplits[0] == 0)
		     {
                        fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame);
                        fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandY | kLHintsExpandX));
		     }
		     else
		     {
                        fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame | kFixedWidth);
                        fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandY));
		     }
		  }
	       }

               if( (vertSplits[i]-j) > 0)
               {
                  if(DBGSIG > 1) printf("TGSplitter(): Step 0a (k = %d)\n", k);

                  frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kFixedHeight | kSunkenFrame);
                  fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
                  TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
                  frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
                  fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX));

                  if(DBGSIG > 1) printf("TGSplitter(): Step 1a\n");

                  hsplit[nrofmed] = new TGHSplitter(fInter[nrofmed]);
                  hsplit[nrofmed]->SetFrame(frames[k], kTRUE);
                  fInter[nrofmed]->AddFrame(hsplit[nrofmed], new TGLayoutHints(kLHintsExpandX));

                  if(DBGSIG > 1) printf("TGSplitter(): Step 2a\n");

                  k++;
               }
               else
               {
                  if(DBGSIG > 1) printf("TGSplitter(): Step 0b (k = %d)\n", k);

                  frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
                  fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
                  TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
                  frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
                  fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

                  if(DBGSIG > 1) printf("TGSplitter(): Step 1b\n");
                  k++;
               }
	    }
	    else
	    {
               if(DBGSIG > 1) printf("TGSplitter(): Step 0c (k = %d)\n", k);

	       if(vertSplits[0] == 0)
                  frames[k] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame | kFixedWidth);
	       else
                  frames[k] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
               fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
               TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
               frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
	       if(vertSplits[0] == 0)
                  fLayout[idtotal]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandY));
	       else
                  fLayout[idtotal]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

               if(DBGSIG > 1) printf("TGSplitter(): Step 1c\n");
               k++;
	    }
	 }

         if(DBGSIG > 1) printf("TGSplitter(): i = %d, horSplits = %d\n", i, horSplits[0]);

         if( (i != horSplits[0]) && (vertSplits[0] != 0) )
	 {
	    vsplit[i] = new TGVSplitter(fLayout[idtotal]);
	    if( vertSplits[i] > 0 )
               vsplit[i]->SetFrame(fInter[nrofmed], kTRUE);
	    else
               vsplit[i]->SetFrame(frames[k], kTRUE);
	    fLayout[idtotal]->AddFrame(vsplit[nrofmed], new TGLayoutHints(kLHintsExpandY));
	    nrofmed++;
	 }
	 else if( (i != horSplits[0]) && (vertSplits[0] == 0) )
	 {
            k--;
	    vsplit[i] = new TGVSplitter(fLayout[idtotal]);
            vsplit[i]->SetFrame(frames[k], kTRUE);
	    fLayout[idtotal]->AddFrame(vsplit[i], new TGLayoutHints(kLHintsExpandY));
            k++;
	 }
      }

      if(DBGSIG > 1) printf("TGSplitter(): Finished horizontal layout\n");
   }
   else if(strcmp("vertical",majorSplit) == 0)
   {
/*      fLayout[idtotal] = new TGCompositeFrame(parent, 300, 300, kHorizontalFrame);
      for(int i = 0; i <= vertSplits[0]; i++)
      {
         minorSplits += horSplits[i];
         if(horSplits[i] > 0)
	    nrofmed++;
      }

      if(DBGSIG > 1) printf("TGSplitter(): Number of intermediate frames = %d, all minor splits = %d\n", nrofmed, minorSplits);

      TGCompositeFrame *fInter[nrofmed];
      TGVSplitter *vsplit[vertSplits[0]];
      TGHSplitter *hsplit[minorSplits];

      nrofmed = 0;

      for(int i = 0; i <= vertSplits[0]; i++)
      {
         if(DBGSIG > 1) printf("TGSplitter(): i = %d\n",i);
         for(int j = 0; j <= horSplits[i]; j++)
	 {
            if(DBGSIG > 1) printf("TGSplitter(): j = %d, horSplits = %d\n",j, horSplits[i]);
	    if( horSplits[i] > 0 )
	    {
	       if(j == 0)
	       {
                  if(DBGSIG > 1) printf("TGSplitter(): horSplits - j = %d (nrofmed = %d)\n", (horSplits[i]-j), nrofmed);
		  if(i > 0)
		  {
                     fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame);
                     fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandY | kLHintsExpandX));
		  }
		  else
		  {
                     fInter[nrofmed] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], 200, kVerticalFrame | kFixedWidth);
                     fLayout[idtotal]->AddFrame(fInter[nrofmed], new TGLayoutHints(kLHintsExpandY));
		  }
	       }

               if( (horSplits[i]-j) > 0)
               {
                  if(DBGSIG > 1) printf("TGSplitter(): Step 0a (k = %d)\n", k);

                  frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kFixedHeight | kSunkenFrame);
                  fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
                  TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
                  frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
                  fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX));

                  if(DBGSIG > 1) printf("TGSplitter(): Step 1a\n");

                  hsplit[nrofmed] = new TGHSplitter(fInter[nrofmed]);
                  hsplit[nrofmed]->SetFrame(frames[k], kTRUE);
                  fInter[nrofmed]->AddFrame(hsplit[nrofmed], new TGLayoutHints(kLHintsExpandX));

                  if(DBGSIG > 1) printf("TGSplitter(): Step 2a\n");

                  k++;
               }
               else
               {
                  if(DBGSIG > 1) printf("TGSplitter(): Step 0b (k = %d)\n", k);

                  frames[k] = new TGCompositeFrame(fInter[nrofmed], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
                  fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
                  TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
                  frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
                  fInter[nrofmed]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

                  if(DBGSIG > 1) printf("TGSplitter(): Step 1b\n");
                  k++;
               }
	    }
	    else
	    {
               if(DBGSIG > 1) printf("TGSplitter(): Step 0c (k = %d)\n", k);

               frames[k] = new TGCompositeFrame(fLayout[idtotal], frameWidth[k], frameHeight[k], kVerticalFrame | kSunkenFrame);
               fTitle = new TGHorizontalFrame(frames[k], 100, titHeight, kFixedHeight | kSunkenFrame);
               TGTitleLabel(frames[k], fTitle, frameTitles[k], (Pixel_t)FORECOLOR, (Pixel_t)BACKCOLOR, FONT);
               frames[k]->AddFrame(fTitle, new TGLayoutHints(kLHintsExpandX | kLHintsTop) );
               fLayout[idtotal]->AddFrame(frames[k], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

               if(DBGSIG > 1) printf("TGSplitter(): Step 1c\n");
               k++;
	    }
	 }

         if(DBGSIG > 1) printf("TGSplitter(): i = %d, vertSplits = %d\n", i, vertSplits[0]);

         if(i != vertSplits[0])
	 {
	    vsplit[i] = new TGVSplitter(fLayout[idtotal]);
	    if( horSplits[i] > 0 )
               vsplit[i]->SetFrame(fInter[nrofmed], kTRUE);
	    else
               vsplit[i]->SetFrame(frames[k], kTRUE);
	    fLayout[idtotal]->AddFrame(vsplit[nrofmed], new TGLayoutHints(kLHintsExpandY));
	 }

	 nrofmed++;
      }

      if(DBGSIG > 1) printf("TGSplitter(): Finished vertical layout\n");*/
   }
   else
      return false;

   return true;
}

// Function for setting up the layout
void TGAppMainFrame::LayoutRead(int nrframes, int *w, int *h)
{
   int WM, HM, k = 0, start = 0;

   std::ifstream ilayout;
   
   char *cTemp, *cTemp2, readTemp[1024];
   cTemp = new char[512];
   cTemp2 = new char[512];
   sprintf(cTemp, "%s/layout/selected_layout.txt", rootdir);
   ilayout.open(cTemp, std::ifstream::in);
   if(ilayout.is_open())
   {
      ilayout >> cTemp2;
   }
   ilayout.close();
   printf("Loaded layout file is: %s\n", cTemp2);

   sprintf(cTemp, "%s/layout/%s", rootdir, cTemp2);
   ilayout.open(cTemp, std::ifstream::in);
   if(ilayout.is_open())
   {
      while(1)
      {
         if(ilayout.peek() == '#')
	 {
	    ilayout.getline(readTemp, 1024, '\n');
	    if(DBGSIG > 1) printf("LayoutRead(): readTemp = %s\n", readTemp);
	 }
	 else if(ilayout.peek() == '\n')
	    ilayout.ignore(1, '\n');
	 else
	 {  
	    if(start == 0)
	    {
	       ilayout >> WM >> HM >> readTemp;
	       ilayout.ignore(1, '\n');
	       start++;
	       if(DBGSIG > 1) printf("LayoutRead(): W = %d, H = %d, Name = %s\n", WM, HM, readTemp);
	    }
	    else
	    {
	       ilayout >> w[k] >> h[k] >> readTemp;
	       ilayout.ignore(1, '\n');
	       if(DBGSIG > 1) printf("LayoutRead(): w[%d] = %d, h[%d] = %d, Name = %s\n", k, w[k], k, h[k], readTemp);
	       k++;

	       if(k == nrframes) break;
	    }
	 }
      }
   }

   ilayout.close();
   delete[] cTemp;
   delete[] cTemp2;
}

// Function for saving the current layout
void TGAppMainFrame::LayoutSave()
{
   TGFileInfo file_info;
   const char *filetypes[] = {"Layout","*.layout",0,0};
   char *cTemp;
   file_info.fFileTypes = filetypes;
   cTemp = new char[1024];
   sprintf(cTemp, "%s/layout", rootdir);
   file_info.fIniDir = StrDup(cTemp);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDSave, &file_info);
   delete[] cTemp;

   if(file_info.fFilename != NULL)
   {
      if(DBGSIG) printf("LayoutSave(): The layout save name: %s\n", file_info.fFilename);
    
      std::ofstream olayout;
    
      olayout.open(file_info.fFilename, std::ofstream::out);
      if(olayout.is_open())
      {
         olayout << "# Whole window width and height" << std::endl;
         olayout << fMain->GetWidth() << "\t" << fMain->GetHeight() << "\tmain" << std::endl << std::endl;
         olayout << "# ADST viewer subwindows width and height" << std::endl;
         for(int i = 0; i < adstwin; i++)
            olayout << adstLayout[i]->GetWidth() << "\t" << adstLayout[i]->GetHeight() << "\tadstwindow" << i << std::endl;
         olayout << std::endl;
         olayout << "# MVA subwindows width and height" << std::endl;
         for(int i = 0; i < mvawin; i++)
            olayout << mvaLayout[i]->GetWidth() << "\t" << mvaLayout[i]->GetHeight() << "\tmvawindow" << i << std::endl;
      }
      else
         printf("Error! Save file can not be opened (please do not use default.layout since it is write protected).\n");
      olayout.close();
   }
}

// Function for setting a user created layout
void TGAppMainFrame::LayoutSet()
{
   TGFileInfo file_info;
   int ret = 0;
   const char *filetypes[] = {"Layout","*.layout",0,0};
   char *cTemp, *layoutdir;
   file_info.fFileTypes = filetypes;
   layoutdir = new char[1024];
   sprintf(layoutdir, "%s/layout", rootdir);
   file_info.fIniDir = StrDup(layoutdir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain, kFDOpen, &file_info);

   if(file_info.fFilename != NULL)
   {
      if(DBGSIG) printf("LayoutSet(): The layout save name: %s\n", file_info.fFilename);
      cTemp = new char[512];
      remove_before_last(file_info.fFilename, '/', cTemp);
      if(DBGSIG) printf("LayoutSet(): New selected layout: %s\n", cTemp);

      FILE *fp;
      sprintf(layoutdir, "%s/layout/selected_layout.txt", rootdir);
      fp = fopen(layoutdir, "w");
      fprintf(fp, "%s", cTemp);
      fclose(fp);

      sprintf(layoutdir, "Please restart the program to enable the selected layout (%s) for future use.", cTemp);
      new TGMsgBox(gClient->GetRoot(), fMain, "Setting new layout", layoutdir, kMBIconAsterisk, kMBOk, &ret);
      delete[] cTemp;
   }
   delete[] layoutdir;
}

/*// Function for saving all current measurement settings in a file and reopening them on next program open
void TGAppMainFrame::SaveMeasSettings()
{
   char *cTemp;
   cTemp = new char[1024];
   sprintf(cTemp, "%s/settings/measurement_settings.txt", rootdir);

   std::ofstream osettings;
   
   osettings.open(cTemp, std::ofstream::out);
   if(osettings.is_open())
   {
      osettings << "# Settings pane" << std::endl;
      for(int i = 0; i < 4; i++)
         osettings << (int)scansOn->widgetChBox[i]->IsDown() << std::endl;
      osettings << vHardlimit->widgetNE[0]->GetNumber() << std::endl;
      osettings << NCH->widgetNE[0]->GetNumber() << std::endl;
      osettings << posUnits->widgetCB->GetSelected() << std::endl;
      osettings << rotUnits->widgetCB->GetSelected() << std::endl;
      osettings << oscConnect->widgetTE->GetText() << std::endl;
      osettings << laserInfo->widgetTE->GetText() << std::endl;
      osettings << chtemp->widgetNE[0]->GetNumber() << std::endl;
      osettings << (int)liveDisp->widgetChBox[0]->IsDown() << std::endl;
      osettings << std::endl;
      
      osettings << "# Main measurement window" << std::endl;
      osettings << vOutCh->widgetCB->GetSelected() << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << (int)vOutOpt->widgetChBox[i]->IsDown() << std::endl;
      osettings << vOutStart->widgetNE[0]->GetNumber() << std::endl;
      osettings << vOutStop->widgetNE[0]->GetNumber() << std::endl;
      osettings << vOutStep->widgetNE[0]->GetNumber() << std::endl;

      osettings << zPosMin->widgetNE[0]->GetNumber() << std::endl;
      osettings << zPosMax->widgetNE[0]->GetNumber() << std::endl;
      osettings << zPosStep->widgetNE[0]->GetNumber() << std::endl;
      osettings << xPosMin->widgetNE[0]->GetNumber() << std::endl;
      osettings << xPosMax->widgetNE[0]->GetNumber() << std::endl;
      osettings << xPosStep->widgetNE[0]->GetNumber() << std::endl;
      osettings << yPosMin->widgetNE[0]->GetNumber() << std::endl;
      osettings << yPosMax->widgetNE[0]->GetNumber() << std::endl;
      osettings << yPosStep->widgetNE[0]->GetNumber() << std::endl;

      osettings << rotPosMin->widgetNE[0]->GetNumber() << std::endl;
      osettings << rotPosMax->widgetNE[0]->GetNumber() << std::endl;
      osettings << rotPosStep->widgetNE[0]->GetNumber() << std::endl;

      osettings << evtNum->widgetNE[0]->GetNumber() << std::endl;
      osettings << fileName->widgetTE->GetText() << std::endl;

      printf("Saving measurement settings to file.\n");
   }
   else
      printf("Error! Save file can not be opened.\n");
   osettings.close();
}

// Function for saving all current analysis settings in a file and reopening them on next program open
void TGAppMainFrame::SaveAnalSettings()
{
   char *cTemp;
   cTemp = new char[1024];
   sprintf(cTemp, "%s/settings/analysis_settings.txt", rootdir);

   std::ofstream osettings;
   
   osettings.open(cTemp, std::ofstream::out);
   if(osettings.is_open())
   {
      osettings << "# Analysis" << std::endl;
      // Integrate spectrum
      for(int i = 0; i < 3; i++)
         osettings << (int)intSpect->widgetChBox[i]->IsDown() << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << resol2d->widgetNE[i]->GetNumber() << std::endl;

      // Relative PDE
      osettings << (int)relPde->widgetChBox[0]->IsDown() << std::endl;
      osettings << (int)midPeak->widgetChBox[0]->IsDown() << std::endl;
      osettings << zeroAngle->widgetNE[0]->GetNumber() << std::endl;

      // Breakdown voltage
      osettings << minPeak->widgetNE[0]->GetNumber() << std::endl;
      osettings << peakSepCalc->widgetNE[0]->GetNumber() << std::endl;

      // Surface scan
      for(int i = 0; i < 2; i++)
         osettings << (int)surfScanOpt->widgetChBox[i]->IsDown() << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << resolSurf->widgetNE[i]->GetNumber() << std::endl;

      // Fitting
      osettings << fitSigma->widgetNE[0]->GetNumber() << std::endl;
      osettings << fitTresh->widgetNE[0]->GetNumber() << std::endl;
      osettings << fitInter->widgetNE[0]->GetNumber() << std::endl;
      osettings << adcOffset->widgetNE[0]->GetNumber() << std::endl;
      osettings << accError->widgetNE[0]->GetNumber() << std::endl;
      osettings << pedesLow->widgetNE[0]->GetNumber() << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << (int)fitChecks->widgetChBox[i]->IsDown() << std::endl;
      osettings << std::endl;

      osettings << "# Histogram controls" << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << adcRange->widgetNE[i]->GetNumber() << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << tdcRange->widgetNE[i]->GetNumber() << std::endl;
      for(int i = 0; i < 2; i++)
         osettings << yRange->widgetNE[i]->GetNumber() << std::endl;
      osettings <<  selectCh->widgetNE[0]->GetNumber() << std::endl;
      for(int i = 0 ; i < 2; i++)
         osettings  << (int)histOpt->widgetChBox[i]->IsDown() << std::endl;
      osettings <<  posUnitsPlot->widgetCB->GetSelected() << std::endl;
                    
      printf("Saving analysis settings to file.\n");
   }                
   else             
      printf("Error! Save file can not be opened.\n");
   osettings.close();
}

// Function for getting all measurement settings from a file
void TGAppMainFrame::GetMeasSettings()
{
   char *cTemp, readTemp[1024];
   cTemp = new char[1024];
   sprintf(cTemp, "%s/settings/measurement_settings.txt", rootdir);

   int iTemp, k = 0;
   double dTemp;

   std::ifstream isettings;
   
   isettings.open(cTemp, std::ifstream::in);
   if(isettings.is_open())
   {
      printf("Opening measurement settings from file.\n");

      while(1)
      {
         if(isettings.peek() == '#')
         {
            isettings.getline(readTemp, 1024, '\n');
            if(DBGSIG > 1) printf("GetMeasSettings(): readTemp = %s\n", readTemp);
         }
         else if(isettings.peek() == '\n')
            isettings.ignore(1, '\n');
         else
         {  
	    if(k == 0)
	    {
	       // Scan settings
               for(int i = 0; i < 4; i++)
	       {
                  isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  if(iTemp == 0) scansOn->widgetChBox[i]->SetState(kButtonUp);
                  else if(iTemp == 1) scansOn->widgetChBox[i]->SetState(kButtonDown);
	       }
	       // Hard voltage limit
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               vHardlimit->widgetNE[0]->SetNumber(dTemp);
	       // Number of channels
	       isettings >> iTemp;
	       isettings.ignore(1, '\n');
               NCH->widgetNE[0]->SetNumber(iTemp);
	       // Position units
	       isettings >> iTemp;
	       isettings.ignore(1, '\n');
               posUnits->widgetCB->Select(iTemp);
	       // Rotation units
	       isettings >> iTemp;
	       isettings.ignore(1, '\n');
               rotUnits->widgetCB->Select(iTemp);
	       // Oscilloscope IP address
               isettings.getline(readTemp, 1024, '\n');
               oscConnect->widgetTE->SetText(readTemp);
	       // Laser info
               isettings.getline(readTemp, 1024, '\n');
               laserInfo->widgetTE->SetText(readTemp);
	       // Chamber temperature
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               chtemp->widgetNE[0]->SetNumber(dTemp);
	       // Live display
               isettings >> iTemp;
	       isettings.ignore(1, '\n');
               if(iTemp == 0) liveDisp->widgetChBox[0]->SetState(kButtonUp);
               else if(iTemp == 1) liveDisp->widgetChBox[0]->SetState(kButtonDown);

	       k++;
	    }
	    else if(k == 1)
	    {
	       // Voltage supply channel
	       isettings >> iTemp;
	       isettings.ignore(1, '\n');
               vOutCh->widgetCB->Select(iTemp);
	       // Voltage supply options
               for(int i = 0; i < 2; i++)
	       {
                  isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  if(iTemp == 0) vOutOpt->widgetChBox[i]->SetState(kButtonUp);
                  else if(iTemp == 1) vOutOpt->widgetChBox[i]->SetState(kButtonDown);
	       }
	       // Voltage supply min, max and step
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               vOutStart->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               vOutStop->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               vOutStep->widgetNE[0]->SetNumber(dTemp);
	       // Z axis scan min, max and step
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               zPosMin->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               zPosMax->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               zPosStep->widgetNE[0]->SetNumber(dTemp);
	       // X axis scan min, max and step
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               xPosMin->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               xPosMax->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               xPosStep->widgetNE[0]->SetNumber(dTemp);
	       // Y axis scan min, max and step
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               yPosMin->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               yPosMax->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               yPosStep->widgetNE[0]->SetNumber(dTemp);
	       // Rotation min, max and step
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               rotPosMin->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               rotPosMax->widgetNE[0]->SetNumber(dTemp);
	       isettings >> dTemp;
	       isettings.ignore(1, '\n');
               rotPosStep->widgetNE[0]->SetNumber(dTemp);
	       // Number of events
	       isettings >> iTemp;
	       isettings.ignore(1, '\n');
               evtNum->widgetNE[0]->SetNumber(iTemp);
	       // Save filename
               isettings.getline(readTemp, 1024, '\n');
               fileName->widgetTE->SetText(readTemp);

	       k++;
	    }
	    else
	       break;
	 }
      }
   }
   else
      printf("Error! Measurement settings file can not be opened. Using defaults.\n");
}

// Function for getting all analysis settings from a file
void TGAppMainFrame::GetAnalSettings()
{
   char *cTemp, readTemp[1024];
   cTemp = new char[1024];
   sprintf(cTemp, "%s/settings/analysis_settings.txt", rootdir);

   int iTemp, k = 0;
   double dTemp;

   std::ifstream isettings;
   
   isettings.open(cTemp, std::ifstream::in);
   if(isettings.is_open())
   {
      printf("Opening analysis settings from file.\n");

      while(1)
      {
         if(isettings.peek() == '#')
         {
            isettings.getline(readTemp, 1024, '\n');
            if(DBGSIG > 1) printf("GetAnalSettings(): readTemp = %s\n", readTemp);
         }
         else if(isettings.peek() == '\n')
            isettings.ignore(1, '\n');
         else
         {  
	    if(k == 0)
	    {
	       // Integrate spectrum options
               for(int i = 0; i < 3; i++)
	       {
                  isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  if(iTemp == 0) intSpect->widgetChBox[i]->SetState(kButtonUp);
                  else if(iTemp == 1) intSpect->widgetChBox[i]->SetState(kButtonDown);
	       }
	       // Integrate spectrum 2D resolution
               for(int i = 0; i < 2; i++)
	       {
	          isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  resol2d->widgetNE[i]->SetNumber(iTemp);
	       }
	       // Relative PDE normalization
               isettings >> iTemp;
	       isettings.ignore(1, '\n');
               if(iTemp == 0) relPde->widgetChBox[0]->SetState(kButtonUp);
               else if(iTemp == 1) relPde->widgetChBox[0]->SetState(kButtonDown);
	       // Relative PDE middle peak
               isettings >> iTemp;
	       isettings.ignore(1, '\n');
               if(iTemp == 0) midPeak->widgetChBox[0]->SetState(kButtonUp);
               else if(iTemp == 1) midPeak->widgetChBox[0]->SetState(kButtonDown);
	       // Relative PDE zero angle setting
               isettings >> dTemp;
               isettings.ignore(1, '\n');
               zeroAngle->widgetNE[0]->SetNumber(dTemp);
	       // Breakdown voltage minimum number of detected peaks
               isettings >> iTemp;
               isettings.ignore(1, '\n');
               minPeak->widgetNE[0]->SetNumber(iTemp);
	       // Breakdown voltage separation calculation
               isettings >> iTemp;
               isettings.ignore(1, '\n');
               peakSepCalc->widgetNE[0]->SetNumber(iTemp);
	       // Surface scan options
               for(int i = 0; i < 2; i++)
	       {
                  isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  if(iTemp == 0) surfScanOpt->widgetChBox[i]->SetState(kButtonUp);
                  else if(iTemp == 1) surfScanOpt->widgetChBox[i]->SetState(kButtonDown);
	       }
	       // Surface scan 2D resolution
               for(int i = 0; i < 2; i++)
	       {
	          isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  resolSurf->widgetNE[i]->SetNumber(iTemp);
	       }
	       // Fitting - sigma
               isettings >> dTemp;
               isettings.ignore(1, '\n');
               fitSigma->widgetNE[0]->SetNumber(dTemp);
	       // Fitting - S/N ratio
               isettings >> dTemp;
               isettings.ignore(1, '\n');
               fitTresh->widgetNE[0]->SetNumber(dTemp);
	       // Fitting - interpolation
               isettings >> iTemp;
               isettings.ignore(1, '\n');
               fitInter->widgetNE[0]->SetNumber(iTemp);
	       // Fitting - ADC offset
               isettings >> dTemp;
               isettings.ignore(1, '\n');
               adcOffset->widgetNE[0]->SetNumber(dTemp);
	       // Fitting - Acceptable error
               isettings >> dTemp;
               isettings.ignore(1, '\n');
               accError->widgetNE[0]->SetNumber(dTemp);
	       // Fitting - Pedestal lower limit
               isettings >> dTemp;
               isettings.ignore(1, '\n');
               pedesLow->widgetNE[0]->SetNumber(dTemp);
	       // Fitting - settings
               for(int i = 0; i < 2; i++)
	       {
                  isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  if(iTemp == 0) fitChecks->widgetChBox[i]->SetState(kButtonUp);
                  else if(iTemp == 1) fitChecks->widgetChBox[i]->SetState(kButtonDown);
	       }

	       k++;
	    }
	    else if(k == 1)
	    {
	       // ADC range
               for(int i = 0; i < 2; i++)
	       {
	          isettings >> dTemp;
	          isettings.ignore(1, '\n');
                  adcRange->widgetNE[i]->SetNumber(dTemp);
	       }
	       // TDC range
               for(int i = 0; i < 2; i++)
	       {
	          isettings >> dTemp;
	          isettings.ignore(1, '\n');
                  tdcRange->widgetNE[i]->SetNumber(dTemp);
	       }
	       // Y axis range
               for(int i = 0; i < 2; i++)
	       {
	          isettings >> dTemp;
	          isettings.ignore(1, '\n');
                  yRange->widgetNE[i]->SetNumber(dTemp);
	       }
	       // Displayed channel
               isettings >> iTemp;
               isettings.ignore(1, '\n');
               selectCh->widgetNE[0]->SetNumber(iTemp);
	       // Display options
               for(int i = 0; i < 2; i++)
	       {
                  isettings >> iTemp;
	          isettings.ignore(1, '\n');
                  if(iTemp == 0) histOpt->widgetChBox[i]->SetState(kButtonUp);
                  else if(iTemp == 1) histOpt->widgetChBox[i]->SetState(kButtonDown);
	       }
	       // Display position units
	       isettings >> iTemp;
	       isettings.ignore(1, '\n');
               posUnitsPlot->widgetCB->Select(iTemp);

	       k++;
	    }
	    else
	       break;
	 }
      }
   }
   else
      printf("Error! Measurement settings file can not be opened. Using defaults.\n");
}*/

// Layout function for the main window (width and height)
void layoutMainWindow(int *w, int *h)
{
   std::ifstream ilayout;
   
   char *cTemp, *cTemp2, readTemp[1024];
   cTemp = new char[512];
   cTemp2 = new char[512];
   sprintf(cTemp, "%s/layout/selected_layout.txt", rootdir);
   ilayout.open(cTemp, std::ifstream::in);
   if(ilayout.is_open())
   {
      ilayout >> cTemp2;
   }
   ilayout.close();
   if(DBGSIG) printf("layoutMainWindow(): Loaded layout file is: %s\n", cTemp2);

   sprintf(cTemp, "%s/layout/%s", rootdir, cTemp2);
   ilayout.open(cTemp, std::ifstream::in);
   if(ilayout.is_open())
   {
      while(1)
      {
         if(ilayout.peek() == '#')
	    ilayout.getline(readTemp, 1024, '\n');
	 else if(ilayout.peek() == '\n')
	    ilayout.ignore(1, '\n');
	 else
	 {  
	    ilayout >> *w >> *h;
	    if(DBGSIG > 1) printf("layoutMainWindow(): W = %d, H = %d\n", *w, *h);
	    break;
	 }
      }
   }

   ilayout.close();
   delete[] cTemp;
   delete[] cTemp2;
}

