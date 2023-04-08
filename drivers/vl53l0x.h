/* 
 * File:   vl53l0x.h
 * Author: paolocarlobernardi
 * 
    PIC settings prerequisite:
    - I2C enabled in fast speed mode (400KHz).
    - Timer0 enabled with IRQ and a period of 1msec. The only thing to do
      with TMR0_ISR is to increment the millisec variable every time interrupt
      occurred.
 * 
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


#include "../mcc_generated_files/mcc.h"

#ifndef VL53L0X_H
#define	VL53L0X_H

#define VL53L0X_I2C_DEFAULT_ADDRESS 0x29    // 0x52 shifted left by one

/*
 * Device settings for multiple sensors
 */
#define MAX_LASER 3
#define LASER0_ADDR 0x29
#define LASER1_ADDR 0x2A
#define LASER2_ADDR 0x2B
#define LASER0 0            // Just an id
#define LASER1 1
#define LASER2 2
#define LASER_TIMEOUT 50    // Milliseconds


/*
 * Copied from "vl53l0x_device.h" ST API original file
 */

typedef uint8_t VL53L0X_DeviceError;
#define VL53L0X_DEVICEERROR_NONE                        ((VL53L0X_DeviceError) 0)
#define VL53L0X_DEVICEERROR_VCSELCONTINUITYTESTFAILURE  ((VL53L0X_DeviceError) 1)
#define VL53L0X_DEVICEERROR_VCSELWATCHDOGTESTFAILURE    ((VL53L0X_DeviceError) 2)
#define VL53L0X_DEVICEERROR_NOVHVVALUEFOUND             ((VL53L0X_DeviceError) 3)
#define VL53L0X_DEVICEERROR_MSRCNOTARGET                ((VL53L0X_DeviceError) 4)
#define VL53L0X_DEVICEERROR_SNRCHECK                    ((VL53L0X_DeviceError) 5)
#define VL53L0X_DEVICEERROR_RANGEPHASECHECK             ((VL53L0X_DeviceError) 6)
#define VL53L0X_DEVICEERROR_SIGMATHRESHOLDCHECK         ((VL53L0X_DeviceError) 7)
#define VL53L0X_DEVICEERROR_TCC                         ((VL53L0X_DeviceError) 8)
#define VL53L0X_DEVICEERROR_PHASECONSISTENCY            ((VL53L0X_DeviceError) 9)
#define VL53L0X_DEVICEERROR_MINCLIP                     ((VL53L0X_DeviceError) 10)
#define VL53L0X_DEVICEERROR_RANGECOMPLETE               ((VL53L0X_DeviceError) 11)
#define VL53L0X_DEVICEERROR_ALGOUNDERFLOW               ((VL53L0X_DeviceError) 12)
#define VL53L0X_DEVICEERROR_ALGOOVERFLOW                ((VL53L0X_DeviceError) 13)
#define VL53L0X_DEVICEERROR_RANGEIGNORETHRESHOLD        ((VL53L0X_DeviceError) 14)


#define VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE           0
#define VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE     1
#define VL53L0X_CHECKENABLE_SIGNAL_REF_CLIP             2
#define VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD      3
#define VL53L0X_CHECKENABLE_SIGNAL_RATE_MSRC            4
#define VL53L0X_CHECKENABLE_SIGNAL_RATE_PRE_RANGE       5
#define VL53L0X_CHECKENABLE_NUMBER_OF_CHECKS            6


typedef uint8_t VL53L0X_GpioFunctionality;
#define VL53L0X_GPIOFUNCTIONALITY_OFF                       ((VL53L0X_GpioFunctionality)  0) // NO Interrupt
#define VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW     ((VL53L0X_GpioFunctionality)  1) // Level Low (value < thresh_low)
#define VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH    ((VL53L0X_GpioFunctionality)  2) // Level High (value > thresh_high)
#define VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT     ((VL53L0X_GpioFunctionality)  3) // Out Of Window (value < thresh_low OR value > thresh_high)
#define VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY         ((VL53L0X_GpioFunctionality)  4) // New Sample Ready


