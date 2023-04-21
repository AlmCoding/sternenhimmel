#include "outputNum2McpPin.hpp"

namespace gpio_expander {

ExpanderPin_t outputNum2McpPin(int32_t output_number) {
  /**
   * output_number [1..120]
   */

  // Connector shift
  if (output_number <= 108) {
    output_number += 6;
  } else if (output_number <= 114) {
    output_number -= 108;
  }

  // Reverse number
  output_number = 121 - output_number;

  uint32_t mcp_offset = 0;
  if (output_number > 60) {
    output_number -= 60;
    mcp_offset = 4;
  }

  int32_t rj45_connector_number = output_number / 6;
  if ((output_number % 6) > 0) {
    output_number++;
  }
  int32_t rj45_connector_pin = output_number - (rj45_connector_number - 1) * 6;

  int32_t range[] = { 6, 5, 4, 3, 2, 1 };
  int32_t pseudo_rj_45_connector_pin = range[rj45_connector_pin - 1];
  int32_t pseudo_pin_number = (rj45_connector_number - 1) * 6 + pseudo_rj_45_connector_pin;

  int32_t mcp_number = pseudo_pin_number / 16;
  if ((pseudo_pin_number % 16) > 0) {
    pseudo_pin_number++;
  }
  int32_t pin_number = pseudo_pin_number - (mcp_number - 1) * 16;

  ExpanderPin_t expander_pin;
  expander_pin.chip_number = mcp_offset + mcp_number - 1;
  expander_pin.pin_number = pin_number - 1;

  return expander_pin;
}

} // namespace gpio_expander
