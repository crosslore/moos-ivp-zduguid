/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                       */
/*    DATE: March 14th, 2019                                */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include <cmath>
#include <cstdlib>
#include <iterator>
#include <list>
#include <stdint.h>
#include <string>

#include "Point.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class GenPath : public AppCastingMOOSApp
{
  public:
    GenPath();
    ~GenPath(){};

  protected: // Standard MOOSApp functions to overload  
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

  protected: // Standard AppCastingMOOSApp function to overload 
    bool buildReport();

  protected:
    void registerVariables();
    double GetDistance(double x1, double y1, double x2, double y2);
    Point  GetClosestPoint(double x, double y);

  private:
    double m_nav_x;
    double m_nav_y;
    bool   m_first_received;
    bool   m_last_received;
    uint   m_visit_radius;
    uint   m_points_received;
    uint   m_points_planned;
    std::list<Point> m_point_list;
    std::list<Point> m_point_copy;
    std::list<Point> m_point_visited;
    std::list<Point> m_point_unvisited;
};

#endif 
