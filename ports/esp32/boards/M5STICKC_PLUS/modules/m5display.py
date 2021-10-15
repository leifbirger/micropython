
from machine import Pin, SPI
from st7789 import ST7789
import colors
import vga1_bold_16x32 as font

class M5DISPLAY(ST7789):
    
  def __init__(self):

    self.spi = SPI(-1,baudrate=4000000,sck=Pin(13), mosi=Pin(15), miso=Pin(4), phase=1, polarity=1)
    print("display init")
    super().__init__(self.spi, 135, 240, reset=Pin(18, Pin.OUT), dc=Pin(23, Pin.OUT), cs=Pin(5, Pin.OUT))
    
    #super().init()


if __name__ == "__main__":
  if True:
   print("m5stickc")
   display = M5DISPLAY()
   if False: 
    display.line(10, 40, 100, 120, colors.GREEN)
    display.fill_rect(20, 50, 100, 100, colors.RED)
    display.text(font, "m5display C", 10, 200)    
