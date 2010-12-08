
// ep128emu -- portable Enterprise 128 emulator
// Copyright (C) 2003-2010 Istvan Varga <istvanv@users.sourceforge.net>
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

#ifndef EP128EMU_FDC765_HPP
#define EP128EMU_FDC765_HPP

#include "ep128emu.hpp"

namespace CPC464 {

  class FDC765 {
   public:
    struct FDCCommandParams {
      uint8_t   commandCode;
      uint8_t   unitNumber;             // drive number (0 to 3)
      uint8_t   physicalSide;
      uint8_t   cylinderID;
      uint8_t   headID;
      uint8_t   sectorID;
      uint8_t   sectorSizeCode;         // sector size = 0x80 << sectorSizeCode
      uint8_t   lastSectorID;
      uint8_t   gapLen;
      uint8_t   sectorDataLength;
      uint8_t   sectorCnt;
      uint8_t   fillerByte;
      uint8_t   statusRegister0;
      uint8_t   statusRegister1;
      uint8_t   statusRegister2;
      uint8_t   statusRegister3;
    };
    struct FDCSectorID {
      uint8_t   cylinderID;
      uint8_t   headID;
      uint8_t   sectorID;
      uint8_t   sectorSizeCode;
      bool      isDeleted;
    };
    typedef enum {
      CPCDISK_NO_ERROR = 0,
      CPCDISK_ERROR_UNKNOWN = -1,
      CPCDISK_ERROR_INVALID_COMMAND = -2,
      CPCDISK_ERROR_NO_DISK = -3,
      CPCDISK_ERROR_NOT_READY = -4,
      CPCDISK_ERROR_WRITE_PROTECTED = -5,
      CPCDISK_ERROR_INVALID_TRACK = -6,
      CPCDISK_ERROR_INVALID_SIDE = -7,
      CPCDISK_ERROR_BAD_CYLINDER = -8,
      CPCDISK_ERROR_WRONG_CYLINDER = -9,
      CPCDISK_ERROR_SECTOR_NOT_FOUND = -10,
      CPCDISK_ERROR_DELETED_SECTOR = -11,
      CPCDISK_ERROR_END_OF_CYLINDER = -12,
      CPCDISK_ERROR_READ_FAILED = -13,
      CPCDISK_ERROR_WRITE_FAILED = -14
    } CPCDiskError;
   protected:
    FDCCommandParams  cmdParams;
    uint32_t  timeCounter;              // updated at a rate of 500 Hz
    uint32_t  prvTimeCounter;
    uint32_t  totalDataBytes;
    uint32_t  dataBytesRemaining;
    // 0: idle
    // 1: command phase
    // 2: execution phase
    // 3: result phase
    uint8_t   fdcState;
    bool      dataDirectionIsRead;
    bool      motorOn;
    uint8_t   motorSpeed;               // should be >= 100 for drive ready
    bool      motorStateChanging;
    uint8_t   *sectorBuf;
    uint8_t   physicalCylinders[4];     // stored separately for each drive
    uint8_t   rotationAngles[4];        // 0 to 99 (300 RPM = 100 2ms ticks)
    // ----------------
    void updateStatusRegisters(CPCDiskError errorCode);
    void processFDCCommand();
    void setMotorState_(bool isEnabled);
    uint32_t updateDrives_();
    EP128EMU_INLINE uint32_t updateDrives()
    {
      if (timeCounter != prvTimeCounter)
        return updateDrives_(); // return number of 2ms ticks since last update
      return 0U;
    }
   public:
    FDC765();
    virtual ~FDC765();
    virtual void reset();
    inline bool getMotorState() const
    {
      return motorOn;
    }
    inline void setMotorState(bool isEnabled)
    {
      if (isEnabled != motorOn)
        setMotorState_(isEnabled);
    }
    virtual uint8_t readMainStatusRegister();
    virtual uint8_t readDataRegister();
    virtual void writeDataRegister(uint8_t n);
    virtual uint8_t readDataRegisterDebug();
    // read internal state for debugging
    virtual uint8_t debugRead(uint16_t addr);
    // returns onValue * (256 ^ driveNum) if a drive is active, or 0 otherwise
    // should be called at a rate of 500 Hz
    inline uint32_t getLEDState(uint8_t onValue)
    {
      timeCounter++;
      if (fdcState != 0)
        return (uint32_t(onValue) << (cmdParams.unitNumber << 3));
      return 0U;
    }
   protected:
    virtual bool haveDisk() const = 0;
    virtual bool getIsTrack0() const = 0;
    virtual bool getIsWriteProtected() const = 0;
    virtual uint8_t getPhysicalTrackSectors(int c) const = 0;
    virtual uint8_t getCurrentTrackSectors() const = 0;
    // returns the ID (C, H, R, N) of the physical sector specified by c, s
    // NOTE: the numbering of 's' starts from 0
    virtual bool getPhysicalSectorID(FDCSectorID& sectorID,
                                     int c, int s) const = 0;
    // returns the ID of a physical sector on the current cylinder
    virtual bool getPhysicalSectorID(FDCSectorID& sectorID, int s) const = 0;
    // returns the next physical sector (starting from 0) on the current
    // cylinder and side at rotation angle 'n' / 'd' (0 to 1, 0 is the ID
    // address mark of the first sector),
    // or -1 if there are no sectors or the specified parameters are invalid
    virtual int getPhysicalSector(int n, int d) const = 0;
    // returns the position (0 to 1) of the ID address mark of physical sector
    // 's' on the current cylinder and side, multiplied by 'd',
    // or -1 if the sector does not exist or the parameters are invalid
    virtual int getPhysicalSectorPos(int s, int d) const = 0;
    // read/write physical sector on the current cylinder
    virtual CPCDiskError readSector(int physicalSector,
                                    uint8_t& statusRegister1,
                                    uint8_t& statusRegister2) = 0;
    // NOTE: bit 6 of 'statusRegister2' is used as input (1 = deleted sector)
    virtual CPCDiskError writeSector(int physicalSector,
                                     uint8_t& statusRegister1,
                                     uint8_t& statusRegister2) = 0;
    virtual void stepIn(int nSteps = 1) = 0;
    virtual void stepOut(int nSteps = 1) = 0;
  };

}       // namespace CPC464

#endif  // EP128EMU_FDC765_HPP

