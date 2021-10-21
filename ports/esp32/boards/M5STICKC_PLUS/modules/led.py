import machine
from machine import Pin
from time import sleep

led = Pin (10, Pin.OUT)

def blink():
    #blink LED
    print("Led blink start")
    led.value(1)
    sleep(1)
    led.value(0)



    sleep(1)
    led.value(1)
    sleep(1)
    led.value(0)
    sleep(1)
    led.value(1)
    print("Led blink stoped")


if __name__ == "__main__":
    blink()


