#include "../include/substructure.h"
#include "../include/workstation.h"
#include <stdio.h> 

TSubStructure::TSubStructure()
{
   // ID of certain objects
   /*
    * id = 0 -> Label + Text Entry
    * id = 1 -> Label + Text Entry + Button
    * id = 2 -> Label + Button
    * id = 3 -> Label + Number Entry
    * id = 4 -> Text Entry + Button
    * id = 5 -> Label + Dropdown menu
    * id = 6 -> 2 or more buttons
    * id = 7 -> Label + 2 Number Entries
    * id = 8 -> Checkbutton list
    * id = 9 -> Button + horizontal progress bar
    * id = 10 -> Label + horizontal progress bar
    * id = 11 -> Button + horizontal progress bar + Text Entry
    * id = 12 -> Checkbutton + Number Entry
    * id = 13 -> Checkbutton + Text Entry
    * id = 14 -> Label + 2 or more buttons
    * id = 15 -> Label + Number Entry + Button
    * id = 16 -> Label + Text Entry + 2 or more buttons
    * id = 17 -> Label + Dropdown menu + 2 or more buttons
   */
}

TSubStructure::~TSubStructure()
{
   // TODO (delete and new not working as they should)
   delete f0;
   delete f0expandX;
   delete f1;
   delete f2;
   delete f3;
   delete outsidebox;
   delete lab;
   delete fH1;

   delete widgetTE;
   for(int i = 0; i < 6; i++) delete widgetTB[i];
   for(int i = 0; i < 2; i++) delete widgetNE[i];
   delete widgetCB;
   for(int i = 0; i < 9; i++) delete widgetChBox[i];
   delete widgetPB;
}

