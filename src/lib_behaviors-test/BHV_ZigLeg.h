/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.h                                    */
/*    DATE: 23 Apr 2019                                     */
/************************************************************/

#ifndef Pulse_HEADER
#define Pulse_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYRangePulse.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "OF_Reflector.h"


class BHV_ZigLeg : public IvPBehavior {
public:
  BHV_ZigLeg(IvPDomain);
  ~BHV_ZigLeg() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  IvPFunction* buildIvPFunction(double peak_heading);

protected: // Local Utility functions

protected: // Configuration parameters

protected: // State variables
  IvPFunction *m_zigleg_ipf;
  double m_leg_heading;
  double m_curr_time;
  double m_wpt_time;
  double m_peak_heading;
  int    m_zig_heading_offset;
  int    m_zig_delay;
  int    m_zig_time;
  bool   m_calc_new_ipf;
  bool   m_calc_reset_ipf;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_ZigLeg(domain);}
}
#endif
