/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_zduguid.cpp                  */
/*    DATE: 26 Feb 2019                                     */
/************************************************************/

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <string>
#include <vector>
#include "MBUtils.h"
#include "CommunicationAngle_zduguid.h"

using namespace std;


//---------------------------------------------------------
// Constructor

CommunicationAngle_zduguid::CommunicationAngle_zduguid()
{
  m_surface_sound_speed  = 1480;
  m_sound_speed_gradient = 0.016;
  m_water_depth          = 6000;
  m_z_sound_speed_vanish = - m_surface_sound_speed / m_sound_speed_gradient;
  m_max_radius           = m_water_depth - m_z_sound_speed_vanish;
  m_own_nav_x            = 0;
  m_own_nav_y            = 0;
  m_own_nav_z            = 0;
  m_col_nav_x            = 0;
  m_col_nav_y            = 0;
  m_col_nav_z            = 0;
}


//---------------------------------------------------------
// OnNewMail

bool CommunicationAngle_zduguid::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    // handle vehicle name
    if ((msg.GetKey() == "VEHICLE_NAME") && 
        (msg.IsString())){
      m_vehicle_name = msg.GetString();
    }

    // handle own x navigation position
    else if ((msg.GetKey() == "NAV_X") && 
             (msg.IsDouble())){
      m_own_nav_x = msg.GetDouble();
    }

    // handle own y navigation position
    else if ((msg.GetKey() == "NAV_Y") && 
             (msg.IsDouble())){
      m_own_nav_y = msg.GetDouble();
    }

    // handle own z navigation position
    else if ((msg.GetKey() == "NAV_DEPTH") && 
             (msg.IsDouble())){
      m_own_nav_z = msg.GetDouble();
    }

    // handle collaborator name -> register for collaborator variables
    else if ((msg.GetKey() == "COLLABORATOR_NAME") && 
             (msg.IsString())){
      m_collaborator_name = msg.GetString();
      RegisterCollaboratorVariables();
    }

    // when collaborator name is specified, read in the collaborator positions
    if (!m_collaborator_name.empty()) {

      // handle collaborator x navigation position
      if ((msg.GetKey() == m_collaborator_name + "_NAV_X") && 
          (msg.IsDouble())){
        m_col_nav_x = msg.GetDouble();
      }

      // handle collaborator y navigation position
      else if ((msg.GetKey() == m_collaborator_name + "_NAV_Y") && 
               (msg.IsDouble())){
        m_col_nav_y = msg.GetDouble();
      }

      // handle collaborator z navigation position
      else if ((msg.GetKey() == m_collaborator_name + "_NAV_DEPTH") && 
               (msg.IsDouble())){
        m_col_nav_z = msg.GetDouble();
      }
    }
   }
	
   return(true);
}


//---------------------------------------------------------
// OnConnectToServer

bool CommunicationAngle_zduguid::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}


//---------------------------------------------------------
// Iterate

bool CommunicationAngle_zduguid::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // compute the local coordinates of the own and collaborator vehicles
  double local_r = getHorizontalDistance(m_own_nav_x, m_own_nav_y,
                                         m_col_nav_x, m_col_nav_y);
  double local_z = m_col_nav_z - m_own_nav_z;

  // compute the midpoint of the line between self and collaborator
  // + this midpoint allows us to calculate the center of the circular arc 
  double midpoint_normal = local_z / local_r;
  double midpoint_r      = local_r / 2;
  double midpoint_z      = local_z / 2; 

  // compute the location of the center of the arc that connects the vehicles 
  // + position of circular arc center is given in local vehicle coordinates
  double circle_center_z = m_own_nav_z + midpoint_z - m_z_sound_speed_vanish;
  double circle_center_r = midpoint_r  + (midpoint_normal * circle_center_z);
  double circle_radius   = hypot(circle_center_r, circle_center_z);
  double elevation_angle = atan(circle_center_r / circle_center_z) * 
                           (-180.0 / M_PI);


  cout << "local z:             " << local_z << endl;
  cout << "local r:             " << local_r << endl;

  cout << "circle center r pos: " << circle_center_r << endl;
  cout << "circle center z pos: " << circle_center_z << endl;
  cout << "circle radius:       " << circle_radius << endl;
  cout << "maximum radius:      " << m_max_radius << "\n" << endl;

  // check if is possible to communicate without moving the vehicle
  // + use law of cosines to get the Transmission Loss of the communication
  if ((circle_radius  <= m_max_radius) || 
      (circle_center_r > local_r)){
    double straight_dist   = hypot(local_r, local_z);
    double arc_theta       = acos(1 - (pow(straight_dist,2) / 
                                      (2 * pow(circle_radius,2))));
    double arc_length      = arc_theta * circle_radius;
    double trans_loss      = 20 * log10(arc_length);

    // generate the strings used to notify 
    string elev_string = getAcousticString(elevation_angle, trans_loss);
    string nav_string  = getNavigationString(m_own_nav_x, 
                                             m_own_nav_y, 
                                             m_own_nav_z);
    Notify("ACOUSTIC_PATH",         elev_string);
    Notify("CONNECTIVITY_LOCATION", nav_string);

  // compute the position that the AUV must travel to in order to communicate
  } else {
    vector<double> position = getNewLocation(m_max_radius);
    string elev_string      = getAcousticString(NAN, NAN);
    string nav_string       = getNavigationString(position.at(0),
                                                  position.at(1),
                                                  position.at(2));
    Notify("ACOUSTIC_PATH", elev_string);
    Notify("CONNECTIVITY_LOCATION", nav_string);
  }

  return(true);
  AppCastingMOOSApp::PostReport();  
}