// Widget with Label and Text Entry
bool TSubStructure::TGLabelTEntry(TGWindow *parent, int w, int h, const char *label, const char *deftext = "", const char *layout = "oneline")
{
   id = 0;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);

   if(strcmp("oneline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,6,2,2,2);
   }
   else if(strcmp("twoline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kVerticalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,0,2,2,2);
   }
   lab = new TGLabel(outsidebox, label);
   outsidebox->AddFrame(lab, f0);
   widgetTE = new TGTextEntry(outsidebox, deftext);
   outsidebox->AddFrame(widgetTE, f0expandX);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label, Text Entry and Button
bool TSubStructure::TGLabelTEntryButton(TGWindow *parent, int w, int h, const char *label, const char *deftext = "", const char *buttext = "Set", const char *layout = "oneline")
{
   id = 1;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);

   if(strcmp("oneline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,6,6,2,2);
      lab = new TGLabel(outsidebox, label);
      outsidebox->AddFrame(lab, f0);
      widgetTE = new TGTextEntry(outsidebox, deftext);
      widgetTE->Resize(w-12,22);
      outsidebox->AddFrame(widgetTE, f0expandX);
      widgetTB[0] = new TGTextButton(outsidebox, buttext);
      widgetTB[0]->SetTextJustify(36);
      widgetTB[0]->SetWrapLength(-1);
      widgetTB[0]->Resize(60,22);
      outsidebox->AddFrame(widgetTB[0], f0);
   }
   else if(strcmp("twoline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kVerticalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,0,6,2,2);
      lab = new TGLabel(outsidebox, label);
      outsidebox->AddFrame(lab, f0);

      fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
      if(DBGSIG > 1) fH1->SetBackgroundColor(200);
      widgetTE = new TGTextEntry(fH1, deftext);
      widgetTE->Resize(w-12,22);
      fH1->AddFrame(widgetTE, f0expandX);
      widgetTB[0] = new TGTextButton(fH1, buttext);
      widgetTB[0]->SetTextJustify(36);
      widgetTB[0]->SetWrapLength(-1);
      widgetTB[0]->Resize(60,22);
      fH1->AddFrame(widgetTB[0], f0);
      outsidebox->AddFrame(fH1, f0expandX);
   }

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label and Button
bool TSubStructure::TGLabelButton(TGWindow *parent, int w, int h, const char *label, const char *buttext = "Set", const char *pos = "left")
{
   id = 2;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   widgetTB[0] = new TGTextButton(fH1, buttext);
   widgetTB[0]->SetTextJustify(36);
   widgetTB[0]->SetWrapLength(-1);
   widgetTB[0]->Resize(60,22);
   fH1->AddFrame(widgetTB[0], f1);
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label and Number Entry
bool TSubStructure::TGLabelNEntry(TGWindow *parent, int w, int h, const char *label, double defval, double *format, const char *pos)
{
   id = 3;

   TGNumberFormat::EStyle numtype;
   TGNumberFormat::EAttribute negpos;
   TGNumberFormat::ELimit numlim;
   bool arelimits[] = {false,false};

   // Number type (integer, real)
   if( (int)format[1] == 0 ) numtype = TGNumberFormat::kNESInteger;
   else if( (int)format[1] == 1 ) numtype = TGNumberFormat::kNESRealOne;
   else if( (int)format[1] == 2 ) numtype = TGNumberFormat::kNESRealTwo;
   else if( (int)format[1] == 3 ) numtype = TGNumberFormat::kNESRealThree;
   else if( (int)format[1] == 4 ) numtype = TGNumberFormat::kNESRealFour;
   else if( (int)format[1] == -1 ) numtype = TGNumberFormat::kNESReal;

   // Negative or positive
   if( (int)format[2] == 0 ) negpos = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format[2] == 1 ) negpos = TGNumberFormat::kNEAPositive;
   else if( (int)format[2] == 2 ) negpos = TGNumberFormat::kNEANonNegative;

   // Limits
   if( (int)format[3] == 0 ) numlim = TGNumberFormat::kNELNoLimits;
   else if( (int)format[3] == 1 ) { numlim = TGNumberFormat::kNELLimitMax; arelimits[1] = true; }
   else if( (int)format[3] == 2 ) { numlim = TGNumberFormat::kNELLimitMinMax; arelimits[0] = true; arelimits[1] = true; }
   else if( (int)format[3] == -1 ) { numlim = TGNumberFormat::kNELLimitMin; arelimits[0] = true; }

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   if( (int)format[1] == 0 )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], (int)format[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, 0, (int)format[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim);
   }
   else if( (((int)format[1] > 0) && ((int)format[1] < 5)) || ((int)format[1] == -1) )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], format[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, 0, format[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim);
   }
   else
   {
      delete outsidebox;
      delete lab;
      return false;
   }
   fH1->AddFrame(widgetNE[0], f1);
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Text Entry and Button
bool TSubStructure::TGTEntryButton(TGWindow *parent, int w, int h, const char *deftext = "", const char *buttext = "Set")
{
   id = 4;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,2,6,2,2);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   widgetTE = new TGTextEntry(outsidebox, deftext);
   widgetTE->Resize(w-12,22);
   outsidebox->AddFrame(widgetTE, f0expandX);
   widgetTB[0] = new TGTextButton(outsidebox, buttext);
   widgetTB[0]->SetTextJustify(36);
   widgetTB[0]->SetWrapLength(-1);
   widgetTB[0]->Resize(60,22);
   outsidebox->AddFrame(widgetTB[0], f0);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label and Dropdown menu
bool TSubStructure::TGLabelDrop(TGWindow *parent, int w, int h, const char *label, int nrentries, vector<string> entrytext, const char *selecttext)
{
   id = 5;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,6,2,2,2);

   int sel = 0;

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   lab = new TGLabel(outsidebox, label);
   outsidebox->AddFrame(lab, f0);
   widgetCB = new TGComboBox(outsidebox, 200);
   for(int i = 0; i < nrentries; i++)
   {
      widgetCB->AddEntry(entrytext[i].c_str(), i);
      if( strcmp(entrytext[i].c_str(), selecttext) == 0 )
         sel = i;
   }
   widgetCB->Resize(50,22);
   widgetCB->Select(sel);
   outsidebox->AddFrame(widgetCB, f0expandX);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with 2 or more buttons (up to 6)
bool TSubStructure::TGMultiButton(TGWindow *parent, int w, int h, int nrbuttons, vector<string> buttext, const char *pos)
{
   id = 6;

   if(nrbuttons > 6) return false;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   for(int i = 0; i < nrbuttons; i++)
   {
      widgetTB[i] = new TGTextButton(fH1, buttext[i].c_str());
      widgetTB[i]->SetTextJustify(36);
      widgetTB[i]->SetWrapLength(-1);
      widgetTB[i]->Resize((w-6-8)/nrbuttons,22);
      if(i == 0) fH1->AddFrame(widgetTB[i], f0);
      else fH1->AddFrame(widgetTB[i], f1);
   }
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label and two Number Entries
bool TSubStructure::TGLabelDoubleNEntry(TGWindow *parent, int w, int h, const char *label, double defval1, double *format1, double defval2, double *format2, const char *pos)
{
   id = 7;

   TGNumberFormat::EStyle numtype1, numtype2;
   TGNumberFormat::EAttribute negpos1, negpos2;
   TGNumberFormat::ELimit numlim1, numlim2;
   bool arelimits[] = {false,false,false,false};

   // Number type (integer, real)
   if( (int)format1[1] == 0 ) numtype1 = TGNumberFormat::kNESInteger;
   else if( (int)format1[1] == 1 ) numtype1 = TGNumberFormat::kNESRealOne;
   else if( (int)format1[1] == 2 ) numtype1 = TGNumberFormat::kNESRealTwo;
   else if( (int)format1[1] == 3 ) numtype1 = TGNumberFormat::kNESRealThree;
   else if( (int)format1[1] == 4 ) numtype1 = TGNumberFormat::kNESRealFour;
   else if( (int)format1[1] == -1 ) numtype1 = TGNumberFormat::kNESReal;

   if( (int)format2[1] == 0 ) numtype2 = TGNumberFormat::kNESInteger;
   else if( (int)format2[1] == 1 ) numtype2 = TGNumberFormat::kNESRealOne;
   else if( (int)format2[1] == 2 ) numtype2 = TGNumberFormat::kNESRealTwo;
   else if( (int)format2[1] == 3 ) numtype2 = TGNumberFormat::kNESRealThree;
   else if( (int)format2[1] == 4 ) numtype2 = TGNumberFormat::kNESRealFour;
   else if( (int)format2[1] == -1 ) numtype2 = TGNumberFormat::kNESReal;

   // Negative or positive
   if( (int)format1[2] == 0 ) negpos1 = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format1[2] == 1 ) negpos1 = TGNumberFormat::kNEAPositive;
   else if( (int)format1[2] == 2 ) negpos1 = TGNumberFormat::kNEANonNegative;

   if( (int)format2[2] == 0 ) negpos2 = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format2[2] == 1 ) negpos2 = TGNumberFormat::kNEAPositive;
   else if( (int)format2[2] == 2 ) negpos2 = TGNumberFormat::kNEANonNegative;

   // Limits
   if( (int)format1[3] == 0 ) numlim1 = TGNumberFormat::kNELNoLimits;
   else if( (int)format1[3] == 1 ) { numlim1 = TGNumberFormat::kNELLimitMax; arelimits[1] = true; }
   else if( (int)format1[3] == 2 ) { numlim1 = TGNumberFormat::kNELLimitMinMax; arelimits[0] = true; arelimits[1] = true; }
   else if( (int)format1[3] == -1 ) { numlim1 = TGNumberFormat::kNELLimitMin; arelimits[0] = true; }

   if( (int)format2[3] == 0 ) numlim2 = TGNumberFormat::kNELNoLimits;
   else if( (int)format2[3] == 1 ) { numlim2 = TGNumberFormat::kNELLimitMax; arelimits[3] = true; }
   else if( (int)format2[3] == 2 ) { numlim2 = TGNumberFormat::kNELLimitMinMax; arelimits[2] = true; arelimits[3] = true; }
   else if( (int)format2[3] == -1 ) { numlim2 = TGNumberFormat::kNELLimitMin; arelimits[2] = true; }

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   if( (int)format1[1] == 0 )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, (int)format1[4], (int)format1[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, (int)format1[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, 0, (int)format1[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1);
   }
   else if( (((int)format1[1] > 0) && ((int)format1[1] < 5)) || ((int)format1[1] == -1) )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, format1[4], format1[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, format1[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, 0, format1[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1);
   }
   else
   {
      delete outsidebox;
      delete lab;
      return false;
   }
   fH1->AddFrame(widgetNE[0], f1);

   if( (int)format2[1] == 0 )
   {
      if( arelimits[2] && arelimits[3] )
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, (int)format2[4], (int)format2[5]);
      else if( arelimits[2] )
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, (int)format2[4], 0);
      else if( arelimits[3] )
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, 0, (int)format2[4]);
      else
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2);
   }
   else if( (((int)format2[1] > 0) && ((int)format2[1] < 5)) || ((int)format2[1] == -1) )
   {
      if( arelimits[2] && arelimits[3] )
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, format2[4], format2[5]);
      else if( arelimits[2] )
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, format2[4], 0);
      else if( arelimits[3] )
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, 0, format2[4]);
      else
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2);
   }
   else
   {
      delete outsidebox;
      delete lab;
      return false;
   }
   fH1->AddFrame(widgetNE[1], f1);
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with 1 or more check boxes (up to 12)
bool TSubStructure::TGCheckList(TGWindow *parent, int w, int h, int nrchecks, vector<string> labels, vector<int> onoff, const char *layout, const char *pos)
{
   id = 8;

   if(nrchecks > 18) return false;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   if(strcmp("left",pos) == 0)
      f3 = new TGLayoutHints(kLHintsLeft,2,2,2,2);
   else if(strcmp("center",pos) == 0)
      f3 = new TGLayoutHints(kLHintsCenterX,2,2,2,2);
   else if(strcmp("right",pos) == 0)
      f3 = new TGLayoutHints(kLHintsRight,2,2,2,2);

   double nrlines = -1.;
   if(strcmp("horizontal", layout) == 0) nrlines = 0.;
   else if(strcmp("vertical", layout) == 0) nrlines = 0.;
   else if(strcmp("twoline", layout) == 0) nrlines = 2.;
   else if(strcmp("threeline", layout) == 0) nrlines = 3.;
   else if(strcmp("fourline", layout) == 0) nrlines = 4.;
   else if(strcmp("fiveline", layout) == 0) nrlines = 5.;
   else if(strcmp("sixline", layout) == 0) nrlines = 6.;
   else if(strcmp("sevenline", layout) == 0) nrlines = 7.;
   else if(strcmp("eightline", layout) == 0) nrlines = 8.;
   else if(strcmp("nineline", layout) == 0) nrlines = 9.;

   if(nrlines == 0.)
   {
      if(strcmp("horizontal", layout) == 0)
      {
         outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
         if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
         fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
         if(DBGSIG > 1) fH1->SetBackgroundColor(200);

         for(int i = 0; i < nrchecks; i++)
         {
            widgetChBox[i] = new TGCheckButton(fH1, labels[i].c_str());
            if(onoff[i] == 0)
               widgetChBox[i]->SetState(kButtonUp);
            else if(onoff[i] == 1)
               widgetChBox[i]->SetState(kButtonDown);
            widgetChBox[i]->Resize((w-6-8)/nrchecks,22);
            if(i == 0) fH1->AddFrame(widgetChBox[i], f0);
            else fH1->AddFrame(widgetChBox[i], f1);
         }

         outsidebox->AddFrame(fH1, f2);
      }
      else if(strcmp("vertical", layout) == 0)
      {
         outsidebox = new TGCompositeFrame(parent, w-6, h, kVerticalFrame | kFixedWidth); 
         if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);

         for(int i = 0; i < nrchecks; i++)
         {
            widgetChBox[i] = new TGCheckButton(outsidebox, labels[i].c_str());
            if(onoff[i] == 0)
               widgetChBox[i]->SetState(kButtonUp);
            else if(onoff[i] == 1)
               widgetChBox[i]->SetState(kButtonDown);
            widgetChBox[i]->Resize((w-6-8)/nrchecks,22);
            outsidebox->AddFrame(widgetChBox[i], f3);
         }
      }
   }
   else if(nrlines > 0.0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kVerticalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);

      for(int j = 0; j < (int)nrlines; j++)
      {
         fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
         if(DBGSIG > 1) fH1->SetBackgroundColor(200);

         for(int i = TMath::Ceil(j*nrchecks/nrlines); i < TMath::Ceil((j+1.)*nrchecks/nrlines); i++)
         {
            widgetChBox[i] = new TGCheckButton(fH1, labels[i].c_str());
            if(onoff[i] == 0)
               widgetChBox[i]->SetState(kButtonUp);
            else if(onoff[i] == 1)
               widgetChBox[i]->SetState(kButtonDown);
            widgetChBox[i]->Resize((w-6-8)/nrchecks,22);
            if(i == TMath::Ceil(j*nrchecks/nrlines)) fH1->AddFrame(widgetChBox[i], f0);
            else fH1->AddFrame(widgetChBox[i], f1);
         }
         outsidebox->AddFrame(fH1, f2);
      }
   }
   else
      return false;

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Button and horizontal progress bar
bool TSubStructure::TGButtonProgress(TGWindow *parent, int w, int h, const char *buttext = "Set")
{
   id = 9;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,20,2,2,2);
   f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   widgetTB[0] = new TGTextButton(fH1, buttext);
   widgetTB[0]->SetTextJustify(36);
   widgetTB[0]->SetWrapLength(-1);
   widgetTB[0]->Resize(60,22);
   fH1->AddFrame(widgetTB[0], f0);
   widgetPB = new TGHProgressBar(fH1, TGProgressBar::kStandard, w/2);
   widgetPB->ShowPosition();
   widgetPB->SetRange(0,100);
   widgetPB->SetBarColor("green");
   fH1->AddFrame(widgetPB, f1);

   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Button and horizontal progress bar
