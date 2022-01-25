import machine
from time import sleep
    
class LED():
    
    def __init__(self):
        from machine import Pin
        self.led = Pin(25, Pin.OUT)
        self.on()
        sleep(1)
        self.off()
        

    def on(self):
        self.led.value(0)
        
    def off(self):
        self.led.value(1)


    def blinkNoOf(self,noOf):
        x = range(noOf)
        for n in x:
            self.on()
            sleep(1)
            self.off()
            sleep(1)

    def blink(self):
        print("Led blink start")
        self.off()
        sleep(1)
        self.blinkNoOf(3)
        print("Led blink stoped")


if __name__ == "__main__":
    

    led = LED()
    
    led.on()
    led.blink()
    led.on()
    sleep(2)
    led.off()
    sleep(2)
    led.on()
    
    led.blinkNoOf(5)


