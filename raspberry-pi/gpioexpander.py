from mcp23017 import mcp23017
import time
import math


OFFSET_TIME = 0.5
PULSE_TIME = 0.9
PAUSE_TIME = 0.5
PULSE_COUNT = 4
STAY_TIME = 16.0


class GPIOexpander:    
    def __init__(self):
        self._mcps = None
        self.setup_expander()
        
    def setup_expander(self):
        # Instantiate mcps
        self._mcps = [mcp23017(slave) for slave in range(0x20, 0x27)]
        # Set ports to output
        [mcp.config_ports(0x00, 0x00) for mcp in self._mcps]
        # Set ports to zero
        self.clear_outputs()
    
    def clear_outputs(self):
        [mcp.write_ports(0x00, 0x00) for mcp in self._mcps]
        
    def write_outputs(self, outputs=None):
        for output in outputs:
            output_number = output[0]
            output_state = output[1]
            
            if output_number not in range(1, 109):
                continue
            
            mcp_number, pin_number = self.translate_output(output_number)

            mcp = self._mcps[mcp_number - 1]
            mcp.write_pin(pin_number, output_state)
            
    def animate_outputs(self, outputs):
        self.clear_outputs()
        time.sleep(OFFSET_TIME)
        
        outputs_high = [[output, True] for output in outputs]
        outputs_low = [[output, False] for output in outputs]
        
        for i in range(int(PULSE_COUNT)):
            self.write_outputs(outputs_high)
            time.sleep(PULSE_TIME)
            self.write_outputs(outputs_low)
            time.sleep(PAUSE_TIME)
        
        self.write_outputs(outputs_high)
        time.sleep(STAY_TIME)
        self.write_outputs(outputs_low)
        
    def translate_output(self, number):
        # Reverse number
        number = [i for i in range(1, 109)][-number]
        
        rj45_connector_number = math.ceil(number / 6)
        rj45_connector_pin = number - (rj45_connector_number-1)*6
        
        pseudo_rj_45_connector_pin = [i for i in range(1, 7)][-rj45_connector_pin]
        pseudo_pin_number = (rj45_connector_number - 1) * 6 + pseudo_rj_45_connector_pin
        
        mcp_number = math.ceil(pseudo_pin_number / 16)
        pin_number = (pseudo_pin_number % 16)
        pin_number = pseudo_pin_number - (mcp_number - 1) * 16

        return mcp_number, pin_number
    
    def test_expander(self):
        for mcp_number in range(1, 8):
            for pin_number in range(1, 17):
                mcp = self._mcps[mcp_number-1]
                print("{}:{}".format(mcp_number, pin_number))
                mcp.write_pin(pin_number, True)
                time.sleep(0.5)
                self.clear_outputs()
                
    def test_expander_mapping(self):
        for i in range(1, 109):
            self.clear_outputs()
            self.write_outputs([[i, True]])
            time.sleep(0.1)
            
        for i in reversed(range(1, 109)):
            self.clear_outputs()
            self.write_outputs([[i, True]])
            time.sleep(0.1)
            
        self.clear_outputs()
        

if __name__ == '__main__':
    #write_mcp_pin(1, 1, 1)
    a = GPIOexpander()
    a.animate_outputs([1,3,56,66,56])
    a.test_expander_mapping()
