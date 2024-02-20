#include "clsp.hpp"

CLSPJoystick::CLSPJoystick() {
  if (libusb_init(NULL) < 0) {
    throw std::runtime_error("Unable to init libusb context");
  }

  this->usb_handle = libusb_open_device_with_vid_pid(NULL, 0x25bb, 0x00d3);
  if (this->usb_handle == nullptr) {
    throw std::runtime_error("Unable to open device");
  }

  auto usb_device = libusb_get_device(this->usb_handle);

  // N.B.: Needs to be supported by the OS
  if (libusb_set_auto_detach_kernel_driver(this->usb_handle, true) < 0) {
    throw std::runtime_error("Unable to auto detach kernel driver");
  }

  libusb_claim_interface(this->usb_handle, INTERFACE_MAIN);
  libusb_claim_interface(this->usb_handle, INTERFACE_FX);

  std::cout << "Device opened and claimed" << std::endl;
  std::cout << "Initialisation ..." << std::endl;

  initSequence();

  std::cout << "Device ready !" << std::endl;
}

CLSPJoystick::~CLSPJoystick() {
  std::cout << "Stopping all effects and resetting device" << std::endl;

  playEffect(false, 0);
  deviceControl(true);

  std::cout << "Releasing device interface" << std::endl;

  libusb_release_interface(this->usb_handle, INTERFACE_MAIN);
  libusb_release_interface(this->usb_handle, INTERFACE_FX);
  libusb_close(this->usb_handle);
  libusb_exit(NULL);
}

