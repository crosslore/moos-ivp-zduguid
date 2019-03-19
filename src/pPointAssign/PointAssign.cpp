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
#include "Point.h"
#include "PointAssign.h"

using namespace std;


//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  m_assign_by_region = true;
  m_first_received   = false;
  m_last_received    = false;
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
      if      (msg.GetString() == "firstpoint") m_first_received = true;
      else if (msg.GetString() == "lastpoint")  m_last_received  = true;
      Point new_point = Point(msg.GetString());
      m_point_list.push_back(new_point);

    // ignore outgoing mail that is going to the vehicles
    } else if ((key == m_vpublish1) || 
               (key == m_vpublish2) || 
               (key == "UTS_PAUSE")) {

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

  // wait to assign points until firstpoint and lastpoint are received 
  if (m_first_received && m_last_received) {

    // iterate through the queue of points and assign them accordingly
    list<Point>::iterator p;
    for(p=m_point_list.begin(); p!=m_point_list.end(); p++) {

      // publish the firstpoint and lastpoint markers when they arise
      if ((p->GetString() == "firstpoint") || 
          (p->GetString() == "lastpoint")) {
        Notify(m_vpublish1, p->GetString());
        Notify(m_vpublish2, p->GetString());

      // otherwise publish view points to the vehicle and to pMarineViewer
      } else {
        string vcolor1   = "dodgerblue";
        string vcolor2   = "fuchsia";

        // assign points into East and West regions 
        if (m_assign_by_region) {
          if (p->GetX() < m_mid_x) {
            Notify(m_vpublish1, p->GetString());
            postViewPoint(p->GetX(), p->GetY(), p->GetIDStr(), vcolor1);
          } else {
            Notify(m_vpublish2, p->GetString());
            postViewPoint(p->GetX(), p->GetY(), p->GetIDStr(), vcolor2);
          }

        // assign points in alternating fashion based on their id number
        } else {
          if (p->GetIDNum() % 2 == 0) {
            Notify(m_vpublish1, p->GetString());
            postViewPoint(p->GetX(), p->GetY(), p->GetIDStr(), vcolor1);
          } else {
            Notify(m_vpublish2, p->GetString());
            postViewPoint(p->GetX(), p->GetY(), p->GetIDStr(), vcolor2);
          }
        }
      }
    }
    // clear the queue to avoid assignment for every iterate loop
    m_point_list.clear();
  }
  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: postViewPoint()

 void PointAssign::postViewPoint(int x, int y, string id, string color)
 {
   string point_string = "x="      + to_string(x) + ",y=" + to_string(y) 
                       + ",label=" + id           + ",vertex_size=4"
                       + ",vertex_color=" + color;
   Notify("VIEW_POINT", point_string);
 }


//---------------------------------------------------------
// Procedure: OnStartUp()

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
  Notify("UTS_PAUSE","false");
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
  Register("UTS_PAUSE",0);
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
  m_msgs << "First Received: " << to_string(m_first_received) << endl;
  m_msgs << "Last Received:  " << to_string(m_last_received)  << endl;
  m_msgs << "Bounding Box:   " 
         << "<" + to_string(m_min_x) << "," << to_string(m_min_y) << ">" << endl
         << "                " 
         << "<" + to_string(m_min_x) << "," << to_string(m_max_y) << ">" << endl
         << "                "
         << "<" + to_string(m_max_x) << "," << to_string(m_max_y) << ">" << endl
         << "                " 
         << "<" + to_string(m_max_x) << "," << to_string(m_min_y) << ">" << endl;
  m_msgs << "Size of Queue:  " << to_string(m_point_list.size()) << "\n" << endl;
  return(true);
}

