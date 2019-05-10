/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: SpiralPath.cpp                                  */
/*    DATE: 2019 May                                        */
/************************************************************/

#include <iterator>
#include <string>
#include "MBUtils.h"
#include "ACTable.h"
#include "SpiralPath.h"
#include "XYObject.h"
#include "XYVector.h"
#include "XYPoint.h"

using namespace std;


//---------------------------------------------------------
// Constructor

SpiralPath::SpiralPath()
{
  // loiter parameters
  m_osx               = 0;
  m_osy               = 0;
  m_loiter_x          = 0;
  m_loiter_y          = 0;
  m_loiter_radius     = 0;
  m_loiter_delta      = 0;
  m_loiter_offset     = 0;

  // variables parsed from the Front Estimator
  m_num_estimate_rcd  = 0;
  m_num_cycles        = 0;
  m_req_new_estimate  = false;
  m_est_offset        = 0;
  m_est_angle         = 0;
  m_est_amplitude     = 0;
  m_est_period        = 0;
  m_est_wavelength    = 0;
  m_est_alpha         = 0;
  m_est_beta          = 0;
  m_est_tempnorth     = 0;
  m_est_tempsouth     = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool SpiralPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

    if(key == "SPIRAL_REPORT_PARAMS")
      handleParameterEstimate(sval);

    else if(key == "LOITER_REPORT")
      handleLoiterReport(sval);

    else if(key == "NAV_X")
      m_osx = atof(sval.c_str());

    else if(key == "NAV_Y")
      m_osy = atof(sval.c_str());

    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SpiralPath::OnConnectToServer()
{
   registerVariables();
   return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SpiralPath::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  // // request a new estimate report
  // // if (m_req_new_estimate) {
  // if (true) {
  //   // m_req_new_estimate = false;
  //   m_num_estimate_req++;
  // }

  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SpiralPath::OnStartUp()
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

    if(param == "delta") 
      handled = setLoiterDelta(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}


//------------------------------------------------------------
// Procedure: handleParameterEstimate()

void SpiralPath::handleParameterEstimate(string request_string)
{
  m_num_estimate_rcd++;
  m_latest_estimate = request_string;

  // parse all of the values in the parameter estimate
  vector<string> svector = parseString(request_string, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if (param == "vname")
      m_est_vname = value;
    else if (param == "offset")
      m_est_offset = atof(value.c_str());
    else if (param == "angle")
      m_est_angle = atof(value.c_str());
    else if (param == "amplitude")
      m_est_amplitude = atof(value.c_str());
    else if (param == "period")
      m_est_period = atof(value.c_str());
    else if (param == "wavelength")
      m_est_wavelength = atof(value.c_str());
    else if (param == "alpha")
      m_est_alpha = atof(value.c_str());
    else if (param == "beta")
      m_est_beta = atof(value.c_str());
    else if (param == "tempnorth")
      m_est_tempnorth = atof(value.c_str());
    else if (param == "tempsouth")
      m_est_tempsouth = atof(value.c_str());
    else
      reportConfigWarning("Invalid spiral request for " + GetAppName());
  }

  // generate the vector aligned with the front
  XYVector front_vector;
  front_vector.setPosition(0,     m_est_offset);
  front_vector.setVectorMA(70, 90-m_est_angle);
  front_vector.setHeadSize(15);
  front_vector.set_vertex_size(5);
  front_vector.set_edge_size(4);
  front_vector.set_label("front_estimate");
  front_vector.set_edge_color("magenta");
  front_vector.set_vertex_color("magenta");

  // generate the vector aligned with the amplitude of the front
  XYVector amplitude_vector;
  amplitude_vector.setPosition(0,  m_est_offset);
  amplitude_vector.setVectorMA(m_est_amplitude, -m_est_angle);
  amplitude_vector.setHeadSize(5);
  amplitude_vector.set_vertex_size(4);
  amplitude_vector.set_edge_size(2);
  amplitude_vector.set_label("amplitude_estimate");
  amplitude_vector.set_edge_color("yellowgreen");
  amplitude_vector.set_vertex_color("yellowgreen");

  // adjust the loiter position depending upon the current parameters
  m_loiter_offset += m_loiter_delta;
  m_loiter_radius  = m_est_amplitude;
  m_loiter_x = front_vector.xpos() + m_loiter_offset*cos(m_est_angle*PI/180.0);
  m_loiter_y = front_vector.ypos() + m_loiter_offset*sin(m_est_angle*PI/180.0);

  // adjust loiter delta parameter to keep the vehicle within the op region
  if (m_loiter_x > 120)
    m_loiter_delta = - abs(m_loiter_delta);
  if (m_loiter_x < -20) 
    m_loiter_delta = abs(m_loiter_delta);

  // plot the point
  XYPoint loiter_center;
  loiter_center.set_vertex(m_loiter_x, m_loiter_y);
  loiter_center.set_label("loiter_center");
  loiter_center.set_vertex_size(10);
  loiter_center.set_vertex_color("darkorange");

  // display the vector information 
  Notify("VIEW_VECTOR", front_vector.get_spec()); 
  Notify("VIEW_VECTOR", amplitude_vector.get_spec()); 
  Notify("VIEW_POINT",  loiter_center.get_spec());
  // TODO
  // update the encircle behavior 
  //  -> polygon = radial:: x=60, y=-105, radius=30, pts=10
  //  -> pEncircle subscribed to "ENCIRCLE_POSITION"
  //  -> pEncircle update form: "circle_position = x=60,y=-105,radius=30"
  ostringstream encircle_os;
  encircle_os << "polygon = radial:: "
              << "x="       << m_loiter_x 
              << ",y="      << m_loiter_y 
              << ",radius=" << m_loiter_radius
              << ",pts=10"; 
  string encircle_str = encircle_os.str();
  if (m_loiter_radius > 0)
    Notify("SPIRAL_UPDATES", encircle_str);
  // Notify("ENCIRCLE_ACTIVE", "false");
}


//------------------------------------------------------------
// Procedure: handleLoiterReport()

void SpiralPath::handleLoiterReport(string request_string)
{
  string index = tokStringParse(request_string, "index", ',', '=');
  if (index == "0"){
    m_num_cycles++;
    // // TODO
    // // update the encircle behavior 
    // //  -> polygon = radial:: x=60, y=-105, radius=30, pts=10
    // //  -> pEncircle subscribed to "ENCIRCLE_POSITION"
    // //  -> pEncircle update form: "circle_position = x=60,y=-105,radius=30"
    // ostringstream encircle_os;
    // encircle_os << "polygon = radial:: "
    //             << "x="       << m_loiter_x 
    //             << ",y="      << m_loiter_y 
    //             << ",radius=" << m_loiter_radius
    //             << ",pts=10"; 
    // string encircle_str = encircle_os.str();
    // if (m_loiter_radius > 0)
    //   Notify("SPIRAL_UPDATES", encircle_str);
    // // Notify("ENCIRCLE_ACTIVE", "false");
  }
}


//---------------------------------------------------------
// Procedure: setLoiterDelta()

bool SpiralPath::setLoiterDelta(string str)
{
  if(!isNumber(str))
    return(false);
  
  double dval = atof(str.c_str());
  if(dval > 10)
    dval = 10;
  else if (dval < 0)
    dval = 0;

  m_loiter_delta = dval;
  return(true);
}


//---------------------------------------------------------
// Procedure: registerVariables

void SpiralPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SPIRAL_REPORT_PARAMS",0);
  Register("LOITER_REPORT", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SpiralPath::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: SpiralPath                            " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Message Info"                                 << endl;
  m_msgs << "  Num Reports: " << m_num_estimate_rcd        << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "Loiter Info"                                  << endl;
  m_msgs << "  Num Cycles:  " << m_num_cycles              << endl;
  m_msgs << "  Position:     <" << m_osx << "," << m_osy << ">" << endl;
  m_msgs << "--------------------------------------------" << endl;
  m_msgs << "Parameter Estimation Info"                    << endl;
  m_msgs << "  vname:       " << m_est_vname               << endl; 
  m_msgs << "  offset:      " << m_est_offset              << endl; 
  m_msgs << "  angle:       " << m_est_angle               << endl; 
  m_msgs << "  amplitude:   " << m_est_amplitude           << endl; 
  m_msgs << "  period:      " << m_est_vname               << endl; 
  m_msgs << "  wavelength:  " << m_est_wavelength          << endl; 
  m_msgs << "  alpha:       " << m_est_alpha               << endl; 
  m_msgs << "  beta:        " << m_est_beta                << endl; 
  m_msgs << "  tempnorth:   " << m_est_tempnorth           << endl; 
  m_msgs << "  tempsouth:   " << m_est_tempsouth           << endl; 
  return(true);
}






