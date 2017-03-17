#include "../include/combine.h"

void CombineRootfile( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts, vector<string> &filenames )
{
	// TODO fix it!
   cout << "Combining root files." << endl;
   for(int i = 0; i < filenames.size(); i++) cout << "  " << filenames[i] << endl;

   int sigKeys = 0;
// NEW
   for(int i = 0; i < nrkeys.size(); i++)
      sigKeys += (nrkeys[i]-1)/2;
/*   for(int i = 0; i < nrkeys.size(); i++)
      sigKeys += (nrkeys[i]-3)/6;*/
   cout << "Number of signal keys in all files = " << sigKeys << endl;

   int nrfiles = sourcelist->GetSize();
// NEW
   string strOldS, strNewS;
   TTree *treeOldS, *treeNewS, *treeAll;
   TTree *outSig[sigKeys], *outAll;
/*   string strOldS[3], strNewS[3];
   TTree *treeOldS[3], *treeNewS[3], *treeAll[3];
   TTree *outSig[sigKeys], *outSigNeg[sigKeys], *outSigPos[sigKeys], *outAll[3];
   int selectCount[6];*/
   int selectCount[2];
   int fileCount = 0;

   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove(0,2);

   TFile *nextsource = (TFile*)sourcelist->First();
   nextsource->cd(path);
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);

   cout << "Get number of files = " << nrfiles << endl;

   fileCount = 0;
   selectCount[0] = 0;
   selectCount[1] = 0;
/*   selectCount[2] = 0;
   selectCount[3] = 0;
   selectCount[4] = 0;
   selectCount[5] = 0;*/

// NEW
   TList *listAll, *listSig[sigKeys];
   target->cd();
   listAll = new TList;
   for(int j = 0; j < sigKeys; j++)
      listSig[j] = new TList;
/*    TList *listAll[3], *listSig[sigKeys], *listSigNeg[sigKeys], *listSigPos[sigKeys];
   target->cd();
   for(int j = 0; j < 3; j++)
      listAll[j] = new TList;
   for(int j = 0; j < sigKeys; j++)
   {
      listSig[j] = new TList;
      listSigNeg[j] = new TList;
      listSigPos[j] = new TList;
   }*/

   while(nextsource)
   {
// NEW
      cout << "Number of keys in file " << nextsource->GetName() << ": " << nrkeys[fileCount] << "(" << (nrkeys[fileCount]-1)/2 << ")" << endl;
      
      for(int j = 1; j <= (nrkeys[fileCount]-1)/2; j++)
      {
         strOldS = "TreeOldS" + IntToStr(j);
         strNewS = "TreeNewS" + IntToStr(j);

         nextsource->cd(path);

         treeOldS = (TTree*)nextsource->Get(strOldS.c_str());
         treeNewS = (TTree*)nextsource->Get(strNewS.c_str());
         treeAll = (TTree*)nextsource->Get("TreeA");

         // Merge the signal trees
         printf("#- %d %d a ---------------------------------------------\n", fileCount, j);

         if(treeOldS->GetEntries() > 0)	// Mean values
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS->GetEntries(), strOldS.c_str(), nextsource->GetKey(strOldS.c_str())->GetTitle());
            listSig[selectCount[0]]->Add(treeOldS);
            selectCount[0]++;
         }

         if(treeNewS->GetEntries() > 0)	// Mean values
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS->GetEntries(), strNewS.c_str(), nextsource->GetKey(strNewS.c_str())->GetTitle());
            listSig[selectCount[0]]->Add(treeNewS);
            selectCount[0]++;
         }
      }

      cout << "selectCount = " << selectCount[0] << ", " << selectCount[1] <</* ", " << selectCount[2] <<*/ endl;

      // Merge all events
      printf("#- %d c ---------------------------------------------\n", fileCount);
      if(treeAll->GetEntries() > 0)	// Mean values
      {
         printf("Found %d entries in tree \"TreeA\" and title \"%s\"\n", (int)treeAll->GetEntries(), nextsource->GetKey("TreeA")->GetTitle());
         listAll->Add(treeAll);
      }

