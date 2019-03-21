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
  int  GetX(){return m_x_coord;};
  int  GetY(){return m_y_coord;};
  int  GetIDNum(){return m_id_num;};
  bool PointEqual(Point p);
  std::string GetIDStr(){return m_id_str;};
  std::string GetString();

protected:
  int m_x_coord;
  int m_y_coord;
  int m_id_num;
  std::string m_id_str;
};

#endif 
