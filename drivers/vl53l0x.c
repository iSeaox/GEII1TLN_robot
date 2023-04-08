/* 
 * File:   vl53l0x.c
 * Author: paolocarlobernardi
 */

/*   Copyright 2017 Paolo Carlo Bernardi

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#include "vl53l0x.h"

/*
 * Before enabling see readme file
 */
//#define LONG_RANGE        // Can be used also with CONTINUOUS
//#define HIGH_ACCURACY
//#define HIGH_SPEED
//#define SINGLE_SHOT
#define CONTINUOUS


/*
 * Module init
 */
bool VL53L0X_Init(VL53L0XDEV *p53) {
    VL53L0X_ClearStruct(p53);
    VL53L0X_DataInit(p53);
    VL53L0X_StaticInit(p53);
    VL53L0X_PerformRefCalibration(p53);    
    return true;
}


/*
 * Write a single byte to a single register of the addressed device 
 */
void WriteRegister(uint8_t addr, uint8_t reg, uint8_t val) {
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK writeTRB[1];
    uint8_t  writeBuffer[2];

    writeBuffer[0] = reg;
    writeBuffer[1] = val;

    I2C1_MasterWriteTRBBuild(&writeTRB[0], writeBuffer, 2, addr);
    while(status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterTRBInsert(1, writeTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;
    }
}


/*
 * Write a 16bit value to a register of the addressed device 
 */
void WriteRegister16(uint8_t addr, uint8_t reg, uint16_t val) {
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK writeTRB[1];
    uint8_t  writeBuffer[3];

    writeBuffer[0] = reg;
    writeBuffer[1] = (val >> 8);        // high value
    writeBuffer[2] = (uint8_t)(val);    // low value

    I2C1_MasterWriteTRBBuild(&writeTRB[0], writeBuffer, 3, addr);
    while(status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterTRBInsert(1, writeTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;
    }
    
}


/*
 * Write a 32bit value to a register of the addressed device 
 */
void WriteRegister32(uint8_t addr, uint8_t reg, uint32_t val) {
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK writeTRB[1];
    uint8_t  writeBuffer[5];

    writeBuffer[0] = reg;
    writeBuffer[1] = (val >> 24);        // high value
    writeBuffer[2] = (val >> 16);
    writeBuffer[3] = (val >> 8);
    writeBuffer[4] = (uint8_t)(val);     // low value

    I2C1_MasterWriteTRBBuild(&writeTRB[0], writeBuffer, 5, addr);
    while(status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterTRBInsert(1, writeTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;
    }
    
}


/*
 * It writes nCount registers starting from dataAddress, fetching from pData array
 */
void WriteRegisters(uint8_t addr, uint8_t dataAddress, uint8_t *pData, uint8_t nCount) {
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK writeTRB[2];
    uint8_t writeBuffer[2];
    uint16_t timeOut;

    status = I2C1_MESSAGE_PENDING;

    // build the write buffer first
    // starting address of the register to read
    writeBuffer[0] = dataAddress;

    // Build TRB for sending address
    I2C1_MasterWriteTRBBuild(&writeTRB[0], writeBuffer, 2, addr);
    I2C1_MasterWriteTRBBuild(&writeTRB[1], pData, nCount, addr);

    timeOut = 0;

    while(status != I2C1_MESSAGE_FAIL)
    {
        I2C1_MasterTRBInsert(2, writeTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // check for max retry and skip this byte
        if (timeOut == 100)
            break;
        else
            timeOut++;
    }    
}


/*
 * Read a single register value of the addressed device
 */
uint8_t ReadRegister(uint8_t addr, uint8_t reg) {
    uint8_t regValue;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    
    I2C1_MasterWriteTRBBuild(&readTRB[0], &reg, 1, addr);
    I2C1_MasterReadTRBBuild(&readTRB[1], &regValue, 1, addr);

    while(status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterTRBInsert(2, readTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;
    }
    
    return regValue;
}


/*
 * Read a single 16 bit register value of the addressed device
 */
uint16_t ReadRegister16(uint8_t addr, uint8_t reg) {
    uint8_t regValueL, regValueH;
    uint16_t regValue;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[3];
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    
    I2C1_MasterWriteTRBBuild(&readTRB[0], &reg, 1, addr);
    I2C1_MasterReadTRBBuild(&readTRB[1], &regValueH, 1, addr);
    I2C1_MasterReadTRBBuild(&readTRB[2], &regValueL, 1, addr);

    while(status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterTRBInsert(3, readTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;
    }
    
    regValue = (uint16_t)regValueH << 8;
    regValue |= regValueL;
    
    return regValue;
}


/*
 * Reads nCount registers into pData array starting from dataAddress.
 */
void ReadRegisters(uint8_t addr, uint8_t dataAddress, uint8_t *pData, uint8_t nCount) {
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    uint8_t writeBuffer[2];
    uint16_t timeOut;

    status = I2C1_MESSAGE_PENDING;

    // build the write buffer first
    // starting address of the register to read
    writeBuffer[0] = dataAddress;

    // Build TRB for sending address
    I2C1_MasterWriteTRBBuild(&readTRB[0], writeBuffer, 2, addr);
    
    // Build TRB for receiving data
    I2C1_MasterReadTRBBuild(&readTRB[1], pData, nCount, addr);

    timeOut = 0;

    while(status != I2C1_MESSAGE_FAIL)
    {
        I2C1_MasterTRBInsert(2, readTRB, &status);
        while(status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // check for max retry and skip this byte
        if (timeOut == 100)
            break;
        else
            timeOut++;
    }
}


/*
 * Change the default i2c address value to newAddr.
 * Prerequisite: Only one sensor at a time must be enabled.
 */
void VL53L0X_SetAddress(uint8_t newAddr)
{
    WriteRegister(VL53L0X_I2C_DEFAULT_ADDRESS, 
            VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS, newAddr);
}


/*
 * Extracted from ST API DataInit function and Arduino library
 */
void VL53L0X_DataInit(VL53L0XDEV *p53) {
    // we don't need to set i2c standard mode (100KHz))
    // I2cWriteRegister(addr, 0x88, 0x00);
    
    // set logical level to 2v8
    WriteRegister(p53->address, VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, 0x01);
    
    // meaning of these settings is unknown
    WriteRegister(p53->address, 0x80, 0x01);
    WriteRegister(p53->address, 0xFF, 0x01);
    WriteRegister(p53->address, 0x00, 0x00);
    p53->stopVariable = ReadRegister(p53->address, 0x91);
    WriteRegister(p53->address, 0x00, 0x01);
    WriteRegister(p53->address, 0xFF, 0x00);
    WriteRegister(p53->address, 0x80, 0x00);

    // disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
    uint8_t oldVal = ReadRegister(p53->address, VL53L0X_REG_MSRC_CONFIG_CONTROL); 
    WriteRegister(p53->address, VL53L0X_REG_MSRC_CONFIG_CONTROL, oldVal | 0x12);
    
    // set final range signal rate limit to 0.25 MCPS (million counts per second)
    VL53L0X_SetSignalRateLimit(p53->address, 0.25);
    
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, 0xFF);
}


/*
 * Extracted from ST API StaticInit function and Arduino library
 */
bool VL53L0X_StaticInit(VL53L0XDEV *p53) {
    uint8_t spadCount;
    bool spadTypeIsAperture;
    uint8_t refSpadMap[6];

    if (!VL53L0X_GetSpadInfo(p53, &spadCount, &spadTypeIsAperture)) { 
        return false;
    }

    /* From Arduino library */
    // The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
    // the API, but the same data seems to be more easily readable from
    // GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
    ReadRegisters(p53->address, VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, refSpadMap, 6);

    VL53L0X_SetReferenceSpads(p53->address, &spadCount, &spadTypeIsAperture, refSpadMap);
    VL53L0X_Tuning(p53->address);
    VL53L0X_SetGpioConfig(p53->address);
    p53->measurement_timing_budget_us = VL53L0X_getMeasurementTimingBudget(p53);
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, 0xE8);
    VL53L0X_setMeasurementTimingBudget(p53);
    
    return true;
}


/*
 * Extracted from ST API VL53L0X_perform_ref_calibration() function and Arduino library
 */
bool VL53L0X_PerformRefCalibration(VL53L0XDEV *p53) {
    
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, 0x01);
    if (!VL53L0X_performSingleRefCalibration(p53, 0x40)) { 
        return false;
    }
    
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, 0x02);
    if (!VL53L0X_performSingleRefCalibration(p53, 0x00)) { 
        return false; 
    }
    
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, 0xE8);
        
    return true;
}


/*
 * From Arduino library:
 */
// Set the return signal rate limit check value in units of MCPS (mega counts
// per second). "This represents the amplitude of the signal reflected from the
// target and detected by the device"; setting this limit presumably determines
// the minimum measurement necessary for the sensor to report a valid reading.
// Setting a lower limit increases the potential range of the sensor but also
// seems to increase the likelihood of getting an inaccurate reading because of
// unwanted reflections from objects other than the intended target.
// Defaults to 0.25 MCPS as initialized by the ST API and this library.
bool VL53L0X_SetSignalRateLimit(uint8_t addr, float limit_Mcps) {
  
    if (limit_Mcps < 0 || limit_Mcps > 511.99) {
      return false;
  }

  WriteRegister16(addr, 
          VL53L0X_REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, 
          (uint16_t)(limit_Mcps * (1 << 7)));
  
  return true;
}


/* 
 * From Arduino library
 */
// Get reference SPAD (single photon avalanche diode) count and type
// based on VL53L0X_get_info_from_device(),
// but only gets reference SPAD count and type
bool VL53L0X_GetSpadInfo(VL53L0XDEV *p53, uint8_t *count, bool *typeIsAperture) {
  uint8_t oldVal;
  
  WriteRegister(p53->address, 0x80, 0x01);
  WriteRegister(p53->address, 0xFF, 0x01);
  WriteRegister(p53->address, 0x00, 0x00);

  WriteRegister(p53->address, 0xFF, 0x06);
  oldVal = ReadRegister(p53->address, 0x83);
  WriteRegister(p53->address, 0x83, oldVal | 0x04);
  WriteRegister(p53->address, 0xFF, 0x07);
  WriteRegister(p53->address, 0x81, 0x01);

  WriteRegister(p53->address, 0x80, 0x01);

  WriteRegister(p53->address, 0x94, 0x6b);
  WriteRegister(p53->address, 0x83, 0x00);

  msecdata timeoutData;
  start_msTimeout(&timeoutData, p53->io_timeout);
  while (ReadRegister(p53->address, 0x83) == 0x00) {
      if (checkTimeoutExpired(&timeoutData)) { return false; }
  }
  
  WriteRegister(p53->address, 0x83, 0x01);
  uint8_t tmp = ReadRegister(p53->address, 0x92);

  *count = tmp & 0x7f;
  *typeIsAperture = (tmp >> 7) & 0x01;

  WriteRegister(p53->address, 0x81, 0x00);
  WriteRegister(p53->address, 0xFF, 0x06);
  oldVal = ReadRegister(p53->address, 0x83);
  WriteRegister(p53->address, 0x83, oldVal  & ~0x04);
  WriteRegister(p53->address, 0xFF, 0x01);
  WriteRegister(p53->address, 0x00, 0x01);

  WriteRegister(p53->address, 0xFF, 0x00);
  WriteRegister(p53->address, 0x80, 0x00);

  return true;
}

/*
 * VL53L0X_set_reference_spads()
 * From Arduino library. It assumes some restrictions ( NVM values are valid )
 * so, should be revisited.
 */
void VL53L0X_SetReferenceSpads(uint8_t addr, uint8_t *count, bool *typeIsAperture, uint8_t *spadMap) {
    
  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, VL53L0X_REG_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
  WriteRegister(addr, VL53L0X_REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, VL53L0X_REG_GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

  uint8_t firstSpadToEnable = *typeIsAperture ? 12 : 0; // 12 is the first aperture spad
  uint8_t spadsEnabled = 0;

  for (uint8_t i = 0; i < 48; i++)
  {
    if (i < firstSpadToEnable || spadsEnabled == *count)
    {
      // This bit is lower than the first one that should be enabled, or
      // (reference_spad_count) bits have already been enabled, so zero this bit
      spadMap[i / 8] &= ~(1 << (i % 8));
    }
    else if ((spadMap[i / 8] >> (i % 8)) & 0x1)
    {
      spadsEnabled++;
    }
  }

  WriteRegisters(addr, VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, spadMap, 6);
    
}


/*
 * VL53L0X_load_tuning_settings()
 * DefaultTuningSettings from vl53l0x_tuning.h
 * Arduino library version.
 */
void VL53L0X_Tuning(uint8_t addr) {
    
  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x00, 0x00);

  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x09, 0x00);
  WriteRegister(addr, 0x10, 0x00);
  WriteRegister(addr, 0x11, 0x00);

  WriteRegister(addr, 0x24, 0x01);
  WriteRegister(addr, 0x25, 0xFF);
  WriteRegister(addr, 0x75, 0x00);

  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x4E, 0x2C);
  WriteRegister(addr, 0x48, 0x00);
  WriteRegister(addr, 0x30, 0x20);
  
  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x30, 0x09);
  WriteRegister(addr, 0x54, 0x00);
  WriteRegister(addr, 0x31, 0x04);
  WriteRegister(addr, 0x32, 0x03);
  WriteRegister(addr, 0x40, 0x83);
  WriteRegister(addr, 0x46, 0x25);
  WriteRegister(addr, 0x60, 0x00);
  WriteRegister(addr, 0x27, 0x00);
  WriteRegister(addr, 0x50, 0x06);
  WriteRegister(addr, 0x51, 0x00);
  WriteRegister(addr, 0x52, 0x96);
  WriteRegister(addr, 0x56, 0x08);
  WriteRegister(addr, 0x57, 0x30);
  WriteRegister(addr, 0x61, 0x00);
  WriteRegister(addr, 0x62, 0x00);
  WriteRegister(addr, 0x64, 0x00);
  WriteRegister(addr, 0x65, 0x00);
  WriteRegister(addr, 0x66, 0xA0);

  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x22, 0x32);
  WriteRegister(addr, 0x47, 0x14);
  WriteRegister(addr, 0x49, 0xFF);
  WriteRegister(addr, 0x4A, 0x00);

  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x7A, 0x0A);
  WriteRegister(addr, 0x7B, 0x00);
  WriteRegister(addr, 0x78, 0x21);

  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x23, 0x34);
  WriteRegister(addr, 0x42, 0x00);
  WriteRegister(addr, 0x44, 0xFF);
  WriteRegister(addr, 0x45, 0x26);
  WriteRegister(addr, 0x46, 0x05);
  WriteRegister(addr, 0x40, 0x40);
  WriteRegister(addr, 0x0E, 0x06);
  WriteRegister(addr, 0x20, 0x1A);
  WriteRegister(addr, 0x43, 0x40);

  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x34, 0x03);
  WriteRegister(addr, 0x35, 0x44);

  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x31, 0x04);
  WriteRegister(addr, 0x4B, 0x09);
  WriteRegister(addr, 0x4C, 0x05);
  WriteRegister(addr, 0x4D, 0x04);

  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x44, 0x00);
  WriteRegister(addr, 0x45, 0x20);
  WriteRegister(addr, 0x47, 0x08);
  WriteRegister(addr, 0x48, 0x28);
  WriteRegister(addr, 0x67, 0x00);
  WriteRegister(addr, 0x70, 0x04);
  WriteRegister(addr, 0x71, 0x01);
  WriteRegister(addr, 0x72, 0xFE);
  WriteRegister(addr, 0x76, 0x00);
  WriteRegister(addr, 0x77, 0x00);

  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x0D, 0x01);

  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x80, 0x01);
  WriteRegister(addr, 0x01, 0xF8);

  WriteRegister(addr, 0xFF, 0x01);
  WriteRegister(addr, 0x8E, 0x01);
  WriteRegister(addr, 0x00, 0x01);
  WriteRegister(addr, 0xFF, 0x00);
  WriteRegister(addr, 0x80, 0x00);

  //Without this next function doesn't work. Not a real problem because
  //it is executed only at boot (but why it happens?)
  __delay_ms(500); 
        
}


