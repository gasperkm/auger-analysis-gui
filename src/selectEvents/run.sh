#!/bin/bash

make clean

source simple_setup_ADST.sh
#source /home/gasper/Gasper/Programi/offline-trunk/env_offline.sh

echo ""
echo "Environment ---------------------------"
echo "G4INSTALL: $G4INSTALL"
echo "FFTW3ROOT: $FFTW3ROOT"
echo "G4INCLUDE: $G4INCLUDE"
echo "AERAROOTIO: $AERAROOTIO"
echo "BOOSTROOT: $BOOSTROOT"
echo "CLHEPHOME: $CLHEPHOME"
echo "AIRESHOME: $AIRESHOME"
echo "G4LIB: $G4LIB"
echo "CDASHOME: $CDASHOME"
echo "AERAROOT: $AERAROOT"
echo "EIGENROOT: $EIGENROOT"
echo "FD_ROOT: $FD_ROOT"
echo "XERCESCROOT: $XERCESCROOT"
echo "ROOTSYS: $ROOTSYS"
echo "CPPUNITROOT: $CPPUNITROOT"
echo "G4SYSTEM: $G4SYSTEM"
echo "AUGEROFFLINEROOT: $AUGEROFFLINEROOT"
echo "ADSTROOT: $ADSTROOT"
echo "PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "PATH: $PATH"
echo "---------------------------------------"
echo ""

which EventBrowser

make
./selectEvents -c edited.cfg selected_HECO_data_2014.root

exit 0
