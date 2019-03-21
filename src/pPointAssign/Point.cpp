/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: Point.cpp                                       */
/*    DATE: 14 Mar 2019                                     */
/************************************************************/

#include <string>
#include <vector>

#include "MBUtils.h"
#include "Point.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;


//---------------------------------------------------------
// Constructor

Point::Point(string visit_point_string)
{
  // parse the visit point string into a Point object
  if ((visit_point_string != "firstpoint") && 
      (visit_point_string != "lastpoint")){
    vector<string> point_vector = parseString(visit_point_string, ",");
    string  x_var    = point_vector[0];
    string  y_var    = point_vector[1];
    string id_var    = point_vector[2];
    string  x_param  = biteStringX(x_var, '=');
    string  y_param  = biteStringX(y_var, '=');
    string id_param  = biteStringX(id_var,'=');
    m_x_coord        = stoi(x_var);
    m_y_coord        = stoi(y_var);
    m_id_num         = stoi(id_var);
    m_id_str         = id_var;

  // encode the first and last point as a Point object
  } else {  
    m_x_coord = 0;
    m_y_coord = 0;
    m_id_num  = 0;
    m_id_str  = visit_point_string;
  }
}


//---------------------------------------------------------
// getString

string Point::GetString()
{
  // return the firstpoint or lastpoint string directly
  if ((m_id_str == "firstpoint") || 
      (m_id_str == "lastpoint")) {
    return(m_id_str);

  // otherwise parse the Point object into a valid string
  } else {
    return ( "x="  +  to_string(m_x_coord) + 
            ",y="  +  to_string(m_y_coord) +
            ",id=" +  m_id_str);
  }
}

//---------------------------------------------------------
// PointEqual

bool Point::PointEqual(Point p)
{
  return((m_x_coord == p.GetX()) &&
         (m_y_coord == p.GetY()) && 
         (m_id_num  == p.GetIDNum()));
}
