import time
from i2c import i2c

from enum import Enum


class mcp23017(i2c):
    # Register addr for IOCON.BANK=0 and IOCON.BANK=1
    IOCON = (0x0A, 0x05)
    GPIOA = (0x12, 0x09)
    GPIOB = (0x13, 0x19)
    IODIRA = (0x00, 0x00)
    IODIRB = (0x01, 0x10)

    def __init__(self, slave):
        i2c.__init__(self, slave)
        self.iocon_bank = 0
        
    def addr(self, addr):
        return addr[self.iocon_bank]
    
    def write(self, register, values):
        return i2c.write(self, self.addr(register), values)
    
    def read(self, register, number):
        return i2c.read(self, self.addr(register), number)
    
    def config_ports(self, port_b, port_a):
        # Config port A and B as outputs
        self.write(mcp23017.IODIRA, [port_a, port_b])
    
    def write_ports(self, port_b, port_a):
        # Write gpio outputs on port A and B
        self.write(mcp23017.GPIOA, [port_a, port_b])
        
    def read_ports(self):
        # Read gpio outputs on port A and B
        port_a_b = self.read(mcp23017.GPIOA, 2)
        return port_a_b[1], port_a_b[0]
    
    def write_pin(self, pin, state=True):
        # Check pin number range
        assert pin in range(1, 17)
        # Construct pin mask
        mask = 1 << (pin - 1)
        mask_b, mask_a = (mask & 0xFFFF).to_bytes(2, 'big')
        # print('{} - {}'.format(mask_b, mask_a))
        # Read current port states
        current_b, current_a = self.read_ports()
        
        if state:
            # Set pin
            new_b = current_b | mask_b
            new_a = current_a | mask_a
        else:
            # Clear pin
            new_b = current_b & (~mask_b)
            new_a = current_a & (~mask_a)
        
        # Write new port states
        self.write_ports(new_b, new_a)


if __name__ == '__main__':
    
    slave_addr = 0x20
    mcp = mcp23017(slave_addr)
    mcp.config_ports(0x00, 0x00)
    
    mcp.write_pin(3,1)
    
    for i in range(100):
        mcp.write_pin(6,0)
        time.sleep(0.5)
        mcp.write_pin(6,1)
        time.sleep(0.5)
    
    mcp.write_ports(0xff, 0x0f)
    time.sleep(1)
    ab = mcp.read_ports()
    print(ab)
    
    exit(0)
