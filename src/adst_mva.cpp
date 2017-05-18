#include "../include/adst_mva.h"

#include <algorithm>
#include <random>
#include <chrono>

Observables::Observables()
{
   // TODO: Add more observables - for galactic maps, add also galactic coordinates of direction
   xmax = -1;
   x0 = -1;
   lambda = -1;
   shfoot = -1;
   sdenergy = -1;
   fdenergy = -1;
   nrmu = -1;
   shwsize = -1;
   ldfbeta = -1;
   curvature = -1;
   risetime = -1;
   risetimerecalc = -1;
   aop = -1;	// TODO: Calculate AOP on your own
   zenithSD = -1;
   azimuthSD = -1;
   zenithFD = -1;
   azimuthFD = -1;
}

Observables::~Observables()
{
}

// Analysis tools class constructor and destructor ------------------------------------------------------
//    Define the TTree names, set up and delete the canvas
AdstMva::AdstMva()
{
/*   gSystem->Load("libRecEventKG.so");
   gSystem->Load("libAnalysisKG.so");*/

/*   c1 = new TCanvas("c1AA","c1AA",1200,800);
   c1->SetGrid();
*/
   fRecEvent = new RecEvent();
   fDetGeo = new DetectorGeometry();
   genshw = new GenShower();
   sdrecshw = new SdRecShower();

   nrtanks = 0;
   nreyes = 0;
   longesttrack = 0;

   shfootlimit = 0.1;

   goodrec = true;

   // Weighting function for determining the error bars (Risetime1000LL.xml)
   fRTWeights = new TFormula("RiseTimeWeights", "(80.0+(5.071e-7+6.48e-4*y-3.051e-4*y*y)*x*x)/z-16.46*y+36.16");
   limitTankDistance[0] = 0.;
   limitTankDistance[1] = 1800.;
   minSignal = 5.0;
   includeSaturated = false;
   minPoints = 3;
   evalDistance = 1000.;

/*   genshw = new GenShower();
   sdrecshw = new SdRecShower();
#ifdef OFFLINENEW
   unishw = new UnivRecShower();
#endif

   outname = "analysis_out/tmva_output.root";

   shfootlimit = 0.1;

   goodrec = true;
   graphical = false;*/

/*   xmaxlimit = 0;
   slantset = -1;
   shfootlimit = 0;
   distlimit = 0;
   pointcnt = 0;
   btemp = SUBPLOT;
   readevent = 0;

   for(int i = 0; i < ALLEYES; i++)
      eyevalid[i] = 0;*/
}

AdstMva::~AdstMva()
{
   delete fRTWeights;
   delete fDetGeo;
   delete fRecEvent;
   delete genshw;
   delete sdrecshw;
//   delete c1;
}
// ------------------------------------------------------------------------------------------------------

// Return an array of active tanks
void AdstMva::GetActiveTanks()
{
   cout << "# Entering function AdstMva::GetActiveTanks()..." << endl;

   SDEvent &sdevt = fRecEvent->GetSDEvent();
   nrtanks = sdevt.GetNumberOfCandidates();

   if(acttanks.size() != 0)
      acttanks.erase(acttanks.begin(), acttanks.end());
   acttanks = sdevt.GetStationVector();

   if(nrtanks == 0)
      cout << "    No reconstructed SD tanks." << endl;
   else
   {
      cout << "Number of active tanks: " << nrtanks << endl;
      for(int i = 0; i < acttanks.size(); i++)
      {
         cout << i << " ID = " << acttanks[i].GetId() << endl;
         if(acttanks[i].GetId() >= 90000)
	 {
	    acttanks.erase(acttanks.begin()+i);
            i--;
	 }
      }
   }
}

// Return an array of active eyes
void AdstMva::GetActiveEyes()
{
   cout << "# Entering function AdstMva::GetActiveEyes()..." << endl;
   double tempmax;
   int iMax;

   nreyes = fRecEvent->GetNEyes();

   if( acteyes.size() != 0 )
      acteyes.erase(acteyes.begin(),acteyes.end());
   acteyes = fRecEvent->GetFDEvents();

   for(int i = 0; i < acteyes.size(); i++)
   {
      cout << i << " ID = " << acteyes[i].GetEyeId() << ", Energy = " << (acteyes[i].GetFdRecShower()).GetEnergy() << ", Track = " << (acteyes[i].GetFdRecShower()).GetTrackLength() << endl;
//      acteyes.push_back(fdevt[i].GetFdRecShower());
//cout << " Eye " << fdevt[i].GetEyeId() << ": Energy = " << acteyes[i].GetEnergy() << " eV, Track = " << acteyes[i].GetTrackLength() << " g/cm^2" << endl;

      if( ((acteyes[i].GetFdRecShower()).GetEnergy() == 0) || ((acteyes[i].GetFdRecShower()).GetTrackLength() == 0) )
      {
	 acteyes.erase(acteyes.begin()+i);
	 i--;
      }
   }

   nreyes = acteyes.size();
   if(nreyes == 0)
      cout << "    No reconstructed FD eyes." << endl;
   else
   {
      cout << "    " << nreyes << " reconstructed FD eyes." << endl;
      for(int i = 0; i < nreyes; i++)
      {
         if(i == 0)
            tempmax = (acteyes[i].GetFdRecShower()).GetTrackLength();
         else
         {
            if((acteyes[i].GetFdRecShower()).GetTrackLength() > tempmax)
               iMax = acteyes[i].GetEyeId();
            tempmax = TMath::Max(tempmax, (acteyes[i].GetFdRecShower()).GetTrackLength());
	 }
      }
   }

   cout << "Maximum track length is = " << tempmax << " g/cm2 (from eye " << iMax << ")" << endl;
   longesttrack = iMax;
}

// Plotting functions of LDF, VEM and longitudinal profile
void AdstMva::PlotLDF(TCanvas *c1)
{
   cout << "# Entering function AdstMva::PlotLDF()..." << endl;

   int *pointcnt;
   pointcnt = new int;
   double *x, *xerr, *y, *yerr;
   x = new double;
   xerr = new double;
   y = new double;
   yerr = new double;

   SdRecShower &sdrecshw = fRecEvent->GetSDEvent().GetSdRecShower();
   LDF shwldf = sdrecshw.GetLDF();

   c1->cd();
   c1->SetGrid();
   c1->SetRightMargin(0.05);
   c1->SetTopMargin(0.05);

   grErr = new TGraphErrors(nrtanks);

   *pointcnt = 0;

   for(int i = 0; i < nrtanks; i++)
   {
      *x = acttanks[i].GetSPDistance();
      *xerr = acttanks[i].GetSPDistanceError();
      *y = acttanks[i].GetTotalSignal();
      *yerr = acttanks[i].GetTotalSignalError();

      grErr->SetPoint(*pointcnt,*x,*y);
      grErr->SetPointError(*pointcnt,*xerr,*yerr);

      (*pointcnt)++;
   }

   TF1 ldffunc = (TF1)shwldf.GetFunction(eNKG);
   ldffunc.Draw();

   c1->SetLogy(1);
   grErr->SetMarkerStyle(20);
   grErr->SetMarkerSize(0.7);
   grErr->SetTitle(";Distance from axis (m);Signal (VEM)");
   grErr->Draw("P;SAME");

   c1->Modified();
   c1->Update();

   delete x;
   delete xerr;
   delete y;
   delete yerr;
   delete pointcnt;
}

void AdstMva::PlotVEM(TCanvas *c1, string sdid)
{
   cout << "# Entering function AdstAnalyseTool::PlotVEM()..." << endl;

   double *range;
   range = new double[3];
   string stemp;
   float *ftemp;
   ftemp = new float;
   int *itemp;
   itemp = new int;

   c1->cd();
   c1->SetGrid();
   c1->SetRightMargin(0.05);
   c1->SetTopMargin(0.05);

   vector<Traces> vemtraces;
   vector<float> trace1;
   vector<float> trace2;
   vector<float> trace3;

   for(int i = 0; i < nrtanks; i++)
   {
      if( acttanks[i].GetId() == atoi(sdid.c_str()) )
      {
         vemtraces = acttanks[i].GetPMTTraces();
         trace1 = vemtraces[0].GetVEMComponent();
         trace2 = vemtraces[1].GetVEMComponent();
         trace3 = vemtraces[2].GetVEMComponent();

         range[0] = acttanks[i].GetSignalStartSlot();
         range[1] = acttanks[i].GetSignalEndSlot();
      }

      printf("\nSecond = %.7le\n", acttanks[i].GetTimeSecond());
      printf("Nanosecond = %.7le\n", acttanks[i].GetTimeNSecond());
      cout << "Signal start slot = " << acttanks[i].GetSignalStartSlot() << endl;
      cout << "Signal end slot = " << acttanks[i].GetSignalEndSlot() << endl;
   }

   if(trace1.size() > 0)
      *itemp = trace1.size();
   if(trace2.size() > 0)
      *itemp = trace2.size();
   if(trace3.size() > 0)
      *itemp = trace3.size();

   cout << "Number of points: " << *itemp << endl;

   gr = new TGraph( *itemp );

   for(int i = 0; i < *itemp; i++)
   {
      *ftemp = 0.;
      if(trace1.size() > 0)
         *ftemp += trace1[i];
      if(trace2.size() > 0)
         *ftemp += trace2[i];
      if(trace3.size() > 0)
         *ftemp += trace3[i];

      gr->SetPoint(i,i,*ftemp);
   }

   gr->SetMarkerStyle(20);
   gr->SetMarkerSize(0.7);
   range[2] = (range[1] - range[0])*0.2;
   stemp = "VEM trace (ID = " + sdid + ");SD tank time (25 ns);Signal (VEM)";
   gr->SetTitle(stemp.c_str());
   gr->Draw("APL");

   cout << "Min range = " << range[0] << ", Max range = " << range[1] << ", Range extension = " << range[2] << ", Final min = " << range[0]-range[2] << ", Final max = " << range[1]+range[2] << endl;
   gr->GetXaxis()->SetRange( range[0]-range[2], range[1]+range[2] );
   gr->GetXaxis()->SetRangeUser( range[0]-range[2], range[1]+range[2] );
   gr->GetXaxis()->SetLimits( range[0]-range[2], range[1]+range[2] );

   c1->Modified();
   c1->Update();

   delete[] range;
   delete ftemp;
   delete itemp;
}