bool TSubStructure::TGButtonProgressTEntry(TGWindow *parent, int w, int h, const char *buttext = "Set", const char *deftext = "Remaining time: ")
{
   id = 11;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,20,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,20,2,2,2);
   f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 5*w/6, h, kFixedWidth); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   widgetTB[0] = new TGTextButton(fH1, buttext);
   widgetTB[0]->SetTextJustify(36);
   widgetTB[0]->SetWrapLength(-1);
   widgetTB[0]->Resize(60,22);
   fH1->AddFrame(widgetTB[0], f0);
   widgetPB = new TGHProgressBar(fH1, TGProgressBar::kStandard, w/4);
   widgetPB->ShowPosition();
   widgetPB->SetRange(0,100);
   widgetPB->SetBarColor("green");
   fH1->AddFrame(widgetPB, f1);
   widgetTE = new TGTextEntry(fH1, deftext);
   fH1->AddFrame(widgetTE, f0expandX);

   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label and horizontal progress bar
bool TSubStructure::TGLabelProgress(TGWindow *parent, int w, int h, const char *label)
{
   id = 10;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,20,2,2,2);
   f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   widgetPB = new TGHProgressBar(fH1, TGProgressBar::kStandard, w/2);
   widgetPB->ShowPosition();
   widgetPB->SetRange(0,100);
   widgetPB->SetBarColor("green");
   fH1->AddFrame(widgetPB, f1);

   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with checkbutton and number entry
