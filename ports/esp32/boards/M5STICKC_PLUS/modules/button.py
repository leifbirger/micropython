import machine
from machine import Pin
from time import sleep_ms
from micropython import const


    
Off = const(0)
Hold = const(1)
Released = const(2)
A = const(1)
A_hold = const(A+Hold)
A_released = const(A+Released)
B = const(4)
B_hold = const(B+Hold)
B_released = const(B+Released)

DebounceTime = const(20)
HoldTime = const(500)
BtnActiveLevel = const(0)


holdState = False
releasedState = False
pinPressed = None

class BUTTON():
    
    def __init__(self):
        from machine import Pin
        self.pina = Pin(37, Pin.IN)
        self.pinb = Pin(39, Pin.IN)
        #btnc = Pin(36, Pin.IN)
        self.pinList = [self.pinb, self.pina]

    def btnMap(self, pPressed, hState, rState):
        if pPressed == None: return Off
        if pPressed == self.pina:
            if hState:
                if rState: return A_released
                else: return A_hold
            else: return A    
        elif pPressed == self.pinb:
            if hState:
                if rState: return B_released
                else: return B_hold
            else: return B
        else: return Off


    def getButton(self):
        global pinPressed
        global pressedState
        global holdState
        global releasedState
        if pinPressed == None:
            for pin in self.pinList:
                if pin.value() == BtnActiveLevel:
                    sleep_ms(DebounceTime)
                if pin.value() == BtnActiveLevel:
                    pinPressed = pin
                    return self.btnMap(pinPressed, holdState, releasedState)
            return Off
        elif not holdState: 
            if pinPressed.value() == BtnActiveLevel:
                sleep_ms(HoldTime)
            else:
                pinPressed = None
                holdState = False
                releasedState = False
                return Off #btnMap(pinPressed, holdState, releasedState)
            if pinPressed.value() == BtnActiveLevel:
                holdState = True
                return self.btnMap(pinPressed, holdState, releasedState)
            else:
                pinPressed = None
                holdState = False
                releasedState = False
                return Off #btnMap(pinPressed, holdState, releasedState)
        else:  #hold state
            if pinPressed.value() != BtnActiveLevel:
                releasedState = True
                returnValue = self.btnMap(pinPressed, holdState, releasedState)
                pinPressed = None
                holdState = False
                releasedState = False
                return returnValue
            sleep_ms(HoldTime)
            if pinPressed.value() == BtnActiveLevel:
                return self.btnMap(pinPressed, holdState, releasedState)
            else:
                releasedState = True
                returnValue = self.btnMap(pinPressed, holdState, releasedState)
                pinPressed = None
                holdState = False
                releasedState = False
                return returnValue
                


if __name__ == "__main__":
    print("Button tester started")
    button = BUTTON()
    while True:
         value = button.getButton()
         if value:
             print("Button " + str(value))
        


