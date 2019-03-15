/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                   */
/*    DATE: March 14th, 2019                                */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include <list>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class PointAssign : public AppCastingMOOSApp
{
public:
  PointAssign();
  ~PointAssign(){};

protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

 protected:
  void registerVariables();

 private: // Configuration variables
  bool m_assign_by_region;
  int  m_min_x;
  int  m_max_x;
  int  m_min_y;
  int  m_max_y;
  int  m_mid_x;
  std::string m_vname1;
  std::string m_vname2;
  std::string m_vpublish1;
  std::string m_vpublish2;
  std::list<std::string>  m_point_queue;
};

#endif 
