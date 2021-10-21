#from m5stack import lcd
from time import sleep
from machine import SoftI2C,  Pin

sleep(1)

#**************************************************************
print("Now seting up APX192")
sleep(1)

i2 = SoftI2C(scl=Pin(22), sda=Pin(21), freq=400000)

import axp192
board = axp192.M5StickCPlus()
power_supply = axp192.AXP192(i2, board=board)

import axp192_read
axp192_read.print_apx_values(power_supply)

sleep(2)

#******************************************************************

print("Now seting up st7789")

from machine import Pin, SPI
from st7789 import ST7789
import colors
import vga1_8x8 as font1
import vga1_8x16 as font2
import vga1_bold_16x16 as font3
import vga1_bold_16x32 as font4

resetp=Pin(18, Pin.OUT)
dcp=Pin(23, Pin.OUT)
csp=Pin(5, Pin.OUT)

spi = SPI(-1,baudrate=4000000,sck=Pin(13), mosi=Pin(15), miso=Pin(4), phase=1, polarity=1)
print("display init")
display = ST7789(spi, 135, 240, reset=Pin(18, Pin.OUT), dc=Pin(23, Pin.OUT), cs=Pin(5, Pin.OUT))
#display = ST7789(spi, 135, 240, reset=Pin(18, Pin.OUT), dc=Pin(23, Pin.OUT), cs=Pin(5, Pin.OUT), rotation=3)
display.init()

sleep(1)

import blinkled
blinkled.blink_led()

print("Print display.line")
display.line(10, 40, 100, 120, colors.BLUE)
display.fill_rect(20, 50, 100, 100, colors.GREEN)
display.fill_rect(50, 100, 50, 50, colors.RED)
#display.text("Nu fungerar det", 0, 200, 0xff0000, colors.CYAN)
sleep(1)
display.init()
display.text(font4, "M5StickC", 00, 000)
display.text(font4, "plus", 30, 50)
display.text(font4, "started", 05, 100)

print("end of test")


