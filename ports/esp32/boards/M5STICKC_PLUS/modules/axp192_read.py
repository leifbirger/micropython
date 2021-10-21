import machine
from machine import Pin
from time import sleep



#from machine import Pin, SPI, I2C


#i2c = I2C(0)
#i2 = I2C(1, scl=Pin(22), sda=Pin(21), freq=400000)

#import axp192


def print_apx_values(pw):
    print("Power status")
    print("BAT_voltage: " + str(pw.batt_voltage()))
    print("BAT_power: " + str(pw.batt_power()))
    print("BAT_charge_current: " + str(pw.batt_charge_current()))
    print("BAT_discharge_current: " + str(pw.batt_discharge_current()))
    print("acin_voltage: " + str(pw.acin_voltage()))
    print("acin_current: " + str(pw.acin_current()))
    print("VUSB_voltage: " + str(pw.vbus_voltage()))
    print("VUSB_current: " + str(pw.vbus_current()))
    print("aps_voltage: " + str(pw.aps_voltage()))
    print("internal_TEMP: " + str(pw.internal_temp()))
    print()


def print_apx_reg(pw, regaddr):
    val = pw.read(regaddr)
    print("Addr: " + hex(regaddr) + " Data: " + hex(val))

def write_print_apx_reg(pw, regaddr, data):
    print("Write AXP regs")
    print("Addr: " + hex(regaddr) + " Data: " + hex(data))
    print_apx_reg(regaddr)
    pw.write(regaddr, data)
    print_apx_reg(regaddr)
    print()

def print_apx_regs(pw):
    print("Regs")
    print_apx_reg(pw, 0x00)
    print_apx_reg(pw, 0x01)
    print_apx_reg(pw, 0x02)
    print_apx_reg(pw, 0x03)
    print_apx_reg(pw, 0x04)
    print_apx_reg(pw, 0x05)
    print_apx_reg(pw, 0x06)
    print_apx_reg(pw, 0x07)
    print_apx_reg(pw, 0x08)
    print_apx_reg(pw, 0x09)
    print_apx_reg(pw, 0x0a)
    print_apx_reg(pw, 0x0b)
    print_apx_reg(pw, 0x0c)
    print_apx_reg(pw, 0x0d)
    print_apx_reg(pw, 0x0e)
    print_apx_reg(pw, 0x0f)
    print_apx_reg(pw, 0x10)
    print_apx_reg(pw, 0x11)
    print_apx_reg(pw, 0x12)
    print_apx_reg(pw, 0x13)
    print_apx_reg(pw, 0x14)
    print_apx_reg(pw, 0x15)

    print_apx_reg(pw, 0x20)
    print_apx_reg(pw, 0x21)
    print_apx_reg(pw, 0x22)
    print_apx_reg(pw, 0x23)
    print_apx_reg(pw, 0x24)
    print_apx_reg(pw, 0x25)
    print_apx_reg(pw, 0x26)
    print_apx_reg(pw, 0x27)
    print_apx_reg(pw, 0x28)
    print()

if __name__ == "__main__":
    print_apx_regs()

    #write_print_apx_reg(0x06, 0x32)
    #write_print_apx_reg(0x28, 0xac)  #0xac  2.8V 3.0V
    #write_print_apx_reg(0x12, 0xff)  #0x4d

    print_apx_regs()
    print_apx_values()

    print()