void AdstMva::PlotLongProf(TCanvas *c1, string fdid)
{
   cout << "# Entering function AdstMva::PlotLongProf()..." << endl;
   double *x, *xerr, *y, *yerr;
   double *cum, *cumerr;
   int *nrpoints, *selid;

   x = new double;
   xerr = new double;
   y = new double;
   yerr = new double;
   cum = new double;
   cumerr = new double;
   nrpoints = new int;
   selid = new int;

   c1->cd();
   c1->SetGrid();
   c1->SetRightMargin(0.05);
   c1->SetTopMargin(0.05);

   vector<FDEvent> fdevt = fRecEvent->GetFDEvents();

   vector<double> slantDepth;
   vector<double> profiledEdX;
   vector<double> profiledEdXerr;

/*   vector<double> xfoot;
   vector<double> yfoot;
   vector<double> yerrfoot;*/

   if(fdid == "Los Leones") *selid = 1;
   else if(fdid == "Los Morados") *selid = 2;
   else if(fdid == "Loma Amarilla") *selid = 3;
   else if(fdid == "Coihueco") *selid = 4;
   else if(fdid == "HEAT") *selid = 5;

   for(int i = 0; i < fdevt.size(); i++)
   {
      if(fdevt[i].GetEyeId() == *selid )
      {
         slantDepth = acteyes[i].GetFdRecShower().GetDepth();
         profiledEdX = acteyes[i].GetFdRecShower().GetEnergyDeposit();
         profiledEdXerr = acteyes[i].GetFdRecShower().GetEnergyDepositError();
      }
   }

   *nrpoints = slantDepth.size();
   cout << "Number of points: " << *nrpoints << endl;

   grErr = new TGraphErrors(*nrpoints);
/*   grErrCum = new TGraphErrors(*nrpoints);
   *cum = 0;
   *cumerr = 0;
   if( xfoot.size() != 0 )
      xfoot.erase(xfoot.begin(),xfoot.end());
   if( yfoot.size() != 0 )
      yfoot.erase(yfoot.begin(),yfoot.end());
   if( yerrfoot.size() != 0 )
      yerrfoot.erase(yerrfoot.begin(),yerrfoot.end());*/

   for(int i = 0; i < *nrpoints; i++)
   {
      grErr->SetPoint(i,slantDepth[i],profiledEdX[i]);
      grErr->SetPointError(i,1.e-7,profiledEdXerr[i]);

/*      *cum += profiledEdX[i];
      *cumerr += profiledEdXerr[i];

      xfoot.push_back(slantDepth[i]);
      yfoot.push_back(*cum);
      yerrfoot.push_back(*cumerr);

      grErrCum->SetPoint(i,slantDepth[i],*cum);
      grErrCum->SetPointError(i,1.e-7,*cumerr);*/

//       cout << "Point " << i << ": " << xfoot[i] << "\t" << yfoot[i] << " (" << yerrfoot[i] << ")" << endl;
   }

   grErr->SetMarkerStyle(20);
   grErr->SetMarkerSize(0.7);
   grErr->SetTitle(";Slant depth (g/cm^{2});dE/dX (PeV/(g/cm^{2}))");
   grErr->GetXaxis()->SetTitleOffset(1.2);
   grErr->GetXaxis()->CenterTitle(kTRUE);
   grErr->GetXaxis()->SetLabelSize(0.028);
   grErr->GetXaxis()->SetLabelOffset(0.015);
   grErr->GetYaxis()->SetTitleOffset(1.3);
   grErr->GetYaxis()->CenterTitle(kTRUE);
   grErr->GetYaxis()->SetLabelSize(0.028);
   grErr->GetYaxis()->SetLabelOffset(0.015);
   grErr->SetTitle("");
   grErr->Draw("AP");

   c1->Modified();
   c1->Update();

   // Plot also the generated profile on top of it
   GenShower genevt = fRecEvent->GetGenShower();
   slantDepth = genevt.GetDepth();
   profiledEdX = genevt.GetEnergyDeposit();
   gr = new TGraph(*nrpoints);

   for(int i = 0; i < *nrpoints; i++)
      gr->SetPoint(i,slantDepth[i],profiledEdX[i]);
   gr->SetLineColor(kRed);
   gr->SetLineWidth(2);
   gr->Draw("LP;SAME");

//   delete grErr;
   delete x;
   delete xerr;
   delete y;
   delete yerr;
   delete cum;
   delete cumerr;
   delete nrpoints;
   delete selid;

/*   string *stemp, *stemp2;
   double *cum, *cumerr;
   int *nrpoints;

   x = new double;
   xerr = new double;
   y = new double;
   yerr = new double;
   cum = new double;
   cumerr = new double;
   nrpoints = new int;

   stemp = new string;
   stemp2 = new string;

   c1 = new TCanvas("c1","c1",1200,800);
   c1->SetGrid();
   c1->SetRightMargin(0.05);
   c1->SetTopMargin(0.05);

   vector<FDEvent> fdevt = fRecEvent->GetFDEvents();

   vector<double> slantDepth;
   vector<double> profiledEdX;
   vector<double> profiledEdXerr;

   for(int i = 0; i < fdevt.size(); i++)
   {
      if(fdevt[i].GetEyeId() == stoi(sepdir[1]))
      {
         slantDepth = acteyes[i].GetDepth();
         profiledEdX = acteyes[i].GetEnergyDeposit();
         profiledEdXerr = acteyes[i].GetEnergyDepositError();
      }
   }

   grErr = new TGraphErrors(slantDepth.size());

   if(btemp)    // only valid if we want separate cumulative longitudinal distribution plots
   {
      grErrtemp = new TGraphErrors(slantDepth.size());
      *cum = 0;
      if( xfoot.size() != 0 )
         xfoot.erase(xfoot.begin(),xfoot.end());
      if( yfoot.size() != 0 )
         yfoot.erase(yfoot.begin(),yfoot.end());
      if( yerrfoot.size() != 0 )
         yerrfoot.erase(yerrfoot.begin(),yerrfoot.end());
   }

   for(int i = 0; i < slantDepth.size(); i++)
   {
      grErr->SetPoint(i,slantDepth[i],profiledEdX[i]);
      grErr->SetPointError(i,1.e-7,profiledEdXerr[i]);

      if(btemp)
      {
         *cum += profiledEdX[i];
         *cumerr += profiledEdXerr[i];

         xfoot.push_back(slantDepth[i]);
         yfoot.push_back(*cum);
         yerrfoot.push_back(*cumerr);

         grErrtemp->SetPoint(i,slantDepth[i],*cum);
         grErrtemp->SetPointError(i,1.e-7,*cumerr);

//       cout << "Point " << i << ": " << xfoot[i] << "\t" << yfoot[i] << " (" << yerrfoot[i] << ")" << endl;
      }
   }

   // Setting output names for plots
   if(stoi(sepdir[1]) == 1)
   {
      *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LL_profile.pdf";
      *stemp2 = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LL_profile.C";
   }
   else if(stoi(sepdir[1]) == 2)
   {
      *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LM_profile.pdf";
      *stemp2 = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LM_profile.C";
   }
   else if(stoi(sepdir[1]) == 3)
   {
      *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LA_profile.pdf";
      *stemp2 = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LA_profile.C";
   }
   else if(stoi(sepdir[1]) == 4)
   {
      *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_CO_profile.pdf";
      *stemp2 = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_CO_profile.C";
   }
   else
   {
      *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_" + sepdir[1] + "_profile.pdf";
      *stemp2 = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_" + sepdir[1] + "_profile.C";
   }

   grErr->SetMarkerStyle(20);
   grErr->SetMarkerSize(0.7);
   sepdir[0] = directivedesc[seldir[0]] + ";Slant depth (g/cm^{2});dE/dX (PeV/(g/cm^{2}))";
   grErr->SetTitle(sepdir[0].c_str());
   grErr->GetXaxis()->SetTitleOffset(1.2);
   grErr->GetXaxis()->CenterTitle(kTRUE);
   grErr->GetXaxis()->SetLabelSize(0.028);
   grErr->GetXaxis()->SetLabelOffset(0.015);
   grErr->GetYaxis()->SetTitleOffset(1.3);
   grErr->GetYaxis()->CenterTitle(kTRUE);
   grErr->GetYaxis()->SetLabelSize(0.028);
   grErr->GetYaxis()->SetLabelOffset(0.015);
   grErr->SetTitle("");
   grErr->Draw("AP");

   // Plot also the generated profile on top of it
   GenShower genevt = fRecEvent->GetGenShower();
   slantDepth = genevt.GetDepth();
   profiledEdX = genevt.GetEnergyDeposit();
   gr = new TGraph(slantDepth.size());

   for(int i = 0; i < slantDepth.size(); i++)
      gr->SetPoint(i,slantDepth[i],profiledEdX[i]);
   gr->SetLineColor(kRed);
   gr->SetLineWidth(2);
   gr->Draw("LP;SAME");
      
   c1->SaveAs((*stemp).c_str());
   c1->SaveAs((*stemp2).c_str());

//-------------------------------------------
   // Plotting instructions for the cumulative distribution
   if(btemp)
   {
      *nrpoints = 0;

      for(int i = 0; i < yfoot.size(); i++)
      {
         if( (yfoot[i] >= shfootlimit*(yfoot[yfoot.size()-1])) && (*nrpoints == 0) )
         {
            *nrpoints = 1;

            // Find the x value of point with y value that is a fraction of the maximum, that lies on a line between two points
            // y = k*x + a
            //    k = (y2 - y1)/(x2 - x1)
            //    a = y2 - (y2 - y1)/(x2 - x1)*x2
            // x = ((x2 - x1)/(y2 - y1))*(y - y2) + x2
            *cum = (xfoot[i] - xfoot[i-1])/(yfoot[i] - yfoot[i-1]); // 1/k = (x2 - x1)/(y2 - y1)
            *cumerr = shfootlimit*(yfoot[yfoot.size()-1]) - yfoot[i]; // y - y2
            *x = (*cum)*(*cumerr) + xfoot[i]; // x = (1/k)*(y - y2) + x2

//          cout << *cum << "\t" << *cumerr << "\t" << *x << endl;

            *cum = (xfoot[i] - xfoot[i-1])/(yfoot[i]+yerrfoot[i] - (yfoot[i-1]+yerrfoot[i-1])); // 1/kerr = (x2 - x1)/(y2err - y1err)
            *cumerr = (yfoot[i]+yerrfoot[i]) - (1/(*cum))*(xfoot[i]); // aerr = y2err - (y2err - y1err)/(x2 - x1)*x2
            *cum = (1/(*cum))*(*x) + (*cumerr); // yerr = kerr*x + aerr
            *yerr = (*cum) - (((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(*x) + (yfoot[i] - ((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(xfoot[i]))); // Dy = yerr - y

            *y = ((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(*x) + (yfoot[i] - ((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(xfoot[i])); // y = k*x + a

            for(int j = i; ; j++)
            {
               if( yfoot[j] >= (*y)+(*yerr) )
               {
//                cout << "Upper limit is = " << xfoot[j] << endl;
                  *cumerr = xfoot[j];
                  break;
               }
            }
            for(int j = i; ; j--)
            {
               if(j == 0)
               {
                  *xerr = xfoot[j];
//                cout << "Lower limit is = " << xfoot[j] << endl;
                  break;
               }

               if( yfoot[j] <= (*y)-(*yerr) )
               {
//                cout << "Lower limit is = " << xfoot[j] << endl;
                  *xerr = xfoot[j];
                  break;
               }
            }

            break;
         }
      }

      grErrtemp->SetTitle(";Slant depth (g/cm^{2});Cumulative dE/dX (PeV/(g/cm^{2}))");
      grErrtemp->GetXaxis()->SetTitleOffset(1.2);
      grErrtemp->GetXaxis()->CenterTitle(kTRUE);
      grErrtemp->GetXaxis()->SetLabelSize(0.028);
      grErrtemp->GetXaxis()->SetLabelOffset(0.015);
      grErrtemp->GetYaxis()->SetTitleOffset(1.4);
      grErrtemp->GetYaxis()->CenterTitle(kTRUE);
      grErrtemp->GetYaxis()->SetLabelSize(0.028);
      grErrtemp->GetYaxis()->SetLabelOffset(0.015);
      grErrtemp->Draw("AL");
      grErrtemp->SetLineColor(kGray+2);

      cout << "Marker at = " << *x << ", " << *xerr << " | " << *y << ", " << *yerr << endl;
      cout << "Box at = " << *xerr << ", " << (*y)+(*yerr) << " | " << *cumerr << ", " << (*y)-(*yerr) << endl;

      // Draw point and a red box do represent the shower foot limiting value and its error
//      TBox *b1 = new TBox(*xerr, (*y)+(*yerr), *cumerr, (*y)-(*yerr));
//      b1->SetLineColor(kRed);
//      b1->SetFillColorAlpha(kRed, 0.3);
//      b1->Draw();

      TGraphAsymmErrors *m1 = new TGraphAsymmErrors(1);
      m1->SetPoint(0,*x,*y);
      m1->SetPointError(0, (*x)-(*xerr), (*cumerr)-(*x), (*yerr), (*yerr));
      m1->SetMarkerColor(kRed);
      m1->SetMarkerSize(0.8);
      m1->SetLineColor(kRed);
      m1->Draw("SAME");

      if(stoi(sepdir[1]) == 1)
         *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LL_cumulative_profile.pdf";
      else if(stoi(sepdir[1]) == 2)
         *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LM_cumulative_profile.pdf";
      else if(stoi(sepdir[1]) == 3)
         *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_LA_cumulative_profile.pdf";
      else if(stoi(sepdir[1]) == 4)
         *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_CO_cumulative_profile.pdf";
      else
         *stemp = string(BASEDIR) + "/results/" + directive[seldir[0]] + "_" + sepdir[1] + "_cumulative_profile.pdf";
      c1->SaveAs((*stemp).c_str());

      delete grErrtemp;
      delete m1;
   }

   delete grErr;
   delete x;
   delete xerr;
   delete y;
   delete yerr;
   delete cum;
   delete cumerr;
   delete nrpoints;
   delete stemp;
   delete stemp2;

   return 0;
*/
}

