import datetime
import random
import serial
import time
arduino = serial.Serial(port='COM11', baudrate=115200, timeout=.1)


def write(x):
    #arduino.write(bytes(x, 'utf-8'))
    arduino.write(b'\x02')
    arduino.write(b'\x0C')
    arduino.write(x.encode('utf-8'))
    arduino.write(b'\xFF')


def write_char(r, c, ch):
    dts = [b'C', bytes([int(r)]), bytes([int(c)]), ch.encode('utf-8'), b'\xFF']
    arduino.write(b''.join(dts))
    #time.sleep(0.04)
    while arduino.read() != b'\x55':
        pass

def write_invalid(ch):
    dts = [ch.encode('utf-8'), b'\xFF']
    arduino.write(b''.join(dts))
    #time.sleep(0.04)
    while arduino.read() != b'\x55':
        pass

def clear_screen():
    arduino.write(b'X')
    # time.sleep(0.04)
    while arduino.read() != b'\x55':
        pass

def write_line(line, data):
    dts = [b'L', bytes([int(line)])]    # Tell arduino we are sending it a line for update and specify number of lines
    for i in range(0, len(data)):       # Pack the rest of the message with line data
        dts.append(data[i].encode('utf-8'))
    dts.append(b'\xFF')
    msg = b''.join(dts)
    arduino.write(b''.join(dts))
    while arduino.read() != b'\x55':
        pass


random.seed()

while True:
    choice = input("Enter (l)ine, (c)haracter, (t)iming, (i)nvalid, or clea(r)? :")
    if choice == 'c':
        r = input("Enter line # :")
        c = input("Enter char # :")
        ch = input("Enter a character: ")
        now = datetime.datetime.now()
        write_char(r, c, ch)
        # wait for ack from arduino
        # ret = ''
        # while ret != b'\x55':
        #     ret = arduino.read()
        #    time.sleep(0.05)
        end = datetime.datetime.now()
        print(f'Arduino ack took {end - now} seconds')

    elif choice == 'l':
        ln = input("Enter line # :")
        msg = input("Enter line contents :")
        write_line(ln, msg)

    elif choice == 'r':
        clear_screen()

    elif choice == 'i':
        ch = input('Enter (invalid) char to send: ')
        write_invalid(ch)

    elif choice == 't':
        msg = ""
        ch = []
        now = datetime.datetime.now()
        # for i in range(0, 100):
        #     x = random.randint(0, 9)
        #     y = random.randint(0, 23)
        #     c = random.randint(33, 125)
        #     write_char(x, y, chr(c))
        iter = 0
        for k in range(1, 100):
            for j in range(0, 10):
                for i in range(0, 25):
                    ch.append(chr(random.randint(33, 125)))
                write_line(str(j), msg.join(ch))
                ch.clear()
            end = datetime.datetime.now()
            print(f'avg loop time: {(end - now)/k} seconds')

    else:
        pass