bool TSubStructure::TGCheckNEntry(TGWindow *parent, int w, int h, const char *label, int onoff, double defval, double *format, const char *pos)
{
   id = 12;

   TGNumberFormat::EStyle numtype;
   TGNumberFormat::EAttribute negpos;
   TGNumberFormat::ELimit numlim;
   bool arelimits[] = {false,false};

   // Number type (integer, real)
   if( (int)format[1] == 0 ) numtype = TGNumberFormat::kNESInteger;
   else if( (int)format[1] == 1 ) numtype = TGNumberFormat::kNESRealOne;
   else if( (int)format[1] == 2 ) numtype = TGNumberFormat::kNESRealTwo;
   else if( (int)format[1] == 3 ) numtype = TGNumberFormat::kNESRealThree;
   else if( (int)format[1] == 4 ) numtype = TGNumberFormat::kNESRealFour;
   else if( (int)format[1] == -1 ) numtype = TGNumberFormat::kNESReal;

   // Negative or positive
   if( (int)format[2] == 0 ) negpos = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format[2] == 1 ) negpos = TGNumberFormat::kNEAPositive;
   else if( (int)format[2] == 2 ) negpos = TGNumberFormat::kNEANonNegative;

   // Limits
   if( (int)format[3] == 0 ) numlim = TGNumberFormat::kNELNoLimits;
   else if( (int)format[3] == 1 ) { numlim = TGNumberFormat::kNELLimitMax; arelimits[1] = true; }
   else if( (int)format[3] == 2 ) { numlim = TGNumberFormat::kNELLimitMinMax; arelimits[0] = true; arelimits[1] = true; }
   else if( (int)format[3] == -1 ) { numlim = TGNumberFormat::kNELLimitMin; arelimits[0] = true; }

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   widgetChBox[0] = new TGCheckButton(fH1, label);
   if(onoff == 0)
      widgetChBox[0]->SetState(kButtonUp);
   else if(onoff == 1)
      widgetChBox[0]->SetState(kButtonDown);