void AdstMva::PrepareOtherTrees(unsigned int nrfiles, int innr)
{
   Observables *othersig = new Observables();
   Observables *othersig_neg = new Observables();
   Observables *othersig_pos = new Observables();
// NEW
   TTree *other_sig_tree;

#if OFFVER == 0
   other_sig_tree = new TTree(("TreeNewS" + IntToStr(innr+1)).c_str(), "Signal tree from new file.");
#elif OFFVER == 1
   other_sig_tree = new TTree(("TreeOldS" + IntToStr(innr+1)).c_str(), "Signal tree from old file.");
#endif

   other_sig_tree->Branch("xmax",      &(othersig->xmax), "xmax/F");
   other_sig_tree->Branch("x0",        &(othersig->x0), "x0/F");
   other_sig_tree->Branch("lambda",    &(othersig->lambda), "lambda/F");
   other_sig_tree->Branch("sdenergy",  &(othersig->sdenergy), "sdenergy/F");
   other_sig_tree->Branch("fdenergy",  &(othersig->fdenergy), "fdenergy/F");
   other_sig_tree->Branch("shfoot",    &(othersig->shfoot), "shfoot/F");
   other_sig_tree->Branch("shwsize",   &(othersig->shwsize), "shwsize/F");
   other_sig_tree->Branch("ldfbeta",   &(othersig->ldfbeta), "ldfbeta/F");
   other_sig_tree->Branch("curvature", &(othersig->curvature), "curvature/F");
   other_sig_tree->Branch("nrmu",      &(othersig->nrmu), "nrmu/F");
   other_sig_tree->Branch("risetime",  &(othersig->risetime), "risetime/F");
   other_sig_tree->Branch("risetimerecalc",  &(othersig->risetimerecalc), "risetimerecalc/F");
   other_sig_tree->Branch("aop",       &(othersig->aop), "aop/F");
   other_sig_tree->Branch("zenithSD",  &(othersig->zenithSD), "zenithSD/F");
   other_sig_tree->Branch("azimuthSD", &(othersig->azimuthSD), "azimuthSD/F");
   other_sig_tree->Branch("zenithFD",  &(othersig->zenithFD), "zenithFD/F");
   other_sig_tree->Branch("azimuthFD", &(othersig->azimuthFD), "azimuthFD/F");

   other_sig_tree->Branch("xmax_neg",      &(othersig_neg->xmax), "xmax_neg/F");
   other_sig_tree->Branch("x0_neg",        &(othersig_neg->x0), "x0_neg/F");
   other_sig_tree->Branch("lambda_neg",    &(othersig_neg->lambda), "lambda_neg/F");
   other_sig_tree->Branch("sdenergy_neg",  &(othersig_neg->sdenergy), "sdenergy_neg/F");
   other_sig_tree->Branch("fdenergy_neg",  &(othersig_neg->fdenergy), "fdenergy_neg/F");
   other_sig_tree->Branch("shfoot_neg",    &(othersig_neg->shfoot), "shfoot_neg/F");
   other_sig_tree->Branch("shwsize_neg",   &(othersig_neg->shwsize), "shwsize_neg/F");
   other_sig_tree->Branch("ldfbeta_neg",   &(othersig_neg->ldfbeta), "ldfbeta_neg/F");
   other_sig_tree->Branch("curvature_neg", &(othersig_neg->curvature), "curvature_neg/F");
   other_sig_tree->Branch("nrmu_neg",      &(othersig_neg->nrmu), "nrmu_neg/F");
   other_sig_tree->Branch("risetime_neg",  &(othersig_neg->risetime), "risetime_neg/F");
   other_sig_tree->Branch("risetimerecalc_neg",  &(othersig_neg->risetimerecalc), "risetimerecalc_neg/F");
   other_sig_tree->Branch("aop_neg",       &(othersig_neg->aop), "aop_neg/F");
   other_sig_tree->Branch("zenithSD_neg",  &(othersig_neg->zenithSD), "zenithSD_neg/F");
   other_sig_tree->Branch("azimuthSD_neg", &(othersig_neg->azimuthSD), "azimuthSD_neg/F");
   other_sig_tree->Branch("zenithFD_neg",  &(othersig_neg->zenithFD), "zenithFD_neg/F");
   other_sig_tree->Branch("azimuthFD_neg", &(othersig_neg->azimuthFD), "azimuthFD_neg/F");

   other_sig_tree->Branch("xmax_pos",      &(othersig_pos->xmax), "xmax_pos/F");
   other_sig_tree->Branch("x0_pos",        &(othersig_pos->x0), "x0_pos/F");
   other_sig_tree->Branch("lambda_pos",    &(othersig_pos->lambda), "lambda_pos/F");
   other_sig_tree->Branch("sdenergy_pos",  &(othersig_pos->sdenergy), "sdenergy_pos/F");
   other_sig_tree->Branch("fdenergy_pos",  &(othersig_pos->fdenergy), "fdenergy_pos/F");
   other_sig_tree->Branch("shfoot_pos",    &(othersig_pos->shfoot), "shfoot_pos/F");
   other_sig_tree->Branch("shwsize_pos",   &(othersig_pos->shwsize), "shwsize_pos/F");
   other_sig_tree->Branch("ldfbeta_pos",   &(othersig_pos->ldfbeta), "ldfbeta_pos/F");
   other_sig_tree->Branch("curvature_pos", &(othersig_pos->curvature), "curvature_pos/F");
   other_sig_tree->Branch("nrmu_pos",      &(othersig_pos->nrmu), "nrmu_pos/F");
   other_sig_tree->Branch("risetime_pos",  &(othersig_pos->risetime), "risetime_pos/F");
   other_sig_tree->Branch("risetimerecalc_pos",  &(othersig_pos->risetimerecalc), "risetimerecalc_pos/F");
   other_sig_tree->Branch("aop_pos",       &(othersig_pos->aop), "aop_pos/F");
   other_sig_tree->Branch("zenithSD_pos",  &(othersig_pos->zenithSD), "zenithSD_pos/F");
   other_sig_tree->Branch("azimuthSD_pos", &(othersig_pos->azimuthSD), "azimuthSD_pos/F");
   other_sig_tree->Branch("zenithFD_pos",  &(othersig_pos->zenithFD), "zenithFD_pos/F");
   other_sig_tree->Branch("azimuthFD_pos", &(othersig_pos->azimuthFD), "azimuthFD_pos/F");

   other_sig_tree->Write();

   delete other_sig_tree;
   delete othersig_neg;
   delete othersig_pos;
/*    TTree *other_sig_tree[3];

#if OFFVER == 0
      other_sig_tree[0] = new TTree(("TreeNewS" + IntToStr(innr+1) + "_mean").c_str(), "Signal tree from new file (mean value).");
      other_sig_tree[1] = new TTree(("TreeNewS" + IntToStr(innr+1) + "_neg").c_str(), "Signal tree from new file (mean value + negative error).");
      other_sig_tree[2] = new TTree(("TreeNewS" + IntToStr(innr+1) + "_pos").c_str(), "Signal tree from new file (mean value + positive error).");
#elif OFFVER == 1
      other_sig_tree[0] = new TTree(("TreeOldS" + IntToStr(innr+1) + "_mean").c_str(), "Signal tree from old file (mean value).");
      other_sig_tree[1] = new TTree(("TreeOldS" + IntToStr(innr+1) + "_neg").c_str(), "Signal tree from old file (mean value + negative error).");
      other_sig_tree[2] = new TTree(("TreeOldS" + IntToStr(innr+1) + "_pos").c_str(), "Signal tree from old file (mean value + positive error).");
#endif

   for(int i = 0; i < 3; i++)
   {
      other_sig_tree[i]->Branch("xmax",      &(othersig->xmax), "xmax/F");
      other_sig_tree[i]->Branch("x0",        &(othersig->x0), "x0/F");
      other_sig_tree[i]->Branch("lambda",    &(othersig->lambda), "lambda/F");
      other_sig_tree[i]->Branch("fdenergy",  &(othersig->fdenergy), "fdenergy/F");
      other_sig_tree[i]->Branch("shfoot",    &(othersig->shfoot), "shfoot/F");
      other_sig_tree[i]->Branch("shwsize",   &(othersig->shwsize), "shwsize/F");
      other_sig_tree[i]->Branch("ldfbeta",   &(othersig->ldfbeta), "ldfbeta/F");
      other_sig_tree[i]->Branch("curvature", &(othersig->curvature), "curvature/F");
      other_sig_tree[i]->Branch("nrmu",      &(othersig->nrmu), "nrmu/F");
      other_sig_tree[i]->Branch("risetime",  &(othersig->risetime), "risetime/F");
      other_sig_tree[i]->Branch("risetimerecalc",  &(othersig->risetimerecalc), "risetimerecalc/F");
      other_sig_tree[i]->Branch("aop",       &(othersig->aop), "aop/F");

//      other_sig_tree[i]->Fill();

      other_sig_tree[i]->Write();
   }

   delete other_sig_tree[0];
   delete other_sig_tree[1];
   delete other_sig_tree[2];*/
   delete othersig;
}

int AdstMva::GetEyeBestTrack()
{
   cout << "# Entering function AdstMva::GetEyeBestTrack()..." << endl;
   double longtrack = 0.0;
   double xmaxerr = 1000.;
   double chi2ndf = 0.0;
   int best[acteyes.size()];
   int noisy[acteyes.size()];
   int itemp[] = {-1,-1,-1,-1};
   bool noisecol = true;
   int retval = -1;

   for(int i = 0; i < acteyes.size(); i++)
   {
      best[i] = 0;
      noisy[i] = 0;

      cout << i << ": Event type: " << acteyes[i].GetEventType() << ", ";
      cout << "Event class: " << acteyes[i].GetEventClass() << ", ";
      cout << "Current track length: " << (acteyes[i].GetFdRecShower()).GetTrackLength() << ", ";
      cout << "Current track Xmax error: " << (acteyes[i].GetFdRecShower()).GetXmaxError() << ", ";
//      cout << "Current track Cher. Fraction: " << (acteyes[i].GetFdRecShower()).GetCherenkovFraction() << ", ";
      cout << "Current track GH Chi2/Ndf: " << (acteyes[i].GetFdRecShower()).GetGHChi2() << "/" << (acteyes[i].GetFdRecShower()).GetGHNdf() << " (" << (double)((acteyes[i].GetFdRecShower()).GetGHChi2())/((acteyes[i].GetFdRecShower()).GetGHNdf()) << ")" << endl;

      if( (acteyes[i].GetFdRecShower()).GetTrackLength() >= longtrack )
      {
         itemp[0] = i;
         longtrack = (acteyes[i].GetFdRecShower()).GetTrackLength();
      }

      if( (acteyes[i].GetFdRecShower()).GetXmaxError() <= xmaxerr )
      {
         itemp[1] = i;
         xmaxerr = (acteyes[i].GetFdRecShower()).GetXmaxError();
      }

      if( (double)((acteyes[i].GetFdRecShower()).GetGHChi2())/((acteyes[i].GetFdRecShower()).GetGHNdf()) >= chi2ndf )
      {
         itemp[2] = i;
         chi2ndf = (double)((acteyes[i].GetFdRecShower()).GetGHChi2())/((acteyes[i].GetFdRecShower()).GetGHNdf());
      }

      if( acteyes[i].GetEventClass() == "'Noise'" )
         noisy[i] = 1;
   }

   for(int i = 0; i < acteyes.size(); i++)
   {
      noisecol = true;

      // Longest track
      if(itemp[0] == i)
         best[i]++;
      // Smallest Xmax error
      if(itemp[1] == i)
         best[i]++;
      // Longitudinal fit - Chi2/Ndf closest to 1
      if(itemp[2] == i)
         best[i]++;

      // Event is not classified as Noise (prefer actual shower events)
      if(noisy[i] == 0)
         best[i]++;
      else
      {
         if(acteyes.size() > 1)
	 {
	    // Checking to see if all eyes are classified as Noise (if yes, disregard Noise for selection)
	    for(int j = 0; j < acteyes.size(); j++)
	    {
	       cout << j << ": noisecol = " << (int)noisecol << ", noisy[j] = " << noisy[j] << ", noisecol && noisy[j] " << (int)(noisecol && noisy[j]) << endl;
               noisecol = noisecol && noisy[j];
	    }

	    if( (!noisecol) && (noisy[i] == 1) )
	       best[i] = 0;
	 }
      }
      
      cout << "Eye " << i << ": FD best values = " << best[i] << endl;
   }

   for(int i = 0; i < acteyes.size(); i++)
   {
      if(TMath::MaxElement(acteyes.size(), best) == best[i])
      {
         retval = i;
	 break;
      }
   }
   
//   cout << "The longest track is " << longtrack << endl;
   cout << "The highest quality track is " << retval << " (" << itemp[0] << "," << itemp[1] << "," << itemp[2] << " = " << TMath::MaxElement(acteyes.size(), best) << ")" << endl;

   return retval;
}

