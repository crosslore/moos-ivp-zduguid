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

#include "Point.h"
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
  bool buildReport();
  void registerVariables();
  void postViewPoint(int x, int y, std::string id, std::string color);

 private: // Configuration variables
  bool m_assign_by_region;
  bool m_first_received;
  bool m_last_received;
  int  m_min_x;
  int  m_max_x;
  int  m_min_y;
  int  m_max_y;
  int  m_mid_x;
  std::string m_vname1;
  std::string m_vname2;
  std::string m_vpublish1;
  std::string m_vpublish2;
  std::list<Point>  m_point_list;
};

#endif 