//   widgetChBox[0]->Resize(w-14,22);
   fH1->AddFrame(widgetChBox[0], f0);
   if( (int)format[1] == 0 )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], (int)format[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, 0, (int)format[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim);
   }
   else if( (((int)format[1] > 0) && ((int)format[1] < 5)) || ((int)format[1] == -1) )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], format[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, 0, format[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim);
   }
   else
   {
      delete outsidebox;
      delete lab;
      return false;
   }
   fH1->AddFrame(widgetNE[0], f1);
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with checkbutton and text entry
bool TSubStructure::TGCheckTEntry(TGWindow *parent, int w, int h, const char *label, int onoff, const char *deftext, const char *layout)
{
   id = 13;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);

   if(strcmp("oneline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,6,2,2,2);
   }
   else if(strcmp("twoline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kVerticalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,0,2,2,2);
   }

   widgetChBox[0] = new TGCheckButton(outsidebox, label);
   if(onoff == 0)
      widgetChBox[0]->SetState(kButtonUp);
   else if(onoff == 1)
      widgetChBox[0]->SetState(kButtonDown);
   outsidebox->AddFrame(widgetChBox[0], f0);
   widgetTE = new TGTextEntry(outsidebox, deftext);
   outsidebox->AddFrame(widgetTE, f0expandX);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with checkbutton and double number entry
bool TSubStructure::TGCheckDoubleNEntry(TGWindow *parent, int w, int h, const char *label, int onoff, double defval1, double *format1, double defval2, double *format2, const char *pos)
{
   id = 14;

   TGNumberFormat::EStyle numtype1, numtype2;
   TGNumberFormat::EAttribute negpos1, negpos2;
   TGNumberFormat::ELimit numlim1, numlim2;
   bool arelimits[] = {false,false,false,false};

   // Number type (integer, real)
   if( (int)format1[1] == 0 ) numtype1 = TGNumberFormat::kNESInteger;
   else if( (int)format1[1] == 1 ) numtype1 = TGNumberFormat::kNESRealOne;
   else if( (int)format1[1] == 2 ) numtype1 = TGNumberFormat::kNESRealTwo;
   else if( (int)format1[1] == 3 ) numtype1 = TGNumberFormat::kNESRealThree;
   else if( (int)format1[1] == 4 ) numtype1 = TGNumberFormat::kNESRealFour;
   else if( (int)format1[1] == -1 ) numtype1 = TGNumberFormat::kNESReal;

   if( (int)format2[1] == 0 ) numtype2 = TGNumberFormat::kNESInteger;
   else if( (int)format2[1] == 1 ) numtype2 = TGNumberFormat::kNESRealOne;
   else if( (int)format2[1] == 2 ) numtype2 = TGNumberFormat::kNESRealTwo;
   else if( (int)format2[1] == 3 ) numtype2 = TGNumberFormat::kNESRealThree;
   else if( (int)format2[1] == 4 ) numtype2 = TGNumberFormat::kNESRealFour;
   else if( (int)format2[1] == -1 ) numtype2 = TGNumberFormat::kNESReal;

   // Negative or positive
   if( (int)format1[2] == 0 ) negpos1 = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format1[2] == 1 ) negpos1 = TGNumberFormat::kNEAPositive;
   else if( (int)format1[2] == 2 ) negpos1 = TGNumberFormat::kNEANonNegative;

   if( (int)format2[2] == 0 ) negpos2 = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format2[2] == 1 ) negpos2 = TGNumberFormat::kNEAPositive;
   else if( (int)format2[2] == 2 ) negpos2 = TGNumberFormat::kNEANonNegative;

   // Limits
   if( (int)format1[3] == 0 ) numlim1 = TGNumberFormat::kNELNoLimits;
   else if( (int)format1[3] == 1 ) { numlim1 = TGNumberFormat::kNELLimitMax; arelimits[1] = true; }
   else if( (int)format1[3] == 2 ) { numlim1 = TGNumberFormat::kNELLimitMinMax; arelimits[0] = true; arelimits[1] = true; }
   else if( (int)format1[3] == -1 ) { numlim1 = TGNumberFormat::kNELLimitMin; arelimits[0] = true; }

   if( (int)format2[3] == 0 ) numlim2 = TGNumberFormat::kNELNoLimits;
   else if( (int)format2[3] == 1 ) { numlim2 = TGNumberFormat::kNELLimitMax; arelimits[1] = true; }
   else if( (int)format2[3] == 2 ) { numlim2 = TGNumberFormat::kNELLimitMinMax; arelimits[0] = true; arelimits[1] = true; }
   else if( (int)format2[3] == -1 ) { numlim2 = TGNumberFormat::kNELLimitMin; arelimits[0] = true; }

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   widgetChBox[0] = new TGCheckButton(fH1, label);
   if(onoff == 0)
      widgetChBox[0]->SetState(kButtonUp);
   else if(onoff == 1)
      widgetChBox[0]->SetState(kButtonDown);
//   widgetChBox[0]->Resize(w-14,22);
   fH1->AddFrame(widgetChBox[0], f0);
   if( (int)format1[1] == 0 )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, (int)format1[4], (int)format1[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, (int)format1[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, 0, (int)format1[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, (int)defval1, (int)format1[0], 999, numtype1, negpos1, numlim1);
   }
   else if( (((int)format1[1] > 0) && ((int)format1[1] < 5)) || ((int)format1[1] == -1) )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, format1[4], format1[5]);
      else if( arelimits[0] )
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, format1[4], 0);
      else if( arelimits[1] )
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1, 0, format1[4]);
      else
         widgetNE[0] = new TGNumberEntry(fH1, defval1, (int)format1[0], 999, numtype1, negpos1, numlim1);
   }
   else
   {
      delete outsidebox;
      delete lab;
      return false;
   }
   fH1->AddFrame(widgetNE[0], f1);

   if( (int)format2[1] == 0 )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, (int)format2[4], (int)format2[5]);
      else if( arelimits[0] )
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, (int)format2[4], 0);
      else if( arelimits[1] )
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, 0, (int)format2[4]);
      else
         widgetNE[1] = new TGNumberEntry(fH1, (int)defval2, (int)format2[0], 999, numtype2, negpos2, numlim2);
   }
   else if( (((int)format2[1] > 0) && ((int)format2[1] < 5)) || ((int)format2[1] == -1) )
   {
      if( arelimits[0] && arelimits[1] )
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, format2[4], format2[5]);
      else if( arelimits[0] )
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, format2[4], 0);
      else if( arelimits[1] )
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2, 0, format2[4]);
      else
         widgetNE[1] = new TGNumberEntry(fH1, defval2, (int)format2[0], 999, numtype2, negpos2, numlim2);
   }
   else
   {
      delete outsidebox;
      delete lab;
      return false;
   }
   fH1->AddFrame(widgetNE[1], f1);

   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with a label and 2 or more buttons (up to 6)
