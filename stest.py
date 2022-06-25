# Importing Libraries
import serial
import time
arduino = serial.Serial(port='COM4', baudrate=115200, timeout=.1)


def write(x):
    #arduino.write(bytes(x, 'utf-8'))
    arduino.write(b'\x02')
    arduino.write(b'\x0C')
    arduino.write(x.encode('utf-8'))
    arduino.write(b'\xFF')


def write_pos(r, c, ch):
    dts = [b'C', bytes([int(r)]), bytes([int(c)]), ch.encode('utf-8'), b'\xFF']
    arduino.write(b''.join(dts))  # EOM


def write_line(line, data):
    dts = [b'L', bytes([int(line)])]    # Tell arduino we are sending it a line for update and specify number of lines
    for i in range(0, len(data)):       # Pack the rest of the message with line data
        dts.append(data[i].encode('utf-8'))
    dts.append(b'\xFF')
    msg = b''.join(dts)
    arduino.write(b''.join(dts))


while True:
    choice = input("Enter (l)ine, (c)haracter, (t)iming, or clea(r)? :")
    if choice == 'c':
        r = input("Enter row # :")
        c = input("Enter col # :")
        ch = input("Enter a character: ")
        write_pos(r, c, ch)
    elif choice == 'l':
        ln = input("Enter line # :")
        msg = input("Enter line contents :")
        write_line(ln, msg)
    elif choice == 'r':
        for r in range(0, 10):
            write_line(r, "                         ")
            time.sleep(0.5)
    else:
        t = input("Enter timing value (ms) :")
        for r in range(0, 10):
            for c in range(0, 25):
                write_pos(r, c, chr(33+r+c))
                time.sleep(int(t)/1000)
