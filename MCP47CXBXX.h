/*
    Author: Guillotine

        I2C Driver for Microchip MCP47CXBXX using an arduino
        This library has only been tested with PN MCP47CVB12-E/MF which is a 10-bit 2 channel DAC

        The DACs default to a gain of 1 and Vcc as the reference
*/

#ifndef MCP47CXBXX_H_
#define MCP47CXBXX_H_

#include <Wire.h>

//I2C address based on pin 2 (A0) and pin 8 (A1)
#define DAC_ADR_A0  0
#define DAC_ADR_A1  0
#define DAC_DUAL
#define DAC_RES     10    //options are 12, 10, 8
#define DAC_GAIN    1     //Output op-amp gain, other option is to have the gain be 2
//#define DAC_SINGLE

#if DAC_RES == 8    //TODO change this
  #define DAC_MAX_VALUE 255
#elif DAC_RES == 10
  #define DAC_MAX_VALUE 1023
#elif DAC_RES == 12
  #define DAC_MAX_VALUE 4095
#else
  #define DAC_MAX_VALUE 0
#endif

//Registers
#define DAC_WIPER_REG0      0x00
#define DAC_WIPER_REG1      0x01
#define DAC_VREF_REG        0x08
#define DAC_POWERDOWN_REG   0x09
#define DAC_GAIN_REG        0x0A

//General Call Commands
#define I2C_RESET           0x06
#define I2C_WAKE            0x0A
//Register settings

//VREF
#define DAC_VREF_VDD        0b00    //VDD is the reference
#define DAC_VREF_INTERNAL   0b01    //internal bandgap ref of 1.214V
#define DAC_VREF_EXT_UNBUFF 0b10    //external unbuffered reference - does not use the internal opamp
#define DAC_VREF_EXT_BUFF   0x11    //external buffered reference - uses the internal opamp

//POWER
#define DAC_PWR_NORMAL      0b00    //Normal operation
#define DAC_PWR_DOWN_1K     0b01    //Powered down, with 1kohm resistor to ground on outputs
#define DAC_PWR_DOWN_100K   0b10    //Powered down, with 100kohm resistor to ground on outputs
#define DAC_PWR_DOWN_HIZ    0b11    //Powered down, with Hi-Z on outputs

//GAIN
#define DAC_GAIN_1X         0       //channel 1, output gain of 1
#define DAC_GAIN_2X         1       //channel 1, output gain of 2

//STATUS - Read only
#define DAC_STATUS_POR      1<<7    //A 1 on this bit indicates POR/BOR event occured since last read, guessing it clears after reading
#define DAC_STATUS_MTPMA    1<<6    //A 1 indicates memory is being accessed


#define DAC_ADR_DEFAULT (0b01100000)| DAC_ADR_A1<<1 | DAC_ADR_A0 


class MCP47CXBXX_DAC {
  public:
    MCP47CXBXX_DAC();
      bool begin(uint8_t resolution, uint8_t channel_count = 2, uint8_t i2c_address = DAC_ADR_DEFAULT);
      bool setOutput(uint8_t channel, uint16_t value);
      bool setPwr(uint8_t channel_0_setting, uint8_t channel_1_setting);
      bool setVref(uint8_t channel_0_setting, uint8_t channel_1_setting);
      bool setGain(uint8_t channel_0_setting, uint8_t channel_1_setting);
      bool readOutput(uint8_t channel, uint16_t *value); //TODO maybe change to return the actual output instead of using a pointer
      bool Wake();    //TODO implement this
      bool Reset();   //TODO implement this
  private:
      bool Write(uint8_t address, uint8_t command, uint8_t value_MSB, uint8_t value_LSB);
      bool Read(uint8_t address, uint8_t command, uint16_t* value);
      bool general_command(uint8_t command);
};

#endif