bool TSubStructure::TGLabelMultiButton(TGWindow *parent, int w, int h, const char *label, int nrbuttons, vector<string> buttext, const char *pos)
{
   id = 14;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   for(int i = 0; i < nrbuttons; i++)
   {
      widgetTB[i] = new TGTextButton(fH1, buttext[i].c_str());
      widgetTB[i]->SetTextJustify(36);
      widgetTB[i]->SetWrapLength(-1);
      widgetTB[i]->Resize((w-6-8)/nrbuttons,22);
      if(i == 0) fH1->AddFrame(widgetTB[i], f0);
      else fH1->AddFrame(widgetTB[i], f1);
   }
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label, Number Entry and Button
bool TSubStructure::TGLabelNEntryButton(TGWindow *parent, int w, int h, const char *label, double defval, double *format, const char *buttext, const char *layout)
{
   id = 15;

   TGNumberFormat::EStyle numtype;
   TGNumberFormat::EAttribute negpos;
   TGNumberFormat::ELimit numlim;
   bool arelimits[] = {false,false};

   // Number type (integer, real)
   if( (int)format[1] == 0 ) numtype = TGNumberFormat::kNESInteger;
   else if( (int)format[1] == 1 ) numtype = TGNumberFormat::kNESRealOne;
   else if( (int)format[1] == 2 ) numtype = TGNumberFormat::kNESRealTwo;
   else if( (int)format[1] == 3 ) numtype = TGNumberFormat::kNESRealThree;
   else if( (int)format[1] == 4 ) numtype = TGNumberFormat::kNESRealFour;
   else if( (int)format[1] == -1 ) numtype = TGNumberFormat::kNESReal;

   // Negative or positive
   if( (int)format[2] == 0 ) negpos = TGNumberFormat::kNEAAnyNumber;
   else if( (int)format[2] == 1 ) negpos = TGNumberFormat::kNEAPositive;
   else if( (int)format[2] == 2 ) negpos = TGNumberFormat::kNEANonNegative;

   // Limits
   if( (int)format[3] == 0 ) numlim = TGNumberFormat::kNELNoLimits;
   else if( (int)format[3] == 1 ) { numlim = TGNumberFormat::kNELLimitMax; arelimits[1] = true; }
   else if( (int)format[3] == 2 ) { numlim = TGNumberFormat::kNELLimitMinMax; arelimits[0] = true; arelimits[1] = true; }
   else if( (int)format[3] == -1 ) { numlim = TGNumberFormat::kNELLimitMin; arelimits[0] = true; }

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);

   if(strcmp("oneline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,6,6,2,2);
      lab = new TGLabel(outsidebox, label);
      outsidebox->AddFrame(lab, f0);
      if( (int)format[1] == 0 )
      {
         if( arelimits[0] && arelimits[1] )
            widgetNE[0] = new TGNumberEntry(outsidebox, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], (int)format[5]);
         else if( arelimits[0] )
            widgetNE[0] = new TGNumberEntry(outsidebox, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], 0);
         else if( arelimits[1] )
            widgetNE[0] = new TGNumberEntry(outsidebox, (int)defval, (int)format[0], 999, numtype, negpos, numlim, 0, (int)format[4]);
         else
            widgetNE[0] = new TGNumberEntry(outsidebox, (int)defval, (int)format[0], 999, numtype, negpos, numlim);
      }
      else if( (((int)format[1] > 0) && ((int)format[1] < 5)) || ((int)format[1] == -1) )
      {
         if( arelimits[0] && arelimits[1] )
            widgetNE[0] = new TGNumberEntry(outsidebox, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], format[5]);
         else if( arelimits[0] )
            widgetNE[0] = new TGNumberEntry(outsidebox, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], 0);
         else if( arelimits[1] )
            widgetNE[0] = new TGNumberEntry(outsidebox, defval, (int)format[0], 999, numtype, negpos, numlim, 0, format[4]);
         else
            widgetNE[0] = new TGNumberEntry(outsidebox, defval, (int)format[0], 999, numtype, negpos, numlim);
      }
      else
      {
         delete outsidebox;
         delete lab;
         return false;
      }
      outsidebox->AddFrame(widgetNE[0], f0);
      widgetTB[0] = new TGTextButton(outsidebox, buttext);
      widgetTB[0]->SetTextJustify(36);
      widgetTB[0]->SetWrapLength(-1);
      widgetTB[0]->Resize(60,22);
      outsidebox->AddFrame(widgetTB[0], f0);
   }
   else if(strcmp("twoline", layout) == 0)
   {
      outsidebox = new TGCompositeFrame(parent, w-6, h, kVerticalFrame | kFixedWidth); 
      if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
      f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,0,6,2,2);
      lab = new TGLabel(outsidebox, label);
      outsidebox->AddFrame(lab, f0);

      fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
      if(DBGSIG > 1) fH1->SetBackgroundColor(200);
      if( (int)format[1] == 0 )
      {
         if( arelimits[0] && arelimits[1] )
            widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], (int)format[5]);
         else if( arelimits[0] )
            widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, (int)format[4], 0);
         else if( arelimits[1] )
            widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim, 0, (int)format[4]);
         else
            widgetNE[0] = new TGNumberEntry(fH1, (int)defval, (int)format[0], 999, numtype, negpos, numlim);
      }
      else if( (((int)format[1] > 0) && ((int)format[1] < 5)) || ((int)format[1] == -1) )
      {
         if( arelimits[0] && arelimits[1] )
            widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], format[5]);
         else if( arelimits[0] )
            widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, format[4], 0);
         else if( arelimits[1] )
            widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim, 0, format[4]);
         else
            widgetNE[0] = new TGNumberEntry(fH1, defval, (int)format[0], 999, numtype, negpos, numlim);
      }
      else
      {
         delete outsidebox;
         delete lab;
         return false;
      }
      fH1->AddFrame(widgetNE[0], f0);
      widgetTB[0] = new TGTextButton(fH1, buttext);
      widgetTB[0]->SetTextJustify(36);
      widgetTB[0]->SetWrapLength(-1);
      widgetTB[0]->Resize(60,22);
      fH1->AddFrame(widgetTB[0], f0);
      outsidebox->AddFrame(fH1, f0expandX);
   }

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with a label, text entry and 2 or more buttons (up to 6)
bool TSubStructure::TGLabelTEntryMultiButton(TGWindow *parent, int w, int h, const char *label, const char *deftext, int nrbuttons, vector<string> buttext, const char *pos)
{
   id = 16;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   widgetTE = new TGTextEntry(fH1, deftext);
   fH1->AddFrame(widgetTE, f0expandX);
   for(int i = 0; i < nrbuttons; i++)
   {
      widgetTB[i] = new TGTextButton(fH1, buttext[i].c_str());
      widgetTB[i]->SetTextJustify(36);
      widgetTB[i]->SetWrapLength(-1);
      widgetTB[i]->Resize((w-6-8)/nrbuttons,22);
      if(i == 0) fH1->AddFrame(widgetTB[i], f0);
      else fH1->AddFrame(widgetTB[i], f1);
   }
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}