//---------------------------------------------------------
// OnStartUp

bool CommunicationAngle_zduguid::OnStartUp()
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
// RegisterVariables

void CommunicationAngle_zduguid::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VEHICLE_NAME", 0);       // Name of the ownship platform
  Register("COLLABORATOR_NAME", 0);  // Name of collaborator
  Register("NAV_X", 0);              // Ownship East location
  Register("NAV_Y", 0);              // Ownship North location
  Register("NAV_DEPTH", 0);          // Ownship depth
  Register("NAV_HEADING", 0);        // Ownship East heading
  Register("NAV_SPEED", 0);          // Ownship East speed

  // where solution should be posted
  Register("ACOUSTIC_PATH", 0);
  Register("CONNECTIVITY_LOCATION", 0);

  // reference solution
  Register("ACOUSTIC_PATH_REF", 0);
  Register("CONNECTIVITY_LOCATION_REF", 0);
}


//---------------------------------------------------------
// RegisterCollaboratorVariables

void CommunicationAngle_zduguid::RegisterCollaboratorVariables()
{
  Register(m_collaborator_name + "_NAV_X", 0);       // Collaborator East location
  Register(m_collaborator_name + "_NAV_Y", 0);       // Collaborator North location
  Register(m_collaborator_name + "_NAV_DEPTH", 0);   // Collaborator depth
  Register(m_collaborator_name + "_NAV_HEADING", 0); // Collaborator East heading
  Register(m_collaborator_name + "_NAV_SPEED", 0);   // Collaborator East speed
}


//---------------------------------------------------------
// buildReport

bool CommunicationAngle_zduguid::buildReport()
{
  m_msgs << "Own Position: <"      << to_string(m_own_nav_x) << "," <<
    to_string(m_own_nav_y) << ","  << to_string(m_own_nav_z) << ">" << endl;
  m_msgs << "Col Position: <"      << to_string(m_col_nav_x) << "," <<
    to_string(m_col_nav_y) << ","  << to_string(m_col_nav_z) << ">" << endl;
  return(true);
}


//---------------------------------------------------------
// getHorizontalDistance

double CommunicationAngle_zduguid::getHorizontalDistance(double x1, 
                                                         double y1, 
                                                         double x2, 
                                                         double y2)
{
  double delta_x = x2 - x1;
  double delta_y = y2 - y1;
  return(hypot(delta_x, delta_y));
}


//---------------------------------------------------------
// getNewLocation

vector<double> CommunicationAngle_zduguid::getNewLocation(double R_max)
{
  // since we do not need to run an optimization, we use a greedy approach
  // + constrain self to the depth of the collaborator and solve for X and Y
  double delta_x = m_col_nav_x - m_own_nav_x;
  double delta_y = m_col_nav_y - m_own_nav_y;
  double delta_d = hypot(delta_x, delta_y);

  // get the normal that points directly towards the collaborator
  double norm_x  = delta_x / delta_d;
  double norm_y  = delta_y / delta_d;
  double width   = pow((pow(R_max, 2) - 
                        pow(m_col_nav_z - m_z_sound_speed_vanish, 2)), 0.5);
  double new_r   = delta_d - (width * 2);

  // travel in direction of collaborator until within communication range
  double new_x   = norm_x * new_r;
  double new_y   = norm_y * new_r;
  double new_z   = m_col_nav_z;

  // convert the new coordinates to vector form
  vector<double> position;
  position.push_back(new_x);
  position.push_back(new_y);
  position.push_back(new_z);

  return(position);
}


//---------------------------------------------------------
// getAcousticString

string CommunicationAngle_zduguid::getAcousticString(double angle,
                                                     double loss)
{
  string elev_string = "elev_angle="         + to_string(angle) + 
                       ",transmission_loss=" + to_string(loss) + 
                       ",id=zduguid@mit.edu";
  return(elev_string);
}


//---------------------------------------------------------
// getNavigationString

string CommunicationAngle_zduguid::getNavigationString(double x, 
                                                       double y,
                                                       double z)
{
  string nav_string  = "x="      + to_string(x) + 
                       ",y="     + to_string(y) + 
                       ",depth=" + to_string(z) +
                       ",id=zduguid@mit.edu";
  return(nav_string);
}
