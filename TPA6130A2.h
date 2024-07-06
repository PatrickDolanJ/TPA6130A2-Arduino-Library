/*
Created by: PatrickDolanJ
*/

/*---------------DATASHEET--------------------------------------------------------
  https://datasheet.lcsc.com/szlcsc/Texas-Instruments-TI-TPA6130A2RTJT_C206091.pdf
---------------------------------------------------------------------------------*/

#pragma once
#include <Arduino.h>

#define I2C_ADDRESS 0x60

enum Mode
{
  M_STEREO = 0,
  M_DUAL_MONO = 1,
  M_BRIDGE_TIED_LOAD_RECEIVER = 2
};

class TPA6130A2
{

public:
  // Setup
  TPA6130A2();
  void init(byte defaultVol, Mode mode);
  void init(byte defaultVol, Mode mode, Wire *wire);
  // Setters
  void writeGain(byte Gain);
  void writeLeftMute(bool isMute);
  void writeRightMute(bool isMute);
  void writeRightImpedance(bool isHigh);
  void writeLeftImpedance(bool isHigh);
  void writeLeftEnable(bool isEnabled);
  void writeRightEnable(bool isEnabled);
  void writeMode(Mode mode);

  // Getters
  byte getAddress();
  byte getGain();
  bool getLeftMute();
  bool getRightMute();
  bool getLeftImpedance();
  bool getRightImpedance();
  bool getLeftEnable();
  bool getRightEnable();

private:
  Mode mode;

  byte gain;
  byte MAX_GAIN = 63;
  byte MIN_GAIN = 0;
  byte clampGain(byte data);
  TwoWire *wire = Wire;

  bool leftMute = false;
  bool rightMute = false;
  bool leftEnabled = true;
  bool rightEnabled = true;
  bool rightHighImpedance = false;
  bool leftHighImpedance = false;

  // Senders
  void sendControlRegister();
  void sendVolumeRegister();
  void sendImpedanceRegister();
  // Readers
  void readVolumeRegister();
  void readControlRegister();
  void readImpedanceRegister();
  // Builders
  byte buildControlRegisterData();
  byte buildVolumeRegisterData();
  byte buildImpedanceRegisterData();
  // Comms
  void sendData(byte register, byte data);
  byte sendRead(byte register);
};