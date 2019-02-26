/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                   */
/*    DATE: 25 Feb 2019                                     */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include <list>
#include <string>
#include <stdint.h>

#include "PrimeEntry.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"


class PrimeFactor : public AppCastingMOOSApp
{
public:
  PrimeFactor();
  ~PrimeFactor() {};

protected: // Standard MOOSApp functions to overload  
  bool        OnNewMail(MOOSMSG_LIST &NewMail);
  bool        Iterate();
  bool        OnConnectToServer();
  bool        OnStartUp();
  bool        buildReport();

protected:
  void        RegisterVariables();

  uint64_t               m_num_received;
  uint64_t               m_num_calculated;
  std::list<PrimeEntry>  m_work_queue;

private: // Configuration variables

private: // State variables
};

#endif 
