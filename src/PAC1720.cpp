/******************************************************************************
MCP3421.cpp
Library for MCP3421 ADC
Bobby Schulz @ Northern Widget LLC & UMN
2/20/2018

MCP3421 is a 18 bit ADC, PGA, and various modes of resolution operation in order to facilitate various data rates

"Simplicity is prerquisite for reliability"
-Edsger W. Dijkstra

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Arduino.h>
#include <PAC1720.h>
#include <Wire.h>



PAC1720::PAC1720(int _ADR)  //Set address
{
  ADR = _ADR; 
  // Wire.begin();  
}

PAC1720::PAC1720(void)  //Use default address
{
  ADR = 0x4C; //Default address
}

bool PAC1720::begin() //Initalize system 
{
  Wire.begin();  
  //Setup device for default operation
  SetConfig(C1RA, 0x00); //No averaging on CH1 bus measurment 
  SetConfig(C2RA, 0x00); //No averaging on CH2 bus measurment 
  SetConfig(C1RS, 0b11); //11 bit sample on CH1 bus
  SetConfig(C2RS, 0b11); //11 bit sample on CH2 bus
  SetConfig(C1CSS, 0b101); //11 bit sense, min sample time, on CH1 sense
  SetConfig(C2CSS, 0b101); //11 bit sense, min sample time, on CH2 sense
  SetConfig(C1SR, 0b11); //Set for 80mV FSR on CH1
  SetConfig(C2SR, 0b11); //Set for 80mV FSR on CH2
  SetConfig(C1SA, 0b00); //No averaging on CH1 sense measurment 
  SetConfig(C2SA, 0b00); //No averaging on CH2 sense measurment 
}

float PAC1720::GetBusVoltage(int Unit)
{
  unsigned int Den = 2048; //Default 
  unsigned int DenBits = 3; //Default

  if(Unit == CH1) {
    DenBits = GetConfig(C1RS);
  }
  if(Unit == CH2) {
    DenBits = GetConfig(C2RS);
  }

  // Den = (2 << (Den + 7)); //Calulate deniminator from number of bits
  Den = pow(2, DenBits + 8); //Calulate deniminator from number of bits
  float FSV = 40.0 - 40.0/float(Den); //Calculate full scale voltage

  Wire.beginTransmission(ADR);
  Wire.write(BUS1 + int(Unit)*2); //Write to bus voltage reg+offset
  int Error1 = Wire.endTransmission();  //FIX! Add error notifcation

  Wire.requestFrom(ADR, 2);
  unsigned int Data1 = Wire.read(); //Read out bytes
  unsigned int Data2 = Wire.read();

  int VSource = ((Data1 << 3) | Data2 >> 5);
  float V = FSV*(float(VSource)/float(Den - 1));
  if(Error1 == 0) return V; //Return value in volts
  else return 0; //Return 0 in error state 
}

float PAC1720::GetSenseVoltage(int Unit)
{
  //FSR = 10, 20, 40, 80mV
  float FSR = 80; //Set to 80 by default
  unsigned int Den = 2047; //Set to 2047 by default

  if(Unit == CH1) {
    //FSR = 10, 20, 40, 80mV
    FSR = (2 << GetConfig(C1SR))*10;
    Den = 6 + GetConfig(C1CSS);
  }
  if(Unit == CH2) {
    //FSR = 10, 20, 40, 80mV
    FSR = (2 << GetConfig(C2SR))*10;
    Den = 6 + GetConfig(C2CSS);
  }

  if(Den > 11) Den = 11; //FIX! Make cleaner 
  Den = (2 << Den) - 1;
  float FSC = FSR; //FSR = mV

  Wire.beginTransmission(ADR);
  Wire.write(SENSE1 + 2*Unit); //Write to bus voltage reg+offset
  uint8_t Error = Wire.endTransmission();

  Wire.requestFrom(ADR, 2);
  int Data1 = Wire.read(); //Read out bytes
  int Data2 = Wire.read();

  // if((Data2 & 0x0F) == 0x0F) return 0; //Check if low bits (fixed at 0) are set, if so, reading is bad, return 0 //FIX??
  int16_t VSense = ((Data1 << 4) | Data2 >> 4); //IMPORTANT! Must be int16_t, NOT int! Should be equitable, but if use int, it gets treated as unsigned later. Not very cash money. 
  
  if((VSense & 0x800) == 0x800) VSense = VSense | 0xF000; //If sign bit it set, pad left FIX!
  float I = FSC*float(VSense)/float(Den);
  if(Error == 0) return I; //Return in mV FIX??
  else return 0; //If I2C state is not good, return 0
}

float PAC1720::GetCurrent(int Unit, float R)
{
  //FSR = 10, 20, 40, 80mV
  float FSR = 80; //Set to 80 by default
  unsigned int Den = 2047; //Set to 2047 by default

  if(Unit == CH1) {
    //FSR = 10, 20, 40, 80mV
    FSR = (2 << GetConfig(C1SR))*10;
    Den = 6 + GetConfig(C1CSS);
  }
  if(Unit == CH2) {
    //FSR = 10, 20, 40, 80mV
    FSR = (2 << GetConfig(C2SR))*10;
    Den = 6 + GetConfig(C2CSS);
  }

  if(Den > 11) Den = 11; //FIX! Make cleaner 
  Den = (2 << Den) - 1;
  float FSC = FSR/R; //FSR = mV, R = mOhms

  Wire.beginTransmission(ADR);
  Wire.write(SENSE1 + 2*Unit); //Write to bus voltage reg+offset
  uint8_t Error = Wire.endTransmission();

  Wire.requestFrom(ADR, 2);
  int Data1 = Wire.read(); //Read out bytes
  int Data2 = Wire.read();

  // if((Data2 & 0x0F) == 0x0F) return 0; //Check if low bits (fixed at 0) are set, if so, reading is bad, return 0 //FIX??
  int16_t VSense = ((Data1 << 4) | Data2 >> 4); //IMPORTANT! Must be int16_t, NOT int! Should be equitable, but if use int, it gets treated as unsigned later. Not very cash money. 
  
  if((VSense & 0x800) == 0x800) VSense = VSense | 0xF000; //If sign bit it set, pad left FIX!
  float I = FSC*float(VSense)/float(Den);
  if(Error == 0) return I*1000.0; //Return in mA FIX??
  else return 0; //If I2C state is not good, return 0
  
}

int PAC1720::GetConfig(Config Value) //Return the value of the given configuration 
{
  uint8_t Reg = Value >> 8; //Pull out register
  uint8_t Offset = (Value & 0x0F); //Grab offset value 
  uint8_t Mask = (Value >> 4) & 0x0F; //Grab config mask

  Wire.beginTransmission(ADR);
  Wire.write(Reg);
  Wire.endTransmission(); //FIX! Add error condition 

  Wire.requestFrom(ADR, 1);
  int Data = Wire.read();

  return (Data >> Offset) & Mask;
}

int PAC1720::SetConfig(Config Value, uint8_t NewVal) //Set the value of the given configuration 
{
  uint8_t Reg = (Value >> 8); //Pull out register
  uint8_t Offset = (Value & 0x0F); //Grab offset value 
  uint8_t Mask = (Value >> 4) & 0x0F; //Grab config mask

  Wire.beginTransmission(ADR);
  Wire.write(Reg);
  Wire.endTransmission();

  Wire.requestFrom(ADR, 1);
  uint8_t PrevVal = Wire.read(); //Grab current state of register
  PrevVal = PrevVal & ~(Mask << Offset); //Clear desired bits, preserve rest

  uint8_t WriteVal = (NewVal << Offset) | PrevVal; //Shift desired value into correct position, or with other bits

  Wire.beginTransmission(ADR);
  Wire.write(Reg);
  Wire.write(WriteVal); //Write adjusted value
  return Wire.endTransmission(); //Return error condition
}









