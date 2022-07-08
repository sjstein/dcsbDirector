import argparse
import serial
import socket
import struct
import time

def dump_bytes(bdata):
    rstr = ''
    for c in range(0, len(bdata)):
        rstr += f'{"%02x" % (bdata[c])}'
    return rstr

def dump_byte(bdata):
    return f'{"%02x" % (bdata)}'

def dump_chars(cdata):
    rstr = ''
    for c in range(0, len(cdata)):
        rstr += f'{cdata[c]}'
    return rstr

def dump_char(cdata):
    if (cdata < 0x20) | (cdata > 0x7e):
        # Replace non-printable ascii characters with midplane period char
        cdata = 0xb7
    return f'{chr(cdata)}'

def dump_memory(memory, rlow, rhi, blk_size):
    for addr in range(rlow, rhi, blk_size):
        print(f'{mem_str(addr)} ', end="")
        for i in range (0, blk_size):
            print(f'{dump_byte(memory[addr + i][0])}', end="")
        print(' | ', end="")
        for i in range (0, blk_size):
            print(f'{dump_char(memory[addr + i][0])}', end="")
        print('')

def mem_str(addr):
    return "%04x"%(addr)

def hex_dump(data, step):
    i = 0
    rstr = ''
    while i < len(data):
        for c in range(0, min(len(data) - i, step)):
            rstr += dump_byte(data[i + c])
        if min(len(data) - i, step) != step:
            for c in range(0, step - len(data) % step):
                rstr += '  '
        rstr += ' | '
        for c in range(0, min(len(data) - i, step)):
            rstr += dump_char(data[i + c])
        rstr += '\n'
        i += step
    return rstr

def char_pos(addr, addr_low, width):
    if addr < addr_low:
        return -1, -1
    offset = addr - addr_low
    r = offset // width
    c = offset % width
    return r, c

def write_pos(r, c, ch):
    arduino.write(b'C')
    arduino.write(bytes([int(r)]))
    arduino.write(bytes([int(c)]))
    arduino.write(bytes([int(ch)]))
    #arduino.write(ch.encode('utf-8'))
    arduino.write(EOMFLAG)


# Define contiguous memory range which we are interested in
MEM_RNG_LO = 0x11c0
MEM_RNG_HI = 0x12b0

# DCS-BIOS specific defaults
SYNC_SIZE = 4               # number of bytes in sync frame
MC_ADDR = '239.255.50.10'   # Multicast address
MC_PORT = 5010              # Multicast port

# Serial port defaults
SP_NAME = 'COM10'
EOMFLAG = b'\xFF'

DEBUG = True

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='dcsb Director')
    parser.add_argument('-i', '--ipaddr', help=f'Specify multicast address [{MC_ADDR}]')
    parser.add_argument('-p', '--port', help=f'Specify multicast port [{MC_PORT}]')
    parser.add_argument('-s', '--serial', help=f'Name of serial port [{SP_NAME}]')
    args = parser.parse_args()
    if args.ipaddr:
        MC_ADDR = args.ipaddr
    if args.port:
        MC_PORT = args.port
    if args.serial:
        SP_NAME = args.serial
    
    # Open COM port
    arduino = serial.Serial(port=SP_NAME, baudrate=115200, timeout=.1)

    # Configure and open multicast listener per DCS BIOS settings
    print(f'Listening for multicast on {MC_ADDR}')
    multicast_group = MC_ADDR
    server_address = ('', MC_PORT)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(server_address)
    group = socket.inet_aton(multicast_group)
    mreq = struct.pack('4sL', group, socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    # Define dicts to hold memory states
    new_mem = {} # Memory snapshot from DCS-BIOS stream
    old_mem = {} # Last updated memory state
    updated_mem_loc = [] # List of memory locations with new bytes

    # Clear memory space
    for i in range(MEM_RNG_LO,MEM_RNG_HI + 1):
        new_mem[i] = b'\x00'
        old_mem[i] = b'\x00'

    while True:
        data, address = sock.recvfrom(1024)
        if DEBUG:
            print(f'received {len(data)} bytes from {address}:')
            print(f'{hex_dump(data, 8)}')

        # Parse packet
        # First verify the DCS-BIOS sync-sequence (0x55,0x55,0x55,0x55)
        for i in range(0,SYNC_SIZE):
            if data[i] != 0x55:
                print(f'Error in DCS-BIOS sync sequence at byte position {i+1}')
                exit(-1)

        ci = SYNC_SIZE  # Start SYNC_SIZE bytes in (past sync sequence)
        while ( ci < len(data)):
            # Parse the memory address and data block size in little-endian byte order
            m_lo = data[ci]
            m_hi = data[ci+1]
            bsize_lo = data[ci+2]
            bsize_hi = data[ci+3]
            # Combine the two bytes into ints representing address and data size
            blk_size = (bsize_hi << 8) + bsize_lo
            mem_addr = (m_hi << 8) + m_lo
            if DEBUG:
                print(f'Data block found\nAddr: {"%04x" %(mem_addr)}\nSize: {"%04x" %(blk_size)}\nCont: ', end="")
                for di in range (0, blk_size):
                    print(f'{"%02x" % (data[di + ci + SYNC_SIZE])}', end="")
                print(f' [{dump_chars(data[ci+SYNC_SIZE : ci+SYNC_SIZE+blk_size])}]')

            # Populate memory map dict
            if MEM_RNG_LO <= mem_addr < MEM_RNG_HI:     # Is memory location within our block of interest?
                for addr in range(mem_addr, min(mem_addr + blk_size, MEM_RNG_HI)):
                    new_mem[addr] = chr(data[addr - mem_addr + ci + SYNC_SIZE]).encode()
                    if old_mem[addr] != new_mem[addr]:
                        # We have an updated byte
                        updated_mem_loc.append(addr)
                if DEBUG:
                    dump_memory(new_mem, MEM_RNG_LO, MEM_RNG_HI, 24)

            for addr in updated_mem_loc:
                if DEBUG:
                    print(f'Found updated byte at {mem_str(addr)}, old val = {dump_byte(old_mem[addr][0])}'
                          f'[{dump_char(old_mem[addr][0])}] | '
                          f'new val = {dump_byte(new_mem[addr][0])}'
                          f'[{dump_char(old_mem[addr][0])}]')
                # Send update message to arduino
                l,c = char_pos(addr, MEM_RNG_LO, 24)
                d = new_mem[addr][0]
                #print(f'L{l}C{"%02x"%(c)}D{dump_byte(old_mem[addr][0])}')
                write_pos(l,c,d)
                # Wait for arduino to ack
                time.sleep(0.02)
                updated_mem_loc.remove(addr)
                old_mem[addr] = new_mem[addr]


            ci += blk_size + SYNC_SIZE
    print(f'completed test run')
    exit()