/*      cout << "Number of keys in file " << nextsource->GetName() << ": " << nrkeys[fileCount] << "(" << (nrkeys[fileCount]-3)/6 << ")" << endl;
      
      for(int j = 1; j <= (nrkeys[fileCount]-3)/6; j++)
      {
         strOldS[0] = "TreeOldS" + IntToStr(j) + "_mean";
         strNewS[0] = "TreeNewS" + IntToStr(j) + "_mean";
         strOldS[1] = "TreeOldS" + IntToStr(j) + "_neg";
         strNewS[1] = "TreeNewS" + IntToStr(j) + "_neg";
         strOldS[2] = "TreeOldS" + IntToStr(j) + "_pos";
         strNewS[2] = "TreeNewS" + IntToStr(j) + "_pos";

         nextsource->cd(path);

         treeOldS[0] = (TTree*)nextsource->Get(strOldS[0].c_str());
         treeNewS[0] = (TTree*)nextsource->Get(strNewS[0].c_str());
         treeAll[0] = (TTree*)nextsource->Get("TreeA_mean");
         treeOldS[1] = (TTree*)nextsource->Get(strOldS[1].c_str());
         treeNewS[1] = (TTree*)nextsource->Get(strNewS[1].c_str());
         treeAll[1] = (TTree*)nextsource->Get("TreeA_neg");
         treeOldS[2] = (TTree*)nextsource->Get(strOldS[2].c_str());
         treeNewS[2] = (TTree*)nextsource->Get(strNewS[2].c_str());
         treeAll[2] = (TTree*)nextsource->Get("TreeA_pos");

         // Merge the signal trees
         printf("#- %d %d a ---------------------------------------------\n", fileCount, j);

         if(treeOldS[0]->GetEntries() > 0)	// Mean values
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS[0]->GetEntries(), strOldS[0].c_str(), nextsource->GetKey(strOldS[0].c_str())->GetTitle());
            listSig[selectCount[0]]->Add(treeOldS[0]);
            selectCount[0]++;
         }

         if(treeOldS[1]->GetEntries() > 0)	// Mean values + Negative error
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS[1]->GetEntries(), strOldS[1].c_str(), nextsource->GetKey(strOldS[1].c_str())->GetTitle());
            listSigNeg[selectCount[1]]->Add(treeOldS[1]);
            selectCount[1]++;
         }

         if(treeOldS[2]->GetEntries() > 0)	// Mean values + Positive error
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS[2]->GetEntries(), strOldS[2].c_str(), nextsource->GetKey(strOldS[2].c_str())->GetTitle());
            listSigPos[selectCount[2]]->Add(treeOldS[2]);
            selectCount[2]++;
         }

         if(treeNewS[0]->GetEntries() > 0)	// Mean values
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS[0]->GetEntries(), strNewS[0].c_str(), nextsource->GetKey(strNewS[0].c_str())->GetTitle());
            listSig[selectCount[0]]->Add(treeNewS[0]);
            selectCount[0]++;
         }

         if(treeNewS[1]->GetEntries() > 0)	// Mean values + Negative error
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS[1]->GetEntries(), strNewS[1].c_str(), nextsource->GetKey(strNewS[1].c_str())->GetTitle());
            listSigNeg[selectCount[1]]->Add(treeNewS[1]);
            selectCount[1]++;
         }

         if(treeNewS[2]->GetEntries() > 0)	// Mean values + Positive error
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS[2]->GetEntries(), strNewS[2].c_str(), nextsource->GetKey(strNewS[2].c_str())->GetTitle());
            listSigPos[selectCount[2]]->Add(treeNewS[2]);
            selectCount[2]++;
         }
      }

      cout << "selectCount = " << selectCount[0] << ", " << selectCount[1] << ", " << selectCount[2] << endl;

      // Merge all events
      printf("#- %d c ---------------------------------------------\n", fileCount);
      if(treeAll[0]->GetEntries() > 0)	// Mean values
      {
         printf("Found %d entries in tree \"TreeA_mean\" and title \"%s\"\n", (int)treeAll[0]->GetEntries(), nextsource->GetKey("TreeA_mean")->GetTitle());
         listAll[0]->Add(treeAll[0]);
      }

      if(treeAll[1]->GetEntries() > 0)	// Mean values + Negative error
      {
         printf("Found %d entries in tree \"TreeA_neg\" and title \"%s\"\n", (int)treeAll[1]->GetEntries(), nextsource->GetKey("TreeA_neg")->GetTitle());
         listAll[1]->Add(treeAll[1]);
      }

      if(treeAll[2]->GetEntries() > 0)	// Mean values + Positive error
      {
         printf("Found %d entries in tree \"TreeA_pos\" and title \"%s\"\n", (int)treeAll[2]->GetEntries(), nextsource->GetKey("TreeA_pos")->GetTitle());
         listAll[2]->Add(treeAll[2]);
      }
*/
      // Move to the next file
      printf("#- %d d -----------------------------------------------\n", fileCount);
      fileCount++;
      nextsource = (TFile*)sourcelist->After(nextsource);
   }

   printf("#- e saving the file --------------------------------------\n");

   string treeTitle[sigKeys];

   target->cd();
   printf("Saving signal trees\n");
   for(int i = 0; i < sigKeys; i++)
   {
      outSig[i] = TTree::MergeTrees(listSig[i]);
      cout << "Events in output signal tree = " << outSig[i]->GetEntries() << endl;
      strOldS = "TreeS" + IntToStr(i+1);
      outSig[i]->SetName(strOldS.c_str());
      treeTitle[i] = outSig[i]->GetTitle();
      outSig[i]->Write();
   }
   printf("Saving all events tree\n");
   outAll = TTree::MergeTrees(listAll);
   cout << "Events in output total tree = " << outAll->GetEntries() << endl;
   outAll->Write();

   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);
}

