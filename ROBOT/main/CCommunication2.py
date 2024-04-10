import serial, time
from robot_orders import *
from strategies import *
from threading import Timer, Thread, Event

BAUDRATE = 9600

# Notifications: à changer
ACK: bytes = bytes([1])
NAK: bytes = bytes([2])

class communication:
    def __init__(self, port):
        self.port = port
        self.s = None
        self.in_coming = False
        self.timer = None
        self.r = None
        self.event_stop = Event()
        self.event_in_coming = Event()
        self.event_end_of_strategy = Event()

    def connexion(self):
        print("Connexion")
        self.s = serial.Serial(port=self.port, baudrate=BAUDRATE)
        print("sleeping...", end="", flush=True)
        self.done()


    def deconnexion(self):
        print("Déconnexion")
        #self.timer.cancel()
        self.s.close()
        print("done close!", flush=True)
        self.event_stop.clear()
        self.event_in_coming.clear()
        self.event_end_of_strategy.clear()

    def stop_request(self):
        self.s.write(stop())
        self.event_stop.set()
        self.done()

    def restart(self):
        self.thread_com = Thread(target=self.th_restart)
        self.thread_com.start()

    def th_restart(self):
        self.s.write(restart())
        self.done()

    def rotate(self, angle):
        self.thread_com = Thread(target=self.th_rotate, args=(angle,))
        self.thread_com.start()

    def th_rotate(self, angle):
        self.s.write(rotate(angle))
        self.done()

    def forward(self, axe, dist):
        self.thread_com = Thread(target=self.th_forward, args=(axe, dist))
        self.thread_com.start()

    def th_forward(self, axe, dist):
        self.s.write(move("F", axe, dist))
        self.done()

    def backward(self, axe, dist):
        self.thread_com = Thread(target=self.th_backward, args=(axe, dist))
        self.thread_com.start()

    def th_backward(self, axe, dist):
        self.s.write(move("B", axe, dist))
        self.done()

    def up(self, axe, dist):
        self.thread_com = Thread(target=self.th_up, args=(axe, dist))
        self.thread_com.start()

    def th_up(self, axe, dist):
        self.s.write(gripper("U", axe, dist))
        self.done()

    def th_down(self, axe, dist):
        self.s.write(gripper("D", axe, dist))
        self.done()

    def down(self, axe, dist):
        self.thread_com = Thread(target=self.th_down, args=(axe, dist))
        self.thread_com.start()

    def th_open(self, grip):
        self.s.write(claw("O", grip))
        self.done()

    def open(self, grip):
        self.thread_com = Thread(target=self.th_open, args=(grip,))
        self.thread_com.start()

    def th_close(self, grip):
        self.s.write(claw("C", grip))
        self.done()

    def close(self, grip):
        self.thread_com = Thread(target=self.th_close, args=(grip,))
        self.thread_com.start()

    def do_strategy(self, strategy):
        for step in strategy:
            if self.event_end_of_strategy.is_set():
                break
            if step[0]=="P":
                print("Point", step)
                self.r = str(step)
            else:
                self.r = str(step)
                self.s.write(step)
                print(step, sep='')
                self.done()
            time.sleep(0.1)

        print("End of strategy !")
        self.r = "...done\nEnd of strategy !\n"

    def thread_strategy(self, strategy):
        self.thread_stra = Thread(target=self.do_strategy, args=(strategy,))
        self.event_end_of_strategy.clear()
        self.thread_stra.start()

    def wait_for_ack(self):
        c = 0
        while c == 0:
            c = self.s.in_waiting
            if self.event_stop.is_set():
                self.event_stop.clear()
                self.event_end_of_strategy.set()
                break
        if c > 0:
            message = self.s.read_until(b"\n")
            print("message:",message)
        self.event_in_coming.set()


    def task_wait_for_ack(self):
        #print("Wait for reception")
        self.thread_wait_for_ack = Thread(target = self.wait_for_ack)
        self.thread_wait_for_ack.start()

    def done(self):
        self.task_wait_for_ack()
        self.event_in_coming.wait()
        self.event_in_coming.clear()
        print('...done\n')
        self.r = "...done\n"



def main():

    com = communication("COM11")
    #done(com)
    com.connexion()
    time.sleep(3)
    print("Stratégie 1")
    com.thread_strategy(STRATEGY1)



    # while True:
    #     com.forward("X", 100)
    #     time.sleep(5)
    #     com.forward("Y", 100)
    #     time.sleep(5)
    #     com.forward("Z", 1000)
    #     time.sleep(1)
    #     com.stop_request()
    #     time.sleep(1)
    STOP = input("Stop : ")
    if STOP == "y":
        com.stop_request()
    time.sleep(3)

    com.deconnexion()
    #t.cancel()

if __name__ == "__main__":
    main()

