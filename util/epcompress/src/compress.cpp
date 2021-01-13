
// compressor utility for Enterprise 128 programs
// Copyright (C) 2007-2019 Istvan Varga <istvanv@users.sourceforge.net>
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

#include "ep128emu.hpp"
#include "compress.hpp"
#include "compress3.hpp"
#include "decompm2.hpp"
#include "decompress3.hpp"

static void defaultProgressMessageCb(void *userData, const char *msg)
{
  (void) userData;
  if (msg != (char *) 0 && msg[0] != '\0')
    std::fprintf(stderr, "%s\n", msg);
}

static bool defaultProgressPercentageCb(void *userData, int n)
{
  (void) userData;
  if (n != 100)
    std::fprintf(stderr, "\r  %3d%%    ", n);
  else
    std::fprintf(stderr, "\r  %3d%%    \n", n);
  return true;
}

namespace Ep128Compress {

  void Compressor::CompressionParameters::limitParameters()
  {
    if (optimizeIterations < 2)
      optimizeIterations = 2;
    if (optimizeIterations > 64)
      optimizeIterations = 64;
    if (splitOptimizationDepth < 1)
      splitOptimizationDepth = 1;
    if (splitOptimizationDepth > 10)
      splitOptimizationDepth = 10;
    if (minLength < 1)
      minLength = 1;
    if (minLength > 3)
      minLength = 3;
    if (maxOffset < 1)
      maxOffset = 1;
    if (maxOffset > 524288)
      maxOffset = 524288;
    if (blockSize < 1)
      blockSize = 0;
    else if (blockSize < 16)
      blockSize = 16;
    if (blockSize > 65536)
      blockSize = 65536;
  }

  Compressor::CompressionParameters::CompressionParameters()
    : optimizeIterations(40),
      splitOptimizationDepth(1),
      minLength(1),
      maxOffset(65536),
      blockSize(0)
  {
  }

  Compressor::CompressionParameters::CompressionParameters(
      const CompressionParameters& r)
  {
    optimizeIterations = r.optimizeIterations;
    splitOptimizationDepth = r.splitOptimizationDepth;
    minLength = r.minLength;
    maxOffset = r.maxOffset;
    blockSize = r.blockSize;
    this->limitParameters();
  }

  Compressor::CompressionParameters&
      Compressor::CompressionParameters::operator=(
          const CompressionParameters& r)
  {
    optimizeIterations = r.optimizeIterations;
    splitOptimizationDepth = r.splitOptimizationDepth;
    minLength = r.minLength;
    maxOffset = r.maxOffset;
    blockSize = r.blockSize;
    this->limitParameters();
    return (*this);
  }

  void Compressor::CompressionParameters::setCompressionLevel(int n)
  {
    n = (n > 1 ? (n < 10 ? n : 10) : 1);
    optimizeIterations = 40;
    splitOptimizationDepth = size_t(n);
  }

  // --------------------------------------------------------------------------

  void Compressor::progressMessage(const char *msg)
  {
    if (msg == (char *) 0)
      msg = "";
    progressMessageCallback(progressMessageUserData, msg);
  }

  bool Compressor::setProgressPercentage(int n)
  {
    n = (n > 0 ? (n < 100 ? n : 100) : 0);
    if (n != prvProgressPercentage) {
      prvProgressPercentage = n;
      return progressPercentageCallback(progressPercentageUserData, n);
    }
    return true;
  }

  Compressor::Compressor(std::vector< unsigned char >& outBuf_)
    : outBuf(outBuf_),
      progressCnt(0),
      progressMax(1),
      progressDisplayEnabled(false),
      prvProgressPercentage(-1),
      progressMessageCallback(&defaultProgressMessageCb),
      progressMessageUserData((void *) 0),
      progressPercentageCallback(&defaultProgressPercentageCb),
      progressPercentageUserData((void *) 0),
      config()
  {
    outBuf.clear();
  }

  Compressor::~Compressor()
  {
  }

  void Compressor::getCompressionParameters(CompressionParameters& cfg) const
  {
    cfg = config;
  }

  void Compressor::setCompressionParameters(const CompressionParameters& cfg)
  {
    config = cfg;
  }

  void Compressor::setCompressionLevel(int n)
  {
    config.setCompressionLevel(n);
  }

  void Compressor::setProgressMessageCallback(
      void (*func)(void *userData, const char *msg), void *userData_)
  {
    if (func) {
      progressMessageCallback = func;
      progressMessageUserData = userData_;
    }
    else {
      progressMessageCallback = &defaultProgressMessageCb;
      progressMessageUserData = (void *) 0;
    }
  }

  void Compressor::setProgressPercentageCallback(
      bool (*func)(void *userData, int n), void *userData_)
  {
    if (func) {
      progressPercentageCallback = func;
      progressPercentageUserData = userData_;
    }
    else {
      progressPercentageCallback = &defaultProgressPercentageCb;
      progressPercentageUserData = (void *) 0;
    }
  }

  // --------------------------------------------------------------------------

  Decompressor::Decompressor()
  {
  }

  Decompressor::~Decompressor()
  {
  }

  // --------------------------------------------------------------------------

  Compressor * createCompressor(int compressionType,
                                std::vector< unsigned char >& outBuf)
  {
    switch (compressionType) {
    case 3:
      return new Compressor_M3(outBuf);
    }
    throw Ep128Emu::Exception("internal error: invalid compression type");
  }

  Decompressor * createDecompressor(int compressionType)
  {
    switch (compressionType) {
    case 3:
      return new Decompressor_M3();
    }
    throw Ep128Emu::Exception("internal error: invalid compression type");
  }

  int decompressData(std::vector< std::vector< unsigned char > >& outBuf,
                     const std::vector< unsigned char >& inBuf,
                     int compressionType)
  {
    if (compressionType >= 0 && compressionType != 3)
      throw Ep128Emu::Exception("internal error: invalid compression type");
    if (compressionType < 0) {
      // auto-detect compression type
      compressionType = 3;
    }
    Decompressor  *decomp = createDecompressor(compressionType);
    try {
      decomp->decompressData(outBuf, inBuf);
    }
    catch (...) {
      delete decomp;
      throw;
    }
    delete decomp;
    return compressionType;
  }

  int decompressData(std::vector< unsigned char >& outBuf,
                     const std::vector< unsigned char >& inBuf,
                     int compressionType)
  {
    if (compressionType >= 0 && (compressionType < 2 || compressionType > 3))
      throw Ep128Emu::Exception("internal error: invalid compression type");
    if (compressionType < 0) {
      // auto-detect compression type
      try {
        Ep128Emu::decompressData(outBuf, &(inBuf.front()), inBuf.size());
        return 2;
      }
      catch (Ep128Emu::Exception) {
      }
      compressionType = 3;
    }
    if (compressionType == 2) {
      Ep128Emu::decompressData(outBuf, &(inBuf.front()), inBuf.size());
      return 2;
    }
    Decompressor  *decomp = createDecompressor(compressionType);
    try {
      decomp->decompressData(outBuf, inBuf);
    }
    catch (...) {
      delete decomp;
      throw;
    }
    delete decomp;
    return compressionType;
  }

}       // namespace Ep128Compress

