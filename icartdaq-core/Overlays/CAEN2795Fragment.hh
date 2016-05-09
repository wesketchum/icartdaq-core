#ifndef icartdaq_Overlays_CAEN2795Fragment_hh
#define icartdaq_Overlays_CAEN2795Fragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"

#include <iostream>
#include <vector>

#include "ica_base/structures.h"
#include "ica_base/packs.h"

// Implementation of "CAEN2795Fragment", an artdaq::Fragment overlay class
// used for ICARUS DAQ demo

namespace icarus {
  class CAEN2795Fragment;
  std::ostream & operator << (std::ostream &, CAEN2795Fragment const &);

  class CAEN2795FragmentMetadata;
  std::ostream & operator << (std::ostream &, CAEN2795FragmentMetadata const&);

  class CAEN2795StatFragment;
  std::ostream & operator << (std::ostream &, CAEN2795StatFragment const &);
  class CAEN2795Stat;
  std::ostream & operator << (std::ostream &, CAEN2795Stat const &);

}

struct icarus::CAEN2795DataBlock {
  
  //DataTile::Header header;
  typedef struct Header{
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
  } Header_t;
  
  //data
  typedef struct Data{
    uint32_t event_number : 24;
    uint32_t unused1      :  8;
    uint32_t time_stamp;
    
    uint16_t* data;
  } Data_t;

  Header_t header;
  Data_t   data;
    
};

class icarus::CAEN2795Stat{

public:

  typedef struct icarus::CAEN2795StatBlock {
  
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
  } CAEN2795StatBlock_t;

  CAEN2795Stat(){};
  CAEN2795Stat(statpack pk)
  { statblk = *(reinterpret_cast<CAEN2795StatBlock *>(&pk)); };

  CAEN2795StatBlock const& stat_pack() { return statblk; }

private:
  CAEN2795StatBlock_t statblk;

};

struct icarus::CAEN2795EventBlock {

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


class icarus::CAEN2795FragmentMetadata {

public:

  typedef uint32_t data_t; //fundamental unit of metadata
  typedef data_t   id_t; //type for board IDs

  CAEN2795FragmentMetadata(){}
  CAEN2795FragmentMetadata(data_t run_number,
			   data_t n_boards,
			   data_t channels_per_board,
			   data_t samples_per_channel,
			   data_t adcs_per_sample,
			   std::vector<id_t> const& idvec)
  { 
    _run_number = run_number;
    _num_boards = n_boards;
    _channels_per_board = channels_per_board;
    _samples_per_channel = samples_per_channel;
    _num_adc_bits = adcs_per_sample;
    SetBoardIDs(idvec);
  }

  data_t const& run_number() const { return _run_number; }
  data_t const& samples_per_channel() const { return _samples_per_channel; }
  data_t const& num_adc_bits() const { return _num_adc_bits; }
  data_t const& channels_per_board() const { return _channels_per_board; }
  data_t const& num_boards() const { return _num_boards; }
  
  id_t   const& board_id(size_t i) const
  { BoardExists(i); return _board_ids[i]; }

  void  SetBoardID(size_t i,id_t id)
  { BoardExists(i); _board_ids[i] = id; }
  void  SetBoardIDs(std::vector<id_t> const& idvec)
  { CheckNBoards(idvec.size()); _board_ids = idvec; } 

  void BoardExists(size_t i) const;
  void CheckNBoards(size_t i) const;

  size_t ExpectedDataSize() const 
  { return _num_boards*(sizeof(CAEN2795DataBlock::Header)+2*sizeof(uint32_t)+_channels_per_board*_samples_per_channel*sizeof(uint16_t)); }


private:

  data_t _run_number;
  data_t _samples_per_channel;  
  data_t _num_adc_bits;
  data_t _channels_per_board;
  data_t _num_boards;
  std::vector<id_t> _board_ids;
  