/* Device register map */

#define VL53L0X_REG_SYSRANGE_START                                  0x00    // mask existing bit in #VL53L0X_REG_SYSRANGE_START
#define VL53L0X_REG_SYSRANGE_MODE_MASK                              0x0F    // bit 0 in #VL53L0X_REG_SYSRANGE_START write 1 toggle state in continuous mode and arm next shot in single shot mode
#define VL53L0X_REG_SYSRANGE_MODE_START_STOP                        0x01    // bit 1 write 0 in #VL53L0X_REG_SYSRANGE_START set single shot mode
#define VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT                        0x00    // bit 1 write 1 in #VL53L0X_REG_SYSRANGE_START set back-to-back operation mode
#define VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK                        0x02    // bit 2 write 1 in #VL53L0X_REG_SYSRANGE_START set timed operation mode
#define VL53L0X_REG_SYSRANGE_MODE_TIMED                             0x04    // bit 3 write 1 in #VL53L0X_REG_SYSRANGE_START set histogram operation mode
#define VL53L0X_REG_SYSRANGE_MODE_HISTOGRAM                         0x08

#define VL53L0X_REG_SYSTEM_THRESH_HIGH                              0x0C
#define VL53L0X_REG_SYSTEM_THRESH_LOW                               0x0E

#define VL53L0X_REG_SYSTEM_SEQUENCE_CONFIG                          0x01
#define VL53L0X_REG_SYSTEM_RANGE_CONFIG                             0x09
#define VL53L0X_REG_SYSTEM_INTERMEASUREMENT_PERIOD                  0x04

#define VL53L0X_REG_SYSTEM_INTERRUPT_CONFIG_GPIO                    0x0A
#define VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_DISABLED                  0x00
#define VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_LEVEL_LOW                 0x01
#define VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_LEVEL_HIGH                0x02
#define VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_OUT_OF_WINDOW             0x03
#define VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY          0x04

#define VL53L0X_REG_GPIO_HV_MUX_ACTIVE_HIGH                         0x84

#define VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR                          0x0B

/* Result registers */
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS                         0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS                             0x14

#define VL53L0X_REG_RESULT_CORE_PAGE  1
#define VL53L0X_REG_RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN           0xBC
#define VL53L0X_REG_RESULT_CORE_RANGING_TOTAL_EVENTS_RTN            0xC0
#define VL53L0X_REG_RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF           0xD0
#define VL53L0X_REG_RESULT_CORE_RANGING_TOTAL_EVENTS_REF            0xD4
#define VL53L0X_REG_RESULT_PEAK_SIGNAL_RATE_REF                     0xB6

/* Algo register */
#define VL53L0X_REG_ALGO_PART_TO_PART_RANGE_OFFSET_MM               0x28
#define VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS                        0x8A

/* Check Limit registers */
#define VL53L0X_REG_MSRC_CONFIG_CONTROL                             0x60

#define VL53L0X_REG_PRE_RANGE_CONFIG_MIN_SNR                        0X27
#define VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_LOW                0x56
#define VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH               0x57
#define VL53L0X_REG_PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT              0x64

#define VL53L0X_REG_FINAL_RANGE_CONFIG_MIN_SNR                      0X67
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW              0x47
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH             0x48
#define VL53L0X_REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT     0x44


#define VL53L0X_REG_PRE_RANGE_CONFIG_SIGMA_THRESH_HI                0X61
#define VL53L0X_REG_PRE_RANGE_CONFIG_SIGMA_THRESH_LO                0X62

/* PRE RANGE registers */
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD                   0x50
#define VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI              0x51
#define VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO              0x52

