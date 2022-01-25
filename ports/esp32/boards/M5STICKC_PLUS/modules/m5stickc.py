from time import sleep
print("Now seting up APX192")
from axp192plus import AXP192PLUS
from led import LED
from st7789plus import ST7789PLUS
from button import BUTTON
from rtcplus import RTCPLUS
from sound import SOUND
from wifi import WIFI
import machine

class M5STICKCP(AXP192PLUS, LED, ST7789PLUS, BUTTON, RTCPLUS, SOUND):
    
    def __init__(self):    

        from machine import Pin
        from machine import SoftI2C as I2C
        i2 = I2C(scl=Pin(22), sda=Pin(21), freq=400000)
        AXP192PLUS.__init__(self, i2)
        LED.__init__(self)
        ST7789PLUS.__init__(self)
        BUTTON.__init__(self)
        RTCPLUS.__init__(self, i2)

        self.wlan = WIFI('','')
        SOUND.__init__(self)
        
        
    def cpu_reset(self):
        print("m5stickcp reset")
        sleep(1)
        machine.reset()
        
    def class_test(self):
        print("m5stickcp class test")


def module_test():
    print("m5stickcp module test")
    
    
#******************************************************************
if __name__ == "__main__":
       
    from time import sleep
    
    board = M5STICKCP()


    board.print_axp_values()
        
    board.blink()
    
    board.sound(300, 3)
    
    board.testpicture()
    board.testtext()
    sleep(2)
    board.clear()
    
    value=board.getButton()

    board.print_rtc()
    
    board.wlan.show()