/*
 * Set interrupt config to new sample ready
 */
void VL53L0X_SetGpioConfig(uint8_t addr) {
  WriteRegister(addr, VL53L0X_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
  uint8_t oldVal = ReadRegister(addr, VL53L0X_REG_GPIO_HV_MUX_ACTIVE_HIGH);
  WriteRegister(addr, VL53L0X_REG_GPIO_HV_MUX_ACTIVE_HIGH, oldVal & ~0x10); // active low
  WriteRegister(addr, VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR, 0x01);    
}


/*
 * From Arduino library
 */
// Get the measurement timing budget in microseconds
// based on VL53L0X_get_measurement_timing_budget_micro_seconds()
// in us
uint32_t VL53L0X_getMeasurementTimingBudget(VL53L0XDEV *p53)
{
    uint16_t const StartOverhead     = 1910; // note that this is different than the value in set_
    uint16_t const EndOverhead        = 960;
    uint16_t const MsrcOverhead       = 660;
    uint16_t const TccOverhead        = 590;
    uint16_t const DssOverhead        = 690;
    uint16_t const PreRangeOverhead   = 660;
    uint16_t const FinalRangeOverhead = 550;

    uint32_t budget_us = StartOverhead + EndOverhead;
    
    VL53L0X_getSequenceStepEnables(p53);
    VL53L0X_getSequenceStepTimeouts(p53);

    if (p53->enables.tcc)
    {
      budget_us += (p53->timeouts.msrc_dss_tcc_us + TccOverhead);
    }

    if (p53->enables.dss)
    {
      budget_us += 2 * (p53->timeouts.msrc_dss_tcc_us + DssOverhead);
    }
    else if (p53->enables.msrc)
    {
      budget_us += (p53->timeouts.msrc_dss_tcc_us + MsrcOverhead);
    }

    if (p53->enables.pre_range)
    {
      budget_us += (p53->timeouts.pre_range_us + PreRangeOverhead);
    }

    if (p53->enables.final_range)
    {
      budget_us += (p53->timeouts.final_range_us + FinalRangeOverhead);
    }

    return budget_us;
}


/*
 * From Arduino library
 */
// Get sequence step enables
// based on VL53L0X_GetSequenceStepEnables()
void VL53L0X_getSequenceStepEnables(VL53L0XDEV *p53)
{
  uint8_t sequence_config = ReadRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG);
  
  p53->enables.tcc          = (sequence_config >> 4) & 0x1;
  p53->enables.dss          = (sequence_config >> 3) & 0x1;
  p53->enables.msrc         = (sequence_config >> 2) & 0x1;
  p53->enables.pre_range    = (sequence_config >> 6) & 0x1;
  p53->enables.final_range  = (sequence_config >> 7) & 0x1;
}


