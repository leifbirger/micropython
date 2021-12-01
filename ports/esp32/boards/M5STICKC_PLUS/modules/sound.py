from machine import Pin, PWM
from time import sleep, sleep_ms

piNo = 2

class SOUND():
    
    def __init__(self):
        pin=Pin(piNo)
        self.buzzer=PWM(pin)
        self.buzzer.duty(0)
        self.pr = False
        
    def __del__(self):
        self.buzzer.duty(0)
        self.buzzer.deinit()       
    
    def debug(self, deb=True):
        self.pr=deb
     

    def sound(self, freq, duration=1, duty=512):
        if self.pr:
            print("sound start:" + str(freq) + " :" + str(duration) + " :" +
              str(duty))

        self.buzzer.duty(duty)
        self.buzzer.freq(freq)
        sleep_ms(int(duration*1000))
        self.buzzer.duty(0)
        #self.buzzer.deinit()
        
    def sound_off(self):
        self.buzzer.freq(0)
        self.buzzer.duty(0)
  
if __name__ == "__main__":
    s = SOUND()
    print("start")
    s.sound(1000)
    s.sound_off()
    sleep(2)
    s.sound(200)
    sleep(2)
    s.sound(3000)
    sleep(2)
    print("end")
    
    s.debug()
    for i in range(0, 1023, int(1024/50)):
      s.sound(800, 3, i)   
    
    for i in range(0, 20000, int(20000/100)):
      s.sound(i, 0.5)      
    