/******************************************************************************
MCP3421.h
Library for MCP3421 ADC
Bobby Schulz @ Northern Widget LLC & UMN
2/20/2018

MCP3421 is a 18 bit ADC, PGA, and various modes of resolution operation in order to facilitate various data rates

"Simplicity is prerquisite for reliability"
-Edsger W. Dijkstra

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef PAC1720_h
#define PAC1720_h

#include "Arduino.h"

#define SENSE1 0x0D
#define SENSE2 0x0F
#define BUS1 0x11
#define BUS2 0x13

enum Config {  //Setup configuration values, upper byte is register address, lower byte is shift and mask, lower nibble is shift, upper nibble is mask
  C1RS = 0x0A32,
  C1RA = 0x0A30,
  C2RS = 0x0A36,
  C2RA = 0x0A34,
  C1CSS = 0x0B74, 
  C1SA = 0x0B32,
  C1SR = 0x0B30,
  C2CSS = 0x0C74,
  C2SA = 0x0C32,
  C2SR = 0x0C30
};

enum Channel {
    CH1 = 0,
    CH2 = 1
};

class PAC1720
{
  public:

    PAC1720(int _ADR);
    PAC1720(void);

    bool begin(void);
    float GetBusVoltage(Channel Unit);
    float GetSenseVoltage(Channel Unit);
    float GetCurrent(Channel Unit, float R);

  private:
    int ADR;
    int GetConfig(Config Value);
    int SetConfig(Config Value, uint8_t NewVal);

};

#endif