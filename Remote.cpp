#include "Remote.h"
#include <stdint.h>
#include <IRremote.hpp>

Remote::Remote(uint8_t ir_remote_pin, Mapping mapping) {
  _ir_remote_pin = ir_remote_pin;
  _mapping = mapping;
}

void Remote::init() {
  if (_mapping == Mapping::GENERIC_4_BY_6) {
    Serial.println("Remote: setting mappings for generic 4x6 IR remote");
    _mappings[0] = CommandToButton{ 0x1, Button::UP };
    _mappings[1] = CommandToButton{ 0x4, Button::LEFT };
    _mappings[2] = CommandToButton{ 0x5, Button::CENTER };
    _mappings[3] = CommandToButton{ 0x8, Button::HARD_LEFT };
    _mappings[4] = CommandToButton{ 0x9, Button::DOWN };
    _mappings[5] = CommandToButton{ 0xA, Button::HARD_RIGHT };
    _mappings[6] = CommandToButton{ 0x6, Button::RIGHT };
  } else {
    Serial.println("Remote: unsupported mapping");
  }

  IrReceiver.begin(_ir_remote_pin, ENABLE_LED_FEEDBACK);
  Serial.println("Remote ready");
}

Button Remote::read_button() {
  if (IrReceiver.decode()) {
    // IrReceiver.printIRResultShort(&Serial);
    Button button = lookup_button(IrReceiver.decodedIRData.command);
    IrReceiver.resume();
    return button;
  }
  return Button::NONE;
}

Button Remote::lookup_button(uint8_t command) {
  bool found = false;
  CommandToButton c2b;
  for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++)
    if (_mappings[i].command == command) {
      c2b = _mappings[i];
      found = true;
    }

  if (!found)
    return Button::NONE;
  return c2b.button;
}