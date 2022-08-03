import argparse
import serial
import socket
import struct
import time


def dump_bytes(bdata):
    rstr = ''
    for cnt in range(0, len(bdata)):
        rstr += f'{"%02x" % (bdata[cnt])}'
    return rstr


def dump_byte(bdata):
    return f'{"%02x" % bdata}'


def dump_chars(cdata):
    rstr = ''
    for cnt in range(0, len(cdata)):
        rstr += f'{cdata[cnt]}'
    return rstr


def dump_char(cdata):
    if (cdata < 0x20) | (cdata > 0x7e):
        # Replace non-printable ascii characters with mid-plane period char
        cdata = 0xb7
    return f'{chr(cdata)}'


def dump_line(memory, lnum):
    rstr = ''
    st_addr = addr_from_line(lnum)
    for cnt in range(0, LINE_LEN):
        rstr += f'{memory[st_addr + cnt][0]}'
    return rstr


def dump_memory(memory, rlow, rhi, blk_sz):
    for adr in range(rlow, rhi, blk_sz):
        print(f'{mem_str(adr)} ', end="")
        for cnt in range(0, blk_sz):
            print(f'{dump_byte(memory[adr + cnt][0])}', end="")
        print(' | ', end="")
        for cnt in range(0, blk_sz):
            print(f'{dump_char(memory[adr + cnt][0])}', end="")
        print('')


def mem_str(adr):
    return "%04x" % adr


def hex_dump(dat, step):
    cnt = 0
    rstr = ''
    while cnt < len(dat):
        for c1 in range(0, min(len(dat) - cnt, step)):
            rstr += dump_byte(dat[cnt + c1])
        if min(len(dat) - cnt, step) != step:
            for c1 in range(0, step - len(dat) % step):
                rstr += '  '
        rstr += ' | '
        for c1 in range(0, min(len(dat) - cnt, step)):
            rstr += dump_char(dat[cnt + c1])
        rstr += '\n'
        cnt += step
    return rstr


def char_pos(adr, adr_low, width):
    if adr < adr_low:
        return -1, -1
    offset = adr - adr_low
    r = offset // width
    d = offset % width
    return r, d


def write_char(x, y, ch):
    dts = [CHRFLAG, bytes([int(x)]), bytes([int(y)]), bytes([int(ch)]), EOMFLAG]
    arduino.write(b''.join(dts))


def write_line(memory, lnum):
    dts = [LINFLAG, bytes([int(lnum)])]
    base_adr = addr_from_line(lnum)
    for cnt in range(0, LINE_LEN):
        dts.append(memory[base_adr + cnt])
    dts.append(EOMFLAG)
    arduino.write(b''.join(dts))


def line_from_addr(adr):
    return (adr - MEM_RNG_LO) // LINE_LEN


def addr_from_line(lnum):
    return MEM_RNG_LO + lnum*LINE_LEN


# Define contiguous memory range which we are interested in
MEM_RNG_LO = 0x11c0
MEM_RNG_HI = 0x12b0
LINE_LEN = 24   # Number of characters per line on CDU display

# DCS-BIOS specific defaults
SYNC_SIZE = 4               # number of bytes in sync frame
MC_ADDR = '239.255.50.10'   # Multicast address
MC_PORT = 5010              # Multicast port
EOMBYTE = 0x55              # Byte sequence indicating end of multicast header

# Serial port defaults
SP_NAME = 'COM10'

# Serial message flags
EOMFLAG = b'\xFF'
LINFLAG = b'L'
CHRFLAG = b'C'
ACKFLAG = b'\x55'

