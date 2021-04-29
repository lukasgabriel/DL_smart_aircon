# dl_aircon.py

import json

msg = json.loads("""{"power": true, "temp": 16, "mode": 8, "fan":1, "timer":false, "timer_val":0, "unit_f":false}""")

def reverse_bits(n):
    result = 0
    for i in range(8):
        result  <<= 1
        result |= n & 1
        n >>= 1
    return result


def assemble_message(power: bool, temp: int, mode: int, fan:int, timer: bool, timer_val: int, unit_f: bool):
    buf = 0x12000000

    if unit_f:
        assert 61 <= temp <= 89
        buf |= reverse_bits(temp)
    else:
        assert 16 <= temp <= 32
        temp -= 16 
        buf |= reverse_bits(temp)

    buf |= reverse_bits(timer_val) << 8

    if power:
        buf |= 0x1 << 8
    if timer: 
        buf |= 0x1 << 9
    if unit_f: 
        buf |= 0x1 << 10

    if (mode == 8 or mode == 2 or mode == 1):
        buf |= mode << 16
    else:
        buf |= 0x8 << 16

    if fan in [4, 2, 1]:
        buf |= fan << 20
    else:
        buf |= 0x2 << 20

    return(buf)

print(hex(assemble_message(**msg)))
