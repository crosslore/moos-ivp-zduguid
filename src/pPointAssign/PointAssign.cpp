/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                 */
/*    DATE: March 14th, 2019                                */
/************************************************************/

#include <iterator>
#include <list>
#include <string>
#include <stdio.h>
#include <vector>
#include "MBUtils.h"
#include "ACTable.h"
#include "PointAssign.h"

using namespace std;


//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  m_assign_by_region = false;
  m_vname1           = "GILDA";
  m_vname2           = "HENRY";
  m_vpublish1        = "VISIT_POINT_GILDA";
  m_vpublish2        = "VISIT_POINT_HENRY";
  m_min_x            =  -25;
  m_max_x            =  200;
  m_min_y            = -175;
  m_max_y            =  -25;
  m_mid_x            = (m_min_x + m_max_x) / 2;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();

    // handle incoming points variable 
    if ((key == "VISIT_POINT") && 
        (msg.IsString())){
      m_point_queue.push_back(msg.GetString());

    // ignore outgoing mail that is going to the vehicles
    } else if ((key == m_vpublish1) || (key == m_vpublish2)) {

    // report any unhandled mail
    } else if(key != "APPCAST_REQ") {
      reportRunWarning("Unhandled Mail: " + key);
    }
  }
   return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
   registerVariables();
   return(true);
}


//---------------------------------------------------------
// Procedure: Iterate(), happens AppTick times per second

bool PointAssign::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // minimum size of the point queue for assigning points
  if (m_point_queue.size() > 2) {

    // iterate through the queue of points and assign them accordingly
    STRING_LIST::iterator p;
    for(p=m_point_queue.begin(); p!=m_point_queue.end(); p++) {

      // publish the firstpoint and lastpoint markers when they arise
      if ((*p == "firstpoint") || (*p == "lastpoint")) {
        Notify(m_vpublish1, *p);
        Notify(m_vpublish2, *p);
      } else {
        string original_string      = *p;
        vector<string> point_vector = parseString(*p, ",");

        // assign points into East and West regions 
        if (m_assign_by_region) {
          string x_var    = point_vector.front();
          string x_param  = biteStringX(x_var, '=');
          int    x_coord  = stoi(x_var);
          if (x_coord < m_mid_x) Notify(m_vpublish1, original_string);
          else                   Notify(m_vpublish2, original_string);

        // assign points in alternating fashion based on their id number
        } else {
          string id_var    = point_vector.back();
          string id_param  = biteStringX(id_var, '=');
          int    id_coord  = stoi(id_var);
          if (id_coord % 2 == 0) Notify(m_vpublish1, original_string);
          else                   Notify(m_vpublish2, original_string);
        }
      }
    }
    // clear the queue to avoid assignment for every iterate loop
    m_point_queue.clear();
  }
  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  registerVariables();	
  return(true);
}


//---------------------------------------------------------
// Procedure: registerVariables

void PointAssign::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
  Register("VISIT_POINT_GILDA", 0);
  Register("VISIT_POINT_HENRY", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool PointAssign::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File: PointAssign                            \n";
  m_msgs << "============================================ \n";
  m_msgs << "Vehicle 1 Name: " << m_vname1 << endl;
  m_msgs << "Vehicle 2 Name: " << m_vname2 << endl;
  m_msgs << "Bounding Box:   " 
         << "<" + to_string(m_min_x) << "," << to_string(m_min_y) << ">" << endl
         << "                " 
         << "<" + to_string(m_min_x) << "," << to_string(m_max_y) << ">" << endl
         << "                "
         << "<" + to_string(m_max_x) << "," << to_string(m_max_y) << ">" << endl
         << "                " 
         << "<" + to_string(m_max_x) << "," << to_string(m_min_y) << ">" << endl;
  m_msgs << "Size of Queue:  " << to_string(m_point_queue.size()) << "\n" << endl;
  return(true);
}

