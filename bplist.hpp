
// ep128emu -- portable Enterprise 128 emulator
// Copyright (C) 2003-2006 Istvan Varga <istvanv@users.sourceforge.net>
// http://sourceforge.net/projects/ep128emu/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef EP128EMU_BPLIST_HPP
#define EP128EMU_BPLIST_HPP

#include "ep128.hpp"
#include <vector>

namespace Ep128 {

  class BreakPoint {
   private:
    // 0x00000000: memory (16 bit address),
    // 0x04000000: memory (8 bit segment + 14 bit address),
    // 0x08000000: I/O (16 bit address)
    // + 0x01000000 if set for read
    // + 0x02000000 if set for write
    // + priority (0 to 3) * 0x00400000
    // + address
    uint32_t  n_;
   public:
    BreakPoint(bool isIO_, bool haveSegment_, bool r_, bool w_,
               uint8_t segment_, uint16_t addr_, int priority_)
    {
      this->n_ = (r_ ? 0x01000000 : 0x00000000)
                 + (w_ ? 0x02000000 : 0x00000000);
      if (!(this->n_))
        this->n_ = 0x03000000;
      this->n_ += (priority_ > 0 ?
                   (priority_ < 3 ? ((uint32_t) priority_ << 22) : 0x00C00000)
                   : 0x00000000);
      if (isIO_)
        this->n_ += (0x08000000 + (uint32_t) (addr_ & 0xFFFF));
      else if (haveSegment_)
        this->n_ += (0x04000000 + ((uint32_t) (segment_ & 0xFF) << 14)
                                + (uint32_t) (addr_ & 0x3FFF));
      else
        this->n_ += (0x00000000 + (uint32_t) (addr_ & 0xFFFF));
    }
    bool isIO() const
    {
      return !!(this->n_ & 0x08000000);
    }
    bool haveSegment() const
    {
      return !!(this->n_ & 0x04000000);
    }
    bool isRead() const
    {
      return !!(this->n_ & 0x01000000);
    }
    bool isWrite() const
    {
      return !!(this->n_ & 0x02000000);
    }
    int priority() const
    {
      return (int) ((this->n_ & 0x00C00000) >> 22);
    }
    uint8_t segment() const
    {
      if (this->haveSegment())
        return ((uint8_t) (this->n_ >> 14) & 0xFF);
      return 0;
    }
    uint16_t addr() const
    {
      if (this->haveSegment())
        return ((uint16_t) (this->n_ & 0x3FFF));
      return ((uint16_t) this->n_ & 0xFFFF);
    }
    bool operator<(const BreakPoint& bp) const
    {
      return (this->n_ < bp.n_);
    }
  };

  class BreakPointList {
   private:
    std::vector<BreakPoint> lst_;
   public:
    BreakPointList()
    {
    }
    BreakPointList(const std::string& lst);
    void addMemoryBreakPoint(uint8_t segment, uint16_t addr,
                             bool r, bool w, int priority);
    void addMemoryBreakPoint(uint16_t addr, bool r, bool w, int priority);
    void addIOBreakPoint(uint16_t addr, bool r, bool w, int priority);
    size_t getBreakPointCnt() const
    {
      return this->lst_.size();
    }
    const BreakPoint& getBreakPoint(size_t ndx) const
    {
      return ((const BreakPointList *) this)->lst_.at(ndx);
    }
    std::string getBreakPointList();
    void saveState(File::Buffer&);
    void saveState(File&);
    void loadState(File::Buffer&);
    void registerChunkType(File&);
  };

}       // namespace Ep128

#endif  // EP128EMU_BPLIST_HPP

