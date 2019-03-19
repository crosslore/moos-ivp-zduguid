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
    double GetDistance(int x1, int y1, int x2, int y2);
    Point  GetClosestPoint(int x, int y);

  private:
    bool m_first_received;
    bool m_last_received;
    uint m_capture_radius;
    uint m_slip_radius;
    uint m_points_received;
    uint m_points_planned;
    uint m_points_visited;
    uint m_points_unvisited;
    std::list<Point> m_point_list;
};

#endif 