#define VL53L0X_REG_SYSTEM_HISTOGRAM_BIN                            0x81
#define VL53L0X_REG_HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT           0x33
#define VL53L0X_REG_HISTOGRAM_CONFIG_READOUT_CTRL                   0x55

#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD                 0x70
#define VL53L0X_REG_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI            0x71
#define VL53L0X_REG_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO            0x72
#define VL53L0X_REG_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS           0x20

#define VL53L0X_REG_MSRC_CONFIG_TIMEOUT_MACROP                      0x46


#define VL53L0X_REG_SOFT_RESET_GO2_SOFT_RESET_N                     0xBF
#define VL53L0X_REG_IDENTIFICATION_MODEL_ID                         0xC0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID                      0xC2

#define VL53L0X_REG_OSC_CALIBRATE_VAL                               0xF8


#define VL53L0X_SIGMA_ESTIMATE_MAX_VALUE                            65535

/* equivalent to a range sigma of 655.35mm */
#define VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH                       0x32
#define VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0                0xB0
#define VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_1                0xB1
#define VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_2                0xB2
#define VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_3                0xB3
#define VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_4                0xB4
#define VL53L0X_REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_5                0xB5

#define VL53L0X_REG_GLOBAL_CONFIG_REF_EN_START_SELECT               0xB6
#define VL53L0X_REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD             0x4E    // 0x14E
#define VL53L0X_REG_DYNAMIC_SPAD_REF_EN_START_OFFSET                0x4F    // 0x14F
#define VL53L0X_REG_POWER_MANAGEMENT_GO1_POWER_FORCE                0x80

/*
 * Speed of light in um per 1E-10 Seconds
 */
#define VL53L0X_SPEED_OF_LIGHT_IN_AIR                               2997
#define VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV               0x89
#define VL53L0X_REG_ALGO_PHASECAL_LIM                               0x30    // 0x130
#define VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT                    0x30

// Decode VCSEL (vertical cavity surface emitting laser) pulse period in PCLKs
// from register value
// based on VL53L0X_decode_vcsel_period()
#define decodeVcselPeriod(reg_val)      (((reg_val) + 1) << 1)

// Encode VCSEL pulse period register value from period in PCLKs
// based on VL53L0X_encode_vcsel_period()
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)

// Calculate macro period in *nanoseconds* from VCSEL period in PCLKs
// based on VL53L0X_calc_macro_period_ps()
// PLL_period_ps = 1655; macro_period_vclks = 2304
#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)


typedef enum { 
    VcselPeriodPreRange,
    VcselPeriodFinalRange
} vcselPeriodType;

// TCC: Target CentreCheck
// MSRC: Minimum Signal Rate Check
// DSS: Dynamic Spad Selection
typedef struct {
  bool tcc, msrc, dss, pre_range, final_range;
} SequenceStepEnables;

typedef struct {
  uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;
  uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
  uint32_t msrc_dss_tcc_us,    pre_range_us,    final_range_us;
} SequenceStepTimeouts;


/*
 * Used to have multiple sensors each of which has unique settings
 */
typedef struct {
    uint8_t address;
    uint8_t stopVariable;
    bool ioHighLevel;
    uint16_t io_timeout;
    bool did_timeout;
    uint16_t timeout_start_ms;
    uint32_t measurement_timing_budget_us;
    SequenceStepEnables enables;
    SequenceStepTimeouts timeouts;
}VL53L0XDEV;


/*
 * Array of multiple sensors
 */
VL53L0XDEV vl53l0xDev[MAX_LASER];
uint8_t laserAddresses[MAX_LASER] = {LASER0_ADDR, LASER1_ADDR, LASER2_ADDR};


/*
 * Used to have a timeout counter based on millisec
 */
typedef struct {
    uint16_t msecStart;
    uint16_t timeout;
} msecdata;


/*
 * Private Functions
 */
