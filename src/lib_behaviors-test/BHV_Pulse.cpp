/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                   */
/*    DATE: 23 Apr 2019                                     */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"
#include "XYRangePulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // state variables 
  m_nav_x          = 0.0;
  m_nav_y          = 0.0;
  m_curr_time      = 0.0;
  m_wpt_time       = 0.0;
  m_pulse_range    = 10;
  m_pulse_duration = 5;
  m_pulse_delay    = 5;

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y, WPT_INDEX");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "pulse_range") && (double_val > 0) && isNumber(val)) {
    m_pulse_range = double_val;
    return(true);
  }
  else if((param == "pulse_duration") && (double_val > 0) && isNumber(val)) {
    m_pulse_duration = double_val;
    return(true);
  }
  else if((param == "pulse_delay") && (double_val > 0) && isNumber(val)) {
    m_pulse_delay = double_val;
    return(true);
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  // update the current time 
  m_curr_time = IvPBehavior::getBufferCurrTime();

  // retrieve the latest nav_x and nav_y data from the information buffer 
  bool ok_x, ok_y, ok_w;
  m_nav_x = getBufferDoubleVal("NAV_X", ok_x);
  m_nav_y = getBufferDoubleVal("NAV_Y", ok_y);
  if(!ok_x || !ok_y) {
    postWMessage("No ownship X/Y info in info_buffer.");
    return(0);
  }

  // if waypoint update occurred in the current iteration, reset the time
  if (IvPBehavior::getBufferTimeVal("WPT_INDEX") == 0) {
      // only generate pulse once waypoint is reached (not after DEPLOY)
      if (getBufferDoubleVal("WPT_INDEX", ok_w) > 0){
      m_wpt_time = m_curr_time; 
    } 
  }

  // pulse if it has been 5 seconds since the last waypoint update
  if (((m_curr_time - m_wpt_time) > m_pulse_delay) && (m_wpt_time > 0)) {
    // generate pulse to be visualize 
    XYRangePulse pulse;
    pulse.set_x(m_nav_x);
    pulse.set_y(m_nav_y);
    pulse.set_label("bhv_pulse");
    pulse.set_rad(m_pulse_range);
    pulse.set_duration(m_pulse_duration);
    pulse.set_time(m_curr_time);
    pulse.set_color("edge", "yellow");
    pulse.set_color("fill", "yellow");
    string spec = pulse.get_spec();
    postMessage("VIEW_RANGE_PULSE", spec);
    // reset waypoint time to prevent pulses from being generated constantly
    m_wpt_time = -1;
  }

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

