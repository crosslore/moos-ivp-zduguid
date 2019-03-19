/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                     */
/*    DATE: March 14th, 2019                                */
/************************************************************/

#include <cmath>
#include <cstdlib>
#include <iterator>
#include <list>
#include <stdint.h>
#include <string>

#include "Point.h"
#include "MBUtils.h"
#include "GenPath.h"
#include "XYObject.h"
#include "XYSegList.h"

using namespace std;


//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_first_received   = false;
  m_last_received    = false;
  m_capture_radius   = 3;
  m_slip_radius      = 15;
  m_points_received  = 0;
  m_points_planned   = 0;
  m_points_visited   = 0;
  m_points_unvisited = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

    // handle incoming points to visit 
    if ((key == "VISIT_POINT") && 
        (msg.IsString())){
      if      (msg.GetString() == "firstpoint") m_first_received = true;
      else if (msg.GetString() == "lastpoint")  m_last_received  = true;
      else    {m_points_received++;             m_points_unvisited++;}
      Point new_point = Point(msg.GetString());
      m_point_list.push_back(new_point);

    // ignore mail for traverse updates 
    } else if (key == "TRAVERSE_UPDATES") {

    // report error for mail not handled by AppCastingMOOSApp
    } else if(key != "APPCAST_REQ") 
      reportRunWarning("Unhandled Mail: " + key);
   }
   return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
   registerVariables();
   return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool GenPath::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // wait to generate path until firstpoint and lastpoint are received 
  if (m_first_received && m_last_received) {

    // initialize variables to keep track of 
    int  current_x = 0;
    int  current_y = 0;
    bool update_made = false;
    XYSegList traverse_seglist;

    // continually select the closest point until all points are part of the plan
    while (m_point_list.size() > 2) {
      Point next_point = GetClosestPoint(current_x, current_y);
      current_x = next_point.GetX();
      current_y = next_point.GetY();
      m_points_planned++;
      traverse_seglist.add_vertex(current_x, current_y);
      if (!update_made) update_made = true;
    }

    // update the waypoint behavior with the new path
    if (update_made){
      string update_str = "points = " + traverse_seglist.get_spec();
      Notify("TRAVERSE_UPDATES", update_str);
      AppCastingMOOSApp::PostReport();
    }
  }
  return(true);
}


//---------------------------------------------------------
// Procedure: GetDistance()

double GenPath::GetDistance(int x1, int y1, int x2, int y2)
{
  int delta_x = x2 - x1;
  int delta_y = y2 - y1;
  return(hypot(delta_x, delta_y));
}


//---------------------------------------------------------
// Procedure: GetClosestPoint()

Point GenPath::GetClosestPoint(int x_ref, int y_ref)
{
  // iterate through the list of points received
  double closest_distance = numeric_limits<double>::max();
  list<Point>::iterator closest;
  list<Point>::iterator p;
  for(p=m_point_list.begin(); p!=m_point_list.end(); p++) {

    // ignore the first and last point when selecting the closest point
    if ((p->GetString() != "firstpoint") && 
        (p->GetString() != "lastpoint")) {

      // extract the closest valid point remaining in the list
      if (GetDistance(x_ref, y_ref, p->GetX(), p->GetY()) < closest_distance){
        closest_distance = GetDistance(x_ref, y_ref, p->GetX(), p->GetY());
        closest          = p;
      }
    } 
  }
  // return the closest point and remove said point from the list 
  Point closest_point = Point(closest->GetString());
  m_point_list.erase(closest);
  return(closest_point);
}


//---------------------------------------------------------
// Procedure: OnStartUp()

bool GenPath::OnStartUp()
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

void GenPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
  Register("TRAVERSE_UPDATES", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool GenPath::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File: GenPath                                \n";
  m_msgs << "============================================ \n";
  m_msgs << "First Received:   " << to_string(m_first_received) << endl;
  m_msgs << "Last Received:    " << to_string(m_last_received)  << endl;
  m_msgs << "Points Received:  " << m_points_received  << endl;
  m_msgs << "Points Planned:   " << m_points_planned   << endl;  
  m_msgs << "Points Visited:   " << m_points_visited   << endl;
  m_msgs << "Points Unvisited: " << m_points_unvisited << endl;
  m_msgs << "Capture Radius:   " << m_capture_radius   << endl;
  m_msgs << "Slip Radius:      " << m_slip_radius      << endl;
  return(true);
}