/*void RewriteRootfile( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts, vector<string> &filenames )
{
   cout << "Rewriting root files." << endl;
   for(int i = 0; i < filenames.size(); i++) cout << "  " << filenames[i] << endl;

   int sigKeys = 0;
   for(int i = 0; i < nrkeys.size(); i++)
      sigKeys += (nrkeys[i]-3)/6;
   printf("Number of signal keys in all files = %d\n", sigKeys);

   if(sigKeys == 1)
   {
      printf("Only one key found in root file. Aborting.\n");
      return;
   }

   int nrfiles = sourcelist->GetSize();
   string strOldS, strNewS, strOldB, strNewB;
   TTree *treeOldS, *treeNewS, *treeOldB, *treeNewB, *treeAll;
   TTree *outSig[sigKeys], *outBack[sigKeys], *outAll;

   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove(0,2);

   TFile *nextsource = (TFile*)sourcelist->First();
   nextsource->cd(path);
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);

   printf("Get number of files: %d\n", nrfiles);

   TList *listAll, *listSig[sigKeys], *listBack[sigKeys];
   target->cd();
   listAll = new TList;
   for(int j = 0; j < sigKeys; j++)
   {
      listSig[j] = new TList;
      listBack[j] = new TList;
   }

   printf("Number of keys in file %s: %d (%d)\n", nextsource->GetName(), nrkeys[0], (nrkeys[0]-1)/4);

   for(int j = 1; j <= (nrkeys[0]-1)/4; j++)
   {
      strOldS = "TreeOldS" + IntToStr(j);
      strNewS = "TreeNewS" + IntToStr(j);
      strOldB = "TreeOldB" + IntToStr(j);
      strNewB = "TreeNewB" + IntToStr(j);

      nextsource->cd(path);

      treeOldS = (TTree*)nextsource->Get(strOldS.c_str());
      treeNewS = (TTree*)nextsource->Get(strNewS.c_str());
      treeAll = (TTree*)nextsource->Get("TreeA");
      treeOldB = (TTree*)nextsource->Get(strOldB.c_str());
      treeNewB = (TTree*)nextsource->Get(strNewB.c_str());

      // Rewrite the signal trees
      printf("#- %d a ---------------------------------------------\n", j);

      if(treeOldS->GetEntries() > 0)
      {
         printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS->GetEntries(), strOldS.c_str(), nextsource->GetKey(strOldS.c_str())->GetTitle());
         listSig[j-1]->Add(treeOldS);
      }

      if(treeNewS->GetEntries() > 0)
      {
         printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS->GetEntries(), strNewS.c_str(), nextsource->GetKey(strNewS.c_str())->GetTitle());
         listSig[j-1]->Add(treeNewS);
      }

      // Rewrite the background trees
      printf("#- %d b ---------------------------------------------\n", j);

      if(treeOldB->GetEntries() > 0)
      {
         printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldB->GetEntries(), strOldB.c_str(), nextsource->GetKey(strOldB.c_str())->GetTitle());
         listBack[j-1]->Add(treeOldB);
      }

      if(treeNewB->GetEntries() > 0)
      {
         printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewB->GetEntries(), strNewB.c_str(), nextsource->GetKey(strNewB.c_str())->GetTitle());
         listBack[j-1]->Add(treeNewB);
      }

   }

   // Merge all events
   printf("#- c ---------------------------------------------\n");
   if(treeAll->GetEntries() > 0)
   {
      printf("Found %d entries in tree \"TreeA\" and title \"%s\"\n", (int)treeAll->GetEntries(), nextsource->GetKey("TreeA")->GetTitle());
      listAll->Add(treeAll);
   }

   printf("#- e saving the file --------------------------------------\n");

//   string treeTitle[sigKeys];

   target->cd();
   printf("Saving signal trees\n");
   for(int i = 0; i < sigKeys; i++)
   {
      outSig[i] = TTree::MergeTrees(listSig[i]);
      strOldS = "TreeS" + IntToStr(i+1);
      outSig[i]->SetName(strOldS.c_str());
//      treeTitle[i] = outSig[i]->GetTitle();
      outSig[i]->Write();
   }
   printf("Saving all events tree\n");
   outAll = TTree::MergeTrees(listAll);
   outAll->Write();
   printf("Saving background trees\n");
   for(int i = 0; i < sigKeys; i++)
   {
      outBack[i] = TTree::MergeTrees(listBack[i]);
      strOldS = "TreeB" + IntToStr(i+1);
      outBack[i]->SetName(strOldS.c_str());
//      outBack[i]->SetTitle((RemoveFromFirst(treeTitle[i], "Signal tree from", "Background tree without events from")).c_str());
      outBack[i]->Write();
   }

   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);
}*/