# Default debug verbosity
DEBUG = 0

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='dcsb Director')
    parser.add_argument('-i', '--ipaddr', help=f'Specify multicast address [{MC_ADDR}]')
    parser.add_argument('-p', '--port', help=f'Specify multicast port [{MC_PORT}]')
    parser.add_argument('-s', '--serial', help=f'Name of serial port [{SP_NAME}]')
    parser.add_argument('-d', '--debug', help=f'Debug level (0-3) [{DEBUG}]')
    args = parser.parse_args()
    if args.ipaddr:
        MC_ADDR = args.ipaddr
    if args.port:
        MC_PORT = args.port
    if args.serial:
        SP_NAME = args.serial
    if args.debug:
        DEBUG = args.debug

    print(f'DCS-BIOS Director running')
    print(f' Listening on : {MC_ADDR}  {MC_PORT}')
    print(f' Writing to   : {SP_NAME}')

    # Open multicast port
    multicast_group = MC_ADDR
    server_address = ('', MC_PORT)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(server_address)
    group = socket.inet_aton(multicast_group)
    mreq = struct.pack('4sL', group, socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    # Open COM port
    try:
        arduino = serial.Serial(port=SP_NAME, baudrate=115200, timeout=.1)
    except serial.serialutil.SerialException:
        print(f'dcsbDirector: Unable to open serial port "{SP_NAME}". Exiting.')
        exit(-1)

    # Define members to hold memory states
    new_mem = {}            # Memory snapshot from DCS-BIOS stream
    old_mem = {}            # Last updated memory state
    updated_mem_loc = []    # List of memory locations with new bytes
    updated_lines = []      # List of lines which have updated char(s)

    # Clear memory space
    for i in range(MEM_RNG_LO, MEM_RNG_HI + 1):
        new_mem[i] = b'\x00'
        old_mem[i] = b'\x00'

    while True:
        data, address = sock.recvfrom(1024)
        if DEBUG > 2:
            print(f'received {len(data)} bytes from {address}:')
            print(f'{hex_dump(data, 8)}')

        # Parse packet
        # First verify the DCS-BIOS sync-sequence (0x55,0x55,0x55,0x55)
        for i in range(0, SYNC_SIZE):
            if data[i] != EOMBYTE:
                print(f'Error in DCS-BIOS sync sequence at byte position {i+1}')
                exit(-1)
                # Maybe we should do something a bit more fault-tolerant here?
                # For example, just clear out the current stream, reset values and begin again?

        ci = SYNC_SIZE  # Start SYNC_SIZE bytes in (past sync sequence)
        while ci < len(data):
            # Parse the memory address and data block size in little-endian byte order
            m_lo = data[ci]
            m_hi = data[ci+1]
            bsize_lo = data[ci+2]
            bsize_hi = data[ci+3]
            # Combine the two bytes into ints representing address and data size
            blk_size = (bsize_hi << 8) + bsize_lo
            mem_addr = (m_hi << 8) + m_lo
            if DEBUG > 2:
                print(f'Data block found\nAddr: {"%04x" % mem_addr}\nSize: {"%04x" % blk_size}\nCont: ', end="")
                for di in range(0, blk_size):
                    print(f'{"%02x" % (data[di + ci + SYNC_SIZE])}', end="")
                print(f' [{dump_chars(data[ci+SYNC_SIZE : ci+SYNC_SIZE+blk_size])}]')

            # Populate memory map dict
            if MEM_RNG_LO <= mem_addr < MEM_RNG_HI:     # Is memory location within our block of interest?
                for addr in range(mem_addr, min(mem_addr + blk_size, MEM_RNG_HI)):
                    new_mem[addr] = chr(data[addr - mem_addr + ci + SYNC_SIZE]).encode('raw_unicode_escape')
                    if old_mem[addr] != new_mem[addr]:
                        # We have an updated byte
                        updated_mem_loc.append(addr)
                        if not line_from_addr(addr) in updated_lines:
                            updated_lines.append(line_from_addr(addr))  # Keep track of updated lines
                if DEBUG > 0:
                    dump_memory(new_mem, MEM_RNG_LO, MEM_RNG_HI, LINE_LEN)
                    print('---- --')

            for line in updated_lines:
                if DEBUG > 1:
                    print(f'Found updated line: {line}, old val = {dump_line(old_mem, line)}, '
                          f'new val = {dump_line(new_mem, line)}')

                # Determine if more than one character needs to be updated. If not, just update a single char
                changed = []    # A list of changed memory addresses in this particular line
                for addr in range(addr_from_line(line), addr_from_line(line)+24):
                    if old_mem[addr] != new_mem[addr]:
                        changed.append(addr)
                if len(changed) > 1:                    # More than one character has changed, so write an entire line.
                    base_addr = addr_from_line(line)    # Copy new line memory locations into old.
                    for index in range(0, LINE_LEN):
                        old_mem[base_addr+index] = new_mem[base_addr+index]
                    write_line(old_mem, line)           # Send update message to arduino.
                else:                                   # Only one character in the line has changed,
                    addr = changed[0]                   # so just update that single char.
                    old_mem[addr] = new_mem[addr]
                    l, c = char_pos(addr, MEM_RNG_LO, 24)
                    write_char(l, c, old_mem[addr][0])
                updated_lines.remove(line)

                ts = time.perf_counter()
                # noinspection PyUnboundLocalVariable
                while arduino.read() != ACKFLAG:
                    if time.process_time() - ts > 2:
                        print(f'dcsbDirector: Timeout waiting for ACK from serial port')
                    pass
            ci += blk_size + SYNC_SIZE
