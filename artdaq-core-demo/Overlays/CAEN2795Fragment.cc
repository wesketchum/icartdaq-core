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

void demo::CAEN2795Fragment::checkADCData(int daq_adc_bits) const {
  demo::CAEN2795Fragment::adc_t const * adcPtr(findBadADC(daq_adc_bits));
  if (adcPtr != dataEnd()) {
    throw cet::exception("IllegalADCVal")
        << "Illegal value of ADC word #"
        << (adcPtr - dataBegin())
        << ": 0x"
        << std::hex
        << *adcPtr
        << ".";
  }
}

std::ostream & demo::operator << (std::ostream & os, CAEN2795Fragment const & f) {
  os << "CAEN2795Fragment event size: "
     << f.hdr_event_size()
     << ", run number: "
     << f.hdr_run_number()
     << "\n";

  return os;
}

