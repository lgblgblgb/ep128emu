
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

#ifndef EP128EMU_FILEIO_HPP
#define EP128EMU_FILEIO_HPP

#include "ep128.hpp"
#include <map>

namespace Ep128 {

  class File {
   public:
    class Buffer {
     private:
      unsigned char *buf;
      size_t  curPos, dataSize, allocSize;
     public:
      unsigned char readByte();
      bool readBoolean();
      int32_t readInt32();
      uint32_t readUInt32();
      double readFloat();
      std::string readString();
      void writeByte(unsigned char n);
      void writeBoolean(bool n);
      void writeInt32(int32_t n);
      void writeUInt32(uint32_t n);
      void writeFloat(double n);
      void writeString(const std::string& n);
      void writeData(const unsigned char *buf_, size_t nBytes);
      void setPosition(size_t pos);
      void clear();
      inline size_t getPosition() const
      {
        return curPos;
      }
      inline size_t getDataSize() const
      {
        return dataSize;
      }
      inline const unsigned char * getData() const
      {
        return buf;
      }
      Buffer();
      Buffer(const unsigned char *buf_, size_t nBytes);
      ~Buffer();
    };
    // ----------------
    typedef enum {
      EP128EMU_CHUNKTYPE_END_OF_FILE =    0x00000000,
      EP128EMU_CHUNKTYPE_Z80_STATE =      0x45508001,
      EP128EMU_CHUNKTYPE_MEMORY_STATE =   0x45508002,
      EP128EMU_CHUNKTYPE_IO_STATE =       0x45508003,
      EP128EMU_CHUNKTYPE_DAVE_STATE =     0x45508004,
      EP128EMU_CHUNKTYPE_NICK_STATE =     0x45508005,
      EP128EMU_CHUNKTYPE_BREAKPOINTS =    0x45508006,
      EP128EMU_CHUNKTYPE_CONFIG_DB =      0x45508007
    } ChunkType;
    // ----------------
    class ChunkTypeHandler {
     public:
      ChunkTypeHandler()
      {
      }
      virtual ~ChunkTypeHandler();
      virtual ChunkType getChunkType() const = 0;
      virtual void processChunk(Buffer& buf) = 0;
    };
   private:
    Buffer  buf;
    std::map< int, ChunkTypeHandler * > chunkTypeDB;
   public:
    void addChunk(ChunkType type, const Buffer& buf_);
    void processAllChunks();
    void writeFile(const char *fileName, bool useHomeDirectory = false);
    void registerChunkType(ChunkTypeHandler *);
    File();
    File(const char *fileName, bool useHomeDirectory = false);
    ~File();
  };

}       // namespace Ep128

#endif  // EP128EMU_FILEIO_HPP

