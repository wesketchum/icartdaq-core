#ifndef artdaq_demo_Overlays_CAEN2795Fragment_hh
#define artdaq_demo_Overlays_CAEN2795Fragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"

#include <ostream>
#include <vector>

// Implementation of "CAEN2795Fragment", an artdaq::Fragment overlay class
// used for ICARUS DAQ demo

namespace demo {
  class CAEN2795Fragment;

  // Let the "<<" operator dump the ToyFragment's data to stdout
  std::ostream & operator << (std::ostream &, CAEN2795Fragment const &);
}

class demo::CAEN2795Fragment {
  public:

  typedef uint16_t adc_t; //using 16 bit words for the adc

  //events from readout start with 32-bit word containing event number,
  //and 32-bit word containing timestamp
  struct CAEN2795Header{
    uint32_t ev_num : 24;
    uint32_t unused1 : 8;

    uint32_t time_st;
  };


  // The "Metadata" struct is used to store info primarily related to
  // the upstream hardware environment from where the fragment came

  // "data_t" is a typedef of the fundamental unit of data the
  // metadata structure thinks of itself as consisting of; it can give
  // its size via the static "size_words" variable (
  // ToyFragment::Metadata::size_words )

  struct Metadata {

    typedef uint32_t data_t;

    data_t channels_per_board : 12;
    data_t num_boards         : 12;
    data_t num_adc_bits       : 8;

    static size_t const size_words = 1ul; // Units of Metadata::data_t
  };

  static_assert (sizeof (Metadata) == Metadata::size_words * sizeof (Metadata::data_t), "CAEN2795Fragment::Metadata size changed");


  // The "Header" struct contains "metadata" specific to the fragment
  // which is not hardware-related

  // Header::data_t -- not to be confused with Metadata::data_t ! --
  // describes the standard size of a data type not just for the
  // header data, but ALSO the physics data beyond it; the size of the
  // header in units of Header::data_t is given by "size_words", and
  // the size of the fragment beyond the header in units of
  // Header::data_t is given by "event_size"

  // Notice only the first 28 bits of the first 32-bit unsigned
  // integer in the Header is used to hold the event_size ; this means
  // that you can't represent a fragment larger than 2**28 units of
  // data_t, or 1,073,741,824 bytes

  struct Header {
    typedef uint32_t data_t;

    typedef uint32_t event_size_t;  
    typedef uint32_t run_number_t;

    event_size_t event_size : 28;
    event_size_t unused_1   :  4;

    run_number_t run_number : 32;

    static size_t const size_words = 2ul; // Units of Header::data_t
  };

  static_assert (sizeof (Header) == Header::size_words * sizeof (Header::data_t), "ToyFragment::Header size changed");

  // The constructor simply sets its const private member "artdaq_Fragment_"
  // to refer to the artdaq::Fragment object

  CAEN2795Fragment(artdaq::Fragment const & f) : artdaq_Fragment_(f) {}

  // const getter functions for the data in the header

  Header::event_size_t hdr_event_size() const { return header_()->event_size; } 
  Header::run_number_t hdr_run_number() const { return header_()->run_number; }
  static constexpr size_t hdr_size_words() { return Header::size_words; }

  // The number of ADC values describing data beyond the header
  size_t total_adc_values() const {
    return (hdr_event_size() - hdr_size_words()) * adcs_per_word_();
  }

  //Start of the CAEN2795 firmware Header, returned as a pointer
  //Defaults to the header for the master board.
  CAEN2795Header const * CAEN2795_hdr(uint16_t) const//;
  { return reinterpret_cast<CAEN2795Header const *>(header_() + 1); }

  uint32_t CAEN2795_hdr_ev_num(uint16_t b=0)  { return CAEN2795_hdr(b)->ev_num; }
  uint32_t CAEN2795_hdr_time_st(uint16_t b=0) { return CAEN2795_hdr(b)->time_st; }


  // Start of the ADC values, returned as a pointer to the ADC type
  adc_t const * dataBegin(uint16_t b=0) const {
    return reinterpret_cast<adc_t const *>(CAEN2795_hdr(b) + 1);
  }

  // End of the ADC values, returned as a pointer to the ADC type
  adc_t const * dataEnd(uint16_t b=0) const//;
  { return dataBegin(b) + total_adc_values(); }

  // Functions to check if any ADC values are corrupt

  // findBadADC() checks to make sure that the ADC type (adc_t) variable
  // holding the ADC value doesn't contain bits beyond the expected
  // range, i.e., can't be evaluated to a larger value than the max
  // permitted ADC value

  adc_t const * findBadADC(int daq_adc_bits) const {
    return std::find_if(dataBegin(), dataEnd(), 
			[&](adc_t const adc) -> bool { 
			  return (adc >> daq_adc_bits); });
  }

  bool fastVerify(int daq_adc_bits) const {
    return (findBadADC(daq_adc_bits) == dataEnd());
  };

  // Defined in CAEN2795Fragment.cc, this throws if any ADC appears corrupt
  void checkADCData(int daq_adc_bits) const; 


  // Largest ADC value possible
  size_t adc_range(int daq_adc_bits) {
    return (1ul << daq_adc_bits );
  }

  protected:

  // Functions to translate between size (in bytes) of an ADC, size of
  // this fragment overlay's concept of a unit of data (i.e.,
  // Header::data_t) and size of an artdaq::Fragment's concept of a
  // unit of data (the artdaq::Fragment::value_type).

  static constexpr size_t adcs_per_word_() {
    return sizeof(Header::data_t) / sizeof(adc_t);
  }

  // header_() simply takes the address of the start of this overlay's
  // data (i.e., where the CAEN2795Fragment::Header object begins) and
  // casts it as a pointer to CAEN2795Fragment::Header

  Header const * header_() const {
    return reinterpret_cast<CAEN2795Fragment::Header const *>(artdaq_Fragment_.dataBeginBytes());
  }

  Metadata const * metadata_() const { return artdaq_Fragment_.metadata<CAEN2795Fragment::Metadata>(); }

private:

  artdaq::Fragment const & artdaq_Fragment_;

};

#endif /* artdaq_demo_Overlays_ToyFragment_hh */