/*
 * From Arduino library
 */
// Get sequence step timeouts
// based on get_sequence_step_timeout(),
// but gets all timeouts instead of just the requested one, and also stores
// intermediate values
void VL53L0X_getSequenceStepTimeouts(VL53L0XDEV *p53)
{
    uint8_t addr = p53->address;
    
    p53->timeouts.pre_range_vcsel_period_pclks = VL53L0X_getVcselPulsePeriod(addr, VcselPeriodPreRange);

    p53->timeouts.msrc_dss_tcc_mclks = ReadRegister(addr, VL53L0X_REG_MSRC_CONFIG_TIMEOUT_MACROP) + 1;
    p53->timeouts.msrc_dss_tcc_us = VL53L0X_timeoutMclksToMicroseconds(p53->timeouts.msrc_dss_tcc_mclks,
            p53->timeouts.pre_range_vcsel_period_pclks);

    p53->timeouts.pre_range_mclks = VL53L0X_decodeTimeout(ReadRegister16(addr, VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
    p53->timeouts.pre_range_us = VL53L0X_timeoutMclksToMicroseconds(p53->timeouts.pre_range_mclks,
            p53->timeouts.pre_range_vcsel_period_pclks);

    p53->timeouts.final_range_vcsel_period_pclks = VL53L0X_getVcselPulsePeriod(addr, VcselPeriodFinalRange);

    p53->timeouts.final_range_mclks =
      VL53L0X_decodeTimeout(ReadRegister16(addr, VL53L0X_REG_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

    if (p53->enables.pre_range)
    {
      p53->timeouts.final_range_mclks -= p53->timeouts.pre_range_mclks;
    }

    p53->timeouts.final_range_us =
      VL53L0X_timeoutMclksToMicroseconds(p53->timeouts.final_range_mclks, p53->timeouts.final_range_vcsel_period_pclks);
}


/*
 * From Arduino library
 */
// Get the VCSEL pulse period in PCLKs for the given period type.
// based on VL53L0X_get_vcsel_pulse_period()
uint8_t VL53L0X_getVcselPulsePeriod(uint8_t addr, vcselPeriodType type) {
  if (type == VcselPeriodPreRange) {
    return decodeVcselPeriod(ReadRegister(addr, VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD));
  }
  else if (type == VcselPeriodFinalRange) {
    return decodeVcselPeriod(ReadRegister(addr, VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD));
  }
  else { return 255; }
}


/*
 * From Arduino library
 */
// Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_us()
uint32_t VL53L0X_timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks) {
  uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);
  return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}


/*
 * From Arduino library
 */
// Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_mclks()
uint32_t VL53L0X_timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks) {
  uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);
  return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}