void AdstMva::RewriteObservables(unsigned int nrfiles, int innr, Observables **sig, Observables **all, double frac)
{
   string stemp, stemp2;
   int *itemp;
   itemp = new int;
   double dtemp[2];

   cout << "# Entering function AdstMva::RewriteObservables()..." << endl;
   bool singlerun = false;
   bool sampling = false;
   int nrall;

   if(nrfiles == 1)
   {
      innr = 0;
      singlerun = true;
   }

   if(frac != -1)
      sampling = true;

   cout << "# New input file (" << inname << ") ---------------------------------" << endl;

   // Prepare signal and background trees (all = complete set of events, back = only events that are not signal)
// NEW
#if OFFVER == 0
   stemp = "TreeOldS" + IntToStr(innr+1);
#elif OFFVER == 1
   stemp = "TreeNewS" + IntToStr(innr+1);
#endif
   stemp2 = "Signal tree from file " + inname + ".";

   sig_tree = new TTree(stemp.c_str(), stemp2.c_str());

   sig_tree->Branch("xmax",      &(sig[0]->xmax),      "xmax/F");
   sig_tree->Branch("x0",        &(sig[0]->x0),        "x0/F");
   sig_tree->Branch("lambda",    &(sig[0]->lambda),    "lambda/F");
   sig_tree->Branch("sdenergy",  &(sig[0]->sdenergy),  "sdenergy/F");
   sig_tree->Branch("fdenergy",  &(sig[0]->fdenergy),  "fdenergy/F");
   sig_tree->Branch("shfoot",    &(sig[0]->shfoot),    "shfoot/F");
   sig_tree->Branch("shwsize",   &(sig[0]->shwsize),   "shwsize/F");
   sig_tree->Branch("ldfbeta",   &(sig[0]->ldfbeta),   "ldfbeta/F");
   sig_tree->Branch("curvature", &(sig[0]->curvature), "curvature/F");
   sig_tree->Branch("nrmu",      &(sig[0]->nrmu),      "nrmu/F");
   sig_tree->Branch("risetime",  &(sig[0]->risetime),  "risetime/F");
   sig_tree->Branch("risetimerecalc",  &(sig[0]->risetimerecalc),  "risetimerecalc/F");
   sig_tree->Branch("aop",       &(sig[0]->aop),       "aop/F");
   sig_tree->Branch("zenithSD",  &(sig[0]->zenithSD),  "zenithSD/F");
   sig_tree->Branch("azimuthSD", &(sig[0]->azimuthSD), "azimuthSD/F");
   sig_tree->Branch("zenithFD",  &(sig[0]->zenithFD),  "zenithFD/F");
   sig_tree->Branch("azimuthFD", &(sig[0]->azimuthFD), "azimuthFD/F");

   sig_tree->Branch("xmax_neg",      &(sig[1]->xmax),      "xmax_neg/F");
   sig_tree->Branch("x0_neg",        &(sig[1]->x0),        "x0_neg/F");
   sig_tree->Branch("lambda_neg",    &(sig[1]->lambda),    "lambda_neg/F");
   sig_tree->Branch("sdenergy_neg",  &(sig[1]->sdenergy),  "sdenergy_neg/F");
   sig_tree->Branch("fdenergy_neg",  &(sig[1]->fdenergy),  "fdenergy_neg/F");
   sig_tree->Branch("shfoot_neg",    &(sig[1]->shfoot),    "shfoot_neg/F");
   sig_tree->Branch("shwsize_neg",   &(sig[1]->shwsize),   "shwsize_neg/F");
   sig_tree->Branch("ldfbeta_neg",   &(sig[1]->ldfbeta),   "ldfbeta_neg/F");
   sig_tree->Branch("curvature_neg", &(sig[1]->curvature), "curvature_neg/F");
   sig_tree->Branch("nrmu_neg",      &(sig[1]->nrmu),      "nrmu_neg/F");
   sig_tree->Branch("risetime_neg",  &(sig[1]->risetime),  "risetime_neg/F");
   sig_tree->Branch("risetimerecalc_neg",  &(sig[1]->risetimerecalc),  "risetimerecalc_neg/F");
   sig_tree->Branch("aop_neg",       &(sig[1]->aop),       "aop_neg/F");
   sig_tree->Branch("zenithSD_neg",  &(sig[1]->zenithSD),  "zenithSD_neg/F");
   sig_tree->Branch("azimuthSD_neg", &(sig[1]->azimuthSD), "azimuthSD_neg/F");
   sig_tree->Branch("zenithFD_neg",  &(sig[1]->zenithFD),  "zenithFD_neg/F");
   sig_tree->Branch("azimuthFD_neg", &(sig[1]->azimuthFD), "azimuthFD_neg/F");

   sig_tree->Branch("xmax_pos",      &(sig[2]->xmax),      "xmax_pos/F");
   sig_tree->Branch("x0_pos",        &(sig[2]->x0),        "x0_pos/F");
   sig_tree->Branch("lambda_pos",    &(sig[2]->lambda),    "lambda_pos/F");
   sig_tree->Branch("sdenergy_pos",  &(sig[2]->sdenergy),  "sdenergy_pos/F");
   sig_tree->Branch("fdenergy_pos",  &(sig[2]->fdenergy),  "fdenergy_pos/F");
   sig_tree->Branch("shfoot_pos",    &(sig[2]->shfoot),    "shfoot_pos/F");
   sig_tree->Branch("shwsize_pos",   &(sig[2]->shwsize),   "shwsize_pos/F");
   sig_tree->Branch("ldfbeta_pos",   &(sig[2]->ldfbeta),   "ldfbeta_pos/F");
   sig_tree->Branch("curvature_pos", &(sig[2]->curvature), "curvature_pos/F");
   sig_tree->Branch("nrmu_pos",      &(sig[2]->nrmu),      "nrmu_pos/F");
   sig_tree->Branch("risetime_pos",  &(sig[2]->risetime),  "risetime_pos/F");
   sig_tree->Branch("risetimerecalc_pos",  &(sig[2]->risetimerecalc),  "risetimerecalc_pos/F");
   sig_tree->Branch("aop_pos",       &(sig[2]->aop),       "aop_pos/F");
   sig_tree->Branch("zenithSD_pos",  &(sig[2]->zenithSD),  "zenithSD_pos/F");
   sig_tree->Branch("azimuthSD_pos", &(sig[2]->azimuthSD), "azimuthSD_pos/F");
   sig_tree->Branch("zenithFD_pos",  &(sig[2]->zenithFD),  "zenithFD_pos/F");
   sig_tree->Branch("azimuthFD_pos", &(sig[2]->azimuthFD), "azimuthFD_pos/F");
   
   all_tree->Branch("xmax",      &(all[0]->xmax),      "xmax/F");
   all_tree->Branch("x0",        &(all[0]->x0),        "x0/F");
   all_tree->Branch("lambda",    &(all[0]->lambda),    "lambda/F");
   all_tree->Branch("sdenergy",  &(all[0]->sdenergy),  "sdenergy/F");
   all_tree->Branch("fdenergy",  &(all[0]->fdenergy),  "fdenergy/F");
   all_tree->Branch("shfoot",    &(all[0]->shfoot),    "shfoot/F");
   all_tree->Branch("shwsize",   &(all[0]->shwsize),   "shwsize/F");
   all_tree->Branch("ldfbeta",   &(all[0]->ldfbeta),   "ldfbeta/F");
   all_tree->Branch("curvature", &(all[0]->curvature), "curvature/F");
   all_tree->Branch("nrmu",      &(all[0]->nrmu),      "nrmu/F");
   all_tree->Branch("risetime",  &(all[0]->risetime),  "risetime/F");
   all_tree->Branch("risetimerecalc",  &(all[0]->risetimerecalc),  "risetimerecalc/F");
   all_tree->Branch("aop",       &(all[0]->aop),       "aop/F");
   all_tree->Branch("zenithSD",  &(all[0]->zenithSD),  "zenithSD/F");
   all_tree->Branch("azimuthSD", &(all[0]->azimuthSD), "azimuthSD/F");
   all_tree->Branch("zenithFD",  &(all[0]->zenithFD),  "zenithFD/F");
   all_tree->Branch("azimuthFD", &(all[0]->azimuthFD), "azimuthFD/F");
   
   all_tree->Branch("xmax_neg",      &(all[1]->xmax),      "xmax_neg/F");
   all_tree->Branch("x0_neg",        &(all[1]->x0),        "x0_neg/F");
   all_tree->Branch("lambda_neg",    &(all[1]->lambda),    "lambda_neg/F");
   all_tree->Branch("sdenergy_neg",  &(all[1]->sdenergy),  "sdenergy_neg/F");
   all_tree->Branch("fdenergy_neg",  &(all[1]->fdenergy),  "fdenergy_neg/F");
   all_tree->Branch("shfoot_neg",    &(all[1]->shfoot),    "shfoot_neg/F");
   all_tree->Branch("shwsize_neg",   &(all[1]->shwsize),   "shwsize_neg/F");
   all_tree->Branch("ldfbeta_neg",   &(all[1]->ldfbeta),   "ldfbeta_neg/F");
   all_tree->Branch("curvature_neg", &(all[1]->curvature), "curvature_neg/F");
   all_tree->Branch("nrmu_neg",      &(all[1]->nrmu),      "nrmu_neg/F");
   all_tree->Branch("risetime_neg",  &(all[1]->risetime),  "risetime_neg/F");
   all_tree->Branch("risetimerecalc_neg",  &(all[1]->risetimerecalc),  "risetimerecalc_neg/F");
   all_tree->Branch("aop_neg",       &(all[1]->aop),       "aop_neg/F");
   all_tree->Branch("zenithSD_neg",  &(all[1]->zenithSD),  "zenithSD_neg/F");
   all_tree->Branch("azimuthSD_neg", &(all[1]->azimuthSD), "azimuthSD_neg/F");
   all_tree->Branch("zenithFD_neg",  &(all[1]->zenithFD),  "zenithFD_neg/F");
   all_tree->Branch("azimuthFD_neg", &(all[1]->azimuthFD), "azimuthFD_neg/F");
   
   all_tree->Branch("xmax_pos",      &(all[2]->xmax),      "xmax_pos/F");
   all_tree->Branch("x0_pos",        &(all[2]->x0),        "x0_pos/F");
   all_tree->Branch("lambda_pos",    &(all[2]->lambda),    "lambda_pos/F");
   all_tree->Branch("sdenergy_pos",  &(all[2]->sdenergy),  "sdenergy_pos/F");
   all_tree->Branch("fdenergy_pos",  &(all[2]->fdenergy),  "fdenergy_pos/F");
   all_tree->Branch("shfoot_pos",    &(all[2]->shfoot),    "shfoot_pos/F");
   all_tree->Branch("shwsize_pos",   &(all[2]->shwsize),   "shwsize_pos/F");
   all_tree->Branch("ldfbeta_pos",   &(all[2]->ldfbeta),   "ldfbeta_pos/F");
   all_tree->Branch("curvature_pos", &(all[2]->curvature), "curvature_pos/F");
   all_tree->Branch("nrmu_pos",      &(all[2]->nrmu),      "nrmu_pos/F");
   all_tree->Branch("risetime_pos",  &(all[2]->risetime),  "risetime_pos/F");
   all_tree->Branch("risetimerecalc_pos",  &(all[2]->risetimerecalc),  "risetimerecalc_pos/F");
   all_tree->Branch("aop_pos",       &(all[2]->aop),       "aop_pos/F");
   all_tree->Branch("zenithSD_pos",  &(all[2]->zenithSD),  "zenithSD_pos/F");
   all_tree->Branch("azimuthSD_pos", &(all[2]->azimuthSD), "azimuthSD_pos/F");
   all_tree->Branch("zenithFD_pos",  &(all[2]->zenithFD),  "zenithFD_pos/F");
   all_tree->Branch("azimuthFD_pos", &(all[2]->azimuthFD), "azimuthFD_pos/F");

/*   for(int i = 0; i < 3; i++)
   {
#if OFFVER == 0
      if(i == 0)
         stemp = "TreeOldS" + IntToStr(innr+1) + "_mean";
      else if(i == 1)
         stemp = "TreeOldS" + IntToStr(innr+1) + "_neg";
      else if(i == 2)
         stemp = "TreeOldS" + IntToStr(innr+1) + "_pos";
#elif OFFVER == 1
      if(i == 0)
         stemp = "TreeNewS" + IntToStr(innr+1) + "_mean";
      else if(i == 1)
         stemp = "TreeNewS" + IntToStr(innr+1) + "_neg";
      else if(i == 2)
         stemp = "TreeNewS" + IntToStr(innr+1) + "_pos";
#endif
      if(i == 0)
         stemp2 = "Signal tree from file " + inname + " (mean value).";
      else if(i == 1)
         stemp2 = "Signal tree from file " + inname + " (mean value + negative error).";
      else if(i == 2)
         stemp2 = "Signal tree from file " + inname + " (mean value + positive error).";
      
      sig_tree[i] = new TTree(stemp.c_str(), stemp2.c_str());

      sig_tree[i]->Branch("xmax",      &(sig[i]->xmax),      "xmax/F");
      sig_tree[i]->Branch("x0",        &(sig[i]->x0),        "x0/F");
      sig_tree[i]->Branch("lambda",    &(sig[i]->lambda),    "lambda/F");
      sig_tree[i]->Branch("fdenergy",  &(sig[i]->fdenergy),  "fdenergy/F");
      sig_tree[i]->Branch("shfoot",    &(sig[i]->shfoot),    "shfoot/F");
      sig_tree[i]->Branch("shwsize",   &(sig[i]->shwsize),   "shwsize/F");
      sig_tree[i]->Branch("ldfbeta",   &(sig[i]->ldfbeta),   "ldfbeta/F");
      sig_tree[i]->Branch("curvature", &(sig[i]->curvature), "curvature/F");
      sig_tree[i]->Branch("nrmu",      &(sig[i]->nrmu),      "nrmu/F");
      sig_tree[i]->Branch("risetime",  &(sig[i]->risetime),  "risetime/F");
      sig_tree[i]->Branch("risetimerecalc",  &(sig[i]->risetimerecalc),  "risetimerecalc/F");
      sig_tree[i]->Branch("aop",       &(sig[i]->aop),       "aop/F");
    
      all_tree[i]->Branch("xmax",      &(all[i]->xmax),      "xmax/F");
      all_tree[i]->Branch("x0",        &(all[i]->x0),        "x0/F");
      all_tree[i]->Branch("lambda",    &(all[i]->lambda),    "lambda/F");
      all_tree[i]->Branch("fdenergy",  &(all[i]->fdenergy),  "fdenergy/F");
      all_tree[i]->Branch("shfoot",    &(all[i]->shfoot),    "shfoot/F");
      all_tree[i]->Branch("shwsize",   &(all[i]->shwsize),   "shwsize/F");
      all_tree[i]->Branch("ldfbeta",   &(all[i]->ldfbeta),   "ldfbeta/F");
      all_tree[i]->Branch("curvature", &(all[i]->curvature), "curvature/F");
      all_tree[i]->Branch("nrmu",      &(all[i]->nrmu),      "nrmu/F");
      all_tree[i]->Branch("risetime",  &(all[i]->risetime),  "risetime/F");
      all_tree[i]->Branch("risetimerecalc",  &(all[i]->risetimerecalc),  "risetimerecalc/F");
      all_tree[i]->Branch("aop",       &(all[i]->aop),       "aop/F");
   }*/

   // Open and prepare the ADST files that we wish to read
   fFile = new RecEventFile(inname.c_str(), RecEventFile::eRead);
   fFile->SetBuffers(&fRecEvent);
   fFile->ReadDetectorGeometry(*fDetGeo);

   nrall = fFile->GetNEvents();
   cout << "Number of events: " << nrall << endl;

   // Prepare a shuffled list for sampling
   vector<int> shuflist;
   if(sampling)
   {
      cout << "Preparing a shuffled list for sampling. Number of events in the sample = " << (int)(nrall*frac) << endl;
      for(int i = 0; i < nrall; i++)
         shuflist.push_back(i);

      shuffle(shuflist.begin(), shuflist.end(), default_random_engine((unsigned int)chrono::system_clock::now().time_since_epoch().count()));

      nrall = nrall*frac;
/*      cout << "Shuffled setup:" << endl;
      for(int i = 0; i < (int)(fFile->GetNEvents()*frac); i++)
         cout << "  i = " << i << ": " << shuflist[i] << endl;*/
   }

   vector<int> recfail;
   for(int i = 0; i < 8; i++) recfail.push_back(0);

   // Go over all events in the ADST file and write them out to the output file
   for(int j = 0; j < nrall; j++)
   {
      // goodrec variable determines if FD or SD reconstructions failed - not using results from those
      goodrec = true;

      cout << "# New event (" << j+1 << ") ---------------------------------" << endl;
      if(sampling)
         fFile->ReadEvent(shuflist[j]);
      else
         fFile->ReadEvent(j);

      if(acteyes.size() != 0)
         acteyes.erase(acteyes.begin(),acteyes.end());

      // Go over the FD eye events
      cout << "Number of eyes: " << fRecEvent->GetNEyes() << endl;
      if(fRecEvent->GetNEyes() == 0)
      {
         cout << "Error! No reconstructed eyes for this event." << endl;
         if(goodrec) recfail[0]++;
         goodrec = false;
      }
      else
      {
         acteyes = fRecEvent->GetFDEvents();
         cout << "Size: " << acteyes.size() << endl;

         for(int i = 0; i < acteyes.size(); i++)
         {
            if(!acteyes[i].IsHybridEvent())
            {
               cout << "Error! Eye " << i << " not a hybrid event." << endl;
               goodrec = false;
            }
            else
            {
               cout << "Eye " << i << " is a hybrid event." << endl;
               goodrec = true;
               break;
            }
         }

         if(!goodrec) recfail[1]++;

         *itemp = GetEyeBestTrack();
         if( (*itemp == -1) || ((acteyes[*itemp].GetFdRecShower()).GetEnergy() == 0) )
         {
            cout << "Error! The selected eye has no valid reconstructions." << endl;
            if(goodrec) recfail[2]++;
            goodrec = false;
         }
         else
         {
            // Write out for measured value
            sig[0]->xmax = (acteyes[*itemp].GetFdRecShower()).GetXmax();
            sig[0]->x0 = (acteyes[*itemp].GetFdRecShower()).GetX0();
            sig[0]->lambda = (acteyes[*itemp].GetFdRecShower()).GetLambda();
            sig[0]->fdenergy = (acteyes[*itemp].GetFdRecShower()).GetEnergy();
            sig[0]->zenithFD = (acteyes[*itemp].GetFdRecShower()).GetZenith();
            sig[0]->azimuthFD = (acteyes[*itemp].GetFdRecShower()).GetAzimuth();
            all[0]->xmax = (acteyes[*itemp].GetFdRecShower()).GetXmax();
            all[0]->x0 = (acteyes[*itemp].GetFdRecShower()).GetX0();
            all[0]->lambda = (acteyes[*itemp].GetFdRecShower()).GetLambda();
            all[0]->fdenergy = (acteyes[*itemp].GetFdRecShower()).GetEnergy();
            all[0]->zenithFD = (acteyes[*itemp].GetFdRecShower()).GetZenith();
            all[0]->azimuthFD = (acteyes[*itemp].GetFdRecShower()).GetZenith();

            // Write out for measured value minus error
            sig[1]->xmax = (acteyes[*itemp].GetFdRecShower()).GetXmax() - (acteyes[*itemp].GetFdRecShower()).GetXmaxError();
            sig[1]->x0 = (acteyes[*itemp].GetFdRecShower()).GetX0() - (acteyes[*itemp].GetFdRecShower()).GetX0Error();
            sig[1]->lambda = (acteyes[*itemp].GetFdRecShower()).GetLambda() - (acteyes[*itemp].GetFdRecShower()).GetLambdaError();
            sig[1]->fdenergy = (acteyes[*itemp].GetFdRecShower()).GetEnergy() - (acteyes[*itemp].GetFdRecShower()).GetEnergyError();
            sig[1]->zenithFD = (acteyes[*itemp].GetFdRecShower()).GetZenith() - (acteyes[*itemp].GetFdRecShower()).GetZenithError();
            sig[1]->azimuthFD = (acteyes[*itemp].GetFdRecShower()).GetAzimuth() - (acteyes[*itemp].GetFdRecShower()).GetAzimuthError();
            all[1]->xmax = (acteyes[*itemp].GetFdRecShower()).GetXmax() - (acteyes[*itemp].GetFdRecShower()).GetXmaxError();
            all[1]->x0 = (acteyes[*itemp].GetFdRecShower()).GetX0() - (acteyes[*itemp].GetFdRecShower()).GetX0Error();
            all[1]->lambda = (acteyes[*itemp].GetFdRecShower()).GetLambda() - (acteyes[*itemp].GetFdRecShower()).GetLambdaError();
            all[1]->fdenergy = (acteyes[*itemp].GetFdRecShower()).GetEnergy() - (acteyes[*itemp].GetFdRecShower()).GetEnergyError();
            all[1]->zenithFD = (acteyes[*itemp].GetFdRecShower()).GetZenith() - (acteyes[*itemp].GetFdRecShower()).GetZenithError();
            all[1]->azimuthFD = (acteyes[*itemp].GetFdRecShower()).GetAzimuth() - (acteyes[*itemp].GetFdRecShower()).GetAzimuthError();

            // Write out for measured value plus error
            sig[2]->xmax = (acteyes[*itemp].GetFdRecShower()).GetXmax() + (acteyes[*itemp].GetFdRecShower()).GetXmaxError();
            sig[2]->x0 = (acteyes[*itemp].GetFdRecShower()).GetX0() + (acteyes[*itemp].GetFdRecShower()).GetX0Error();
            sig[2]->lambda = (acteyes[*itemp].GetFdRecShower()).GetLambda() + (acteyes[*itemp].GetFdRecShower()).GetLambdaError();
            sig[2]->fdenergy = (acteyes[*itemp].GetFdRecShower()).GetEnergy() + (acteyes[*itemp].GetFdRecShower()).GetEnergyError();
            sig[2]->zenithFD = (acteyes[*itemp].GetFdRecShower()).GetZenith() + (acteyes[*itemp].GetFdRecShower()).GetZenithError();
            sig[2]->azimuthFD = (acteyes[*itemp].GetFdRecShower()).GetAzimuth() + (acteyes[*itemp].GetFdRecShower()).GetAzimuthError();
            all[2]->xmax = (acteyes[*itemp].GetFdRecShower()).GetXmax() + (acteyes[*itemp].GetFdRecShower()).GetXmaxError();
            all[2]->x0 = (acteyes[*itemp].GetFdRecShower()).GetX0() + (acteyes[*itemp].GetFdRecShower()).GetX0Error();
            all[2]->lambda = (acteyes[*itemp].GetFdRecShower()).GetLambda() + (acteyes[*itemp].GetFdRecShower()).GetLambdaError();
            all[2]->fdenergy = (acteyes[*itemp].GetFdRecShower()).GetEnergy() + (acteyes[*itemp].GetFdRecShower()).GetEnergyError();
            all[2]->zenithFD = (acteyes[*itemp].GetFdRecShower()).GetZenith() + (acteyes[*itemp].GetFdRecShower()).GetZenithError();
            all[2]->azimuthFD = (acteyes[*itemp].GetFdRecShower()).GetAzimuth() + (acteyes[*itemp].GetFdRecShower()).GetAzimuthError();

            if(GetShowerFoot(*itemp, acteyes) == 0)
            {
               // Write out for measured value
               sig[0]->shfoot = shfoot;
               all[0]->shfoot = shfoot;
               // Write out for measured value minus error
               sig[1]->shfoot = shfootmin;
               all[1]->shfoot = shfootmin;
               // Write out for measured value plus error
               sig[2]->shfoot = shfootmax;
               all[2]->shfoot = shfootmax;
            }

            cout << "Values to save: " << endl
                 << "\t- Xmax = " << sig[0]->xmax << " (" << sig[1]->xmax << ", " << sig[2]->xmax << ")" << endl
                 << "\t- X0 = " << sig[0]->x0 << " (" << sig[1]->x0 << ", " << sig[2]->x0 << ")" << endl
                 << "\t- Lambda = " << sig[0]->lambda << " (" << sig[1]->lambda << ", " << sig[2]->lambda << ")" << endl
                 << "\t- FD Energy = " << sig[0]->fdenergy << " (" << sig[1]->fdenergy << ", " << sig[2]->fdenergy << ")" << endl
                 << "\t- Shower foot = " << sig[0]->shfoot << " (" << sig[1]->shfoot << ", " << sig[2]->shfoot << ")" << endl
                 << "\t- FD Zenith angle = " << sig[0]->zenithFD << " (" << sig[1]->zenithFD << ", " << sig[2]->zenithFD << ")" << endl
                 << "\t- FD Azimuth angle = " << sig[0]->azimuthFD << " (" << sig[1]->azimuthFD << ", " << sig[2]->azimuthFD << ")" << endl;
         }
      }

      // Go over the SD tank events
      *sdrecshw = fRecEvent->GetSDEvent().GetSdRecShower();

      if(!(fRecEvent->GetSDEvent().HasTriggeredStations()))
      {
         cout << "Error! No triggered stations in SD reconstruction." << endl;
         if(goodrec) recfail[3]++;
         goodrec = false;
      }

      if(!(fRecEvent->GetSDEvent().HasStations()))
      {
         cout << "Error! No stations in SD reconstruction." << endl;
         if(goodrec) recfail[4]++;
         goodrec = false;
      }

      if(!(fRecEvent->GetSDEvent().HasVEMTraces()))
      {
         cout << "Error! No VEM traces in SD tanks." << endl;
         if(goodrec) recfail[5]++;
         goodrec = false;
      }

      // Write out for measured value
      sig[0]->shwsize = sdrecshw->GetShowerSize();
      sig[0]->sdenergy = sdrecshw->GetEnergy();
      sig[0]->ldfbeta = sdrecshw->GetBeta();
      sig[0]->curvature = sdrecshw->GetCurvature();
      sig[0]->risetime = sdrecshw->GetRiseTimeResults().GetRiseTime1000();
      sig[0]->zenithSD = sdrecshw->GetZenith();
      sig[0]->azimuthSD = sdrecshw->GetAzimuth();
      all[0]->shwsize = sdrecshw->GetShowerSize();
      all[0]->sdenergy = sdrecshw->GetEnergy();
      all[0]->ldfbeta = sdrecshw->GetBeta();
      all[0]->curvature = sdrecshw->GetCurvature();
      all[0]->risetime = sdrecshw->GetRiseTimeResults().GetRiseTime1000();
      all[0]->zenithSD = sdrecshw->GetZenith();
      all[0]->azimuthSD = sdrecshw->GetAzimuth();

      // Write out for measured value minus error
      sig[1]->shwsize = sdrecshw->GetShowerSize() - sdrecshw->GetShowerSizeError();
      sig[1]->sdenergy = sdrecshw->GetEnergy() - sdrecshw->GetEnergyError();
      sig[1]->ldfbeta = sdrecshw->GetBeta() - sdrecshw->GetBetaError();
      sig[1]->curvature = sdrecshw->GetCurvature() - sdrecshw->GetCurvatureError();
      sig[1]->risetime = sdrecshw->GetRiseTimeResults().GetRiseTime1000() - sdrecshw->GetRiseTimeResults().GetRiseTime1000Error();
      sig[1]->zenithSD = sdrecshw->GetZenith() - sdrecshw->GetZenithError();
      sig[1]->azimuthSD = sdrecshw->GetAzimuth() - sdrecshw->GetAzimuthError();
      all[1]->shwsize = sdrecshw->GetShowerSize() - sdrecshw->GetShowerSizeError();
      all[1]->sdenergy = sdrecshw->GetEnergy() - sdrecshw->GetEnergyError();
      all[1]->ldfbeta = sdrecshw->GetBeta() - sdrecshw->GetBetaError();
      all[1]->curvature = sdrecshw->GetCurvature() - sdrecshw->GetCurvatureError();
      all[1]->risetime = sdrecshw->GetRiseTimeResults().GetRiseTime1000() - sdrecshw->GetRiseTimeResults().GetRiseTime1000Error();
      all[1]->zenithSD = sdrecshw->GetZenith() - sdrecshw->GetZenithError();
      all[1]->azimuthSD = sdrecshw->GetAzimuth() - sdrecshw->GetAzimuthError();

      // Write out for measured value plus error
      sig[2]->shwsize = sdrecshw->GetShowerSize() + sdrecshw->GetShowerSizeError();
      sig[2]->sdenergy = sdrecshw->GetEnergy() + sdrecshw->GetEnergyError();
      sig[2]->ldfbeta = sdrecshw->GetBeta() + sdrecshw->GetBetaError();
      sig[2]->curvature = sdrecshw->GetCurvature() + sdrecshw->GetCurvatureError();
      sig[2]->risetime = sdrecshw->GetRiseTimeResults().GetRiseTime1000() + sdrecshw->GetRiseTimeResults().GetRiseTime1000Error();
      sig[2]->zenithSD = sdrecshw->GetZenith() + sdrecshw->GetZenithError();
      sig[2]->azimuthSD = sdrecshw->GetAzimuth() + sdrecshw->GetAzimuthError();
      all[2]->shwsize = sdrecshw->GetShowerSize() + sdrecshw->GetShowerSizeError();
      all[2]->sdenergy = sdrecshw->GetEnergy() + sdrecshw->GetEnergyError();
      all[2]->ldfbeta = sdrecshw->GetBeta() + sdrecshw->GetBetaError();
      all[2]->curvature = sdrecshw->GetCurvature() + sdrecshw->GetCurvatureError();
      all[2]->risetime = sdrecshw->GetRiseTimeResults().GetRiseTime1000() + sdrecshw->GetRiseTimeResults().GetRiseTime1000Error();
      all[2]->zenithSD = sdrecshw->GetZenith() + sdrecshw->GetZenithError();
      all[2]->azimuthSD = sdrecshw->GetAzimuth() + sdrecshw->GetAzimuthError();

      cout << "\t- Shower size (replacement for S1000) = " << sig[0]->shwsize << " (" << sig[1]->shwsize << ", " << sig[2]->shwsize << ")" << endl
           << "\t- SD Energy = " << sig[0]->sdenergy << " (" << sig[1]->sdenergy << ", " << sig[2]->sdenergy << ")" << endl
           << "\t- LDF Beta = " << sig[0]->ldfbeta << " (" << sig[1]->ldfbeta << ", " << sig[2]->ldfbeta << ")" << endl
           << "\t- Curvature R = " << sig[0]->curvature << " (" << sig[1]->curvature << ", " << sig[2]->curvature << ")" << endl
           << "\t- Risetime at 1000m = " << sig[0]->risetime << " (" << sig[1]->risetime << ", " << sig[2]->risetime << ")" << endl
           << "\t- SD Zenith angle = " << sig[0]->zenithSD << " (" << sig[1]->zenithSD << ", " << sig[2]->zenithSD << ")" << endl
           << "\t- SD Azimuth angle = " << sig[0]->azimuthSD << " (" << sig[1]->azimuthSD << ", " << sig[2]->azimuthSD << ")" << endl;

      dtemp[0] = 0;
      dtemp[1] = 0;

      if(GetRisetime(j, sig[0]->risetime, dtemp) == -1)
      {
         cout << "Error: Getting risetime from VEM signal not possible." << endl;
         if(goodrec) recfail[6]++;
         goodrec = false;

         // Write out -1 values, since recalculation failed
         sig[0]->risetimerecalc = -1;
         all[0]->risetimerecalc = -1;
         sig[1]->risetimerecalc = -1;
         all[1]->risetimerecalc = -1;
         sig[2]->risetimerecalc = -1;
         all[2]->risetimerecalc = -1;
      }
      else
      {
         // Write out for measured value
         sig[0]->risetimerecalc = dtemp[0];
         all[0]->risetimerecalc = dtemp[0];
         sig[1]->risetimerecalc = dtemp[0] - dtemp[1];
         all[1]->risetimerecalc = dtemp[0] - dtemp[1];
         sig[2]->risetimerecalc = dtemp[0] + dtemp[1];
         all[2]->risetimerecalc = dtemp[0] + dtemp[1];

         cout << "\t- Risetime (recalculated) = " << sig[0]->risetimerecalc << " (" << sig[1]->risetimerecalc << ", " << sig[2]->risetimerecalc << ")" << endl;

         // Disregard any events, where the error of calculated risetime is above 100% - TODO: must make sure this will not remove good events!
	 if( (dtemp[1])/(dtemp[0]) > 1.0 )
	 {
            cout << "Error! Risetime error above 50%. " << dtemp[1] << endl;
	    if(goodrec) recfail[6]++;
	    goodrec = false;
	 }
      }

      if(sig[0]->risetimerecalc < 0) // TODO: For some reason, some of the Risetime results are -1 -> check why and maybe try to calculate them from actual VEM traces (taken from /data0/gkukec/Programi/offline-trunk/v3r3/Modules/SdReconstruction/Risetime1000LLL/Risetime1000LLL.cc)
      {
         cout << "Error! Risetime not calculated. " << sdrecshw->GetRiseTimeResults().GetRiseTime1000() << endl;
         if(goodrec) recfail[6]++;
         goodrec = false;
      }

      dtemp[0] = 0;
      dtemp[1] = 0;

      cout << "Charge = " << ((fRecEvent->GetSDEvent()).GetStation(0)->GetPMTTraces(ETraceType::eTotalTrace, 1)).GetCharge() << endl;
      // Get the Area-over-Peak values
      if(GetAoP(j, dtemp) == -1)
      {
         cout << "Error: Getting area-over-peak from station PMTs not possible." << endl;
         if(goodrec) recfail[7]++;
         goodrec = false;
      }
      else
      {
         // Write out for measured value
         sig[0]->aop = dtemp[0];
         all[0]->aop = dtemp[0];
         // Write out for measured value minus error
         sig[1]->aop = dtemp[0] - dtemp[1];
         all[1]->aop = dtemp[0] - dtemp[1];
         // Write out for measured value plus error
         sig[2]->aop = dtemp[0] + dtemp[1];
         all[2]->aop = dtemp[0] + dtemp[1];

         cout << "\t- AoP = " << sig[0]->aop << " (" << sig[1]->aop << ", " << sig[2]->aop << ")" << endl;
      }

      // Go over the simulated events (Muon number at ground level) - only if we have simulations!
      *genshw = fRecEvent->GetGenShower();
      sig[0]->nrmu = genshw->GetMuonNumber();
      all[0]->nrmu = genshw->GetMuonNumber();
    
      sig[1]->nrmu = genshw->GetMuonNumber();
      all[1]->nrmu = genshw->GetMuonNumber();

      sig[2]->nrmu = genshw->GetMuonNumber();
      all[2]->nrmu = genshw->GetMuonNumber();
      cout << "\t- Nr. of muons = " << sig[0]->nrmu << " (" << sig[1]->nrmu << ", " << sig[2]->nrmu << ")" << endl;

//#if OFFVER == 1
      // Go over simulation reconstruction (Muon number at ground level) - only if we have PAO data!
//      *unishw = fRecEvent->GetSDEvent().GetUnivRecShower();
//      sig.xmaxmu = genshw->GetXmaxMu();
//      back.xmaxmu = genshw->GetXmaxMu();
//      cout << "\t- Muon Xmax = " << sig.xmaxmu << endl;

//      // In case we have PAO data, calculate the number of muons at ground level
//      cout << "### Checking for muons from PAO data ###" << endl;
//      vector<SdRecStation> stationVector = fRecEvent->GetSDEvent().GetStationVector();
//      for(int i = 0; i < stationVector.size(); i++)
//      {
//         // Only use stations that are valid candidates
//         if( (stationVector[i].IsCandidate()) )
//         {
//            cout << "station ID = " << stationVector[i].GetId() << ", muon component = " << stationVector[i].GetMuonComponent() << ", muon signal = " << stationVector[i].GetMuonSignal() << ", electron signal = " << stationVector[i].GetElectronSignal() << ", photon signal = " << stationVector[i].GetPhotonSignal() << endl;
//         }
//      }
//#endif

      if(goodrec)
      {
// NEW
         sig_tree->Fill();
	 all_tree->Fill();
/*         sig_tree[0]->Fill();
         all_tree[0]->Fill();

         sig_tree[1]->Fill();
         all_tree[1]->Fill();

         sig_tree[2]->Fill();
         all_tree[2]->Fill();*/
      }

      if(sampling && !goodrec)
         nrall++;	// TODO sampling still not working correctly
   }

   cout << recfail[0]+recfail[1]+recfail[2]+recfail[3]+recfail[4]+recfail[5]+recfail[6]+recfail[7] << " events have been removed:" << endl;
   cout << " - No reconstructed FD eyes:      " << recfail[0] << " events" << endl;
   cout << " - Not a hybrid event:            " << recfail[1] << " events" << endl;
   cout << " - No valid FD reconstructions:   " << recfail[2] << " events" << endl;
   cout << " - No triggered SD stations:      " << recfail[3] << " events" << endl;
   cout << " - No reconstructed SD stations:  " << recfail[4] << " events" << endl;
   cout << " - No reconstructed VEM traces:   " << recfail[5] << " events" << endl;
   cout << " - No reconstructed risetime:     " << recfail[6] << " events" << endl;
   cout << " - No Area-over-Peak calculation: " << recfail[7] << " events" << endl;

// NEW
   sig_tree->Write();
/*   sig_tree[0]->Write();
   sig_tree[1]->Write();
   sig_tree[2]->Write();*/
   fFile->Close();

   delete fFile;
   delete itemp;
}

