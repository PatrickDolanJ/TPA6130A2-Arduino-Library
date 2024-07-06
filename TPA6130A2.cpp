#include <Arduino.h>
#include <Wire.h>
#include <TPA6130A2.h>

//----------------Macros-----------------

// registers
#define TPA6130_CONTROL_REGISTER 0x1
#define TPA6130_VOLUME_AND_MUTE_REGISTER 0x2
#define TPA6130_OUTPUT_IMPEDANCE_REGISTER 0x3
#define TPA6130_I2C_ADDRESS_VERSION_REGISTER 0x4

TPA6130A2::TPA6130A2() {}

void TPA6130A2::init(byte defaultGain, Mode mode)
{
    this->gain = defaultGain;
    this->mode = mode;
    this->wire.begin();
    delay(1);
    sendControlRegister();
    sendVolumeRegister();
}

void TPA6130A2(byte defaultGain, Mode mode, Wire *wire){
    this->gain = defaultGain;
    this->mode = mode;
    this.wire = wire;
    this->wire.begin();
    delay(1);
    sendControlRegister();
    sendVolumeRegister();
}
//------------------------Getters-------------------

byte TPA6130A2::getAddress()
{
    return I2C_ADDRESS;
}

// Volume Registers
byte TPA6130A2::getGain()
{
    readVolumeRegister();
    return this->gain;
}

bool TPA6130A2::getLeftMute()
{
    readVolumeRegister();
    return this->leftMute;
}

bool TPA6130A2::getRightMute()
{
    readVolumeRegister();
    return this->rightMute;
}

// Impedance Registers
bool TPA6130A2::getLeftImpedance()
{
    readImpedanceRegister();
    return this->leftHighImpedance;
}

bool TPA6130A2::getRightImpedance()
{
    readImpedanceRegister();
    return this->rightHighImpedance;
}

// Control Registers
bool TPA6130A2::getLeftEnable()
{
    readControlRegister();
    return this->leftEnabled;
}

bool TPA6130A2::getRightEnable()
{
    readControlRegister();
    return this->rightEnabled;
}

//------------------Writes------------------

void TPA6130A2::writeGain(byte gain)
{
    this->gain = clampGain(gain);
    sendVolumeRegister();
}

void TPA6130A2::writeLeftMute(bool isMute)
{
    this->leftMute = isMute;
    sendVolumeRegister();
}

void TPA6130A2::writeRightMute(bool isMute)
{
    this->rightMute = isMute;
    sendVolumeRegister();
}

void TPA6130A2::writeLeftImpedance(bool isHigh)
{
    if (!leftEnabled)
    {
        this->leftHighImpedance = isHigh;
        sendImpedanceRegister();
    }
}

void TPA6130A2::writeRightImpedance(bool isHigh)
{
    if (!rightEnabled)
    {
        this->rightHighImpedance = isHigh;
        sendImpedanceRegister();
    }
}

void TPA6130A2::writeLeftEnable(bool isEnabled)
{
    this->rightEnabled = isEnabled;
    sendControlRegister();
}

void TPA6130A2::writeRightEnable(bool isEnabled)
{
    this->leftEnabled = isEnabled;
    sendControlRegister();
}

void TPA6130A2::writeMode(Mode mode)
{
    this->mode = mode;
    sendControlRegister();
}
//-----------------------------Helpers----------------------------------

//----------------------------------------------------------------------

//----------------------------Validation--------------------------------

byte TPA6130A2::clampGain(byte data)
{
    data = data > this->MAX_GAIN ? this->MAX_GAIN : data;
    data = data < this->MIN_GAIN ? this->MIN_GAIN : data;
    return data;
}

//-------------------------------Senders-----------------------

void TPA6130A2::sendVolumeRegister()
{
    Serial.println("Sending Volume Register...");
    sendData(TPA6130_VOLUME_AND_MUTE_REGISTER, buildVolumeRegisterData());
}

void TPA6130A2::sendControlRegister()
{
    Serial.println("Sending Control Register...");
    sendData(TPA6130_CONTROL_REGISTER, buildControlRegisterData());
}

void TPA6130A2::sendImpedanceRegister()
{
    Serial.println("Sending Impedance Register...");
    sendData(TPA6130_OUTPUT_IMPEDANCE_REGISTER, buildImpedanceRegisterData());
}

//--------------------------Builders ------------------------------

byte TPA6130A2::buildVolumeRegisterData()
{
    byte data = (!this->leftMute << 7) | (!this->rightMute << 6) | (this->gain & 0x3F);
    return data;
}

byte TPA6130A2::buildControlRegisterData()
{
    byte data = (this->leftEnabled << 7) | (this->rightEnabled << 6) | (this->mode << 5);
    return data;
}

byte TPA6130A2::buildImpedanceRegisterData()
{
    return (this->leftHighImpedance << 1) | this->rightHighImpedance;
}

//-------------------------Comms--------------------------------
void TPA6130A2::sendData(byte regAddress, byte data)
{
    byte message[2];
    message[0] = regAddress;
    message[1] = data;
    this->wire.beginTransmission(I2C_ADDRESS);
    Serial.print("Sending data to register:  ");
    Serial.print(regAddress, BIN);
    Serial.print(", data: ");
    Serial.println(data, BIN);
    // May want to break up into this->wire.writes
    this->wire.write(message, 2);
    this->wire.endTransmission();
}

byte TPA6130A2::sendRead(byte regAddress)
{
    byte data;
    this->wire.beginTransmission(I2C_ADDRESS);
    this->wire.write(regAddress);
    this->wire.requestFrom(I2C_ADDRESS, 1);

    if (this->wire.available())
    {
        data = this->wire.read();
        this->wire.endTransmission();
    }
    return data;
}

//------------------------Readers------------------------

void TPA6130A2::readVolumeRegister()
{
    byte data = sendRead(TPA6130_VOLUME_AND_MUTE_REGISTER);

    if (data != NULL)
    {
        this->leftMute = (data >> 7) & 0x1;
        this->rightMute = (data >> 6) & 0x1;
        this->gain = data & 0x3F;
    }
}

void TPA6130A2::readControlRegister()
{
    byte data = sendRead(TPA6130_CONTROL_REGISTER);

    if (data != NULL)
    {
        this->leftEnabled = (data >> 7) & 0x1;
        this->rightEnabled = (data >> 6) & 0x1;
    }
}

void TPA6130A2::readImpedanceRegister()
{
    byte data = sendRead(TPA6130_OUTPUT_IMPEDANCE_REGISTER);

    if (data != NULL)
    {
        leftHighImpedance = data & 0x2;
        rightHighImpedance = data & 0x1;
    }
}