  void UpdateBoardIDSize(){ _board_ids.resize(_num_boards); }

};

class icarus::CAEN2795Fragment {

  public:

  CAEN2795Fragment(artdaq::Fragment const & f) : artdaq_Fragment_(f) {}

  CAEN2795FragmentMetadata const * metadata() const { return artdaq_Fragment_.metadata<CAEN2795FragmentMetadata>(); }

  size_t nBoards() const { return metadata()->num_boards(); }
  size_t nChannels() const { return metadata()->num_boards()*metadata()->channels_per_board(); }
  size_t nSamplesPerChannel() const { return metadata()->samples_per_channel(); }
  size_t nChannelsPerBoard() const { return metadata()->channels_per_board(); }

  //typedef CAEN2795BoardBlock<metadata()->samples_per_channel(),metadtat()->channels_per_board()> CAEN2795BoardBlock_t;

  size_t BoardBlockSize() const { return sizeof(CAEN2795DataBlock::Header_t)+2*sizeof(uint32_t)+nChannelsPerBoard()*nSamplesPerChannel()*sizeof(uint16_t); }
  size_t DataPayloadSize() const { return artdaq_Fragment_.dataSizeBytes(); }

  CAEN2795DataBlock const * CAEN2795Board(uint16_t b) const {
    metadata()->BoardExists(b);
    return ( reinterpret_cast< CAEN2795DataBlock const *>
	     (artdaq_Fragment_.dataBeginBytes() + b*BoardBlockSize()) );
  }


  CAEN2795DataBlock::Header const * CAEN2795BoardHeader(uint16_t b) const
  { return &(CAEN2795Board(b)->header);  }
  CAEN2795DataBlock::Data const * CAEN2795BoardData(uint16_t b) const
  { return &(CAEN2795Board(b)->data);  }


  uint32_t BoardEventNumber(uint16_t b) const { return CAEN2795BoardData(b)->event_number; }
  uint32_t BoardTimeStamp(uint16_t b) const { return CAEN2795BoardData(b)->time_stamp; }
  uint16_t const* BoardData(uint16_t b) const { return CAEN2795BoardData(b)->data; }

  uint16_t adc_val(size_t b,size_t c, size_t s) const 
  { return ( *(CAEN2795BoardData(b)->data+s*nChannelsPerBoard()+c) & (~(1<<(metadata()->num_adc_bits()+1))) ); }

  bool Verify() const;

private:

  artdaq::Fragment const & artdaq_Fragment_;

};

class icarus::CAEN2795StatFragment {

  public:

  CAEN2795StatFragment(artdaq::Fragment const & f) : artdaq_Fragment_(f) {}

  CAEN2795Stat const * metadata() const { return artdaq_Fragment_.metadata<CAEN2795FragmentMetadata>(); }
  CAEN2795Stat const * statpack() const { return metadata(); }

  uint32_t const& stat_token() const { return statpk()->token; }
  uint32_t const& fifo_status_1() const { return statpk()->fifo_status_1; }
  uint32_t        fifo_status_2() const { return statpk()->fifo_status_2; }
  uint64_t        fifo_status() const 
  { uint64_t fifo_status(statpk()->fifo_status_2); (fifo_status << 32); fifo_status+=statpk()->fifo_status_1; return fifo_status; }

  uint32_t const& stat_abs_time() const { return statpk()->abstime; }
  uint32_t const& stat_min_time() const { return statpk()->mintime; }

  uint8_t         stat_packet_format_version() const { return statpk()->pkt_fmt_ver; }
  uint32_t        crate_id() const { return statpk()->crate_it; }
  uint32_t const& size() const { return statpk()->size; }

  size_t DataPayloadSize() const { return artdaq_Fragment_.dataSizeBytes(); }

  bool Verify() const;

private:

  artdaq::Fragment const & artdaq_Fragment_;

};


#endif /* artdaq_demo_Overlays_ToyFragment_hh */
