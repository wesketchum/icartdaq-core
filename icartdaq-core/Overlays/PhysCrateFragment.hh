#ifndef icartdaq_Overlays_PhysCrateFragment_hh
#define icartdaq_Overlays_PhysCrateFragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"

#include <iostream>
#include <vector>

#include "ica_base/structures.h"

// Implementation of "PhysCrateFragment", an artdaq::Fragment overlay class
// used for ICARUS DAQ demo

namespace icarus {
  class PhysCrateFragment;
  std::ostream & operator << (std::ostream &, PhysCrateFragment const &);

  class PhysCrateFragmentMetadata;
  std::ostream & operator << (std::ostream &, PhysCrateFragmentMetadata const&);

  struct PhysCrateDataTileHeader;
  std::ostream & operator << (std::ostream &, struct PhysCrateDataTileHeader const&);

  struct A2795DataBlock;
  std::ostream & operator << (std::ostream &, struct A2795DataBlock const&);
}

//same structure that is found in structures.h
struct icarus::PhysCrateDataTileHeader{
  
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

  PhysCrateDataTileHeader(){};
  PhysCrateDataTileHeader( struct DataTile::Header const& );
};
struct icarus::A2795DataBlock{

  typedef uint32_t header_t;
  typedef uint16_t data_t;

  typedef struct {
    header_t event_number : 24;
    header_t unused1      :  8;
    header_t time_stamp;
  } Header;

  Header  header;
  data_t* data;
};



class icarus::PhysCrateFragmentMetadata {
  
public:
  
  typedef uint32_t data_t; //fundamental unit of metadata
  typedef data_t   id_t; //type for board IDs
  
  PhysCrateFragmentMetadata(){}
  PhysCrateFragmentMetadata(data_t run_number,
			    data_t n_boards,
			    data_t channels_per_board,
			    data_t samples_per_channel,
			    data_t adcs_per_sample,
			    data_t compression,
			   std::vector<id_t> const& idvec)
  { 
    _run_number = run_number;
    _num_boards = n_boards;
    _channels_per_board = channels_per_board;
    _samples_per_channel = samples_per_channel;
    _num_adc_bits = adcs_per_sample;
    _compression_scheme = compression;
    SetBoardIDs(idvec);
  }

  data_t const& run_number() const { return _run_number; }
  data_t const& samples_per_channel() const { return _samples_per_channel; }
  data_t const& num_adc_bits() const { return _num_adc_bits; }
  data_t const& channels_per_board() const { return _channels_per_board; }
  data_t const& num_boards() const { return _num_boards; }
  data_t const& compression_scheme() const { return _compression_scheme; }

  id_t   const& board_id(size_t i) const
  { BoardExists(i); return _board_ids[i]; }

  void  SetBoardID(size_t i,id_t id)
  { BoardExists(i); _board_ids[i] = id; }
  void  SetBoardIDs(std::vector<id_t> const& idvec)
  { CheckNBoards(idvec.size()); _board_ids = idvec; } 

  void BoardExists(size_t i) const;
  void CheckNBoards(size_t i) const;

  size_t ExpectedDataSize() const 
  { return sizeof(PhysCrateDataTileHeader) + 
      _num_boards*(sizeof(A2795DataBlock::Header)+_channels_per_board*_samples_per_channel*sizeof(A2795DataBlock::data_t)); }


private:

  data_t _run_number;
  data_t _samples_per_channel;  
  data_t _num_adc_bits;
  data_t _channels_per_board;
  data_t _num_boards;
  data_t _compression_scheme;
  std::vector<id_t> _board_ids;
  
  void UpdateBoardIDSize(){ _board_ids.resize(_num_boards); }

};

class icarus::PhysCrateFragment {

  public:

  PhysCrateFragment(artdaq::Fragment const & f) : artdaq_Fragment_(f) {}

  PhysCrateFragmentMetadata const * metadata() const { return artdaq_Fragment_.metadata<PhysCrateFragmentMetadata>(); }

  size_t RunNumber() const { return metadata()->run_number(); }
  size_t nBoards() const { return metadata()->num_boards(); }
  size_t nChannels() const { return metadata()->num_boards()*metadata()->channels_per_board(); }
  size_t nSamplesPerChannel() const { return metadata()->samples_per_channel(); }
  size_t nChannelsPerBoard() const { return metadata()->channels_per_board(); }
  size_t CompressionScheme() const { return metadata()->compression_scheme(); }

  bool   isCompressed() const { return (CompressionScheme()==0); }

  size_t DataPayloadSize() const { return artdaq_Fragment_.dataSizeBytes(); }

  PhysCrateDataTileHeader const * DataTileHeader() const {
    return ( reinterpret_cast< PhysCrateDataTileHeader const *>(artdaq_Fragment_.dataBeginBytes()) );
  }

  size_t BoardBlockSize() const { return sizeof(A2795DataBlock::Header)+nChannelsPerBoard()*nSamplesPerChannel()*sizeof(A2795DataBlock::data_t); }

  A2795DataBlock           const* BoardDataBlock(uint16_t b=0) const;
  A2795DataBlock::Header   const& BoardHeader(uint16_t b=0) const;
  A2795DataBlock::header_t        BoardEventNumber(uint16_t b=0) const;
  A2795DataBlock::header_t        BoardTimeStamp(uint16_t b=0) const;
  A2795DataBlock::data_t   const* BoardData(uint16_t b=0) const;

  A2795DataBlock::data_t adc_val(size_t b,size_t c,size_t s) const;

  bool Verify() const;

private:

  artdaq::Fragment const & artdaq_Fragment_;

  void   throwIfCompressed() const;

};


#endif /* artdaq_demo_Overlays_ToyFragment_hh */
