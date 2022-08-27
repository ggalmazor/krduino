#ifndef Remote_h
#define Remote_h
#include <stdint.h>
#define NUMBER_OF_BUTTONS 7

typedef enum class Mapping { SAMSUNG_SMART_TV,
                             GENERIC_4_BY_6 } Mapping;

typedef enum class Button { NONE,
                            UP,
                            RIGHT,
                            DOWN,
                            LEFT,
                            CENTER,
                            HARD_LEFT,
                            HARD_RIGHT } Button;

struct CommandToButton {
  uint8_t command;
  Button button;
};

struct MappingToCommandToButtons {
  Mapping mapping;
  CommandToButton commandsToButtons[];
};

class Remote {
public:
  Remote(uint8_t ir_remote_pin, Mapping _mapping);
  void init();
  Button read_button();
private:
  uint8_t _ir_remote_pin;
  Mapping _mapping;
  CommandToButton _mappings[7];

  Button lookup_button(uint8_t command);
};
#endif