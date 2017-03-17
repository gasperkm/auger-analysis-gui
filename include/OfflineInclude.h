#ifndef __OFFLINE_INCLUDE_H__
#define __OFFLINE_INCLUDE_H__

#define OFFVER 0

// From ADST old
#if OFFVER == 0
   #include "AsciiConverter.h"
   #include "AsciiFDConverter.h"
   #include "AsciiHybridConverter.h"
   #include "AsciiHybridOnlineConverter.h"
   #include "T2TriggerProb.h"

   #include "ACColumnEnergyDeposit.h"
   #include "ACColumn.h"
   #include "ACColumnTypes.h"
   #include "ACFDColumn.h"
   #include "ACGlobalColumn.h"
   #include "AnalysisConsts.h"
   #include "Analysis.h"
   #include "AsciiSDConverter.h"
   #include "AugerUpTime.h"
   #include "CutFileParser.h"
   #include "CutFileParserV1.h"
   #include "Cut.h"
   #include "CutSpec.h"
   #include "DetectorGeometry.h"
   #include "Detector.h"
   #include "EventInfo.h"
   #include "EyeGeometry.h"
   #include "FdAerosols.h"
   #include "FdApertureLight.h"
   #include "FdCloudCameraData.h"
   #include "FDEvent.h"
   #include "FdGenApertureLight.h"
   #include "FdGenGeometry.h"
   #include "FdGenShower.h"
   #include "FdGeometry.h"
   #include "FdLidarData.h"
   #include "FdMultiTrace.h"
   #include "FdRecApertureLight.h"
   #include "FdRecGeometry.h"
   #include "FdRecLevel.h"
   #include "FdRecPixel.h"
   #include "FdRecShower.h"
   #include "FdRecStation.h"
   #include "FDSelection.h"
   #include "FdTelescopeData.h"
   #include "FdTrace.h"
   #include "FileInfo.h"
   #include "GenShower.h"
   #include "GenStation.h"
//   #include "GetTriggerProbRealStations.h"
   #include "LDF.h"
   #include "MonthlyAverageAtmosphere.h"
   #include "MuonMap.h"
   #include "ParticleType.h"
   #include "RecEventFile.h"
   #include "RecEvent.h"
   #include "RecEventVersion.h"
   #include "RecShower.h"
   #include "RecStation.h"
   #include "RiseTime.h"
   #include "SdBadStation.h"
   #include "SDEvent.h"
   #include "SdFootprintData.h"
   #include "SdFootprintEnumerations.h"
   #include "SdRecGeometry.h"
   #include "SdRecLevel.h"
   #include "SdRecShower.h"
   #include "SdRecStation.h"
   #include "SdRiseTime.h"
   #include "SDSelection.h"
   #include "SdTriggerType.h"
   #include "Selection.h"
   #include "Shower.h"
   #include "SimInfo.h"
   #include "SimSelection.h"
   #include "StationStatus.h"
   #include "TelescopeGeometry.h"
   #include "Timing.h"
   #include "Traces.h"
   #include "TraceType.h"
   #include "UtilityFunctions.h"
#endif

// From ADST new
#if OFFVER == 1
   #include "AsciiConverter.h"
   #include "AsciiFDConverter.h"
   #include "AsciiHybridConverter.h"
   #include "AsciiHybridOnlineConverter.h"
   #include "AsciiSDConverter.h"
   #include "T2TriggerProb.h"

   #include "ACColumnEnergyDeposit.h"
   #include "ACColumn.h"
   #include "ACColumnTypes.h"
   #include "ACFDColumn.h"
   #include "ACGlobalColumn.h"
   #include "AnalysisConsts.h"
   #include "Analysis.h"
   #include "AugerUpTime.h"
   #include "ChannelRRecDataQuantities.h"
   #include "CutFileParser.h"
   #include "CutFileParserV1.h"
   #include "Cut.h"
   #include "CutSpec.h"
   #include "DetectorGeometry.h"
   #include "Detector.h"
   #include "EventInfo.h"
   #include "EyeGeometry.h"
   #include "FdAerosols.h"
   #include "FdApertureLight.h"
   #include "FdCloudCameraData.h"
   #include "FDEvent.h"
   #include "FdGenApertureLight.h"
   #include "FdGenGeometry.h"
   #include "FdGenShower.h"
   #include "FdGeometry.h"
   #include "FdLidarData.h"
   #include "FdMultiTrace.h"
   #include "FdRecApertureLight.h"
   #include "FdRecGeometry.h"
   #include "FdRecLevel.h"
   #include "FdRecPixel.h"
   #include "FdRecShower.h"
   #include "FdRecStation.h"
   #include "FDSelection.h"
   #include "FdTelescopeData.h"
   #include "FdTrace.h"
   #include "FileInfo.h"
   #include "GenShower.h"
   #include "GenStation.h"
//   #include "GetTriggerProbRealStations.h"
   #include "LDF.h"
   #include "MDEventADST.h"
   #include "MdRecChannel.h"
   #include "MdRecCounter.h"
   #include "MdRecLevel.h"
   #include "MdRecModule.h"
   #include "MdRecShower.h"
   #include "MdSimCounter.h"
   #include "MdSimScintillator.h"
   #include "MeteoData.h"
   #include "MonthlyAverageAtmosphere.h"
   #include "MuonMap.h"
   #include "ParticleType.h"
   #include "RdBeamPeak.h"
   #include "RdBeamQuants.h"
   #include "RdEvent.h"
   #include "RdLDF.h"
   #include "RdRecChannel.h"
   #include "RdRecChannelParameterStorageMap.h"
   #include "RdRecLevel.h"
   #include "RdRecShower.h"
   #include "RdRecShowerParameterStorageMap.h"
   #include "RdRecStation.h"
   #include "RdRecStationParameterStorageMap.h"
   #include "RdTrace.h"
   #include "RecEventFile.h"
   #include "RecEvent.h"
   #include "RecEventVersion.h"
   #include "RecShower.h"
   #include "RecStation.h"
   #include "RiseTime.h"
   #include "SdBadStation.h"
   #include "SDEvent.h"
   #include "SdFootprintData.h"
   #include "SdFootprintEnumerations.h"
   #include "SdRecGeometry.h"
   #include "SdRecLevel.h"
   #include "SdRecShower.h"
   #include "SdRecStation.h"
   #include "SdRiseTime.h"
   #include "SDSelection.h"
   #include "SdTriggerType.h"
   #include "Selection.h"
   #include "Shower.h"
   #include "ShowerRRecDataQuantities.h"
   #include "SimInfo.h"
   #include "SimRdPulse.h"
   #include "SimSelection.h"
   #include "StationRRecDataQuantities.h"
   #include "StationStatus.h"
   #include "TelescopeGeometry.h"
   #include "Timing.h"
   #include "Traces.h"
   #include "TraceType.h"
   #include "UnivRecShower.h"
   #include "UtilityFunctions.h"
#endif

#endif