void MergeRootfile( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts, vector<string> &filenames, int opt )
{
   cout << "Merging root files." << endl;
   for(int i = 0; i < filenames.size(); i++) cout << "  " << filenames[i] << endl;

   int sigKeys = 0;
// NEW
   for(int i = 0; i < nrkeys.size(); i++)
      sigKeys += (nrkeys[i]-1)/2;
/*    for(int i = 0; i < nrkeys.size(); i++)
      sigKeys += (nrkeys[i]-3)/6;*/
   cout << "Number of signal keys in all files = " << sigKeys << endl;

   int nrfiles = sourcelist->GetSize();
// NEW
   string strOldS, strNewS;
   TTree *treeOldS, *treeNewS, *treeAll;
   TTree *outOld, *outNew, *outAll;
   int selectCount[2];
/*   string strOldS[3], strNewS[3];
   TTree *treeOldS[3], *treeNewS[3], *treeAll[3];
   TTree *outOld[3], *outNew[3], *outAll[3];
   int selectCount[6];*/
   int fileCount = 0;

   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove(0,2);

   TFile *nextsource = (TFile*)sourcelist->First();
   nextsource->cd(path);
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);

   cout << "Get number of files = " << nrfiles << endl;

   fileCount = 0;
   selectCount[0] = 0;
   selectCount[1] = 0;
/*   selectCount[2] = 0;
   selectCount[3] = 0;
   selectCount[4] = 0;
   selectCount[5] = 0;*/

