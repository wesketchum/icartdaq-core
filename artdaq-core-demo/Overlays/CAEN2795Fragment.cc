#include "artdaq-core-demo/Overlays/CAEN2795Fragment.hh"

#include "cetlib/exception.h"

#if 0
namespace {
  unsigned int pop_count (unsigned int n) {
    unsigned int c; 
    for (c = 0; n; c++) n &= n - 1; 
    return c;
  }
}
#endif

demo::CAEN2795Fragment::CAEN2795Header const * demo::CAEN2795Fragment::CAEN2795_hdr(uint16_t b) const 
{
  return ( reinterpret_cast<CAEN2795Header const *>(header_() + 1) + 
	   b*adcs_per_board_()*sizeof(adc_t)/sizeof(CAEN2795Header));
}

std::ostream & demo::operator << (std::ostream & os, CAEN2795Fragment const & f) {
  os << "CAEN2795Fragment event size: "
     << f.hdr_event_size()
     << ", run number: "
     << f.hdr_run_number()
     << "\n";

  return os;
}

