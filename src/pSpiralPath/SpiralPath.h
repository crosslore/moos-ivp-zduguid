/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: SpiralPath.h                                    */
/*    DATE: 2019 May                                        */
/************************************************************/

#ifndef SpiralPath_HEADER
#define SpiralPath_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class SpiralPath : public AppCastingMOOSApp
{
  public:
    SpiralPath();
    ~SpiralPath(){};

  protected: // Standard MOOSApp functions to overload  
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();
    bool setLoiterDelta(std::string);
    void handleSpiralRequest(std::string);
    void handleParameterEstimate(std::string);
    void handleLoiterReport(std::string);

  protected: // Standard AppCastingMOOSApp function to overload 
    bool buildReport();

  protected:
    void registerVariables();

  private: // State variables
    // loiter parameters
    double m_osx;
    double m_osy;
    double m_loiter_x;
    double m_loiter_y;
    double m_loiter_radius;
    double m_loiter_delta;
    double m_loiter_offset;

    // variables parsed from the Front Estimator
    std::string m_latest_estimate;
    std::string m_est_vname;
    uint   m_num_estimate_rcd;
    uint   m_num_cycles;
    bool   m_req_new_estimate;
    double m_est_offset;
    double m_est_angle;
    double m_est_amplitude;
    double m_est_period;
    double m_est_wavelength;
    double m_est_alpha;
    double m_est_beta;
    double m_est_tempnorth;
    double m_est_tempsouth;
};

#endif 
