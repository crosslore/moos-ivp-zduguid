/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                 */
/*    DATE: 25 Feb 2019                                     */
/************************************************************/

#include <cmath>
#include <cstdlib>
#include <iterator>
#include <list>
#include <stdint.h>
#include <string>
#include <sstream>

#include "MBUtils.h"
#include "PrimeEntry.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;


//---------------------------------------------------------
// Constructor

PrimeEntry::PrimeEntry()
{
  m_current_factor   = 2;
  m_current_number   = 0;
  m_orig             = 0;
  m_done             = false;
  m_received_index   = 0;
  m_calculated_index = 0;
  m_start_time       = MOOSTime();
  m_done_time        = 0;
}


//---------------------------------------------------------
// factor

bool PrimeEntry::factor(unsigned long int max_steps)
{ 
  for (uint64_t i = 0; i < max_steps; i++) {

    // condition when no more prime factors exist
    if (m_current_number < pow(m_current_factor, 2)) {
      m_prime_factors.push_back(m_current_number);
      setDone(true);
      break;

    // continue looking for more prime factors
    } else {

      // new factor is found
      if (m_current_number % m_current_factor == 0) {
        m_prime_factors.push_back(m_current_factor);
        m_current_number = m_current_number / m_current_factor;

      // look for bigger factors
      } else {
        m_current_factor++;
      }
    }
  }
  return(true);
}


//---------------------------------------------------------
// getReport

string PrimeEntry::getReport()
{
  string username = "zduguid";
  string report_primes;
  string report_time;
  stringstream report_primes_ss;
  stringstream report_time_ss; 

  // get string from list of prime factors
  if (m_prime_factors.size() > 1){
    list<uint64_t>::iterator p;
    for (p = m_prime_factors.begin(); p != m_prime_factors.end(); p++){
      if (p != m_prime_factors.begin()){
        report_primes_ss << ":";
      }
      report_primes_ss << *p;
    }
    report_primes = report_primes_ss.str();

  // handle case when original number is a prime number (no prime factors)
  } else {
    report_primes = "none:PRIME_NUMBER";
  }

  // get string for the processing time
  report_time_ss << m_done_time - m_start_time;
  report_time = report_time_ss.str();

  // build the report string by following the specification in Lab 5
  string report_string = 
    "orig=" +       to_string(m_orig) + "," + 
    "received=" +   to_string(m_received_index) + "," + 
    "calculated=" + to_string(m_calculated_index) + "," +
    "solve_time=" + report_time + "," + 
    "primes=" +     report_primes + "," + 
    "username=" +   username;

  return(report_string);
}

