#include "clsp.hpp"

int main(int argc, char* argv[]) {
  CLSPJoystick joystick;

  getchar();

  std::cout << "Constant Force" << std::endl;
  joystick.constantForceEffect();

  getchar();

  std::cout << "Ramp Force" << std::endl;
  joystick.ramp();

  getchar();

  std::cout << "Spring" << std::endl;
  joystick.conditionalEffect(CLSP_PERIODIC_COND_SPRING);

  getchar();

  std::cout << "Square wave" << std::endl;
  joystick.periodicEffect(CLSP_PERIODIC_SQUARE);

  getchar();

  std::cout << "Damper" << std::endl;
  joystick.conditionalEffect(CLSP_PERIODIC_COND_DAMPER);

  getchar();

  std::cout << "Sine wave" << std::endl;
  joystick.periodicEffect(CLSP_PERIODIC_SINE);

  getchar();

  std::cout << "Triangle wave" << std::endl;
  joystick.periodicEffect(CLSP_PERIODIC_TRIANGLE);

  getchar();

  std::cout << "Sawtooth up wave" << std::endl;
  joystick.periodicEffect(CLSP_PERIODIC_SAWTOOTHUP);

  getchar();

  std::cout << "Sawtooth down wave" << std::endl;
  joystick.periodicEffect(CLSP_PERIODIC_SAWTOOTHDOWN);

  getchar();

  std::cout << "Inertia" << std::endl;
  joystick.conditionalEffect(CLSP_PERIODIC_COND_INERTIA);

  getchar();

  std::cout << "Friction" << std::endl;
  joystick.conditionalEffect(CLSP_PERIODIC_COND_FRICTION);

  getchar();

  joystick.updateStatus();

  std::cout << "Buttons status : " << joystick.getButtons();
  std::cout << " ; hat switch status : " << joystick.getHat();
  std::cout << " ; x : " << std::get<0>(joystick.getPosition());
  std::cout << " ; y : " << std::get<1>(joystick.getPosition()) << std::endl;

  return 0;
}