void WriteRegister(uint8_t addr, uint8_t reg, uint8_t val);
void WriteRegister16(uint8_t addr, uint8_t reg, uint16_t val);
void WriteRegister32(uint8_t addr, uint8_t reg, uint32_t val);
void WriteRegisters(uint8_t addr, uint8_t dataAddress, uint8_t *pData, uint8_t nCount);
uint8_t ReadRegister(uint8_t addr, uint8_t reg);
uint16_t ReadRegister16(uint8_t addr, uint8_t reg);
void ReadRegisters(uint8_t addr, uint8_t dataAddress, uint8_t *pData, uint8_t nCount);
void VL53L0X_SetAddress(uint8_t newAddr);
void VL53L0X_DataInit(VL53L0XDEV *p53);
bool VL53L0X_StaticInit(VL53L0XDEV *p53);
bool VL53L0X_GetSpadInfo(VL53L0XDEV *p53, uint8_t *count, bool *typeIsAperture);
void VL53L0X_SetReferenceSpads(uint8_t addr, uint8_t *count, bool *typeIsAperture, uint8_t *spadMap);
void VL53L0X_Tuning(uint8_t addr);
void VL53L0X_SetGpioConfig(uint8_t addr);
uint32_t VL53L0X_getMeasurementTimingBudget(VL53L0XDEV *p53);
void VL53L0X_getSequenceStepEnables(VL53L0XDEV *p53);
void VL53L0X_getSequenceStepTimeouts(VL53L0XDEV *p53);
uint8_t VL53L0X_getVcselPulsePeriod(uint8_t addr, vcselPeriodType type);
uint32_t VL53L0X_timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks);
uint32_t VL53L0X_timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks);
uint16_t VL53L0X_decodeTimeout(uint16_t reg_val);
uint16_t VL53L0X_encodeTimeout(uint16_t timeout_mclks);
bool VL53L0X_PerformRefCalibration(VL53L0XDEV *p53);
bool VL53L0X_performSingleRefCalibration(VL53L0XDEV *p53, uint8_t vhv_init_byte);
void VL53L0X_SetTimeout(VL53L0XDEV *p53, uint16_t timeout);
void VL53L0X_ClearStruct(VL53L0XDEV *p53);
void VL53L0X_SetInterMeasurementPeriodMilliSeconds(VL53L0XDEV *p53, uint32_t period_ms);
bool VL53L0X_ReadingHeader(VL53L0XDEV *p53);


/*
 *  Public Functions
 */
bool VL53L0X_Init(VL53L0XDEV *p53);
uint16_t VL53L0X_SingleShotReading(VL53L0XDEV *p53);
void VL53L0X_ContinuousReading(VL53L0XDEV *p53, uint32_t period_ms);
void VL53L0X_StopContinuous(VL53L0XDEV *p53);
uint16_t VL53L0X_ReadRange(VL53L0XDEV *p53);
bool VL53L0X_TimeoutOccurred(VL53L0XDEV *p53);
bool VL53L0X_SetVcselPulsePeriod(VL53L0XDEV *p53, vcselPeriodType type, uint8_t period_pclks);
bool VL53L0X_SetSignalRateLimit(uint8_t addr, float limit_Mcps);
bool VL53L0X_setMeasurementTimingBudget(VL53L0XDEV *p53);


/*
 * Functions used in case of multiple sensors connected on same bus
 */
void VL53L0X_InitDevices(void);


uint16_t  VL53L0X_Read_Distance(uint8_t Num_laser);

uint16_t  VL53L0X_Read_Distance_No_Wait(uint8_t Num_laser);
uint16_t VL53L0X_ReadRange_No_Wait(VL53L0XDEV *p53);

/*
 * Timeout based on millisec variable and timer0 irq with a period of 1ms
 */
uint16_t millisec = 0;
void start_msTimeout(msecdata *data, uint16_t val);
bool checkTimeoutExpired(msecdata *data);


#endif	/* VL53L0X_H */
