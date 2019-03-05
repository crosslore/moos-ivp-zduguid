/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_zduguid.h                    */
/*    DATE: 26 Feb 2019                                     */
/************************************************************/

#ifndef CommunicationAngle_zduguid_HEADER
#define CommunicationAngle_zduguid_HEADER

#include <string>
#include <vector>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h" 

class CommunicationAngle_zduguid : public AppCastingMOOSApp 
{
  public:
    CommunicationAngle_zduguid();
    ~CommunicationAngle_zduguid() {};

  protected:
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    bool buildReport();
    void RegisterVariables();
    void RegisterCollaboratorVariables();

    double getHorizontalDistance(double x1, double y1, double x2, double y2);
    std::vector<double> getNewLocation(double R_max);
    std::string getAcousticString(double angle, double loss);
    std::string getNavigationString(double x, double y, double z);

  protected:
    std::string m_vehicle_name;
    std::string m_collaborator_name;
    double      m_surface_sound_speed;
    double      m_sound_speed_gradient;
    double      m_water_depth;
    double      m_z_sound_speed_vanish;
    double      m_max_radius;
    double      m_own_nav_x;
    double      m_own_nav_y;
    double      m_own_nav_z;
    double      m_col_nav_x;
    double      m_col_nav_y;
    double      m_col_nav_z;
};

#endif 