// NEW
   TList *listAll, *listOld, *listNew;
   target->cd();
   listAll = new TList;
   listOld = new TList;
   listNew = new TList;
   
/*   TList *listAll[3], *listOld[3], *listNew[3];//TODO
   target->cd();
   for(int j = 0; j < 3; j++)
   {
      listAll[j] = new TList;
      listOld[j] = new TList;
      listNew[j] = new TList;
   }*/

   while(nextsource)
   {
// NEW
      for(int j = 1; j <= (nrkeys[fileCount]-1)/2; j++)
      {
         strOldS = "TreeOldS" + IntToStr(j);
         strNewS = "TreeNewS" + IntToStr(j);

         nextsource->cd(path);

         treeOldS = (TTree*)nextsource->Get(strOldS.c_str());
         treeNewS = (TTree*)nextsource->Get(strNewS.c_str());
         treeAll = (TTree*)nextsource->Get("TreeA");

         // Merge the signal trees
         printf("#- %d %d a ---------------------------------------------\n", fileCount, j);

         if(treeOldS->GetEntries() > 0)	// All Old trees
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS->GetEntries(), strOldS.c_str(), nextsource->GetKey(strOldS.c_str())->GetTitle());
            listOld->Add(treeOldS);
            selectCount[0]++;
         }

         if(treeNewS->GetEntries() > 0)	// All New trees
         {
            printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS->GetEntries(), strNewS.c_str(), nextsource->GetKey(strNewS.c_str())->GetTitle());
            listNew->Add(treeNewS);
            selectCount[1]++;
         }
      }

      cout << "selectCount = " << selectCount[0] << ", " << selectCount[1] << ", " << selectCount[2] << endl;

      // Merge all events
      printf("#- %d c ---------------------------------------------\n", fileCount);
      if(treeAll->GetEntries() > 0)	// Mean values
      {
         printf("Found %d entries in tree \"TreeA\" and title \"%s\"\n", (int)treeAll->GetEntries(), nextsource->GetKey("TreeA")->GetTitle());
         listAll->Add(treeAll);
      }

