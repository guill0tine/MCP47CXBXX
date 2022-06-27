#include "MCP47CXBXX.h"

//Instantiates a new MCP47CXBXX_DAC class
MCP47CXBXX_DAC::MCP47CXBXX_DAC() {} //TODO how does this work, what is this even?
//TODO implement begin function

//Resolution can be 8, 10, 12
//Channel count can be 1 or 2
//I2C clock freq defaults to 400kHz but can probably go up to 666kHz. Use a lower clock frequency if running on a breadboard
//Return 0 if no error
uint8_t MCP47CXBXX_DAC::begin(uint8_t resolution, uint8_t channel_count = 2, uint8_t i2c_address = DAC_ADR_DEFAULT, uint32_t clock = 200000) 
{
    uint8_t status_flag = 0;

    Wire.begin();
    Wire.setClock(clock);    //default on an uno is 100000, max it can do appears to be 666kHz, the freq should probably not be 
                              //higher than 444kHz based on comments found in twi.c
    DAC_ADDRESS = i2c_address;
    DAC_RESOLUTION = resolution;
    dac_chcount = channel_count;

    if(DAC_RESOLUTION == 8)
      DAC_MAX_VALUE = 255;
    else if(DAC_RESOLUTION == 10)
      DAC_MAX_VALUE = 1023;
    else if(DAC_RESOLUTION == 12)
      DAC_MAX_VALUE = 4095;
    else
      status_flag = 0;

    status_flag |= setOutput(DAC_WIPER_REG0, 0);
    status_flag |= setOutput(DAC_WIPER_REG1, 0);

    return status_flag;
}

//Individual channel power setting control
//pwr_setting can be DAC_PWR_NORMAL, DAC_PWR_DOWN_1K, DAC_PWR_DOWN_100k, DAC_PWR_DOWN_HIZ
uint8_t MCP47CXBXX_DAC::setPwr(uint8_t channel_0_setting, uint8_t channel_1_setting)
{
  uint8_t value_LSB = 0;
  uint8_t status_flag = 0;
  
  value_LSB = (channel_1_setting<<2)&0b00001100 | (channel_0_setting)&0b00000011;
 
  status_flag = Write(DAC_POWERDOWN_REG, 0, value_LSB);
  return status_flag;
}

uint8_t MCP47CXBXX_DAC::setVref(uint8_t channel_0_setting, uint8_t channel_1_setting)
{
  uint8_t value_LSB = 0;
  uint8_t status_flag = 0;
  
  value_LSB = (channel_1_setting<<2)&0b00001100 | (channel_0_setting)&0b00000011;
 
  status_flag = Write(DAC_VREF_REG, 0, value_LSB);
  return status_flag;
}

uint8_t MCP47CXBXX_DAC::setGain(uint8_t channel_0_setting, uint8_t channel_1_setting)
{
  uint8_t value_MSB = 0;
  uint8_t status_flag = 0;
  
  value_MSB = (channel_1_setting<<1)&0b00000010 | (channel_0_setting)&0b00000001;
 
  status_flag = Write(DAC_GAIN_REG, value_MSB, 0);
  return status_flag;
}

//if continuous set to true, the stop bit will not be sent. The user must code in a stop bit command
//TODO - implement continuous transmit - aovid resending ADRS, save a byte.
//Returns 0 if no error
uint8_t MCP47CXBXX_DAC::setOutput(uint8_t channel, uint16_t value, bool continuous = false)
{
  uint8_t status_flag = 0;
  uint8_t dac_register;
  
  if(channel == 0)
    dac_register = DAC_WIPER_REG0;
  else if(channel == 1)
    dac_register = DAC_WIPER_REG1;
  else
    status_flag = 7;

  if(value > DAC_MAX_VALUE)
  {
    value = DAC_MAX_VALUE;
    status_flag = 8; //returns 8 if value sent to dac was affected. 1-5 used by twi_writeTo, 6 used in MCP47CXBXX_DAC:Write.
  }

  //remove data on the upper bits that are not to be sent to the DAC based on the resolution.
  if(DAC_RESOLUTION == 8)
    value &= 0b0000000011111111;
  if(DAC_RESOLUTION == 10)
    value &= 0b0000001111111111;
  if(DAC_RESOLUTION == 12)
    value &= 0b0000111111111111;
  
  status_flag |= Write(dac_register, ((value>>8)&0xFF), (value&0xFF), !continuous);
    //Write returns 0 if error, 1+ if no error
  
  return status_flag;
   //TODO error handling
}

