/************************************************************/
/*    NAME: Zach Duguid                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"
#include <cmath>

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  m_current_x = 0;
  m_current_y = 0;
  m_previous_x = 0;
  m_previous_y = 0;
  m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    // handle NAV_X update 
    if (msg.GetKey() == "NAV_X"){
      m_current_x = msg.GetDouble();

    // handle NAV_Y update
    } else if (msg.GetKey() == "NAV_Y") {
      m_current_y = msg.GetDouble();
    }
  }
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // calculate distance traveled between sequential navigation measurements
  double delta_x    = m_current_x - m_previous_x;
  double delta_y    = m_current_y - m_previous_y;
  double delta_dist = hypot(delta_x, delta_y);
  m_total_distance += delta_dist;

  // update our previous fix after odometer has been updated 
  m_previous_x      = m_current_x;
  m_previous_y      = m_current_y;

  // post the updated total distance
  Notify("ODOMETRY_DIST", m_total_distance);

  AppCastingMOOSApp::PostReport(); 
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}


//---------------------------------------------------------
// Procedure: buildReport

bool Odometry::buildReport()
{
  m_msgs << "Total Distance Traveled: " << m_total_distance << " (m)" << endl;
  return(true);
}


//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
}