/*      cout << "Number of keys in file " << nextsource->GetName() << ": " << nrkeys[fileCount] << "(" << (nrkeys[fileCount]-3)/6 << ")" << endl;
      
      for(int j = 1; j <= (nrkeys[fileCount]-3)/6; j++)
      {
         strOldS[0] = "TreeOldS" + IntToStr(j) + "_mean";
         strNewS[0] = "TreeNewS" + IntToStr(j) + "_mean";
         strOldS[1] = "TreeOldS" + IntToStr(j) + "_neg";
         strNewS[1] = "TreeNewS" + IntToStr(j) + "_neg";
         strOldS[2] = "TreeOldS" + IntToStr(j) + "_pos";
         strNewS[2] = "TreeNewS" + IntToStr(j) + "_pos";

         nextsource->cd(path);

         treeOldS[0] = (TTree*)nextsource->Get(strOldS[0].c_str());
         treeNewS[0] = (TTree*)nextsource->Get(strNewS[0].c_str());
         treeAll[0] = (TTree*)nextsource->Get("TreeA_mean");
         treeOldS[1] = (TTree*)nextsource->Get(strOldS[1].c_str());
         treeNewS[1] = (TTree*)nextsource->Get(strNewS[1].c_str());
         treeAll[1] = (TTree*)nextsource->Get("TreeA_neg");
         treeOldS[2] = (TTree*)nextsource->Get(strOldS[2].c_str());
         treeNewS[2] = (TTree*)nextsource->Get(strNewS[2].c_str());
         treeAll[2] = (TTree*)nextsource->Get("TreeA_pos");

         // Merge the signal trees
         printf("#- %d %d a ---------------------------------------------\n", fileCount, j);

         for(int i = 0; i < 3; i++)
	 {
            if(treeOldS[i]->GetEntries() > 0)	// All three Old trees
            {
               printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeOldS[i]->GetEntries(), strOldS[i].c_str(), nextsource->GetKey(strOldS[i].c_str())->GetTitle());
               listOld[i]->Add(treeOldS[i]);
               selectCount[i]++;
            }

            if(treeNewS[i]->GetEntries() > 0)	// All three New trees
            {
               printf("Found %d entries in tree \"%s\" and title \"%s\"\n", (int)treeNewS[i]->GetEntries(), strNewS[i].c_str(), nextsource->GetKey(strNewS[i].c_str())->GetTitle());
               listNew[i]->Add(treeNewS[i]);
               selectCount[i]++;
            }
	 }
      }

      cout << "selectCount = " << selectCount[0] << ", " << selectCount[1] << ", " << selectCount[2] << endl;

      // Merge all events
      printf("#- %d c ---------------------------------------------\n", fileCount);
      if(treeAll[0]->GetEntries() > 0)	// Mean values
      {
         printf("Found %d entries in tree \"TreeA_mean\" and title \"%s\"\n", (int)treeAll[0]->GetEntries(), nextsource->GetKey("TreeA_mean")->GetTitle());
         listAll[0]->Add(treeAll[0]);
      }

      if(treeAll[1]->GetEntries() > 0)	// Mean values + Negative error
      {
         printf("Found %d entries in tree \"TreeA_neg\" and title \"%s\"\n", (int)treeAll[1]->GetEntries(), nextsource->GetKey("TreeA_neg")->GetTitle());
         listAll[1]->Add(treeAll[1]);
      }

      if(treeAll[2]->GetEntries() > 0)	// Mean values + Positive error
      {
         printf("Found %d entries in tree \"TreeA_pos\" and title \"%s\"\n", (int)treeAll[2]->GetEntries(), nextsource->GetKey("TreeA_pos")->GetTitle());
         listAll[2]->Add(treeAll[2]);
      }
*/
      // Move to the next file
      printf("#- %d d -----------------------------------------------\n", fileCount);
      fileCount++;
      nextsource = (TFile*)sourcelist->After(nextsource);
   }

// NEW
      cout << "Entries in old list: " << listOld->GetEntries() << endl;
      cout << "Entries in new list: " << listNew->GetEntries() << endl;
      cout << "Entries in all list: " << listAll->GetEntries() << endl;
/*   for(int i = 0; i < 3; i++)
   {
      cout << "New type:" << endl;
      cout << "Entries in old list: " << listOld[i]->GetEntries() << endl;
      cout << "Entries in new list: " << listNew[i]->GetEntries() << endl;
      cout << "Entries in all list: " << listAll[i]->GetEntries() << endl;
   }*/

   printf("#- e saving the file --------------------------------------\n");

// NEW
   string treeTitleOld, treeTitleNew;
//   string treeTitleOld[3], treeTitleNew[3];

   target->cd();
   printf("Saving old signal tree\n");
//   for(int i = 0; i < sigKeys; i++)
//   {
      if(listOld->GetEntries() > 0)
      {
         outOld = TTree::MergeTrees(listOld);
//         strOldS = "TreeOldS" + IntToStr(i+1);
         strOldS = "TreeOldS1";
         outOld->SetName(strOldS.c_str());
      }
      else
      {
/*         strOldS = "TreeOldS" + IntToStr(i+1);
         outOld = (TTree*)((TFile*)sourcelist->First())->Get(strOldS.c_str()); // TODO is this correct?*/
         outOld = (TTree*)((TFile*)sourcelist->First())->Get("TreeOldS1"); // TODO is this correct?
      }

      treeTitleOld = outOld->GetTitle();
      outOld->Write();
      
      printf("Saving new signal tree\n");
      if(listNew->GetEntries() > 0)
      {
         outNew = TTree::MergeTrees(listNew);
//         strOldS = "TreeNewS" + IntToStr(i+1);
         strOldS = "TreeNewS1";
         outNew->SetName(strOldS.c_str());
      }
      else
      {
/*         strOldS = "TreeNewS" + IntToStr(i+1);
         outNew = (TTree*)((TFile*)sourcelist->First())->Get(strOldS.c_str()); // TODO is this correct?*/
         outNew = (TTree*)((TFile*)sourcelist->First())->Get("TreeNewS1"); // TODO is this correct?
      }
      treeTitleNew = outNew->GetTitle();
      outNew->Write();
