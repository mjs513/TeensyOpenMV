import pyb
import machine
import time

import ustruct
import time


class vl53l0x:
    def __init__(self, i2c, address=0x29):
        self.i2c = i2c
        self._address = address
        self.init()
        self.default_settings()

    def _set_reg8(self, address, value):
        data = ustruct.pack('>HB', address, value)
        self.i2c.writeto(self._address, data)

    def _set_reg16(self, address, value):
        data = ustruct.pack('>HH', address, value)
        self.i2c.writeto(self._address, data)

    def _get_reg8(self, address):
        self.i2c.start()
        self.i2c.write(ustruct.pack('>BH', self._address << 1, address))
        data = self.i2c.readfrom(self._address, 1)
        return data[0]

    def _get_reg16(self, address):
        self.i2c.start()
        self.i2c.write(ustruct.pack('>BH', self._address << 1, address))
        data = self.i2c.readfrom(self._address, 2)
        return ustruct.unpack('>B', data)[0]

    def init(self):
        self._set_reg8(0xbf,0x00)
        time.sleep(100)
        #if self._get_reg8(0xbf) != 1:
        #    raise RuntimeError("Failure reset")

        # Recommended setup from the datasheet
        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x00, 0x00)

        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x09, 0x00)
        self._set_reg8(0x10, 0x00)
        self._set_reg8(0x11, 0x00)

        self._set_reg8(0x24, 0x01)
        self._set_reg8(0x25, 0xFF)
        self._set_reg8(0x75, 0x00)

        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x4E, 0x2C)
        self._set_reg8(0x48, 0x00)
        self._set_reg8(0x30, 0x20)

        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x30, 0x09)
        self._set_reg8(0x54, 0x00)
        self._set_reg8(0x31, 0x04)
        self._set_reg8(0x32, 0x03)
        self._set_reg8(0x40, 0x83)
        self._set_reg8(0x46, 0x25)
        self._set_reg8(0x60, 0x00)
        self._set_reg8(0x27, 0x00)
        self._set_reg8(0x50, 0x06)
        self._set_reg8(0x51, 0x00)
        self._set_reg8(0x52, 0x96)
        self._set_reg8(0x56, 0x08)
        self._set_reg8(0x57, 0x30)
        self._set_reg8(0x61, 0x00)
        self._set_reg8(0x62, 0x00)
        self._set_reg8(0x64, 0x00)
        self._set_reg8(0x65, 0x00)
        self._set_reg8(0x66, 0xA0)

        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x22, 0x32)
        self._set_reg8(0x47, 0x14)
        self._set_reg8(0x49, 0xFF)
        self._set_reg8(0x4A, 0x00)

        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x7A, 0x0A)
        self._set_reg8(0x7B, 0x00)
        self._set_reg8(0x78, 0x21)

        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x23, 0x34)
        self._set_reg8(0x42, 0x00)
        self._set_reg8(0x44, 0xFF)
        self._set_reg8(0x45, 0x26)
        self._set_reg8(0x46, 0x05)
        self._set_reg8(0x40, 0x40)
        self._set_reg8(0x0E, 0x06)
        self._set_reg8(0x20, 0x1A)
        self._set_reg8(0x43, 0x40)

        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x34, 0x03)
        self._set_reg8(0x35, 0x44)

        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x31, 0x04)
        self._set_reg8(0x4B, 0x09)
        self._set_reg8(0x4C, 0x05)
        self._set_reg8(0x4D, 0x04)

        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x44, 0x00)
        self._set_reg8(0x45, 0x20)
        self._set_reg8(0x47, 0x08)
        self._set_reg8(0x48, 0x28)
        self._set_reg8(0x67, 0x00)
        self._set_reg8(0x70, 0x04)
        self._set_reg8(0x71, 0x01)
        self._set_reg8(0x72, 0xFE)
        self._set_reg8(0x76, 0x00)
        self._set_reg8(0x77, 0x00)

        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x0D, 0x01)

        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x80, 0x01)
        self._set_reg8(0x01, 0xF8)

        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x8E, 0x01)
        self._set_reg8(0x00, 0x01)
        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x80, 0x00)

    def default_settings(self):
        # "Set I2C standard mode"
        self._set_reg8(0x88, 0x00)

        self._set_reg8(0x80, 0x01)
        self._set_reg8(0xFF, 0x01)
        self._set_reg8(0x00, 0x00)
        stop_variable = self._get_reg8(0x91)
        self._set_reg8(0x00, 0x01)
        self._set_reg8(0xFF, 0x00)
        self._set_reg8(0x80, 0x00)

        # disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
        self._set_reg8(0x60, self._get_reg8(0x60) | 0x12)

        # set final range signal rate limit to 0.25 MCPS (million counts per second)
        #self._set_reg16(0x44, 0.25 * (1 << 7))

        self._set_reg8(0x01, 0xFF)

        self._set_reg8(0x00, 0x02) # Sysrange start


    def identify(self):
        """Retrieve identification information of the sensor."""
        return {
            'model': self._get_reg8(0xc0),
            'revision': (self._get_reg8(0x00c2)),
            'module_revision': (self._get_reg8(0x0003),
                                self._get_reg8(0x0004)),
            'date': self._get_reg16(0x006),
            'time': self._get_reg16(0x008),
        }

    def address(self, address=None):
        """Change the I2C address of the sensor."""
        if address is None:
            return self._address
        if not 8 <= address <= 127:
            raise ValueError("Wrong address")
        self._set_reg8(0x0212, address)
        self._address = address

    def range(self):
        """Measure the distance in millimeters. Takes 0.01s."""

        #self._get_reg8(0x13)
        time.sleep(1)
        #return self._get_reg8(0x62) # Result range value
        data = self._get_reg16(0x14+10)
        self._set_reg8(0x0b, 0x01)
        return data


i2c = machine.I2C(sda=pyb.Pin('P5'), scl=pyb.Pin('P4'), freq=400000)

mysensor = vl53l0x(i2c, 0x29)

while(True):
    print(mysensor.range())



