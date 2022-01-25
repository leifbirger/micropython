# Pi DISPLAY PACK

import colors
import vga1_8x8 as font1
import vga1_8x16 as font2
import vga1_bold_16x16 as font3
import vga1_bold_16x32 as font4

ROWS = 135
COLUMNS = 240

from st7789py_rh import ST7789
class ST7789PLUS(ST7789):
    
    def __init__(self):
        from machine import Pin
        from machine import SoftSPI as SPI
        print("display init")
        #resetp=Pin(18, Pin.OUT)
        dcp=Pin(16, Pin.OUT)
        csp=Pin(17, Pin.OUT)
        spi = SPI(baudrate=4000000,sck=Pin(18), mosi=Pin(19), miso=Pin(21),phase=1, polarity=1)   #Miso not used
        #super().__init__(spi, ROWS, COLUMNS, reset=Pin(18, Pin.OUT), dc=Pin(23, Pin.OUT), cs=Pin(5, Pin.OUT), rotation=3)
        ST7789.__init__(self, spi, ROWS, COLUMNS, reset=None, dc=Pin(16, Pin.OUT), cs=Pin(17, Pin.OUT), rotation=3)
        #self.init()
        #self.testpicture()
        #self.testtext()
        print("end of test")

    def testpicture(self):
         print("Print display.line")
         self.line(10, 40, 100, 120, colors.BLUE)
         self.fill_rect(20, 50, 100, 100, colors.GREEN)
         self.fill_rect(50, 100, 50, 50, colors.RED)
         
    def testtext(self):
        print("Print display.text")
        self.text(font4, "M5StickC 2", 00, 000)
        self.text(font4, "plus", 30, 50)
        self.text(font4, "started", 05, 100)
        
    def writetext(self, msg, x, y):
        self.text(font4, msg, x, y)

    def clear(self):
        #print("Print display.clear")
        #self.fill_rect(0, 0, COLUMNS-1, ROWS-1, colors.BLACK)
        self.fill(colors.BLACK)

if __name__ == "__main__":
       
    from time import sleep
    
    display = ST7789PLUS()
    display.testpicture()
    display.testtext()
    sleep(2)
    display.clear()
    sleep(2)
    display.writetext("kalle", 3, 50)
    