/*
 * From Arduino library
 */
// Decode sequence step timeout in MCLKs from register value
// based on VL53L0X_decode_timeout()
// Note: the original function returned a uint32_t, but the return value is
// always stored in a uint16_t.
uint16_t VL53L0X_decodeTimeout(uint16_t reg_val)
{
  // format: "(LSByte * 2^MSByte) + 1"
  return (uint16_t)((reg_val & 0x00FF) <<
         (uint16_t)((reg_val & 0xFF00) >> 8)) + 1;
}


/*
 * From Arduino library
 */
// Encode sequence step timeout register value from timeout in MCLKs
// based on VL53L0X_encode_timeout()
// Note: the original function took a uint16_t, but the argument passed to it
// is always a uint16_t.
uint16_t VL53L0X_encodeTimeout(uint16_t timeout_mclks)
{
  // format: "(LSByte * 2^MSByte) + 1"

  uint32_t ls_byte = 0;
  uint16_t ms_byte = 0;

  if (timeout_mclks > 0)
  {
    ls_byte = timeout_mclks - 1;

    while ((ls_byte & 0xFFFFFF00) > 0)
    {
      ls_byte >>= 1;
      ms_byte++;
    }

    return (ms_byte << 8) | (ls_byte & 0xFF);
  }
  else { return 0; }
}


