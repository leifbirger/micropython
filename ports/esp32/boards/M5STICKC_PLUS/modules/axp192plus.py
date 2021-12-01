import machine
from machine import Pin
from time import sleep

from axp192 import AXP192
class AXP192PLUS(AXP192):
    
    def __init__(self, i2):
        #from machine import SoftI2C as I2C
        #i2 = I2C(scl=Pin(22), sda=Pin(21), freq=400000)
        import axp192
        board = axp192.M5StickCPlus()
        #super().__init__(i2, board = board)
        AXP192.__init__(self, i2, board = board)

    def power_sleep(self):
        self.write(0x12, 0x01)

    def print_axp_values(self):
        print("Power status")
        print("BAT_voltage: " + str(self.batt_voltage()))
        print("BAT_power: " + str(self.batt_power()))
        print("BAT_charge_current: " + str(self.batt_charge_current()))
        print("BAT_discharge_current: " + str(self.batt_discharge_current()))
        print("acin_voltage: " + str(self.acin_voltage()))
        print("acin_current: " + str(self.acin_current()))
        print("VUSB_voltage: " + str(self.vbus_voltage()))
        print("VUSB_current: " + str(self.vbus_current()))
        print("aps_voltage: " + str(self.aps_voltage()))
        print("internal_TEMP: " + str(self.internal_temp()))
        print()

    def print_axp_reg(self, regaddr):
        val = self.read(regaddr)
        print("Addr: " + hex(regaddr) + " Data: " + hex(val))

    def write_print_axp_reg(self, regaddr, data):
        print("Write AXP regs")
        print("Addr: " + hex(regaddr) + " Data: " + hex(data))
        self.print_axp_reg(regaddr)
        self.write(regaddr, data)
        self.print_axp_reg(regaddr)
        print()

    def print_axp_regs(self):
        print("Regs")
        self.print_axp_reg(0x00)
        self.print_axp_reg(0x01)
        self.print_axp_reg(0x02)
        self.print_axp_reg(0x03)
        self.print_axp_reg(0x04)
        self.print_axp_reg(0x05)
        self.print_axp_reg(0x06)
        self.print_axp_reg(0x07)
        self.print_axp_reg(0x08)
        self.print_axp_reg(0x09)
        self.print_axp_reg(0x0a)
        self.print_axp_reg(0x0b)
        self.print_axp_reg(0x0c)
        self.print_axp_reg(0x0d)
        self.print_axp_reg(0x0e)
        self.print_axp_reg(0x0f)
        self.print_axp_reg(0x10)
        self.print_axp_reg(0x11)
        self.print_axp_reg(0x12)
        self.print_axp_reg(0x13)
        self.print_axp_reg(0x14)
        self.print_axp_reg(0x15)

        self.print_axp_reg(0x20)
        self.print_axp_reg(0x21)
        self.print_axp_reg(0x22)
        self.print_axp_reg(0x23)
        self.print_axp_reg(0x24)
        self.print_axp_reg(0x25)
        self.print_axp_reg(0x26)
        self.print_axp_reg(0x27)
        self.print_axp_reg(0x28)
        print()

if __name__ == "__main__":
       
    power_supply = AXP192PLUS()
    power_supply.print_axp_regs()

    #write_print_apx_reg(0x06, 0x32)
    #write_print_apx_reg(0x28, 0xac)  #0xac  2.8V 3.0V
    #write_print_apx_reg(0x12, 0xff)  #0x4d

    power_supply.print_axp_regs()
    power_supply.print_axp_values()

    print()


