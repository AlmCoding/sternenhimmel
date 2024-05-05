import smbus


class i2c:
    def __init__(self, slave):
        self._slave = slave
        self._bus = smbus.SMBus(1)
        
    def write(self, cmd, vals):
        self._bus.write_i2c_block_data(self._slave, cmd, vals)
        
    def read(self, cmd, num):
        return self._bus.read_i2c_block_data(self._slave, cmd, num)