//   }
   printf("Saving all events tree\n");
   outAll = TTree::MergeTrees(listAll);
   outAll->Write();

/*   for(int i = 0; i < 3; i++)
   {
      if(listOld[i]->GetEntries() == 0)
         delete outOld[i];
      if(listNew[i]->GetEntries() == 0)
         delete outNew[i];
   }*/

   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);
}

void CheckKeys( TDirectory *target, TList *sourcelist, vector<int> &nrkeys, vector<int> &nrevts )
{
   int nrfiles = sourcelist->GetSize();
   int fileCount = 0;
   string strOldS, strNewS;
// NEW   
   TTree *treeOldS, *treeNewS, *treeAll;
// TTree *treeOldS[3], *treeNewS[3], *treeAll[3];

   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove(0,2);

   TFile *nextsource = (TFile*)sourcelist->First();
   nextsource->cd(path);
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);

   while(nextsource)
   {
      cout << fileCount << ": Number of keys = " << nextsource->GetNkeys() << endl;
      nrkeys.push_back(nextsource->GetNkeys());

// NEW
      treeAll = (TTree*)nextsource->Get("TreeA");
cout << "Number of events in tree TreeA = " << treeAll->GetEntries() << endl;
      nrevts.push_back(treeAll->GetEntries());

      for(int j = 1; j <= (nrkeys[fileCount]-1)/2; j++)
      {
         strOldS = "TreeOldS" + IntToStr(j);
         strNewS = "TreeNewS" + IntToStr(j);

         treeOldS = (TTree*)nextsource->Get(strOldS.c_str());
         treeNewS = (TTree*)nextsource->Get(strNewS.c_str());

cout << "Number of events in tree " << strOldS << " = " << treeOldS->GetEntries() << endl;
cout << "Number of events in tree " << strNewS << " = " << treeNewS->GetEntries() << endl;
         nrevts.push_back(treeOldS->GetEntries());
         nrevts.push_back(treeNewS->GetEntries());
      }
/*      
      treeAll[0] = (TTree*)nextsource->Get("TreeA_mean");
      treeAll[1] = (TTree*)nextsource->Get("TreeA_neg");
      treeAll[2] = (TTree*)nextsource->Get("TreeA_pos");
      nrevts.push_back(treeAll[0]->GetEntries());
      nrevts.push_back(treeAll[1]->GetEntries());
      nrevts.push_back(treeAll[2]->GetEntries());

      for(int j = 1; j <= (nrkeys[fileCount]-3)/6; j++)
      {
         strOldS = "TreeOldS" + IntToStr(j) + "_mean";
         strNewS = "TreeNewS" + IntToStr(j) + "_mean";

         treeOldS[0] = (TTree*)nextsource->Get(strOldS.c_str());
         treeNewS[0] = (TTree*)nextsource->Get(strNewS.c_str());

         nrevts.push_back(treeOldS[0]->GetEntries());
         nrevts.push_back(treeNewS[0]->GetEntries());

         strOldS = "TreeOldS" + IntToStr(j) + "_neg";
         strNewS = "TreeNewS" + IntToStr(j) + "_neg";

         treeOldS[1] = (TTree*)nextsource->Get(strOldS.c_str());
         treeNewS[1] = (TTree*)nextsource->Get(strNewS.c_str());

         nrevts.push_back(treeOldS[1]->GetEntries());
         nrevts.push_back(treeNewS[1]->GetEntries());

         strOldS = "TreeOldS" + IntToStr(j) + "_pos";
         strNewS = "TreeNewS" + IntToStr(j) + "_pos";

         treeOldS[2] = (TTree*)nextsource->Get(strOldS.c_str());
         treeNewS[2] = (TTree*)nextsource->Get(strNewS.c_str());

         nrevts.push_back(treeOldS[2]->GetEntries());
         nrevts.push_back(treeNewS[2]->GetEntries());
      }*/

      nextsource = (TFile*)sourcelist->After(nextsource);
      fileCount++;
   }
}

