#ifndef CLS_P_HPP
#define CLS_P_HPP

#include <libusb-1.0/libusb.h>

#include <bitset>
#include <iostream>
#include <limits>
#include <tuple>

#define CLSP_CONSTANT_FORCE 0x01
#define CLSP_RAMP 0x02
#define CLSP_PERIODIC_SQUARE 0x03
#define CLSP_PERIODIC_SINE 0x04
#define CLSP_PERIODIC_TRIANGLE 0x05
#define CLSP_PERIODIC_SAWTOOTHUP 0x06
#define CLSP_PERIODIC_SAWTOOTHDOWN 0x07
#define CLSP_PERIODIC_COND_SPRING 0x08
#define CLSP_PERIODIC_COND_DAMPER 0x09
#define CLSP_PERIODIC_COND_INERTIA 0x0a
#define CLSP_PERIODIC_COND_FRICTION 0x0b
//#define CLSP_CUSTOM_FORCE_DATA 0x0c

class CLSPJoystick {
 public:
  CLSPJoystick();

  ~CLSPJoystick();

  /**
   * Sends a message to the device control report 0x0c
   * @param reset bool value indicating the device needs to be reset. Setting it
   * to false disables the blocking autocenter spring.
   * @return success
   */
  int deviceControl(bool reset);

  /**
   * Sets the device gain
   * @param gain uint [0,255] : gain value
   * @return success
   */
  int setGain(uint8_t gain);

  /**
   * Plays the current set effect
   * @param play bool value triggering the effect rendering
   * @param repetitions number of repetitions of the effect
   * @return success
   */
  int playEffect(bool play, int repetitions);

  /**
   * Sets the constant force effect magnitude parameter
   * @param magnitude uint [0,255] : force magnitude
   * @return success
   */
  int setMagnitudeSettings(uint8_t magnitude);

  /**
   * Sets the ramp effect parameters
   * @param ramp_start int [-128,127] : normalized magnitude at the start of the
   * effect
   * @param ramp_end int [-128,127] : normalized magnitude at the end of the
   * effect
   * @return success
   */
  int setRampSettings(int8_t ramp_start, int8_t ramp_end);

  /**
   * Sets the effect envelope parameter
   * @param attack uint [0,255] : normalized amplitude for the start of the
envelope, from the baseline
   * @param fade uint [0,255] : normalized amplitude to end the envelope, from
baseline
   * @param attack_time uint [0,32767] : transition time to reach the sustain
level, in ms
   * @param fade_time uint [0,32767] : fade time to reach the fade level, in ms
   * @return success
   */
  int setEnvelopeSettings(uint8_t attack, uint8_t fade, uint16_t attack_time,
                          uint16_t fade_time);

  /**
   * Sets the conditional effects parameters
   * @param pos_coeff uint [0,255] : normalized coefficient constant on the
positive side of the neutral position
   * @param neg_coeff uint [0,255] : normalized coefficient constant on the
negative side of the neutral position
   * @param pos_sat uint [0,255] : normalized maximum positive force output
   * @param neg_sat uint [0,255] : normalized maximum negative force output
   * @param deadband uint [0,255] : region around the center point where the
condition is not active
   * @return success
   */
  int setConditionalSettings(uint8_t pos_coeff, uint8_t neg_coeff,
                             uint8_t pos_sat, uint8_t neg_sat,
                             uint8_t deadband);

  /**
   * Sets the periodic effects parameters
   * @param magnitude uint [0,255] : normalized magnitude of the waveform
   * @param offset int [-128,127] : normalized baseline offset of the waveform
   * @param phase uint [0,255] : position in the wave that playback begins,
   * normalized between [0,360]
   * @param period uint [0,32767] : waveform period in ms
   * @return success
   */
  int setPeriodicSettings(uint8_t magnitude, int8_t offset, uint8_t phase,
                          uint8_t period);

  /**
   * Sets the general effect parameters
   * @param function_id uint [1,12] : ID of the effect to play
   * @param duration uint [0,32767] : the total duration of the effect, in ms,
max is infinite
   * @param trigger_interval uint [0,32767] : the auto-repeat interval, for
playback of effects triggered by holding down the trigger button, in ms
   * @param sample_period uint [0,32767] : the period at which the device
should
   * play back the effect, in ms
   * @param gain uint [0, 255] : the gain to be applied to the effect
(normalised scaling factor applied to all magnitudes/envelopes)
   * @param trigger_button uint [1,8] : the identifier or offset of the button
to be used to trigger playback of this effect. A null value unties the effect
from any button
   * @param direction int [-128,127] : effect direction (polar coordinates)
   * @param start_delay uint [0,32767] : start delay in ms
   * @return success
   */
  int setGeneralSettings(uint8_t function_id, uint16_t duration,
                         uint16_t trigger_interval, uint16_t sample_period,
                         uint8_t gain, uint8_t trigger_button, int8_t direction,
                         uint16_t start_delay);

  /**
   * Plays a constant force effect
   */
  void constantForceEffect();

  /**
   * Plays a force ramp effect
   */
  void ramp();

  /**
   * Plays a periodic effect
   * @param effect_id : ID of the waveform to play
   */
  void periodicEffect(uint8_t effect_id);

  /**
   * Plays a conditional effect
   * @param effect_id : ID of the effect to play
   */
  void conditionalEffect(uint8_t effect_id);

  /**
   * Updates the status of the device, i.e. position and buttons status
   */
  void updateStatus();

  /**
   * Returns the last queried position
   * @return tuple of relative coordinates (x,y)
   */
  std::tuple<uint16_t, uint16_t> getPosition();

  /**
   * Returns the last queried buttons status
   * @return biset state of the buttons. Button 4 is sticky.
   */
  std::bitset<8> getButtons();

  /**
   * Returns the last queried hat switch status
   * @return int representing the hat status :  0 -> center, 1 -> up, 3 ->
   * right, 5 -> down, 7 -> left
   */
  int getHat();

 private:
  // USB xfer constants
  const int INTERFACE_MAIN = 0;
  const int INTERFACE_FX = 1;

  const int OUT_ENDPOINT_MAIN = 0x01;
  const int OUT_ENDPOINT_FX = 0x02;

  const int IN_ENDPOINT_MAIN = 0x81;
  const int IN_ENDPOINT_FX = 0x82;

  // Xfer timeout in ms (0 = inf)
  const int TIMEOUT = 0;

  libusb_device_handle* usb_handle = nullptr;

  bool detached_kernel_driver = false;

  // 1b per button, bit4 is sticky
  std::bitset<8> buttons;

  // 1 -> up, 3 -> right, 5 -> down, 7 -> left
  int hat_switch;

  // Between 0 and 65535
  uint16_t x;
  uint16_t y;

  int initSequence();
  int setGlobalFXGains();
};

#endif