/*
 * From Arduino library
 */
// Set the measurement timing budget in microseconds, which is the time allowed
// for one measurement; the ST API and this library take care of splitting the
// timing budget among the sub-steps in the ranging sequence. A longer timing
// budget allows for more accurate measurements. Increasing the budget by a
// factor of N decreases the range measurement standard deviation by a factor of
// sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
// based on VL53L0X_set_measurement_timing_budget_micro_seconds()
bool VL53L0X_setMeasurementTimingBudget(VL53L0XDEV *p53)
{
    uint16_t const StartOverhead      = 1320; // note that this is different than the value in get_
    uint16_t const EndOverhead        = 960;
    uint16_t const MsrcOverhead       = 660;
    uint16_t const TccOverhead        = 590;
    uint16_t const DssOverhead        = 690;
    uint16_t const PreRangeOverhead   = 660;
    uint16_t const FinalRangeOverhead = 550;

    uint32_t const MinTimingBudget = 20000;

    if (p53->measurement_timing_budget_us < MinTimingBudget) { return false; }

    uint32_t used_budget_us = StartOverhead + EndOverhead;

    VL53L0X_getSequenceStepEnables(p53);
    VL53L0X_getSequenceStepTimeouts(p53);

    if (p53->enables.tcc)
    {
      used_budget_us += (p53->timeouts.msrc_dss_tcc_us + TccOverhead);
    }

    if (p53->enables.dss)
    {
      used_budget_us += 2 * (p53->timeouts.msrc_dss_tcc_us + DssOverhead);
    }
    else if (p53->enables.msrc)
    {
      used_budget_us += (p53->timeouts.msrc_dss_tcc_us + MsrcOverhead);
    }

    if (p53->enables.pre_range)
    {
      used_budget_us += (p53->timeouts.pre_range_us + PreRangeOverhead);
    }

    if (p53->enables.final_range)
    {
    used_budget_us += FinalRangeOverhead;

    // "Note that the final range timeout is determined by the timing
    // budget and the sum of all other timeouts within the sequence.
    // If there is no room for the final range timeout, then an error
    // will be set. Otherwise the remaining time will be applied to
    // the final range."

    if (used_budget_us > p53->measurement_timing_budget_us)
    {
      // "Requested timeout too big."
      return false;
    }

    uint32_t final_range_timeout_us = p53->measurement_timing_budget_us - used_budget_us;

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

    // "For the final range timeout, the pre-range timeout
    //  must be added. To do this both final and pre-range
    //  timeouts must be expressed in macro periods MClks
    //  because they have different vcsel periods."

    uint16_t final_range_timeout_mclks = 
            VL53L0X_timeoutMicrosecondsToMclks(final_range_timeout_us, 
            p53->timeouts.final_range_vcsel_period_pclks);

    if (p53->enables.pre_range)
    {
        final_range_timeout_mclks += p53->timeouts.pre_range_mclks;
    }

    WriteRegister16(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, 
            VL53L0X_encodeTimeout(final_range_timeout_mclks));

    // set_sequence_step_timeout() end
    }
    
    return true;
}


/*
 * From Arduino library
 */
