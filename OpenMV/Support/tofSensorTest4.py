import pyb
import machine
import struct
import utime

def bswap(val):
    return struct.unpack('<H', struct.pack('>H', val))[0]
def mread_word_data(adr, reg):
    return bswap(bus.read_word_data(adr, reg))
def mwrite_word_data(adr, reg, data):
    return bus.write_word_data(adr, reg, bswap(data))
def makeuint16(lsb, msb):
    return ((msb & 0xFF) << 8)  | (lsb & 0xFF)
def VL53L0X_decode_vcsel_period(vcsel_period_reg):
# Converts the encoded VCSEL period register value into the real
# period in PLL clocks
    vcsel_period_pclks = (from_bytes_big(vcsel_period_reg) + 1) << 1;
    return vcsel_period_pclks;
def from_bytes_big(b):
    n = 0
    for x in b:
        n <<= 8
        n |= x
    return n

VL53L0X_REG_IDENTIFICATION_MODEL_ID		= 0x00c0
VL53L0X_REG_IDENTIFICATION_REVISION_ID		= 0x00c2
VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD	= 0x0050
VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD	= 0x0070
VL53L0X_REG_SYSRANGE_START			= 0x000

VL53L0X_REG_RESULT_INTERRUPT_STATUS 		= 0x0013
VL53L0X_REG_RESULT_RANGE_STATUS 		= 0x0014

address = 0x29

bus = machine.I2C(sda=pyb.Pin('P5'), scl=pyb.Pin('P4'), freq=400000)

val1 = bus.readfrom_mem(address, VL53L0X_REG_IDENTIFICATION_REVISION_ID, 1)
print ("Revision ID: ", val1)
val1 = bus.readfrom_mem(address, VL53L0X_REG_IDENTIFICATION_MODEL_ID, 1)
print( "Device ID: " , (val1))
#	case VL53L0X_VCSEL_PERIOD_PRE_RANGE:
#		Status = VL53L0X_RdByte(Dev,
#			VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD,
#			&vcsel_period_reg);
val1 = bus.readfrom_mem(address, VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD, 1)
print( "PRE_RANGE_CONFIG_VCSEL_PERIOD=", (val1) , " decode: " , str(VL53L0X_decode_vcsel_period(val1)))


#	case VL53L0X_VCSEL_PERIOD_FINAL_RANGE:
#		Status = VL53L0X_RdByte(Dev,
#			VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD,
#			&vcsel_period_reg);

val1 = bus.readfrom_mem(address, VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD, 1)
print ("FINAL_RANGE_CONFIG_VCSEL_PERIOD=" , (val1) , " decode: " , str(VL53L0X_decode_vcsel_period(val1)))

#		Status = VL53L0X_WrByte(Dev, VL53L0X_REG_SYSRANGE_START, 0x01);
data = bytearray(1)
data[0] = 0x01
bus.writeto_mem(address, VL53L0X_REG_SYSRANGE_START, data)


#		Status = VL53L0X_RdByte(Dev, VL53L0X_REG_RESULT_RANGE_STATUS,
#			&SysRangeStatusRegister);
#		if (Status == VL53L0X_ERROR_NONE) {
#			if (SysRangeStatusRegister & 0x01)
#				*pMeasurementDataReady = 1;
#			else
#				*pMeasurementDataReady = 0;
#		}

cnt = 0
while (cnt < 100): # 1 second waiting time max
        utime.sleep_ms(1)
        val = bus.readfrom_mem(address, VL53L0X_REG_RESULT_RANGE_STATUS,1)
        if (from_bytes_big(val) & 0x01):
                break
        cnt += 1

if (from_bytes_big(val) & 0x01):
        print( "ready")
else:
        print( "not ready")

#	Status = VL53L0X_ReadMulti(Dev, 0x14, localBuffer, 12);
buff = bytearray(12)
bus.readfrom_mem_into(address, 0x14, buff)
print (buff)
print ("ambient count " , str(makeuint16(buff[7], buff[6])))
print ("signal count " , str(makeuint16(buff[9], buff[8])))
        #tmpuint16 = VL53L0X_MAKEUINT16(localBuffer[11], localBuffer[10]);
print ("distance " , str(makeuint16(buff[11], buff[10])))

DeviceRangeStatusInternal = ((buff[0] & 0x78) >> 3)
print (DeviceRangeStatusInternal)

