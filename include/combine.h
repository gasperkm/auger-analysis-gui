#ifndef _COMBINE_H_
#define _COMBINE_H_

#include "../include/auger-analysis-gui.h"
#include "../include/workstation.h"
/*#if OFFVER == 0
  #include "../include/OfflineIncludeOld.h"
#elif OFFVER == 1
  #include "../include/OfflineIncludeNew.h"
#endif*/
/*
#include <vector>
#include <string.h>*/

void CombineRootfile( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts, vector<string> &filenames );
void MergeRootfile( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts, vector<string> &filenames, int opt );
//void RewriteRootfile( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts, vector<string> &filenames );
void CheckKeys( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts );
void hmerge(int nrfiles, TList *files, char *outname);
void hadd(int nrfiles, TList *files, char *outname);

#endif
