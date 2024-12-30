#ifndef OUTPUT_NUM_2_MCP_PIN_H
#define OUTPUT_NUM_2_MCP_PIN_H

#include <iostream>

namespace gpio_expander {

struct McpPin_t {
  uint8_t chip_number;  // [0..7]
  uint8_t pin_number;   // [0..15]
};

McpPin_t outputNum2McpPin(uint8_t output_number);

}  // namespace gpio_expander

#endif  // OUTPUT_NUM_2_MCP_PIN_H