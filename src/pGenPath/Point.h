/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: Point.h                                         */
/*    DATE: 14 March 2019                                   */
/************************************************************/

#ifndef POINT_HEADER
#define POINT_HEADER

#include <list>
#include <string>
#include <stdint.h>
#include <cstdint>

class Point
{
public:
  Point(std::string visit_point_string);
  ~Point(){};
  int GetX(){return m_x_coord;};
  int GetY(){return m_y_coord;};
  std::string GetID(){return m_id;};
  std::string GetString();

protected:
  int m_x_coord;
  int m_y_coord;
  std::string m_id;
};

#endif 
