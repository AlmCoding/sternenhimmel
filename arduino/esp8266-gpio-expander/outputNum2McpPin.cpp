#include "outputNum2McpPin.hpp"
#include <math.h>

namespace gpio_expander {

McpPin_t outputNum2McpPin(uint8_t output_number) {
  /**
   * output_number [1..120]
   */

  int32_t outnum = output_number;

  // Connector shift
  if (outnum <= 108) {
    outnum += 6;
  } else if (outnum <= 114) {
    outnum -= 108;
  }

  // Reverse number
  outnum = 121 - outnum;

  uint32_t mcp_offset = 0;
  if (outnum > 60) {
    outnum -= 60;
    mcp_offset = 4;
  }

  int rj45_connector_number = ceil(float(outnum) / 6);
  int32_t rj45_connector_pin = outnum - (rj45_connector_number - 1) * 6;

  int32_t range[] = { 6, 5, 4, 3, 2, 1 };
  int32_t pseudo_rj_45_connector_pin = range[rj45_connector_pin - 1];
  int32_t pseudo_pin_number = (rj45_connector_number - 1) * 6 + pseudo_rj_45_connector_pin;

  int mcp_number = ceil(float(pseudo_pin_number) / 16);
  int32_t pin_number = pseudo_pin_number - (mcp_number - 1) * 16;

  McpPin_t mcp_pin;
  mcp_pin.chip_number = mcp_offset + mcp_number - 1;
  mcp_pin.pin_number = pin_number - 1;

  return mcp_pin;
}

}  // namespace gpio_expander
