/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                 */
/*    DATE: 25 Feb 2019                                     */
/************************************************************/

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <stdint.h>

#include "MBUtils.h"
#include "PrimeFactor.h"
#include "PrimeEntry.h"

using namespace std;


//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_num_received   = 0;
  m_num_calculated = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail); 
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    // parse through incoming mail
    if ((msg.GetKey() == "NUM_VALUE") && 
        (msg.IsString())){

      // if input is a number add PrimeEntry to the work queue
      if (isNumber(msg.GetString())) {

        // check to make input is greater than zero
        uint64_t input_value = strtoul(msg.GetString().c_str(), NULL, 0); 
        if (input_value > 0){

          // create a PrimeEntry object for the new request
          m_num_received++;
          PrimeEntry entry;
          entry.setOriginalVal(input_value);
          entry.setReceivedIndex(m_num_received);
          m_work_queue.push_back(entry);
          
        // warn the user of bad inputs
        } else {
          AppCastingMOOSApp::reportConfigWarning("ERROR: non-positive integer");
        }
      } else {
        AppCastingMOOSApp::reportConfigWarning("ERROR: non-number input");
      }
    }
  }
   return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // number of operations performed on each item
  unsigned int num_operations = 1000000;

  // iterate through the queue and work on each job for a number of operations 
  list<PrimeEntry>::iterator p;
  for(p=m_work_queue.begin(); p!=m_work_queue.end();) {
    p->factor(num_operations);

    // job has been completely factored, print out report
    if (p->done()) {
      m_num_calculated++;
      p->setCalculatedIndex(m_num_calculated);
      p->setDoneTime(MOOSTime());
      Notify("PRIME_RESULT", p->getReport());
      p = m_work_queue.erase(p);

    // continue factoring
    } else {
      p++;
    }
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()

bool PrimeFactor::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}


//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NUM_VALUE",0);
  Register("PRIME_RESULT",0);
}


//---------------------------------------------------------
// Procedure: buildReport

bool PrimeFactor::buildReport()
{
  m_msgs << "Prime Factorization" << endl;
  m_msgs << "   number of requests received:   "  << 
            to_string(m_num_received)   << endl;
  m_msgs << "   number of requests calculated: "  << 
            to_string(m_num_calculated) << endl;

  return(true);
}

