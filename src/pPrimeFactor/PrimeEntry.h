/************************************************************/
/*    NAME: Zach Duguid                                     */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                    */
/*    DATE: 25 Feb 2019                                     */
/************************************************************/

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

#include <list>
#include <string>
#include <stdint.h>
#include <cstdint>

class PrimeEntry
{
public:
  PrimeEntry();
  ~PrimeEntry() {};

  void setOriginalVal(uint64_t v)     {m_orig=v; m_current_number=v;};
  void setReceivedIndex(uint64_t v)   {m_received_index=v;};
  void setCalculatedIndex(uint64_t v) {m_calculated_index=v;};
  void setDoneTime(double v)          {m_done_time=v;};
  void setDone(bool v)                {m_done=v;};
  bool done()                         {return(m_done);};

  bool factor(unsigned long int max_steps);

  std::string getReport();

protected:
  uint64_t m_current_factor;
  uint64_t m_current_number;
  uint64_t m_orig;
  bool     m_done;
  uint64_t m_received_index;
  uint64_t m_calculated_index;
  double   m_start_time;
  double   m_done_time;

  std::list<uint64_t> m_prime_factors;
};

#endif 