void hmerge(int nrfiles, TList *files, char *outname)
{
   cout << "Number of files: " << nrfiles << endl;
   cout << "Saving into file: " << outname << endl;

   TList *FileList;
   TFile *Target;

   char ctemp[1024];
   vector<int> nrkeys;
   vector<int> nrevts;
   vector<string> filenames;

   // Prepare the files to be merged
   sprintf(ctemp, "%s/dbg/hsimple1.root", rootdir);
   if(gSystem->AccessPathName(ctemp))
   {
     for(int i = 0; i < nrfiles; i++)
     {
       sprintf(ctemp, "%s/dbg/hsimple%d.root", rootdir, i+1);
       gSystem->CopyFile(files->At(i)->GetTitle(), ctemp);
       filenames.push_back(files->At(i)->GetTitle());
     }
   }
   else
     return;

   Target = TFile::Open(outname, "RECREATE");

   FileList = new TList();
   for(int i = 0; i < nrfiles; i++)
   {
     sprintf(ctemp, "%s/dbg/hsimple%d.root", rootdir, i+1);
     FileList->Add(TFile::Open(ctemp));   
   }

   CheckKeys(Target, FileList, nrkeys, nrevts);
   for(int i = 0; i < nrkeys.size(); i++)
      printf("Keys in file %d = %d\n", i, nrkeys[i]);
   for(int i = 0; i < nrevts.size(); i++)
      printf("Events in key %d = %d\n", i, nrevts[i]);

   if(nrfiles > 1)
      MergeRootfile(Target, FileList, nrkeys, nrevts, filenames, 0);
   else
      MergeRootfile(Target, FileList, nrkeys, nrevts, filenames, 1);

   Target->Close();

   for(int i = 0; i < nrfiles; i++)
   {
     sprintf(ctemp, "rm -v %s/dbg/hsimple%d.root", rootdir, i+1);
     system(ctemp);
   }
}

void hadd(int nrfiles, TList *files, char *outname)
{
   cout << "Number of files: " << nrfiles << endl;
   cout << "Saving into file: " << outname << endl;

   TList *FileList;
   TFile *Target;

   char ctemp[1024];
   vector<int> nrkeys;
   vector<int> nrevts;
   vector<string> filenames;

   // Prepare the files to be merged
   sprintf(ctemp, "%s/dbg/hsimple1.root", rootdir);
   if(gSystem->AccessPathName(ctemp))
   {
     for(int i = 0; i < nrfiles; i++)
     {
       sprintf(ctemp, "%s/dbg/hsimple%d.root", rootdir, i+1);
       gSystem->CopyFile(files->At(i)->GetTitle(), ctemp);
       filenames.push_back(files->At(i)->GetTitle());
     }
   }

   Target = TFile::Open(outname, "RECREATE");

   FileList = new TList();
   for(int i = 0; i < nrfiles; i++)
   {
     sprintf(ctemp, "%s/dbg/hsimple%d.root", rootdir, i+1);
     FileList->Add(TFile::Open(ctemp));   
   }

   CheckKeys(Target, FileList, nrkeys, nrevts);
   for(int i = 0; i < nrkeys.size(); i++)
      printf("Keys in file %d = %d\n", i, nrkeys[i]);
   for(int i = 0; i < nrevts.size(); i++)
      printf("Events in key %d = %d\n", i, nrevts[i]);

   if(nrfiles > 1)
      CombineRootfile(Target, FileList, nrkeys, nrevts, filenames);
/*   else
      RewriteRootfile(Target, FileList, nrkeys, nrevts, filenames);*/

   Target->Close();

   for(int i = 0; i < nrfiles; i++)
   {
     sprintf(ctemp, "rm -v %s/dbg/hsimple%d.root", rootdir, i+1);
     system(ctemp);
   }
}
