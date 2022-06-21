#include "MCP47CXBXX.h"

uint8_t dac_address;
uint8_t dac_resolution;
uint8_t dac_chcount;

//Instantiates a new MCP47CXBXX_DAC class
MCP47CXBXX_DAC::MCP47CXBXX_DAC() {} //TODO how does this work, what is this even?
//TODO implement begin function

//Individual channel power setting control
//pwr_setting can be DAC_PWR_NORMAL, DAC_PWR_DOWN_1K, DAC_PWR_DOWN_100k, DAC_PWR_DOWN_HIZ
//example: dac_pwr_config(DAC_PWR_DOWN_1K, DAC_PWR_DOWN_HIZ);
bool MCP47CXBXX_DAC::begin(uint8_t resolution, uint8_t channel_count = 2, uint8_t i2c_address = DAC_ADR_DEFAULT)
{
    Wire.begin();
    dac_address = i2c_address;
    dac_resolution = resolution;
    dac_chcount = channel_count;
}

bool MCP47CXBXX_DAC::setPwr(uint8_t channel_0_setting, uint8_t channel_1_setting)
{
  uint8_t value_LSB = 0;
  
  value_LSB = (channel_1_setting<<2)&0b00001100 | (channel_0_setting)&0b00000011;
 
  Write(dac_address, DAC_POWERDOWN_REG, 0, value_LSB);
}

bool MCP47CXBXX_DAC::setVref(uint8_t channel_0_setting, uint8_t channel_1_setting)
{
  uint8_t value_LSB = 0;
  
  value_LSB = (channel_1_setting<<2)&0b00001100 | (channel_0_setting)&0b00000011;
 
  Write(dac_address, DAC_VREF_REG, 0, value_LSB);
}

bool MCP47CXBXX_DAC::setGain(uint8_t channel_0_setting, uint8_t channel_1_setting)
{
  uint8_t value_MSB = 0;
  
  value_MSB = (channel_1_setting<<1)&0b00000010 | (channel_0_setting)&0b00000001;
 
  Write(dac_address, DAC_GAIN_REG, value_MSB, 0);
}

bool MCP47CXBXX_DAC::setOutput(uint8_t channel, uint16_t value)
{
 
  uint8_t status_flag = 1;
  uint8_t dac_register;
  uint16_t mask_value;
  if(DAC_RES == 8)
    mask_value = 0b0000000011111111;
  if(DAC_RES == 10)
    mask_value = 0b0000001111111111;
  if(DAC_RES == 12)
    mask_value = 0b0000111111111111;
  
  if(channel == 0)
    dac_register = DAC_WIPER_REG0;
  else if(channel == 1)
    dac_register = DAC_WIPER_REG1;
  else
    return 0; //invalid register

  if(value > DAC_MAX_VALUE)
  {
     value = DAC_MAX_VALUE;
     status_flag = 2; //returns 2 if value sent to dac was affected
  }

  value = value&mask_value;   //remove data on the upper bits that are not to be sent to the DAC based on the resolution.
  Write(dac_address, dac_register, ((value>>8)&0x00FF), ((value)&0x00FF));
  return status_flag;
   //TODO error handling
}

bool MCP47CXBXX_DAC::readOutput(uint8_t channel, uint16_t *value)
{
  if(channel == 0)
    channel = DAC_WIPER_REG0;
  else if(channel == 1)
    channel = DAC_WIPER_REG1;
  else
    return 0;   //invalid address

  Read(dac_address, channel, value);

  return 1;

  //TODO error handling
}

//Lower level function to send values to specific registers of a specific I2C device with some address.
//address - I2C address of device
//command - register address
//value - data to write to the register, splite into the MSB and LSB
bool MCP47CXBXX_DAC::general_command(uint8_t command)
{
  Wire.beginTransmission(0x00);
  Wire.write(command);  //command stored in upper 5 bits
  Wire.endTransmission();
}

bool MCP47CXBXX_DAC::Write(uint8_t address, uint8_t command, uint8_t value_MSB, uint8_t value_LSB)
{
  Wire.beginTransmission(address);
  Wire.write((command<<3)&0xF8);  //command stored in upper 5 bits
  Wire.write(value_MSB);
  Wire.write(value_LSB);
  Wire.endTransmission();

  return 1;
  //TODO error handling
}

bool MCP47CXBXX_DAC::Read(uint8_t address, uint8_t command, uint16_t* value)
{
  Wire.beginTransmission(address);
  Wire.write((command<<3)&0xF8 | 0b00000110);  //command stored in upper 5 bits, b1 and b2 set to '1' for read command
  Wire.endTransmission();

  Wire.requestFrom(int(address), 2);    //request 2 bytes from provided address  

  if(2 <= Wire.available())
  {
    
    *value = Wire.read();
    *value = *value << 8;
    *value |= Wire.read();
  }
  
  return 1;
  //TODO error handling
}