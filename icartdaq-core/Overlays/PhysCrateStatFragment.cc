#include "icartdaq-core/Overlays/PhysCrateStatFragment.hh"
#include <iostream>

#include "cetlib/exception.h"

std::ostream & icarus::operator << (std::ostream & os, struct PhysCrateStatBlock const & m) {

  os << "\nPhysCrateStatBlock" << std::dec
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

std::ostream & icarus::operator << (std::ostream & os, PhysCrateStatFragment const & f) {
  os << *(f.StatBlock());
}
bool icarus::CAEN2795StatFragment::Verify() const { return true; }

