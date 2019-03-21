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
  m_nav_x            = 0.0;
  m_nav_y            = 0.0;
  m_first_received   = false;
  m_last_received    = false;
  m_visit_radius     = 3;
  m_points_received  = 0;
  m_points_planned   = 0;
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
      else     m_point_unvisited.push_back(Point(msg.GetString()));
      m_points_received++;
      m_point_list.push_back(Point(msg.GetString()));
      m_point_copy.push_back(Point(msg.GetString()));
    }

    // handle vehicle navigation updates
    else if (key == "NAV_X") {m_nav_x = msg.GetDouble();}
    else if (key == "NAV_Y") {m_nav_y = msg.GetDouble();}

    // ignore mail for traverse updates 
    else if (key == "TRAVERSE_UPDATES") {}

    // generate a new path if any points have been missed
    else if (key == "GENPATH_REGENERATE") {

      // load points on the queue again
      if (m_point_unvisited.size() > 0) {
        list<Point>::iterator p;
        for(p=m_point_unvisited.begin(); p!=m_point_unvisited.end(); p++) {
          m_point_list.push_back(Point(p->GetString()));
        }
      } 

      // load up the original set of points for the next DEPLOY action
      else {
        Notify("STATION_KEEP", "true");
        m_point_list.clear();
        m_point_visited.clear();
        m_point_unvisited.clear();
        list<Point>::iterator p;
        for(p=m_point_copy.begin(); p!=m_point_copy.end(); p++) {
          if ((p->GetString() != "firstpoint") && 
              (p->GetString() != "lastpoint")) {
            m_point_list.push_back(Point(p->GetString()));
            m_point_unvisited.push_back(Point(p->GetString()));
          }
        }
      }
    }

    // report error for mail not handled by AppCastingMOOSApp
    else if(key != "APPCAST_REQ") { reportRunWarning("Unhandled Mail: " + key);}
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

  // generate path using Points stored in m_point_list 
  if (m_first_received && m_last_received) {

    // initialize variables to keep track of 
    int  current_x = 88;
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
    }
  }

  // point is considered visited if it is within the visit radius of the point
  list<Point>::iterator p;
  for(p=m_point_unvisited.begin(); p!=m_point_unvisited.end();) {
    if (GetDistance(p->GetX(), p->GetY(), m_nav_x, m_nav_y) <= m_visit_radius) {
      m_point_visited.push_back(Point(p->GetString()));
      p = m_point_unvisited.erase(p);
    }
    else {p++;} 
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: GetDistance()

double GenPath::GetDistance(double x1, double y1, double x2, double y2)
{
  double delta_x = x2 - x1;
  double delta_y = y2 - y1;
  return(hypot(delta_x, delta_y));
}


//---------------------------------------------------------
// Procedure: GetClosestPoint()

Point GenPath::GetClosestPoint(double x_ref, double y_ref)
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
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("VISIT_POINT", 0);
  Register("RETURN_UPDATES", 0);
  Register("GENPATH_REGENERATE", 0);
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
  m_msgs << "Points Received:  " << m_points_received << "\n"   << endl;
  m_msgs << "TOUR STATUS"        << endl;
  m_msgs << "------------------" << endl; 
  m_msgs << "Capture Radius:   " << m_visit_radius              << endl;
  m_msgs << "Points Visited:   " << m_point_visited.size()      << endl;
  m_msgs << "Points Unvisited: " << m_point_unvisited.size()    << endl;
  m_msgs << "Points Planned:   " << m_points_planned            << endl;
  m_msgs << "Points in Queue:  " << m_point_list.size()         << endl;
  m_msgs << "Current X:        " << m_nav_x                     << endl;
  m_msgs << "Current Y:        " << m_nav_y                     << endl;
  return(true);
}

