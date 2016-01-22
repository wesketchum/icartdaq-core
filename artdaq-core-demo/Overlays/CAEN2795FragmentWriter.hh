#ifndef artdaq_demo_Overlays_CAEN2795FragmentWriter_hh
#define artdaq_demo_Overlays_CAEN2795FragmentWriter_hh

#include "artdaq-core/Data/Fragment.hh"
#include "artdaq-core-demo/Overlays/CAEN2795Fragment.hh"

#include <iostream>

namespace demo {
  class CAEN2795FragmentWriter;
}


class demo::CAEN2795FragmentWriter: public demo::CAEN2795Fragment {
public:


  CAEN2795FragmentWriter(artdaq::Fragment & f); 

  // These functions form overload sets with const functions from
  // demo::CAEN2795Fragment

  adc_t * dataBegin(uint16_t b=0);
  adc_t * dataEnd(uint16_t b=0);

  CAEN2795Header * CAEN2795_hdr(uint16_t b=0);

  // We'll need to hide the const version of header in CAEN2795Fragment in
  // order to be able to perform writes

  Header * header_() {
    assert(artdaq_Fragment_.dataSizeBytes() >= sizeof(Header) );
    return reinterpret_cast<Header *>( artdaq_Fragment_.dataBeginBytes());
  }

  void set_hdr_run_number(Header::run_number_t run_number) { 
    header_()->run_number = run_number;
  }

  void resize(size_t nAdcs);

private:
  size_t calc_event_size_words_(size_t nAdcs);

  static size_t adcs_to_words_(size_t nAdcs);

  // Note that this non-const reference hides the const reference in the base class
  artdaq::Fragment & artdaq_Fragment_;
};

// The constructor will expect the artdaq::Fragment object it's been
// passed to contain the artdaq::Fragment header + the
// CAEN2795Fragment::Metadata object, otherwise it throws

demo::CAEN2795FragmentWriter::CAEN2795FragmentWriter(artdaq::Fragment& f ) :
  CAEN2795Fragment(f), artdaq_Fragment_(f) {
   
  if ( ! f.hasMetadata() || f.dataSizeBytes() > 0 ) {
    throw cet::exception("Error in CAEN2795FragmentWriter: Raw artdaq::Fragment object does not appear to consist of (and only of) its own header + the CAEN2795Fragment::Metadata object");
  }
 
  // Allocate space for the header
  artdaq_Fragment_.resizeBytes( sizeof(Header) );
}


inline demo::CAEN2795Fragment::adc_t * demo::CAEN2795FragmentWriter::dataBegin(uint16_t b) {
  return reinterpret_cast<adc_t *>(CAEN2795_hdr(b) + 1);
}

inline demo::CAEN2795Fragment::adc_t * demo::CAEN2795FragmentWriter::dataEnd(uint16_t b) {
  return (dataBegin(b) + adcs_per_board_() - sizeof(CAEN2795Header)/sizeof(adc_t));
}

inline demo::CAEN2795Fragment::CAEN2795Header * demo::CAEN2795FragmentWriter::CAEN2795_hdr(uint16_t b)
{
  // Make sure there's data past the CAEN2795Fragment header
  assert(artdaq_Fragment_.dataSizeBytes() >= sizeof(Header) + sizeof(artdaq::Fragment::value_type) );
  return ( reinterpret_cast<CAEN2795Header *>(header_() + 1) + 
	   b*adcs_per_board_()*sizeof(adc_t)/sizeof(CAEN2795Header));
}


inline void demo::CAEN2795FragmentWriter::resize(size_t nAdcs) {

  artdaq_Fragment_.resizeBytes(sizeof(Header::data_t) * calc_event_size_words_(nAdcs) );
  header_()->event_size = calc_event_size_words_(nAdcs);
}

inline size_t demo::CAEN2795FragmentWriter::calc_event_size_words_(size_t nAdcs) {
  return adcs_to_words_(nAdcs) + hdr_size_words();
}

inline size_t demo::CAEN2795FragmentWriter::adcs_to_words_(size_t nAdcs) {
  auto mod(nAdcs % adcs_per_word_());
  return (mod == 0) ?
    nAdcs / adcs_per_word_() :
    nAdcs / adcs_per_word_() + 1;
}

#endif /* artdaq_demo_Overlays_CAEN2795FragmentWriter_hh */
