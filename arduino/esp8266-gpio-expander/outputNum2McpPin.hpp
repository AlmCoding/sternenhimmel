#ifndef OUTPUT_NUM_2_MCP_PIN_H
#define OUTPUT_NUM_2_MCP_PIN_H

#include <iostream>

namespace gpio_expander {

struct ExpanderPin_t {
  uint32_t chip_number;
  uint32_t pin_number;
};

ExpanderPin_t outputNum2McpPin(int32_t output_number);

} // namespace gpio_expander

#endif  // OUTPUT_NUM_2_MCP_PIN_H