int CLSPJoystick::deviceControl(bool reset) {
  unsigned char txBuff[2] = {};

  txBuff[0] = 0x0c;

  if (reset) {
    txBuff[1] = 0x04;  // device reset
  } else {
    txBuff[1] = 0x03;  // stop all effects
  }

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::setGain(uint8_t gain = 255) {
  unsigned char txBuff[2] = {};

  txBuff[0] = 0x0d;
  txBuff[1] = gain;

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::playEffect(bool play, int repetitions = 1) {
  unsigned char txBuff[4] = {};

  // Play effect
  txBuff[0] = 0x0a;  // Report ID
  txBuff[1] = 0x01;  // Effect block index

  if (play) {
    txBuff[2] = 0x01;         // Effect start
    txBuff[3] = repetitions;  // Loop count
  } else {
    txBuff[2] = 0x03;  // Effect stop
    txBuff[3] = 0x00;
  }

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::setMagnitudeSettings(uint8_t magnitude = 127) {
  unsigned char txBuff[4] = {};

  // Magnitude command
  txBuff[0] = 0x05;  // Report ID
  txBuff[1] = 0x01;  // Effect block index
  txBuff[2] = magnitude;
  txBuff[3] = 0x00;

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::setRampSettings(int8_t ramp_start = -128,
                                  int8_t ramp_end = 127) {
  unsigned char txBuff[4] = {};

  // Magnitude command
  txBuff[0] = 0x06;  // Report ID
  txBuff[1] = 0x01;  // Effect block index
  txBuff[2] = ramp_start;
  txBuff[3] = ramp_end;

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::setEnvelopeSettings(uint8_t attack = 0, uint8_t fade = 0,
                                      uint16_t attack_time = 300,
                                      uint16_t fade_time = 300) {
  unsigned char txBuff[8] = {};

  // Envelope command
  txBuff[0] = 0x02;                       // Report ID
  txBuff[1] = 0x01;                       // Effect block index
  txBuff[2] = attack;                     // Attack level
  txBuff[3] = fade;                       // Fade level
  txBuff[4] = (attack_time >> 0 & 0xFF);  // Attack time LSB
  txBuff[5] = (attack_time >> 8 & 0xFF);  // Attack time MSB
  txBuff[6] = (fade_time >> 0 & 0xFF);    // Fade time LSB
  txBuff[7] = (fade_time >> 8 & 0xFF);    // Fade time MSB

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::setConditionalSettings(uint8_t pos_coeff = 63,
                                         uint8_t neg_coeff = 63,
                                         uint8_t pos_sat = 127,
                                         uint8_t neg_sat = 127,
                                         uint8_t deadband = 0) {
  int ret = 0;
  unsigned char txBuff[9] = {};

  // Conditional effect settings
  txBuff[0] = 0x03;       // Report ID
  txBuff[1] = 0x01;       // Effect block index
  txBuff[2] = 0x00;       // Block offset
  txBuff[3] = 0xff;       // Center Point offset
  txBuff[4] = pos_coeff;  // Positive coefficient
  txBuff[5] = neg_coeff;  // Negative coefficient
  txBuff[6] = pos_sat;    // Positive saturation
  txBuff[7] = neg_sat;    // Negative saturation
  txBuff[8] = deadband;   // Dead band

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                  sizeof(txBuff), nullptr, TIMEOUT);

  txBuff[2] = 0x01;

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                  sizeof(txBuff), nullptr, TIMEOUT);

  return ret;
}

int CLSPJoystick::setPeriodicSettings(uint8_t magnitude = 127,
                                      int8_t offset = 0xff,
                                      uint8_t phase = 0x00,
                                      uint8_t period = 100) {
  unsigned char txBuff[7] = {};

  // Periodic effect settings
  txBuff[0] = 0x04;                  // Report ID
  txBuff[1] = 0x01;                  // Effect block index
  txBuff[2] = magnitude;             // Magnitude
  txBuff[3] = offset;                // Wave offset
  txBuff[4] = phase;                 // Wave phase
  txBuff[5] = (period >> 0 & 0xFF);  // Wave period LSB
  txBuff[6] = (period >> 8 & 0xFF);  // Wave period MSB

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

int CLSPJoystick::setGeneralSettings(
    uint8_t function_id, uint16_t duration = 3000,
    uint16_t trigger_interval = 0, uint16_t sample_period = 0,
    uint8_t gain = 127, uint8_t trigger_button = 0xff, int8_t direction = 0,
    uint16_t start_delay = 0) {
  unsigned char txBuff[16] = {};

  // General command
  txBuff[0] = 0x01;  // Report ID
  txBuff[1] = 0x01;  // Effect block index
  txBuff[2] = function_id;
  txBuff[3] = (duration >> 0 & 0xFF);          // Effect duration LSB
  txBuff[4] = (duration >> 8 & 0xFF);          // Effect duration MSB
  txBuff[5] = (trigger_interval >> 0 & 0xFF);  // Trigger repeat interval LSB
  txBuff[6] = (trigger_interval >> 8 & 0xFF);  // Trigger repeat interval MSB
  txBuff[7] = (sample_period >> 0 & 0xFF);     // Sample period LSB
  txBuff[8] = (sample_period >> 8 & 0xFF);     // Sample period MSB
  txBuff[9] = gain;                            // Gain
  txBuff[10] = trigger_button;                 // Trigger button
  txBuff[11] = 0x04;       // Axes(b0-b1)/Direction(b2) enable flags
  txBuff[12] = direction;  // Direction
  txBuff[13] = 0x00;       // Axes/Direction Enable flags ?
  txBuff[14] = (start_delay >> 0 & 0xFF);  // Start delay LSB
  txBuff[15] = (start_delay >> 8 & 0xFF);  // Start delay MSB

  return libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_MAIN, txBuff,
                                   sizeof(txBuff), nullptr, TIMEOUT);
}

void CLSPJoystick::ramp() {
  playEffect(false);
  playEffect(false);

  setRampSettings();

  setEnvelopeSettings();

  setGeneralSettings(CLSP_RAMP);

  setRampSettings();

  playEffect(true);
}

void CLSPJoystick::constantForceEffect() {
  playEffect(false);
  playEffect(false);

  setMagnitudeSettings(0x80);

  setEnvelopeSettings();

  setGeneralSettings(CLSP_CONSTANT_FORCE);

  setMagnitudeSettings(0x80);

  playEffect(true);
}

void CLSPJoystick::periodicEffect(uint8_t effect_id) {
  playEffect(false);
  playEffect(false);

  setPeriodicSettings();

  setEnvelopeSettings();

  setGeneralSettings(effect_id, 30000);

  setPeriodicSettings();

  playEffect(true);
}

void CLSPJoystick::conditionalEffect(uint8_t effect_id) {
  playEffect(false);
  playEffect(false);

  setConditionalSettings();

  setGeneralSettings(effect_id);

  setConditionalSettings();

  playEffect(true);
}

void CLSPJoystick::updateStatus() {
  unsigned char rxBuff[7] = {};

  libusb_interrupt_transfer(this->usb_handle, IN_ENDPOINT_MAIN, rxBuff,
                            sizeof(rxBuff), nullptr, TIMEOUT);

  this->buttons = rxBuff[1];
  this->hat_switch = rxBuff[2];
  this->x = (rxBuff[4] << 8) | rxBuff[3];
  this->y = (rxBuff[6] << 8) | rxBuff[5];
}

std::tuple<uint16_t, uint16_t> CLSPJoystick::getPosition() {
  return {this->x, this->y};
}

std::bitset<8> CLSPJoystick::getButtons() { return this->buttons; }

int CLSPJoystick::getHat() { return this->hat_switch; }

int CLSPJoystick::setGlobalFXGains() {
  unsigned char txBuffFX[64] = {};
  int ret = 0;

  txBuffFX[0] = 0x3f;
  txBuffFX[1] = 0x11;
  txBuffFX[4] = 0x01;
  txBuffFX[5] = 0x86;
  txBuffFX[6] = 0x2b;
  txBuffFX[7] = 0x80;
  txBuffFX[8] = 0x21;
  txBuffFX[9] = 0x0a;
  txBuffFX[10] = 0x50;

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                  sizeof(txBuffFX), nullptr, TIMEOUT);

  txBuffFX[4] = 0x02;

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                  sizeof(txBuffFX), nullptr, TIMEOUT);

  txBuffFX[9] = 0x0c;

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                  sizeof(txBuffFX), nullptr, TIMEOUT);

  txBuffFX[4] = 0x01;

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                  sizeof(txBuffFX), nullptr, TIMEOUT);

  return ret;
}

