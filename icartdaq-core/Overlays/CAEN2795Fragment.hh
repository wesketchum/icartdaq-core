#ifndef icartdaq_Overlays_CAEN2795Fragment_hh
#define icartdaq_Overlays_CAEN2795Fragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"

#include <iostream>
#include <vector>

#include "ica_base/structures.h"

// Implementation of "CAEN2795Fragment", an artdaq::Fragment overlay class
// used for ICARUS DAQ demo

namespace icarus {
  class CAEN2795Fragment;
  std::ostream & operator << (std::ostream &, CAEN2795Fragment const &);

  class CAEN2795FragmentMetadata;
  std::ostream & operator << (std::ostream &, CAEN2795FragmentMetadata const&);
}

class icarus::CAEN2795FragmentMetadata {

public:

  typedef uint32_t data_t; //fundamental unit of metadata
  typedef data_t   id_t; //type for board IDs


  struct CAEN2795EventBlock {

    //'EVEN'
    uint32_t token;
    
    uint32_t Run;
    uint32_t Event;
    uint32_t ToD;
    uint32_t AbsTime;
    
    //Conf
    uint32_t pkt_fmt_ver : 8;
    uint32_t trig_type   : 8;
    uint32_t n_crates    : 16;

    uint32_t Size;

  };

  struct CAEN2795StatBlock {

    //'STAT'
    uint32_t token;
    
    //memstat1
    uint32_t fifo_status_1;

    uint32_t fifo_status_2 : 6;
    uint32_t unused1       : 26;

    uint32_t abstime;
    uint32_t mintime;

    //crateid
    uint32_t pkt_fmt_ver : 8;
    uint32_t crate_id    : 24;

    uint32_t size;
    

  };


  CAEN2795FragmentMetadata(){}

  CAEN2795FragmentMetadata(evHead event_header, statpack stat_pack);

  uint32_t const& ev_token() const { return evhead.token; }

  data_t   const& run_number() const { return evhead.Run; }
  data_t   const& event_number() const { return evhead.Event; }

  data_t   const& time_of_day() const { return evhead.ToD; }
  data_t   const& absolute_time() const { return evhead.AbsTime; }

  uint8_t  const& ev_packet_format_version() const { return evhead.pkt_fmt_ver; }
  uint8_t  const& trigger_type() const { return evhead.trigg_type; }
  uint16_t const& n_crates() const { return evhead.n_crates; }
  
  uint32_t const& packet_size() const { return evhead.Size; }


  uint32_t const& stat_token() const { return statpk.token; }
  uint32_t const& fifo_status_1() const { return statpk.fifo_status_1; }
  uint32_t const& fifo_status_2() const { return statpk.fifo_status_2; }
  uint64_t fifo_status() const { return (( statpk.fifo_status_2 << 32 ) | ( statpk.fifo_status_1 )); }

  uint32_t const& stat_abs_time() const { return statpk.abstime; }
  uint32_t const& stat_min_time() const { return statpk.mintime; }

  uint8_t  const& stat_packet_format_version() const { return statpk.pkt_fmt_ver; }

  data_t const& samples_per_channel() const { return _samples_per_channel; }
  data_t num_adc_bits() const { return _num_adc_bits; }
  data_t channels_per_board() const { return _channels_per_board; }
  data_t num_boards() const { return _num_boards; }
  
  data_t crate_id() const { return _crate_id; }
  data_t reserved() const { return _reserved; }

  id_t   const& board_id(size_t i) const
  { BoardExists(i); return _board_ids[i]; }

  void SetRunNumber( data_t r ) { _run_number = r; }
  void SetSubrunNumber( data_t s ) { _subrun_number = s; }
  void SetEventNumber( data_t e ) { _event_number = e; }

  void  SetBoardID(size_t i,id_t id)
  { BoardExists(i); _board_ids[i] = id; }
  void  SetBoardIDs(std::vector<id_t> const& idvec)
  { CheckNBoards(idvec.size()); _board_ids = idvec; } 

  void BoardExists(size_t i) const;
  void CheckNBoards(size_t i) const;

  size_t ExpectedDataSize() const 
  { return _num_boards*(2*sizeof(uint32_t)+_channels_per_board*_samples_per_channel*sizeof(uint16_t)); }


private:

  data_t _run_number;
  data_t _subrun_number;
  data_t _event_number;

  data_t _samples_per_channel;
  
  data_t _num_adc_bits;
  data_t _channels_per_board;
  data_t _num_boards;

  data_t _crate_id;
  data_t _reserved;

  std::vector<id_t> _board_ids;
  
  void UpdateBoardIDSize(){ _board_ids.resize(_num_boards); }

};


class icarus::CAEN2795Fragment {
  public:

  struct CAEN2795DataBlock {

    //DataTile::Header header;

    //'DATA'
    uint32_t token;

    //info1
    uint32_t stop_addr : 16;
    uint32_t mode      : 16;
    
    //info2
    uint32_t dead_peak_mask;

    //info3
    uint32_t tv_trcr  : 16;
    uint32_t abs_time : 16;

    //timeinfo
    uint32_t timeinfo;

    //chID
    uint32_t pkt_fmt_ver  : 8;
    uint32_t crate_id     : 8;
    uint32_t board_id     : 8;
    uint32_t board_status : 8;

    //packSize
    uint32_t packSize;

    //data
    uint32_t event_number : 24;
    uint32_t unused1      :  8;
    uint32_t time_stamp;
    
    uint16_t* data;
    
  };


  CAEN2795Fragment(artdaq::Fragment const & f) : artdaq_Fragment_(f) {}

  CAEN2795FragmentMetadata const * metadata() const { return artdaq_Fragment_.metadata<CAEN2795FragmentMetadata>(); }

  size_t nBoards() const { return metadata()->num_boards(); }
  size_t nChannels() const { return metadata()->num_boards()*metadata()->channels_per_board(); }
  size_t nSamplesPerChannel() const { return metadata()->samples_per_channel(); }
  size_t nChannelsPerBoard() const { return metadata()->channels_per_board(); }

  //typedef CAEN2795BoardBlock<metadata()->samples_per_channel(),metadtat()->channels_per_board()> CAEN2795BoardBlock_t;

  size_t BoardBlockSize() const { return 2*sizeof(uint32_t)+nChannelsPerBoard()*nSamplesPerChannel()*sizeof(uint16_t); }
  size_t DataPayloadSize() const { return artdaq_Fragment_.dataSizeBytes(); }

  CAEN2795BoardBlock const * CAEN2795Board(uint16_t b) const {
    metadata()->BoardExists(b);
    return ( reinterpret_cast< CAEN2795BoardBlock const *>
	     (artdaq_Fragment_.dataBeginBytes() + b*BoardBlockSize()) );
  }
  
  uint32_t BoardEventNumber(uint16_t b) const { return CAEN2795Board(b)->event_number; }
  uint32_t BoardTimeStamp(uint16_t b) const { return CAEN2795Board(b)->time_stamp; }
  uint16_t const* BoardData(uint16_t b) const { return CAEN2795Board(b)->data; }

  uint16_t adc_val(size_t b,size_t c, size_t s) const 
  { return ( *(CAEN2795Board(b)->data+s*nChannelsPerBoard()+c) & (~(1<<(metadata()->num_adc_bits()+1))) ); }

  bool Verify() const;

private:

  artdaq::Fragment const & artdaq_Fragment_;

};

#endif /* artdaq_demo_Overlays_ToyFragment_hh */