// based on VL53L0X_perform_single_ref_calibration()
bool VL53L0X_performSingleRefCalibration(VL53L0XDEV *p53, uint8_t vhv_init_byte) {
    
    WriteRegister(p53->address, VL53L0X_REG_SYSRANGE_START, 0x01 | vhv_init_byte); // VL53L0X_REG_SYSRANGE_MODE_START_STOP
    
    msecdata timeoutData;
    start_msTimeout(&timeoutData, p53->io_timeout);
    while ((ReadRegister(p53->address, VL53L0X_REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
        if (checkTimeoutExpired(&timeoutData)) { return false; }
    }
    
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR, 0x01);
    WriteRegister(p53->address, VL53L0X_REG_SYSRANGE_START, 0x00);

    return true;
}


void VL53L0X_SetTimeout(VL53L0XDEV *p53, uint16_t timeout) {
    p53->io_timeout = timeout;
}


void VL53L0X_ClearStruct(VL53L0XDEV *p53) {
    p53->did_timeout = false;
    p53->io_timeout = 0;
    p53->measurement_timing_budget_us = 0;
    p53->stopVariable = 0;
    p53->timeout_start_ms = 0;
}


/*
 * From Arduino library
 */
// Performs a single-shot range measurement and returns the reading in
// millimeters
// based on VL53L0X_PerformSingleRangingMeasurement()
uint16_t VL53L0X_SingleShotReading(VL53L0XDEV *p53) {
    
    VL53L0X_ReadingHeader(p53);
    WriteRegister(p53->address, VL53L0X_REG_SYSRANGE_START, 0x01);

    msecdata timeoutData;
    start_msTimeout(&timeoutData, p53->io_timeout);
    
    while (ReadRegister(p53->address, VL53L0X_REG_SYSRANGE_START) & 0x01)
    {
      if (checkTimeoutExpired(&timeoutData))
      {
          p53->did_timeout = true;
          return 65535;
      }
    }

    return VL53L0X_ReadRange(p53);
    
}


/*
 * From Arduino library
 * Activates the internal continuous reading of the sensor.
 * If period_ms = 0 it runs at the maximum speed allowed by the sensor.
 */
void VL53L0X_ContinuousReading(VL53L0XDEV *p53, uint32_t period_ms) {
    
    VL53L0X_ReadingHeader(p53);

    if (period_ms != 0)
    {
      VL53L0X_SetInterMeasurementPeriodMilliSeconds(p53, period_ms);
      WriteRegister(p53->address, VL53L0X_REG_SYSRANGE_START, 0x04);
    }
    else
    {
      WriteRegister(p53->address, VL53L0X_REG_SYSRANGE_START, 0x02);
    }
}


/*
 * From Arduino library
 */
void VL53L0X_StopContinuous(VL53L0XDEV *p53) {
  WriteRegister(p53->address, VL53L0X_REG_SYSRANGE_START, 0x01); // VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT

  WriteRegister(p53->address, 0xFF, 0x01);
  WriteRegister(p53->address, 0x00, 0x00);
  WriteRegister(p53->address, 0x91, 0x00);
  WriteRegister(p53->address, 0x00, 0x01);
  WriteRegister(p53->address, 0xFF, 0x00);
}


/*
 * From Arduino library
 */
// Returns a range reading in millimeters
uint16_t VL53L0X_ReadRange(VL53L0XDEV *p53) {
    
    msecdata timeoutData;
    start_msTimeout(&timeoutData, p53->io_timeout);

    while ((ReadRegister(p53->address, VL53L0X_REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
        if (checkTimeoutExpired(&timeoutData)) {
            p53->did_timeout = true;
            return 65535;
        }
    }

    // assumptions: Linearity Corrective Gain is 1000 (default);
    // fractional ranging is not enabled
    uint16_t range = ReadRegister16(p53->address, VL53L0X_REG_RESULT_RANGE_STATUS + 10);
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR, 0x01);

    return range;
}


bool VL53L0X_TimeoutOccurred(VL53L0XDEV *p53) {
  bool tmp = p53->did_timeout;
  p53->did_timeout = false;
  return tmp;
}


/*
 * From Arduino library
 */
// Set the VCSEL (vertical cavity surface emitting laser) pulse period for the
// given period type (pre-range or final range) to the given value in PCLKs.
// Longer periods seem to increase the potential range of the sensor.
// Valid values are (even numbers only):
//  pre:  12 to 18 (initialized default: 14)
//  final: 8 to 14 (initialized default: 10)
// based on VL53L0X_set_vcsel_pulse_period()
bool VL53L0X_SetVcselPulsePeriod(VL53L0XDEV *p53, vcselPeriodType type, uint8_t period_pclks) {
    
  uint8_t vcsel_period_reg = encodeVcselPeriod(period_pclks);

  VL53L0X_getSequenceStepEnables(p53);
  VL53L0X_getSequenceStepTimeouts(p53);

  // "Apply specific settings for the requested clock period"
  // "Re-calculate and apply timeouts, in macro periods"

  // "When the VCSEL period for the pre or final range is changed,
  // the corresponding timeout must be read from the device using
  // the current VCSEL period, then the new VCSEL period can be
  // applied. The timeout then must be written back to the device
  // using the new VCSEL period.
  //
  // For the MSRC timeout, the same applies - this timeout being
  // dependant on the pre-range vcsel period."


  if (type == VcselPeriodPreRange)
  {
    // "Set phase check limits"
    switch (period_pclks)
    {
      case 12:
        WriteRegister(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
        break;

      case 14:
        WriteRegister(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
        break;

      case 16:
        WriteRegister(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
        break;

      case 18:
        WriteRegister(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
        break;

      default:
        // invalid period
        return false;
    }
    WriteRegister(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

    // apply new VCSEL period
    WriteRegister(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    // update timeouts

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE)

    uint16_t new_pre_range_timeout_mclks =
      VL53L0X_timeoutMicrosecondsToMclks(p53->timeouts.pre_range_us, period_pclks);

    WriteRegister16(p53->address, VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
      VL53L0X_encodeTimeout(new_pre_range_timeout_mclks));

    // set_sequence_step_timeout() end

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)

    uint16_t new_msrc_timeout_mclks =
      VL53L0X_timeoutMicrosecondsToMclks(p53->timeouts.msrc_dss_tcc_us, period_pclks);

    WriteRegister(p53->address, VL53L0X_REG_MSRC_CONFIG_TIMEOUT_MACROP,
      (new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));

    // set_sequence_step_timeout() end
  }
  else if (type == VcselPeriodFinalRange)
  {
    switch (period_pclks)
    {
      case 8:
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        WriteRegister(p53->address, VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
        WriteRegister(p53->address, 0xFF, 0x01);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_LIM, 0x30);
        WriteRegister(p53->address, 0xFF, 0x00);
        break;

      case 10:
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        WriteRegister(p53->address, VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
        WriteRegister(p53->address, 0xFF, 0x01);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_LIM, 0x20);
        WriteRegister(p53->address, 0xFF, 0x00);
        break;

      case 12:
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        WriteRegister(p53->address, VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
        WriteRegister(p53->address, 0xFF, 0x01);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_LIM, 0x20);
        WriteRegister(p53->address, 0xFF, 0x00);
        break;

      case 14:
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
        WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        WriteRegister(p53->address, VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
        WriteRegister(p53->address, 0xFF, 0x01);
        WriteRegister(p53->address, VL53L0X_REG_ALGO_PHASECAL_LIM, 0x20);
        WriteRegister(p53->address, 0xFF, 0x00);
        break;

      default:
        // invalid period
        return false;
    }

    // apply new VCSEL period
    WriteRegister(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    // update timeouts

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

    // "For the final range timeout, the pre-range timeout
    //  must be added. To do this both final and pre-range
    //  timeouts must be expressed in macro periods MClks
    //  because they have different vcsel periods."

    uint16_t new_final_range_timeout_mclks =
      VL53L0X_timeoutMicrosecondsToMclks(p53->timeouts.final_range_us, period_pclks);

    if (p53->enables.pre_range)
    {
      new_final_range_timeout_mclks += p53->timeouts.pre_range_mclks;
    }

    WriteRegister16(p53->address, VL53L0X_REG_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
      VL53L0X_encodeTimeout(new_final_range_timeout_mclks));

    // set_sequence_step_timeout end
  }
  else
  {
    // invalid type
    return false;
  }

  // "Finally, the timing budget must be re-applied"

  VL53L0X_setMeasurementTimingBudget(p53);

  // "Perform the phase calibration. This is needed after changing on vcsel period."
  // VL53L0X_perform_phase_calibration() begin

  uint8_t sequence_config = ReadRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG);
  WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, 0x02);
  VL53L0X_performSingleRefCalibration(p53, 0x0);
  WriteRegister(p53->address, VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG, sequence_config);

  // VL53L0X_perform_phase_calibration() end

  return true;
}


/*
 * Used in continuous reading procedure
 */
void VL53L0X_SetInterMeasurementPeriodMilliSeconds(VL53L0XDEV *p53, uint32_t period_ms) {

    uint16_t osc_calibrate_val = ReadRegister16(p53->address, VL53L0X_REG_OSC_CALIBRATE_VAL);
    if (osc_calibrate_val != 0)
    {
      period_ms *= osc_calibrate_val;
    }
    WriteRegister32(p53->address, VL53L0X_REG_SYSTEM_INTERMEASUREMENT_PERIOD, period_ms);

}


/*
 * Used in both continuous reading and single shot procedure
 */
bool VL53L0X_ReadingHeader(VL53L0XDEV *p53) {
    WriteRegister(p53->address, 0x80, 0x01);
    WriteRegister(p53->address, 0xFF, 0x01);
    WriteRegister(p53->address, 0x00, 0x00);
    WriteRegister(p53->address, 0x91, p53->stopVariable);
    WriteRegister(p53->address, 0x00, 0x01);
    WriteRegister(p53->address, 0xFF, 0x00);
    WriteRegister(p53->address, 0x80, 0x00);    
}

void LASER0_SetLow(void)
{
    LATBbits.LATB5 = 0;
}

void LASER1_SetLow(void)
{
    LATBbits.LATB3 = 0;
}

void LASER2_SetLow(void)
{
    LATCbits.LATC5 = 0;
}

void LASER0_SetHigh(void)
{
    LATBbits.LATB5 = 1;
}

void LASER1_SetHigh(void)
{
    LATBbits.LATB3 = 1;
}

void LASER2_SetHigh(void)
{
    LATCbits.LATC5 = 1;
}

/*
 * Init the array of sensors. It uses the name definitions given in pin_manager.h
 */
void VL53L0X_InitDevices(void) {
    // disable both sensors
    //LATDbits.LATD0 = 0;
    //LATDbits.LATD1 = 0;
    __delay_ms(500);
    LASER0_SetLow();
    LASER1_SetLow();
    LASER2_SetLow();
    
    // set default parameters into sensors structure
    for (uint8_t i=0; i<MAX_LASER; i++) {
        vl53l0xDev[i].address = laserAddresses[i];
        vl53l0xDev[i].ioHighLevel = true;
    }
  
        //I'm changing address of LASER1
    LASER2_SetHigh();
    __delay_ms(100);
    VL53L0X_SetAddress(vl53l0xDev[LASER2].address);
    __delay_ms(100);
    VL53L0X_Init(&vl53l0xDev[LASER2]);
    __delay_ms(100);
    VL53L0X_SetTimeout(&vl53l0xDev[LASER2], LASER_TIMEOUT);  
    
    //LATDbits.LATD0 = 0;
    //LATDbits.LATD1 = 1;
    __delay_ms(2000);
    
    //I'm changing address of LASER1
    LASER1_SetHigh();
    __delay_ms(100);
    VL53L0X_SetAddress(vl53l0xDev[LASER1].address);
    __delay_ms(100);
    VL53L0X_Init(&vl53l0xDev[LASER1]);
    __delay_ms(100);
    VL53L0X_SetTimeout(&vl53l0xDev[LASER1], LASER_TIMEOUT);    
    
    //LATDbits.LATD0 = 1;
    //LATDbits.LATD1 = 0;
    __delay_ms(2000);
    
    //LASER0 uses default address
    LASER0_SetHigh();
    __delay_ms(100);
    VL53L0X_Init(&vl53l0xDev[LASER0]);
    __delay_ms(100);
    VL53L0X_SetTimeout(&vl53l0xDev[LASER0], LASER_TIMEOUT);  
    
    //LATDbits.LATD0 = 1;
    //LATDbits.LATD1 = 1;
    __delay_ms(2000);
    
    #ifdef LONG_RANGE
    /*
     * Important note:
     * These two commented functions don't work here. You should set 0.1 value directly into
     * VL53L0X_DataInit function (default is 0.25)
     */
    // VL53L0X_SetSignalRateLimit(&vl53l0xDev[LASER0].address, 0.1);
    // VL53L0X_SetSignalRateLimit(&vl53l0xDev[LASER1].address, 0.1);
    
    VL53L0X_SetVcselPulsePeriod(&vl53l0xDev[LASER0], vcselPeriodType.VcselPeriodPreRange, 18);
    VL53L0X_SetVcselPulsePeriod(&vl53l0xDev[LASER1], vcselPeriodType.VcselPeriodPreRange, 18);
    VL53L0X_SetVcselPulsePeriod(&vl53l0xDev[LASER2], vcselPeriodType.VcselPeriodPreRange, 18);
    VL53L0X_SetVcselPulsePeriod(&vl53l0xDev[LASER0], vcselPeriodType.VcselPeriodFinalRange, 14);
    VL53L0X_SetVcselPulsePeriod(&vl53l0xDev[LASER1], vcselPeriodType.VcselPeriodFinalRange, 14);
    VL53L0X_SetVcselPulsePeriod(&vl53l0xDev[LASER2], vcselPeriodType.VcselPeriodFinalRange, 14);
#endif
    
#ifdef HIGH_ACCURACY
    vl53l0xDev[LASER0].measurement_timing_budget_us = 200000;   //200msec (def. 33msec)
    vl53l0xDev[LASER1].measurement_timing_budget_us = 200000;
    VL53L0X_setMeasurementTimingBudget(&vl53l0xDev[LASER0]);
    VL53L0X_setMeasurementTimingBudget(&vl53l0xDev[LASER1]);
    VL53L0X_setMeasurementTimingBudget(&vl53l0xDev[LASER2]);
#endif
    
#ifdef HIGH_SPEED
    vl53l0xDev[LASER0].measurement_timing_budget_us = 20000;    //20msec
    vl53l0xDev[LASER1].measurement_timing_budget_us = 20000;
    VL53L0X_setMeasurementTimingBudget(&vl53l0xDev[LASER0]);
    VL53L0X_setMeasurementTimingBudget(&vl53l0xDev[LASER1]);
    VL53L0X_setMeasurementTimingBudget(&vl53l0xDev[LASER2]);
#endif
    
#ifdef CONTINUOUS
    VL53L0X_ContinuousReading(&vl53l0xDev[LASER0], 0);
    VL53L0X_ContinuousReading(&vl53l0xDev[LASER1], 0);
    VL53L0X_ContinuousReading(&vl53l0xDev[LASER2], 0);
#endif
}


/*
 * In millisec procedure I need to save the timeout value and the starting
 * value of millisec variable.
 */
void start_msTimeout(msecdata *data, uint16_t val) {
    data->timeout = val;
    data->msecStart = millisec;
}


/*
 * First check if millisec variable has overflowed than adopt the
 * correct condition.
 */
bool checkTimeoutExpired(msecdata *data) {
    
    if (data->timeout == 0) return false;
    
    if (millisec < data->msecStart) {
        uint16_t gap = 0xFFFF - data->msecStart;
        return ((millisec + gap - data->msecStart) > data->timeout);
    }
    
    return ((millisec - data->msecStart) > data->timeout);
}

// Fonction de leture de la distance
uint16_t  VL53L0X_Read_Distance(uint8_t Num_laser)
{
uint16_t tmp_Dist;

#ifdef CONTINUOUS
        tmp_Dist = VL53L0X_ReadRange(&vl53l0xDev[Num_laser]);
#endif

#ifdef SINGLE_SHOT
        tmp_Dist = VL53L0X_SingleShotReading(&vl53l0xDev[Num_laser]);
#endif  
        return(tmp_Dist);
}

uint16_t VL53L0X_ReadRange_No_Wait(VL53L0XDEV *p53) {
    
    msecdata timeoutData;
    //start_msTimeout(&timeoutData, p53->io_timeout);

    if ((ReadRegister(p53->address, VL53L0X_REG_RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
        if (checkTimeoutExpired(&timeoutData)) {
            p53->did_timeout = true;
            return 65535;
        }
    }

    // assumptions: Linearity Corrective Gain is 1000 (default);
    // fractional ranging is not enabled
    uint16_t range = ReadRegister16(p53->address, VL53L0X_REG_RESULT_RANGE_STATUS + 10);
    WriteRegister(p53->address, VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR, 0x01);

    return range;
}

// attention : il faut juste etre sur de laisser 20 ms entre 2 lectures
// fonction non bloquante par rapport a l'autre qui est bloquante 20 ms
uint16_t  VL53L0X_Read_Distance_No_Wait(uint8_t Num_laser)
{
uint16_t tmp_Dist;

#ifdef CONTINUOUS
        tmp_Dist = VL53L0X_ReadRange_No_Wait(&vl53l0xDev[Num_laser]);
#endif

#ifdef SINGLE_SHOT
        tmp_Dist = VL53L0X_SingleShotReading(&vl53l0xDev[Num_laser]);
#endif  
        return(tmp_Dist);
}