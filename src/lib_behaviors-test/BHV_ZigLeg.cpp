/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.cpp                                  */
/*    DATE: 23 Apr 2019                                     */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_ZigLeg.h"
#include "XYRangePulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_ZigLeg::BHV_ZigLeg(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // state variables 
  m_leg_heading        = 0.0;
  m_curr_time          = 0.0;
  m_wpt_time           = 0.0;
  m_peak_heading       = 0.0;
  m_zig_heading_offset = 45;
  m_zig_delay          = 5;
  m_calc_new_ipf       = false;
  m_calc_reset_ipf     = false;
  m_zigleg_ipf         = 0;

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_HEADING, WPT_INDEX");
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_ZigLeg::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "zig_heading_offset") && isNumber(val)) {
    m_zig_heading_offset = double_val;
    return(true);
  }
  else if((param == "zig_delay") && (double_val > 0) && isNumber(val)) {
    m_zig_delay = double_val;
    return(true);
  }
  else if((param == "zig_time") && (double_val > 0) && isNumber(val)) {
    m_zig_time = double_val;
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

void BHV_ZigLeg::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_ZigLeg::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_ZigLeg::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_ZigLeg::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_ZigLeg::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_ZigLeg::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_ZigLeg::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_ZigLeg::onRunState()
{
  // update the current time 
  m_curr_time = IvPBehavior::getBufferCurrTime();

  // if waypoint update occurred in the current iteration, reset the time
  if (IvPBehavior::getBufferTimeVal("WPT_INDEX") == 0) {
    // only generate pulse once waypoint is reached (not after DEPLOY)
    bool ok_w;
    if (getBufferDoubleVal("WPT_INDEX", ok_w) > 0){
      m_wpt_time     = m_curr_time; 
      m_calc_new_ipf = true;
    } 
  }

  // compute a new IvPFunction for the zigleg traversal
  if (m_calc_new_ipf && 
    ((m_curr_time - m_wpt_time) > m_zig_delay)) {
    // get the heading of the leg
    bool ok_h;
    m_leg_heading = getBufferDoubleVal("NAV_HEADING", ok_h);

    // get the reference heading for generating the objective function
    m_peak_heading = m_leg_heading + m_zig_heading_offset;
    if      (m_peak_heading > 360) m_peak_heading-= 360;
    else if (m_peak_heading < 0)   m_peak_heading+= 360;

    // generate the IvPFunction from this peak heading value
    m_zigleg_ipf = buildIvPFunction(m_peak_heading);

    // // avoid generating for the duration of this leg
    m_calc_new_ipf   = false;
    m_calc_reset_ipf = true;
  }

  // reset the IvPFunction to zero once the zig is complete 
  if (m_calc_reset_ipf && 
    ((m_curr_time - m_wpt_time - m_zig_delay) > m_zig_time)) {
    m_zigleg_ipf = 0;
    m_calc_reset_ipf = false; 
  }

  // return a null pointer after the m_zigleg_ipf has been reset
  if (!m_zigleg_ipf) {
    return(m_zigleg_ipf);
  }
  
  // generate a new IvP Function based on the latest peak calculated
  else {
    m_zigleg_ipf = buildIvPFunction(m_peak_heading);
    m_zigleg_ipf->setPWT(m_priority_wt);
    return(m_zigleg_ipf);
  }
}


//---------------------------------------------------------------
// Procedure: buildIvPFunction

IvPFunction* BHV_ZigLeg::buildIvPFunction(double peak_heading){
  // generate the ZAIC peak from the input peak_heading
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(peak_heading);
  crs_zaic.setPeakWidth(10);
  crs_zaic.setBaseWidth(10);
  crs_zaic.setMinMaxUtil(10, 150);
  crs_zaic.setSummitDelta(60);  
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  // extract the IvP function
  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
  return(crs_ipf);
}
