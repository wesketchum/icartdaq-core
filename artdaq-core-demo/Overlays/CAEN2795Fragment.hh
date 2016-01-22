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
  //This is the header in the hardware
  struct CAEN2795Header{
    uint32_t ev_num : 24;
    uint32_t unused1 : 8;

    uint32_t time_st;
  };


  //hardware-specific metadata
  struct Metadata {

    typedef uint32_t data_t; //fundamental unit of metadata
    
    data_t samples_per_channel;

    data_t num_adc_bits        : 8;
    data_t channels_per_board  : 12;
    data_t num_boards          : 12;
    
    static size_t const size_words = 2ul; // Units of Metadata::data_t
  };

  static_assert (sizeof (Metadata) == Metadata::size_words * sizeof (Metadata::data_t), "CAEN2795Fragment::Metadata size changed");


  //non-hardware-specific metadata
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
  CAEN2795Header const * CAEN2795_hdr(uint16_t b=0) const;

  uint32_t CAEN2795_hdr_ev_num(uint16_t b=0)  { return CAEN2795_hdr(b)->ev_num; }
  uint32_t CAEN2795_hdr_time_st(uint16_t b=0) { return CAEN2795_hdr(b)->time_st; }


  // Start of the ADC values, returned as a pointer to the ADC type
  adc_t const * dataBegin(uint16_t b=0) const {
    return reinterpret_cast<adc_t const *>(CAEN2795_hdr(b) + 1);
  }

  // End of the ADC values, returned as a pointer to the ADC type
  adc_t const * dataEnd(uint16_t b=0) const//;
  { return (dataBegin(b) + adcs_per_board_() - sizeof(CAEN2795Header)/sizeof(adc_t)); }


  // Start of the ADC values, returned as a pointer to the ADC type
  adc_t const * dataTotalBegin() const {
    return reinterpret_cast<adc_t const *>(header_() + 1);
  }

  // End of the ADC values, returned as a pointer to the ADC type
  adc_t const * dataTotalEnd() const//;
  { return dataTotalBegin() + total_adc_values(); }


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

  size_t adcs_per_board_() const {
    return ( metadata_()->samples_per_channel*metadata_()->channels_per_board + 
	     sizeof(CAEN2795Header)/sizeof(adc_t) ); 
  }

  Header const * header_() const {
    return reinterpret_cast<CAEN2795Fragment::Header const *>(artdaq_Fragment_.dataBeginBytes());
  }

  Metadata const * metadata_() const { return artdaq_Fragment_.metadata<CAEN2795Fragment::Metadata>(); }

private:

  artdaq::Fragment const & artdaq_Fragment_;

};

#endif /* artdaq_demo_Overlays_ToyFragment_hh */
