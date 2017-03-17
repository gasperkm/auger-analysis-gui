#include "RecEvent.h"
#include "RecEventFile.h"
#include "DetectorGeometry.h"
#include "FileInfo.h"

#include "Analysis.h"

#include <vector>
#include <string>
#include <iostream>
#include <TTree.h>

using namespace std;


int GetOptions(int argc, char** argv);

void Usage(const char* const exe);


enum EAdstMode {
  eFull,
  eMini,
  eMicro
};


string gAnalysisConfig = "analysis.config";
string gOutputFile = "selected.root";
bool gNminusOne = true;
EAdstMode gAdstMode = eFull;
unsigned int gMinNFDEyes = 1;


int
main(int argc, char **argv)
{
  TTree::SetMaxTreeSize(200000000000LL);

  const int nOptions = GetOptions(argc, argv);
  if (nOptions < 0 || argc - nOptions < 1) {
    Usage(argv[0]);
    return 1;
  }

  RecEvent* theRecEvent = new RecEvent();
  DetectorGeometry* theGeometry = new DetectorGeometry();

  const vector<string> dataFileNames(argv + nOptions, argv + argc);
  RecEventFile dataFile(dataFileNames);
  RecEventFile* selectedFile = new RecEventFile(gOutputFile.c_str(), RecEventFile::eWrite);

  switch (gAdstMode) {
  case eFull:
    break;
  case eMini:
    dataFile.SetMiniADST();
    selectedFile->SetMiniADST();
    break;
  case eMicro:
    dataFile.SetMicroADST();
    selectedFile->SetMicroADST();
    break;
  }

  dataFile.SetBuffers(&theRecEvent);
  dataFile.ReadDetectorGeometry(*theGeometry);
  selectedFile->SetBuffers(&theRecEvent);

  Analysis analysis(&theGeometry, &theRecEvent, gAnalysisConfig, gNminusOne);

  const int nEvents = dataFile.GetNEvents();
  const int fivePercent = int(0.05*nEvents);

  while (dataFile.ReadNextEvent() == RecEventFile::eSuccess) {

    const int currentEventNumber = dataFile.GetCurrentEvent();
    if (fivePercent && currentEventNumber && !(currentEventNumber % fivePercent))
      cout << dataFile.GetCurrentEventString() << endl;

    if (analysis.HasSDSelection() && !analysis.IsGoodSD())
      continue;

    if (analysis.HasSimSelection() && !analysis.IsGoodSim())
      continue;

    if (analysis.HasFDSelection()) {
      for (RecEvent::EyeIterator iEye = theRecEvent->EyesBegin();
           iEye != theRecEvent->EyesEnd(); )
        if (!analysis.IsGoodEye(iEye->GetEyeId()))
          theRecEvent->GetFDEvents().erase(iEye);
        else
          ++iEye;
      if (theRecEvent->GetFDEvents().size() < gMinNFDEyes)
        continue;
    }

    selectedFile->WriteEvent();

  }

  cout << "\n"
          " total events   : " << dataFile.GetNEvents() << "\n"
          " selected events: " << selectedFile->GetNEvents() << endl;

  analysis.PrintCutStatistics(false);

  FileInfo theInfo;
  dataFile.ReadFileInfo(theInfo);
  selectedFile->WriteDetectorGeometry(*theGeometry);
  selectedFile->WriteFileInfo(theInfo);
  selectedFile->cd();
  if (gNminusOne)
    analysis.WriteNMinusOne();
  analysis.WriteCutStatistics();
  selectedFile->Close();

  return 0;
}


void
Usage(const char* const exe)
{
  cout << " usage: " << exe << " [<options>] <fileNames>\n"
          "\n"
          " Options: -c: config file for event selection  (default: " << gAnalysisConfig << ")\n"
          "          -o: name of the output file          (default: " << gOutputFile << ")\n"
          "          -n: turn on/off n-minus-one plots    (default: " << (gNminusOne ? "on" : "off") << ")\n"
          "          -m: mode (full/mini/micro)           (default: full)\n"
          "          -e: minimum number of eyes for event (default: 1)\n"
       << endl;
}


/*========================================================

   reads command line options

==========================================================*/

int
GetOptions(int argc, char** argv)
{
  int c;
  while ((c = getopt(argc, argv, "c:o:n:m:e:h")) != -1) {
    switch (c) {
    case 'c':
      gAnalysisConfig = string(optarg);
      cout << " use cut-config: " << gAnalysisConfig << endl;
      break;
    case 'o':
      gOutputFile = string(optarg);
      cout << " use output file: " << gOutputFile << endl;
      break;
    case 'm': {
      const string mode = optarg;
      if (mode == "full") {
        gAdstMode = eFull;
        cout << " ADST-mode: full " << endl;
      } else if (mode == "mini") {
        gAdstMode = eMini;
        cout << " ADST-mode: mini " << endl;
      } else if (mode == "micro") {
        gAdstMode = eMicro;
        cout << " ADST-mode: micro " << endl;
      } else {
        return -2;
      }
      break;
    }
    case 'n': {
      const string nMinusOne = optarg;
      if (nMinusOne == "off") {
        gNminusOne = false;
      } else if (nMinusOne == "on") {
        gNminusOne = true;
      } else {
        cerr << " Wrong usage of option \'n\', please specify \"on\" or \"off\"."
             << endl;
        return -2;
      }
      cout << (gNminusOne ?
               " create and write N-1 histograms" :
               " do not create and write N-1 histograms")
           << endl;
      break;
    }
    case 'e': {
      const string arg = optarg;
      char * pEnd;
      gMinNFDEyes = strtol(&arg[0],&pEnd,10);
      cout << " minimum FD eyes for each event: " << gMinNFDEyes;
      if (!gMinNFDEyes>0) {
	gMinNFDEyes = 1;
	cout << " (wrong input: default setted)";
      }
      cout << endl;
      break;
    }
    case 'h':
      return -2;
    default:
      return -2;
    }
  }
  cout << endl;
  return optind;
}