int AdstMva::GetShowerFoot(int longestEye, vector<FDEvent> fdevt)
{
   double *x, *xerr;
   int *itemp;
   double *dtemp1, *dtemp2, *dtemp3;

   x = new double;
   xerr = new double;
   itemp = new int;
   dtemp1 = new double;
   dtemp2 = new double;
   dtemp3 = new double;

   *x = 0;
   *xerr = 0;

   vector<double> slantDepth;
   vector<double> profiledEdX;
   vector<double> profiledEdXerr;

   slantDepth = (acteyes[longestEye].GetFdRecShower()).GetDepth();
   profiledEdX = (acteyes[longestEye].GetFdRecShower()).GetEnergyDeposit();
   profiledEdXerr = (acteyes[longestEye].GetFdRecShower()).GetEnergyDepositError();

   vector<double> xfoot;
   vector<double> yfoot;
   vector<double> yerrfoot;

   if( xfoot.size() != 0 )
      xfoot.erase(xfoot.begin(),xfoot.end());
   if( yfoot.size() != 0 )
      yfoot.erase(yfoot.begin(),yfoot.end());
   if( yerrfoot.size() != 0 )
      yerrfoot.erase(yerrfoot.begin(),yerrfoot.end());

   for(int i = 0; i < slantDepth.size(); i++)
   {
      *x += profiledEdX[i];
      *xerr += profiledEdXerr[i];
   
      xfoot.push_back(slantDepth[i]);
      yfoot.push_back(*x);
      yerrfoot.push_back(*xerr);

//      cout << xfoot[i] << "\t" << yfoot[i] << "\t" << yerrfoot[i] << endl;
   }
   cout << endl;

   *itemp = 0;

   for(int i = 0; i < yfoot.size(); i++)
   {
      if( (yfoot[i] >= shfootlimit*(yfoot[yfoot.size()-1])) && (*itemp == 0) )
      {
         *itemp = 1;

         // Find the x value of point with y value that is a fraction of the maximum, that lies on a line between two points
         // y = k*x + a
         //    k = (y2 - y1)/(x2 - x1)
         //    a = y2 - (y2 - y1)/(x2 - x1)*x2
         // x = ((x2 - x1)/(y2 - y1))*(y - y2) + x2
//       cout << "Izr.1a: P1 = (" << xfoot[i-1] << "," << yfoot[i-1] << "), P2 = (" << xfoot[i] << "," << yfoot[i] << ")" << endl;

         *dtemp1 = (xfoot[i] - xfoot[i-1])/(yfoot[i] - yfoot[i-1]); // 1/k = (x2 - x1)/(y2 - y1)
         *dtemp2 = shfootlimit*(yfoot[yfoot.size()-1]) - yfoot[i]; // y - y2
         *x = (*dtemp1)*(*dtemp2) + xfoot[i]; // x = (1/k)*(y - y2) + x2

//       cout << "Izr.1b: 1/k = " << *dtemp1 << ", max = " << yfoot[yfoot.size()-1] << ", y - y2 = " << *dtemp2 << ", x = " << *x << endl;
//       cout << "Izr.2a: P1err = (" << xfoot[i-1] << "," << yerrfoot[i-1] << "), P2err = (" << xfoot[i] << "," << yerrfoot[i] << ")" << endl;

         *dtemp1 = (xfoot[i] - xfoot[i-1])/(yfoot[i]+yerrfoot[i] - (yfoot[i-1]+yerrfoot[i-1])); // 1/kerr = (x2 - x1)/(y2err - y1err)
         *dtemp2 = (yfoot[i]+yerrfoot[i]) - (1/(*dtemp1))*(xfoot[i]); // aerr = y2err - (y2err - y1err)/(x2 - x1)*x2
         *dtemp3 = (1/(*dtemp1))*(*x) + (*dtemp2); // yerr = kerr*x + aerr
         *xerr = (*dtemp3) - (((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(*x) + (yfoot[i] - ((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(xfoot[i]))); // Dy = yerr - y

//       cout << "Izr.2b: 1/kerr = " << *dtemp1 << ", a = " << *dtemp2 << ", yerr = " << *dtemp3 << ", Dy = " << *xerr << endl;

         *dtemp1 = ((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(*x) + (yfoot[i] - ((yfoot[i] - yfoot[i-1])/(xfoot[i] - xfoot[i-1]))*(xfoot[i])); // y = k*x + a

         for(int j = i; ; j++)
         {
            if( yfoot[j] >= (*dtemp1)+(*xerr) )
            {
               *dtemp2 = xfoot[j];
//             cout << "Upper limit is = " << xfoot[j] << endl;
               break;
            }
         }

         for(int j = i; ; j--)
         {
            if(j == 0)
            {
               *dtemp3 = xfoot[j];
//             cout << "Lower limit is = " << xfoot[j] << endl;
               break;
            }

            if( yfoot[j] <= (*dtemp1)-(*xerr) )
            {
               *dtemp3 = xfoot[j];
//             cout << "Lower limit is = " << xfoot[j] << endl;
               break;
            }
         }
//       cout << "Izr.2c: x = " << *x << ", y = " << *dtemp1 << ", x+Dx = " << *dtemp2 << ", x-Dx = " << *dtemp3 << ", Dx+ = " << (*dtemp2)-(*x) << ", Dx- = " << (*x)-(*dtemp3) << endl;

         shfoot = *x;
         shfootmin = *dtemp3;
         shfootmax = *dtemp2;
      }
   }

   delete x;
   delete xerr;
   delete itemp;
   delete dtemp1;
   delete dtemp2;
   delete dtemp3;

   return 0;
}

// TODO: Check what to edit!
int AdstMva::GetRisetime(int event, double inRisetime, double *outRisetime)
{
   if(!(fRecEvent->GetSDEvent().HasVEMTraces()))
      return -1;

   cout << "# Entering function AdstMva::GetRisetime()..." << endl;
   double *x;
   double *y;
   double *maxval;
   int *nrpoints;
   double *dtemp;
   int *itemp;
   int start_bin, stop_bin;
   string stemp;

   double byrange[2];
   double bzrange[2];
   byrange[0] = 1.e+40;
   byrange[1] = -1.e+40;
   bzrange[0] = 1.e+40;
   bzrange[1] = -1.e+40;

   x = new double;
   y = new double;
   maxval = new double;
   nrpoints = new int;
   dtemp = new double[2];
   itemp = new int[2];

   vector<SdRecStation> stationVector = fRecEvent->GetSDEvent().GetStationVector();
   cout << "Number of triggered stations: " << stationVector.size() << ", Zenith angle = " << fRecEvent->GetSDEvent().GetSdRecShower().GetZenith() << ", Cos Zenith angle = " << fRecEvent->GetSDEvent().GetSdRecShower().GetCosZenith() << ", Energy = " << fRecEvent->GetSDEvent().GetSdRecShower().GetEnergy() << endl;

   vector<float> time;
   vector<float> vemtrace;

   vector<float> yvalue;

   vector<double> riseVect;
   vector<double> riseVectErr;
   vector<double> distVect;
   vector<double> distVectErr;

   dtemp[0] = 0;
   itemp[0] = 0;

   // Check all stations
   for(int i = 0; i < stationVector.size(); i++)
   {
      // Only use stations that are valid candidates
      if( (stationVector[i].IsCandidate()) /*&& (stationVector[i].GetSPDistance() < 1500.)*/ )
      {
         start_bin = stationVector[i].GetSignalStartSlot() - 4;
         stop_bin = stationVector[i].GetSignalEndSlot();

         if( (start_bin >= stop_bin) || (start_bin < 0) || (start_bin > 5000) ) start_bin = 0;

//         cout << "Tank " << i << " is a candidate (" << start_bin << "," << stop_bin << ")." << endl;

         dtemp[1] = 0;
         itemp[1] = 0;

         // Check all PMTs
         for(int j = 1; j <= 3; j++)
         {
            if(time.size() != 0)
               time.erase(time.begin(), time.end());
            if(yvalue.size() != 0)
               yvalue.erase(yvalue.begin(), yvalue.end());
            *y = 0;
            *maxval = -1.e40;

            vemtrace = stationVector[i].GetVEMTrace(j);
            *nrpoints = vemtrace.size();
//          cout << "PMT " << j << ": Number of points in the VEM trace: " << *nrpoints << " --------------------------------------------------------" << endl;

            // Continue if there is a VEM trace
            if( *nrpoints > 0 )
            {
               itemp[0]++;
               itemp[1]++;

               // Prepare the time vector (each point is multiplied by 25 to get nanoseconds)
               for(int k = 0; k < *nrpoints; k++)
               {
                  if( (k >= start_bin) && (k <= stop_bin) )
                  {
                     time.push_back((float)k*25.);

                     *y += vemtrace[k];
                     if(*y > *maxval) *maxval = *y;
                  
                     yvalue.push_back(*y);
                  }
               }

//             cout << "Number of points in the signal slot: " << yvalue.size() << endl;

               for(int k = 0; k < yvalue.size(); k++)
               {
//                cout << time[k]/25. << "\t" << yvalue[k]/(*maxval) << endl;

                  if(yvalue[k]/(*maxval) <= 0.10)
                  {
                     byrange[0] = yvalue[k]/(*maxval);
                     byrange[1] = yvalue[k+1]/(*maxval);

                     *y = 0.1;
                     // Find the x value of point with y value = *y = 0.1, that lies on a line between two points
                     // y = k*x + a
                     //    k = (y2 - y1)/(x2 - x1)
                     //    a = y2 - (y2 - y1)/(x2 - x1)*x2
                     // x = ((x2 - x1)/(y2 - y1))*(y - y2) + x2
                     *x = ((time[k+1] - time[k])*((*y) - byrange[1]))/(byrange[1] - byrange[0]) + time[k+1];

                     byrange[0] = *x;
                     byrange[1] = *y;
                  }

                  if(yvalue[k]/(*maxval) <= 0.50)
                  {
                     bzrange[0] = yvalue[k]/(*maxval);
                     bzrange[1] = yvalue[k+1]/(*maxval);

                     *y = 0.5;
                     // Find the x value of point with y value = *y = 0.5, that lies on a line between two points
                     // y = k*x + a
                     //    k = (y2 - y1)/(x2 - x1)
                     //    a = y2 - (y2 - y1)/(x2 - x1)*x2
                     // x = ((x2 - x1)/(y2 - y1))*(y - y2) + x2
                     *x = ((time[k+1] - time[k])*((*y) - bzrange[1]))/(bzrange[1] - bzrange[0]) + time[k+1];

                     bzrange[0] = *x;
                     bzrange[1] = *y;
                  }
               }

//             cout << "Reconstructed risetime = " << inRisetime << ", calculated risetime (" << byrange[0]/25. << "," << bzrange[0]/25. << ") = " << bzrange[0] - byrange[0] << endl;
               dtemp[0] += bzrange[0] - byrange[0];
               dtemp[1] += bzrange[0] - byrange[0];
            }
         }

         dtemp[1] = dtemp[1]/itemp[1];
         cout << "Tank " << stationVector[i].GetId() << ", " << stationVector[i].GetSPDistance() << " m: Calculated average risetime (for " << itemp[1] << " PMTs in the tank) = " << dtemp[1] << ", Total signal = " << stationVector[i].GetTotalSignal() << endl;

         // Asymmetry correction
         double eventThetaRec = fRecEvent->GetSDEvent().GetSdRecShower().GetZenith();
         double secZenith = 1/cos(eventThetaRec);
         const double alpha = 96.73 + secZenith*(-282.40 + secZenith*(241.80 - 62.61*secZenith));
         const double gamma = -0.0009572 + secZenith*(0.002068 + secZenith*(-0.001362 + 0.0002861*secZenith));
         const double g = alpha + gamma * stationVector[i].GetSPDistance()*stationVector[i].GetSPDistance();
         const double zeta = stationVector[i].GetAzimuthSP();

         outRisetime[0] = dtemp[1] - g*cos(zeta);
	 outRisetime[1] = fRTWeights->Eval(stationVector[i].GetSPDistance(), secZenith, stationVector[i].GetTotalSignal());
         cout << "Asymmetry corrected risetime = " << outRisetime[0] << " (+- " << outRisetime[1] << "), from actual data = " << stationVector[i].GetAssymCorrRiseTime(eventThetaRec) << " (+- " << stationVector[i].GetAssymCorrRiseTimeError(eventThetaRec) << ")" << endl;

         if( (stationVector[i].GetTotalSignal() > minSignal) )
	 {
	    if( (!stationVector[i].IsLowGainSaturated()) && (!includeSaturated) )
	    {
	       if( (stationVector[i].GetSPDistance() >= limitTankDistance[0]) && (stationVector[i].GetSPDistance() <= limitTankDistance[1]) )
	       {
                  riseVect.push_back(outRisetime[0]);
//                  riseVect.push_back(stationVector[i].GetAssymCorrRiseTime(eventThetaRec));
                  distVect.push_back(stationVector[i].GetSPDistance()/1000.);
                  riseVectErr.push_back(outRisetime[1]);
//                  riseVectErr.push_back(stationVector[i].GetAssymCorrRiseTimeError(eventThetaRec));
                  distVectErr.push_back(stationVector[i].GetSPDistanceError()/1000.);
	       }
	       else
                  cout << "Rejected: Station distance " << distVect[i] << " is outside the limits (" << limitTankDistance[0] << ", " << limitTankDistance[1] << ")." << endl;
	    }
	    else
               cout << "Rejected: Station signal is low gain saturated." << endl;
	 }
	 else
            cout << "Rejected: Station signal " << stationVector[i].GetTotalSignal() << " is below the minimum accepted (" << minSignal << ")." << endl;
      }
   }

/*   int z = 0;
   for(int i = 0; i < riseVect.size(); i++)
      if( (distVect[i] >= limitTankDistance[0]) && (distVect[i] <= limitTankDistance[1]) )
         z++;*/

/*   TCanvas *c1;
   if(event < 20)
      c1 = new TCanvas("c1","c1",1200,900);
*/
   if(riseVect.size() >= minPoints)
   {
      TGraphErrors riseGraph(riseVect.size(), &distVect.front(), &riseVect.front(), 0, &riseVectErr.front());
      TF1 risetimeFit("RisetimeFit", "40+[0]*x+[1]*x*x", limitTankDistance[0]/1000., limitTankDistance[1]/1000.);
      risetimeFit.SetParLimits(0, 0, 10000);
      risetimeFit.SetParLimits(1, 0, 10000);
      RiseTimeFunction(fRecEvent->GetSDEvent().GetSdRecShower().GetZenith(), fRecEvent->GetSDEvent().GetSdRecShower().GetEnergy(), &risetimeFit);
      int ret = riseGraph.Fit(&risetimeFit, "Q", "", limitTankDistance[0]/1000., limitTankDistance[1]/1000.);
      if(!ret)
      {
         outRisetime[0] = risetimeFit.Eval(evalDistance/1000.);
         TVirtualFitter* const fitter = TVirtualFitter::GetFitter();
         const int nPar = fitter->GetNumberTotalParameters();
         const TMatrixD covar(nPar, nPar, fitter->GetCovarianceMatrix());

         outRisetime[1] = sqrt(covar[1][1] + covar[0][0] + 2*covar[0][1]);
      }
      else
         return -1;

      cout << "Reconstructed risetime = " << inRisetime << ", Calculated average risetime (for " << itemp[0] << " PMTs in all tanks, " << riseVect.size() << " fitting points) = " << outRisetime[0] << " (+- " << outRisetime[1] << ")" << endl;
   }
   else
   {
      cout << "Rejected: Only " << riseVect.size() << " valid tanks." << endl;
      outRisetime[0] = -1;
      outRisetime[1] = 0;
      return -1;
   }

/*   if(event < 20)
   {
      c1->SetGrid();
      c1->cd();
      riseGraph.SetMarkerSize(1);
      riseGraph.SetMarkerStyle(20);
      riseGraph.Draw("AP");
      risetimeFit.Draw("same");

      TLine *line = new TLine(limitTankDistance[0], inRisetime, 1000., inRisetime);
      line->SetLineWidth(2);
      line->SetLineStyle(7);
      line->SetLineColor(kBlue+2);
      line->Draw("same");

      line = new TLine(limitTankDistance[0], *outRisetime, 1000., *outRisetime);
      line->SetLineWidth(2);
      line->SetLineStyle(7);
      line->SetLineColor(kOrange+2);
      line->Draw("same");

      riseGraph.GetXaxis()->SetRange(0., 2300.);
      riseGraph.GetXaxis()->SetRangeUser(0., 2300.);
      riseGraph.GetXaxis()->SetLimits(0., 2300.);
      riseGraph.GetYaxis()->SetRange(0., 500.);
      riseGraph.GetYaxis()->SetRangeUser(0., 500.);
      riseGraph.GetYaxis()->SetLimits(0., 500.);
      riseGraph.SetTitle(";Distance from shower axis (m);Risetime of SD tanks (ns)");
      riseGraph.GetXaxis()->SetTitleOffset(1.2);
      riseGraph.GetYaxis()->SetTitleOffset(1.2);

      gPad->Update();

      if(event < 10)
         stemp = string(BASEDIR) + "/results/risetime_graph_0" + IntToStr(event) + ".pdf";
      else
         stemp = string(BASEDIR) + "/results/risetime_graph_" + IntToStr(event) + ".pdf";
      c1->SaveAs(stemp.c_str());
      delete c1;
   }*/

   delete x;
   delete y;
   delete nrpoints;
   delete maxval;
   delete[] dtemp;
   delete[] itemp;

   return 0;
}

// Calculate the area over peak value
int AdstMva::GetAoP(int event, double *outAop)
{
//   if(!(fRecEvent->GetSDEvent().HasVEMTraces()))
//      return -1;

   cout << "# Entering function AdstMva::GetAoP()..." << endl;

   double *chpeak, *charge, *peak;
   chpeak = new double[2];
   charge = new double[2];
   peak = new double[2];

   double *midAop;
   midAop = new double[2];

   int nrpmt, nrstat = 0;

   outAop[0] = 0;
   outAop[1] = 0;

   vector<SdRecStation> stationVector = fRecEvent->GetSDEvent().GetStationVector();
//   cout << "Event " << event << ", " << fRecEvent->GetEventId() << endl;
//   cout << "Number of triggered stations: " << stationVector.size() << ", Zenith angle = " << fRecEvent->GetSDEvent().GetSdRecShower().GetZenith() << ", Cos Zenith angle = " << fRecEvent->GetSDEvent().GetSdRecShower().GetCosZenith() << ", Energy = " << fRecEvent->GetSDEvent().GetSdRecShower().GetEnergy() << endl;

   // Check all stations
   for(int i = 0; i < stationVector.size(); i++)
   {
      charge[0] = 0;
      charge[1] = 0;
      peak[0] = 0;
      peak[1] = 0;
      chpeak[0] = 0;
      chpeak[1] = 0;
      nrpmt = 0;

      // Only use stations that are valid candidates
      if( stationVector[i].IsCandidate() )
      {
         for(int j = 1; j <= 3; j++)
	 {
	    charge[0] = stationVector[i].GetCharge(j);
	    charge[1] = stationVector[i].GetChargeError(j);
//	    cout << " ### " << j << ": " << charge[0] << ", " << charge[1] << endl;
	    peak[0] = stationVector[i].GetPeak(j);
	    peak[1] = stationVector[i].GetPeakError(j);
//	    cout << " ### " << j << ": " << peak[0] << ", " << peak[1] << endl;
	    
            if( (charge[0] != 0) && (peak[0] != 0) )
	    {
	       chpeak[0] += charge[0]/peak[0];
	       chpeak[1] += (charge[0]/peak[0])*TMath::Sqrt( (charge[1]*charge[1])/(charge[0]*charge[0]) + (peak[1]*peak[1])/(peak[0]*peak[0]) );
//	       cout << " ### " << j << ": " << charge[0]/peak[0] << ", " << (charge[0]/peak[0])*TMath::Sqrt( (charge[1]*charge[1])/(charge[0]*charge[0]) + (peak[1]*peak[1])/(peak[0]*peak[0]) ) << endl;
	       nrpmt++;
	    }
	 }

         if(nrpmt > 0)
	 {
	    midAop[0] = chpeak[0]/nrpmt;
	    midAop[1] = chpeak[1]/nrpmt;

//	    cout << "The AoP for " << nrpmt << " PMTs is = " << midAop[0] << " (" << midAop[1] << ")" << endl;

	    outAop[0] += midAop[0];
	    outAop[1] += midAop[1];

	    nrstat++;
	 }
      }
   }

   if(nrstat > 0)
   {
      outAop[0] = outAop[0]/nrstat;
      outAop[1] = outAop[1]/nrstat;

      delete[] chpeak;
      delete[] charge;
      delete[] peak;
      delete[] midAop;

      return 0;
   }
   else
   {
      delete[] chpeak;
      delete[] charge;
      delete[] peak;
      delete[] midAop;

      return -1;
   }
}

void AdstMva::RiseTimeFunction(double zenith, double energy, TF1 *risetimeFit)
{
  //Karen Mora paremeterisation:
  const double secTheta= 1./cos(zenith); 
  const double a_par1= -0.141152; 
  const double a_par2=0.0141074;   
  const double a_par3=1.25107;  
  const double a_par4=-0.405333;   
  
  const double b_par1=0.000904323;   
  const double b_par2=6.4291e-06; 
  const double b_par3=-1.09992; 
  const double b_par4=0.30987;  
  
  double alpha=(a_par1+a_par2*log10(energy))*exp(-0.5*pow((secTheta-a_par3)/a_par4, 2));
  double beta=(b_par1+b_par2*log10(energy))*(1+b_par3* secTheta+b_par4*pow(secTheta, 2));
  
  risetimeFit->SetParameter(0,alpha);
  risetimeFit->SetParameter(1,beta);
}

int AdstMva::SelectEvents(string config, vector<string> infiles)
{
   TTree::SetMaxTreeSize(200000000000LL);
   //TODO
   RecEventFile cutinFile(infiles);
cout << "INSIDE TODO 1" << endl;
   RecEventFile* outputFile = new RecEventFile(outname.c_str(), RecEventFile::eWrite);
cout << "INSIDE TODO 2" << endl;

/*   switch(eAdstMode)
   {
      case eFull:
         break;
      case eMini:
	 fFile.SetMiniAdst();
	 outputFile->SetMiniAdst();
         break;
      case eMicro:
	 fFile.SetMicroAdst();
	 outputFile->SetMicroAdst();
         break;
   }*/
   
   EAdstMode gAdstMode = eFull;
   gNminusOne = true;
   gMinNFDEyes = 1;
   gVerbosity = 10;

   cutinFile.SetBuffers(&fRecEvent);
   cutinFile.ReadDetectorGeometry(*fDetGeo);
   outputFile->SetBuffers(&fRecEvent);
cout << "INSIDE TODO 3" << endl;

#if OFFVER == 0
   Analysis analysis(&fDetGeo, &fRecEvent, config, gNminusOne);
#elif OFFVER == 1
   Analysis analysis(&fDetGeo, &fRecEvent, config, gNminusOne, "SelectEvents", gVerbosity);
   if (gVerbosity >= 10)
   {
      cout << "\nDump of the Analysis:\n";
      analysis.Dump(cout);
      cout << endl;
   }
cout << "INSIDE TODO 4" << endl;

  string type = string(analysis.HasFDSelection() ? " FD" : "") + string(analysis.HasSDSelection() ? " SD" : "") + string(analysis.HasSimSelection() ? " Sim" : "");
  if (type.empty())
    type = " WARNING, no selection found! (pass-through mode)";
  cout << endl << " Analysis:" << type << endl << endl;
#endif

   const int nEvents = cutinFile.GetNEvents();
   const int fivePercent = int(0.05*nEvents);
cout << "INSIDE TODO 5" << endl;

   while(cutinFile.ReadNextEvent() == RecEventFile::eSuccess )
   {
      const int currentEventNumber = cutinFile.GetCurrentEvent();
      if( fivePercent && currentEventNumber && !(currentEventNumber % fivePercent) )
         cout << cutinFile.GetCurrentEventString() << endl;
cout << "INSIDE TODO 6" << endl;

      if(analysis.HasSDSelection() && !analysis.IsGoodSD())
         continue;
cout << "INSIDE TODO 7" << endl;

      if(analysis.HasSimSelection() && !analysis.IsGoodSim())
         continue;
cout << "INSIDE TODO 8" << endl;

      if(analysis.HasFDSelection())
      {
         for(RecEvent::EyeIterator iEye = fRecEvent->EyesBegin(); iEye != fRecEvent->EyesEnd(); )
	 {
            if(!analysis.IsGoodEye(iEye->GetEyeId()))
	       fRecEvent->GetFDEvents().erase(iEye);
	    else
               ++iEye;
	 }

	 if(fRecEvent->GetFDEvents().size() < gMinNFDEyes)
            continue;
      }
cout << "INSIDE TODO 9" << endl;

      outputFile->WriteEvent();
cout << "INSIDE TODO 10" << endl;
   }

   cout << endl << " Total events:    " << cutinFile.GetNEvents() << endl
	        << " Selected events: " << outputFile->GetNEvents() << endl;

   analysis.PrintCutStatistics(false);

   FileInfo theInfo;
   cutinFile.ReadFileInfo(theInfo);
   outputFile->WriteDetectorGeometry(*fDetGeo);
   outputFile->WriteFileInfo(theInfo);
   outputFile->cd();
   if(gNminusOne)
      analysis.WriteNMinusOne();
   analysis.WriteCutStatistics();
   outputFile->Close();

   return 0;
}
