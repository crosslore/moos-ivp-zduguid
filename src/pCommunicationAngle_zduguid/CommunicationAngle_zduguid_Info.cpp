/****************************************************************/
/*   NAME: Zach Duguid                                          */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: CommunicationAngle_zduguid_Info.cpp                  */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "CommunicationAngle_zduguid_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pCommunicationAngle_zduguid application is used for       ");
  blk("  computing the vehicle position and elevation angle of the     ");
  blk("  sonar system in order to maintain communications between AUVs ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pCommunicationAngle_zduguid file.moos [OPTIONS]          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pCommunicationAngle_zduguid with the given process name");
  blk("      rather than pCommunicationAngle_zduguid.                  ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pCommunicationAngle_zduguid.");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pCommunicationAngle_zduguid Example MOOS Configuration          ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pCommunicationAngle_zduguid                     ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pCommunicationAngle_zduguid INTERFACE                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  VEHICLE_NAME                                                  ");
  blk("  COLLABORATOR_NAME                                             ");
  blk("  NAV_X                                                         ");
  blk("  NAV_Y                                                         ");
  blk("  NAV_DEPTH                                                     ");
  blk("  'collaborator'_NAV_X                                          ");
  blk("  'collaborator'_NAV_Y                                          ");
  blk("  'collaborator'_DEPTH                                          ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  ACOUSTIC_PATH                                                 ");
  blk("  CONNECTIVITY_LOCATION                                         ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pCommunicationAngle_zduguid", "gpl");
  exit(0);
}

