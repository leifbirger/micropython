import machine
from machine import Pin
from time import sleep

from pcf8563 import PCF8563
class RTCPLUS(PCF8563):
    
    def __init__(self, i2):
        PCF8563.__init__(self, i2)

    def datetime(self, data=None):
        if data == None:
            return_data = PCF8563.datetime(self)
            return return_data
        else:
            data_l = list(data)
            list_lenght = len(data_l)
            if list_lenght == 8:
                del data_l[7]
            PCF8563.datetime(self, data_l)

    def print_rtc(self):
        print("RTCplus")
        
    def tuple8_to_7(self, data):
        data_l = list(data)
        del data_l[7]
        return data_l


if __name__ == "__main__":
    
    rtc_esp32 = machine.RTC()
    rtc_esp32_data = rtc_esp32.datetime()
    print(type(rtc_esp32_data))
    print(rtc_esp32_data)

    
    from machine import SoftI2C as I2C
    i2 = I2C(scl=Pin(22), sda=Pin(21), freq=400000)
    
    rtc_m5 = RTCPLUS(i2)
    rtc_m5.print_rtc()
    rtc_m5_data = rtc_m5.datetime()
    print(rtc_m5_data)
    rtc_esp32_data_l = rtc_m5.tuple8_to_7(rtc_esp32_data)
    print(rtc_esp32_data_l)

    rtc_m5.datetime(rtc_esp32_data_l)
    rtc_m5_data = rtc_m5.datetime()
    print(rtc_m5_data)
    rtc_m5.datetime(rtc_esp32_data)
    rtc_m5_data = rtc_m5.datetime()
    print(rtc_m5_data)   
    
    print()


