#include "icartdaq-core/Overlays/CAEN2795Fragment.hh"
#include <iostream>

#include "cetlib/exception.h"

void icarus::CAEN2795FragmentMetadata::BoardExists(size_t i) const {
  if(i>_num_boards)
    throw cet::exception("CAEN2795FragmentMetadata::BoardExists") << 
      "Called for board " << i << " when only " << _num_boards << " boards declared.";
}

void icarus::CAEN2795FragmentMetadata::CheckNBoards(size_t i) const {
  if(i!=_num_boards)
    throw cet::exception("CAEN2795FragmentMetadata::CheckNBoards") << 
      "Failed. Asked for " << i << " when " << _num_boards << " boards declared.";
}

std::ostream & icarus::operator << (std::ostream & os, CAEN2795FragmentMetadata const& m){

  os << "\nCAEN2785FragmentMetadata:" << std::dec
     << "\n\tRun Number: " << m.run_number() 
     << "\n\tNumber of ADC bits: " << m.num_adc_bits()
     << "\n\tSamples per channel: " << m.samples_per_channel()
     << "\n\tChannels per board: " << m.channels_per_board()
     << "\n\tNumber of boards: " << m.num_boards();
    for(size_t i_b=0; i_b<m.num_boards(); ++i_b)
      os << "\n\t\tBoard " << i_b << " ID: " << m.board_id(i_b);
    os << std::endl;
    return os;
}

std::ostream & icarus::operator << (std::ostream & os, CAEN2795Stat const & c) {
  auto const& m = c.stat_pack();

  os << "\nCAEN2785StatBlock" << std::dec
     << "\n\tSTAT Token: " << m.stat_token()
     << "\n\tFIFO Status 1: " << std::hex << m.fifo_status_1()
     << "\n\tFIFO Status 2: " << std::hex << m.fifo_status_2()
     << "\n\tFIFO Status: " << std::hex << m.fifo_status()
     << "\n\tAbs time: " << std::dec << m.stat_abs_time()
     << "\n\tMin time: " << m.stat_min_time()
     << "\n\tPacket format version: " << m.stat_packet_format_version()
     << "\n\tCrate id: " << m.crate_id()
     << "\n\tSize: " << m.size();
}

std::ostream & icarus::operator << (std::ostream & os, CAEN2795StatFragment const & f) {
  os << *(f.metadata());
}
bool icarus::CAEN2795StatFragment::Verify() const { return true; }

std::ostream & icarus::operator << (std::ostream & os, CAEN2795Fragment const & f) {
  os << "CAEN2795Fragment: "
     << *(f.metadata());
  os << "\n\tNumber of boards: " << f.nBoards();
  os << "\n\tNumber of channels: " << f.nChannels();
  os << "\n\tBoardBlockSize: " << f.BoardBlockSize();
  os << "\n\tDataPayloadSize: " << f.DataPayloadSize();
  for(size_t i_b=0; i_b<f.nBoards(); ++i_b)
    os << "\n\t\tBoard " << i_b << " (Event Number,Time Stamp)=(" 
       << f.BoardEventNumber(i_b) << "," << f.BoardTimeStamp(i_b) << ")";

  os << std::endl;
  return os;
}

bool icarus::CAEN2795Fragment::Verify() const {
  bool verified=true;
  if(artdaq_Fragment_.dataSizeBytes()!=(BoardBlockSize()*nBoards())){
    std::cout << "CAEN2795Fragment::Verify : Data size not correct!"
	      << "\n\tObserved=" << artdaq_Fragment_.dataSizeBytes() 
	      << ", Expected=" << BoardBlockSize()*nBoards() << std::endl;
    verified=false;
  }
  auto const& ev=BoardEventNumber(0);
  auto const& ts=BoardTimeStamp(0);
  bool boards_agree=true;
  for(size_t i_b=0; i_b<nBoards(); ++i_b)
    if(BoardEventNumber(i_b)!=ev || BoardTimeStamp(i_b)!=ts){
      boards_agree=false;
      break;
    }
  if(!boards_agree){
    std::cout << "CAEN2795Fragment::Verify : Boards don't agree." << std::endl;
    for(size_t i_b=0; i_b<nBoards(); ++i_b)
      std::cout << "\n\tBoard " << i_b << " (Event Number,Time Stamp)=(" 
		<< BoardEventNumber(i_b) << "," << BoardTimeStamp(i_b);
    verified=false;
  }
  
  return verified;
    
}
