#from m5stack import lcd
from time import sleep
from machine import  Pin




#******************************************************************

print("Now seting up st7789")

from machine import Pin, SPI
from st7789 import ST7789
import colors
import vga1_bold_16x32 as font4

spi = SPI(-1,baudrate=4000000,sck=Pin(13), mosi=Pin(15), miso=Pin(4), phase=1, polarity=1)
print("display init")
display = ST7789(spi, 135, 240, reset=Pin(18, Pin.OUT), dc=Pin(23, Pin.OUT), cs=Pin(5, Pin.OUT))
display.init()

print("Display.line")
display.line(10, 40, 100, 120, colors.BLUE)
display.fill_rect(20, 50, 100, 100, colors.GREEN)
display.fill_rect(50, 100, 50, 50, colors.RED)
#display.text("Nu fungerar det", 0, 200, 0xff0000, colors.CYAN)
display.text(font4, "Hej C", 40, 200)

print("end of test")