uint8_t MCP47CXBXX_DAC::readOutput(uint8_t channel, uint16_t *value)
{
  if(channel == 0)
    channel = DAC_WIPER_REG0;
  else if(channel == 1)
    channel = DAC_WIPER_REG1;
  else
    return 0;   //invalid address

  Read(channel, value);

  return 1;

  //TODO error handling
}

//This function is part of the I2C general commands
//Sending this will wake all devices on the bus
uint8_t MCP47CXBXX_DAC::Wake(void)
{
  general_command(I2C_WAKE);
}

//This function is part of the I2C general commands
//Sending this will reset all devices on the bus
uint8_t MCP47CXBXX_DAC::Reset(void)
{
  general_command(I2C_RESET);
}

//general command is sent to all I2C devices and affects all devices on the bus
//command - register address
uint8_t MCP47CXBXX_DAC::general_command(uint8_t command)
{
  Wire.beginTransmission(0x00);
  Wire.write(command);  //command stored in upper 5 bits
  Wire.endTransmission();
}

//Send data to the DAC. The write command consists of the start bit, address, command, 2 bytes o data and the stop bit.
//Returns 0 if no error - note that Wire.Write returns 0 if error and 1+ if no error. However, Wire.endTransmission returns
//0 if no error and 1+ if error
uint8_t MCP47CXBXX_DAC::Write(uint8_t command, uint8_t value_MSB, uint8_t value_LSB, bool sendStop = true)
{
  uint8_t status_flag = 0;

  Wire.beginTransmission(DAC_ADDRESS);
  
  status_flag |= Wire.write((command<<3)&0xF8);  //command stored in upper 5 bits
  status_flag |= Wire.write(value_MSB);
  status_flag |= Wire.write(value_LSB);
  
  if(status_flag == 0) //if error
    status_flag = 6;    //6 as that is an unused error value in twi_writeTo
  if(status_flag == 1)  //no error
    status_flag = 0;
  status_flag |= Wire.endTransmission(true);
  
  return status_flag;
}

//Read values from a specific register in the DAC memory
//returns 0 if no error
uint8_t MCP47CXBXX_DAC::Read(uint8_t command, uint16_t* value)
{
  uint8_t status_flag = 0;

  Wire.beginTransmission(DAC_ADDRESS);

  status_flag |= Wire.write((command<<3)&0xF8 | 0b00000110);  //command stored in upper 5 bits, b1 and b2 set to '1' for read command
  
  if(status_flag == 0) //if error
    status_flag = 6;    //6 as that is an unused error value in twi_writeTo
  if(status_flag == 1)  //no error
    status_flag = 0;
  
  status_flag |= Wire.endTransmission(false); //TODO review if this needs to change to endTransmission(false) and another
                          //endtransmission placed after having read the data

  Wire.requestFrom(int(DAC_ADDRESS), 2);    //request 2 bytes from provided address. Returns number of bytes read.
  //Wire.requestFrom is blocking as a result of twi_readFrom

  if(2 == Wire.available())
  {
    *value = Wire.read();
    *value = *value << 8;
    *value |= Wire.read();
  }
  else  status_flag |= 7; //if 2 bytes aren't available after requesting 2 bytes, then something is wrong
  
  return status_flag;
}