int CLSPJoystick::initSequence() {
  unsigned char txBuffFX[64] = {};

  int ret = 0;

  deviceControl(true);

  deviceControl(false);

  setGain(0xff);

  // Set output report of interface 1 for effect class 1 (0x21)
  unsigned char set_report[4] = {0x01, 0x01, 0x00, 0x00};
  ret = libusb_control_transfer(this->usb_handle, 0x21, 0x09, 0x0301, 0,
                                set_report, sizeof(set_report), TIMEOUT);

  setMagnitudeSettings(0x80);

  setEnvelopeSettings();

  setGeneralSettings(CLSP_CONSTANT_FORCE);

  setMagnitudeSettings(0x80);

  // Init FX loop on the 2nd endpoint
  // May be unnecessary, see if the setGlobalFXGains has any effect
  // Could be for the CAN over USB pipeline
  txBuffFX[0] = 0x3f;
  txBuffFX[1] = 0x11;
  txBuffFX[4] = 0x7e;
  txBuffFX[5] = 0x86;
  txBuffFX[6] = 0x2f;
  txBuffFX[7] = 0x60;
  txBuffFX[8] = 0x21;
  txBuffFX[10] = 0x03;

  ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                  sizeof(txBuffFX), nullptr, TIMEOUT);

  txBuffFX[6] = 0x40;
  txBuffFX[7] = 0x00;
  txBuffFX[8] = 0x10;
  txBuffFX[10] = 0x00;

  for (int i = 1; i < 128; i++) {
    txBuffFX[4] = i;
    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);
  }

  for (int i = 1; i < 3; i++) {
    txBuffFX[4] = i;

    txBuffFX[7] = 0x18;
    txBuffFX[8] = 0x30;

    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);

    txBuffFX[7] = 0x80;
    txBuffFX[8] = 0x21;
    txBuffFX[9] = 0x0a;

    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);

    txBuffFX[9] = 0x0c;

    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);

    txBuffFX[9] = 0x01;

    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);

    txBuffFX[7] = 0x45;
    txBuffFX[8] = 0x30;
    txBuffFX[9] = 0x00;

    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);

    txBuffFX[7] = 0x35;

    ret = libusb_interrupt_transfer(this->usb_handle, OUT_ENDPOINT_FX, txBuffFX,
                                    sizeof(txBuffFX), nullptr, TIMEOUT);
  }
  return ret;
}