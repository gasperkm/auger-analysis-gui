#ifndef __TEST_H__
#define __TEST_H__

#include "root_include.h"
#include <string.h>
#include <vector>
#include <string>

// Format for Number Entry fields:
// [digit space] [num type] [neg-pos] [limit] [min limit], [max limit]
//
// [digit space]: Number of spaces for the entered digits (width)
// [num type]: 0 = Integer, 1 = Real one digit, 2 = Real two digits, 3 = Real three digits, 4 = Real four digits, -1 = Arbitrary real
// [neg-pos]: 0 = Any number, 1 = Positive numbers, 2 = Nonnegative numbers
// [limit]: 0 = No limits, 1 = Maximum limit, 2 = Min and Max limits, -1 = Minimum limit
// [min limit], [max limit]: Min and Max limits, if applicable

class TSubStructure
{
RQ_OBJECT("TSubStructure")

protected:
   TGLabel *lab;
   TGHorizontalFrame *fH1;
   TGLayoutHints *f0, *f0expandX, *f1, *f2, *f3;
public:
   TSubStructure();
   virtual ~TSubStructure();

   // Label + Text Entry -> layout can be "oneline", "twoline"
   bool TGLabelTEntry(TGWindow *parent, int w, int h, const char *label, const char *deftext, const char *layout);
   // Label + Text Entry + Button -> layout can be "oneline", "twoline"
   bool TGLabelTEntryButton(TGWindow *parent, int w, int h, const char *label, const char *deftext, const char *buttext, const char *layout);
   // Label + Button
   bool TGLabelButton(TGWindow *parent, int w, int h, const char *label, const char *buttext, const char *pos);
   // Label + Number Entry
   bool TGLabelNEntry(TGWindow *parent, int w, int h, const char *label, double defval, double *format, const char *pos);
   // Text Entry + Button
   bool TGTEntryButton(TGWindow *parent, int w, int h, const char *deftext, const char *buttext);
   // Label + Dropdown menu
   bool TGLabelDrop(TGWindow *parent, int w, int h, const char *label, int nrentries, vector<string> entrytext, const char *selecttext);
   // 2 or more buttons
   bool TGMultiButton(TGWindow *parent, int w, int h, int nrbuttons, vector<string> buttext, const char *pos);
   // Label + 2 Number Entries
   bool TGLabelDoubleNEntry(TGWindow *parent, int w, int h, const char *label, double defval1, double *format1, double defval2, double *format2, const char *pos);
   // Checkbutton list (1 - 9) -> layout can be "horizontal", "vertical", "twoline", "threeline"
   bool TGCheckList(TGWindow *parent, int w, int h, int nrchecks, vector<string> labels, vector<int> onoff, const char *layout, const char *pos);
   // Button + Horizontal progress bar
   bool TGButtonProgress(TGWindow *parent, int w, int h, const char *buttext);
   // Label + Horizontal progress bar
   bool TGLabelProgress(TGWindow *parent, int w, int h, const char *label);
   // Button + Horizontal progress bar + Text Entry
   bool TGButtonProgressTEntry(TGWindow *parent, int w, int h, const char *buttext, const char *deftext);
   // Checkbutton + Number entry
   bool TGCheckNEntry(TGWindow *parent, int w, int h, const char *label, int onoff, double defval, double *format, const char *pos);
   // Checkbutton + Text entry
   bool TGCheckTEntry(TGWindow *parent, int w, int h, const char *label, int onoff, const char *deftext, const char *layout);
   // Checkbutton + Number entry
   bool TGCheckDoubleNEntry(TGWindow *parent, int w, int h, const char *label, int onoff, double defval1, double *format1, double defval2, double *format2, const char *pos);
   // Label + 2 or more buttons
   bool TGLabelMultiButton(TGWindow *parent, int w, int h, const char *label, int nrbuttons, vector<string> buttext, const char *pos);
   // Label + Number Entry + Button -> layout can be "oneline", "twoline"
   bool TGLabelNEntryButton(TGWindow *parent, int w, int h, const char *label, double defval, double *format, const char *buttext, const char *layout);
   // Label + Text Entry + 2 or more buttons
   bool TGLabelTEntryMultiButton(TGWindow *parent, int w, int h, const char *label, const char *deftext, int nrbuttons, vector<string> buttext, const char *pos);
   // Label + Dropdown menu + 2 or more buttons
   bool TGLabelDropMultiButton(TGWindow *parent, int w, int h, const char *label, int nrentries, vector<string> entrytext, const char *selecttext, int nrbuttons, vector<string> buttext, const char *pos);

   TGTextEntry *widgetTE;
   TGTextButton *widgetTB[6];
   TGNumberEntry *widgetNE[2];
   TGComboBox *widgetCB;
   TGCompositeFrame *outsidebox;
   TGCheckButton *widgetChBox[18];
   TGHProgressBar *widgetPB;
   int id;
};

#endif