// Widget with Label, Dropdown menu and 2 or more buttons
bool TSubStructure::TGLabelDropMultiButton(TGWindow *parent, int w, int h, const char *label, int nrentries, vector<string> entrytext, const char *selecttext, int nrbuttons, vector<string> buttext, const char *pos)
{
   id = 17;

   f0 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,2,2,2,2);
   f1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY,6,2,2,2);
   f0expandX = new TGLayoutHints(kLHintsExpandX | kLHintsCenterY,6,2,2,2);
   if(strcmp("left",pos) == 0)
      f2 = new TGLayoutHints(kLHintsLeft,0,0,0,0);
   else if(strcmp("center",pos) == 0)
      f2 = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   else if(strcmp("right",pos) == 0)
      f2 = new TGLayoutHints(kLHintsRight,0,0,0,0);

   int sel = 0;

   outsidebox = new TGCompositeFrame(parent, w-6, h, kHorizontalFrame | kFixedWidth); 
   if(DBGSIG > 1) outsidebox->SetBackgroundColor(0);
   fH1 = new TGHorizontalFrame(outsidebox, 100, h); 
   if(DBGSIG > 1) fH1->SetBackgroundColor(200);
   lab = new TGLabel(fH1, label);
   fH1->AddFrame(lab, f0);
   widgetCB = new TGComboBox(fH1, 200);
   for(int i = 0; i < nrentries; i++)
   {
      widgetCB->AddEntry(entrytext[i].c_str(), i);
      if( strcmp(entrytext[i].c_str(), selecttext) == 0 )
         sel = i;
   }
   widgetCB->Resize(w/3+w/5,22);
   widgetCB->Select(sel);
   fH1->AddFrame(widgetCB, f0expandX);
   for(int i = 0; i < nrbuttons; i++)
   {
      widgetTB[i] = new TGTextButton(fH1, buttext[i].c_str());
      widgetTB[i]->SetTextJustify(36);
      widgetTB[i]->SetWrapLength(-1);
      widgetTB[i]->Resize((w-6-8)/nrbuttons,22);
      if(i == 0) fH1->AddFrame(widgetTB[i], f0);
      else fH1->AddFrame(widgetTB[i], f1);
   }
   outsidebox->AddFrame(fH1, f2);

   if(DBGSIG > 1) printf("id = %d\n",id);

   return true;
}
