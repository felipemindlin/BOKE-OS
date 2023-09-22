 #ifndef SCANCODES_H
 #define SCANCODES_H

 
 static char ScanCodes[256] = {
    0,   // 0x00 - Null
    0,   // 0x01 - Escape
    '1', // 0x02 - '1'
    '2', // 0x03 - '2'
    '3', // 0x04 - '3'
    '4', // 0x05 - '4'
    '5', // 0x06 - '5'
    '6', // 0x07 - '6'
    '7', // 0x08 - '7'
    '8', // 0x09 - '8'
    '9', // 0x0A - '9'
    '0', // 0x0B - '0'
    '-', // 0x0C - '-'
    '=', // 0x0D - '='
    '\b', // 0x0E - Backspace
    '\t', // 0x0F - Tab
    'Q', // 0x10 - 'Q'
    'W', // 0x11 - 'W'
    'E', // 0x12 - 'E'
    'R', // 0x13 - 'R'
    'T', // 0x14 - 'T'
    'Y', // 0x15 - 'Y'
    'U', // 0x16 - 'U'
    'I', // 0x17 - 'I'
    'O', // 0x18 - 'O'
    'P', // 0x19 - 'P'
    '[', // 0x1A - '['
    ']', // 0x1B - ']'
    '\n', // 0x1C - Enter
    0,   // 0x1D - Control (Left Ctrl)
    'A', // 0x1E - 'A'
    'S', // 0x1F - 'S'
    'D', // 0x20 - 'D'
    'F', // 0x21 - 'F'
    'G', // 0x22 - 'G'
    'H', // 0x23 - 'H'
    'J', // 0x24 - 'J'
    'K', // 0x25 - 'K'
    'L', // 0x26 - 'L'
    ';', // 0x27 - ';'
    '\'', // 0x28 - '''
    '`', // 0x29 - '`'
    0,   // 0x2A - Shift (Left Shift)
    '\\', // 0x2B - '\'
    'Z', // 0x2C - 'Z'
    'X', // 0x2D - 'X'
    'C', // 0x2E - 'C'
    'V', // 0x2F - 'V'
    'B', // 0x30 - 'B'
    'N', // 0x31 - 'N'
    'M', // 0x32 - 'M'
    0,   // 0x33 - ',' (comma)
    0,   // 0x34 - '.' (period)
    0,   // 0x35 - '/' (forward slash)
    0,   // 0x36 - Shift (Right Shift)
    0,   // 0x37 - (Print Screen)
    0,   // 0x38 - Alt (Left Alt)
    ' ', // 0x39 - Space
    0,   // 0x3A - Caps Lock
    0,   // 0x3B - (F1)
    0,   // 0x3C - (F2)
    0,   // 0x3D - (F3)
    0,   // 0x3E - (F4)
    0,   // 0x3F - (F5)
    0,   // 0x40 - (F6)
    0,   // 0x41 - (F7)
    0,   // 0x42 - (F8)
    0,   // 0x43 - (F9)
    0,   // 0x44 - (F10)
    0,   // 0x45 - Num Lock
    0,   // 0x46 - Scroll Lock
    '7', // 0x47 - Numpad 7
    '^', // 0x48 - Numpad 8
    '9', // 0x49 - Numpad 9
    '-', // 0x4A - Numpad '-'
    '4', // 0x4B - Numpad 4
    '5', // 0x4C - Numpad 5
    '6', // 0x4D - Numpad 6
    '+', // 0x4E - Numpad '+'
    '1', // 0x4F - Numpad 1
    'v', // 0x50 - Numpad 2
    '3', // 0x51 - Numpad 3
    '0', // 0x52 - Numpad 0
    '.', // 0x53 - Numpad '.'
    0,   // 0x54 - (Unused)
    0,   // 0x55 - (Unused)
    0,   // 0x56 - (Unused)
    0,   // 0x57 - (F11)
    0,   // 0x58 - (F12)
    0,   // 0x59 - (Unused)
    0,   // 0x5A - (Unused)
    0,   // 0x5B - (Unused)
    0,   // 0x5C - (Unused)
    0,   // 0x5D - (Unused)
    0,   // 0x5E - (Unused)
    0,   // 0x5F - (Unused)
    0,   // 0x60 - (Unused)
    0,   // 0x61 - (Unused)
    0,   // 0x62 - (Unused)
    0,   // 0x63 - (Unused)
    0,   // 0x64 - (Unused)
    0,   // 0x65 - (Unused)
    0,   // 0x66 - (Unused)
    0,   // 0x67 - (Unused)
    0,   // 0x68 - (Unused)
    0,   // 0x69 - (Unused)
    0,   // 0x6A - (Unused)
    0,   // 0x6B - (Unused)
    0,   // 0x6C - (Unused)
    0,   // 0x6D - (Unused)
    0,   // 0x6E - (Unused)
    0,   // 0x6F - (Unused)
    0,   // 0x70 - (Unused)
    0,   // 0x71 - (Unused)
    0,   // 0x72 - (Unused)
    0,   // 0x73 - (Unused)
    0,   // 0x74 - (Unused)
    0,   // 0x75 - (Unused)
    0,   // 0x76 - (Unused)
    0,   // 0x77 - (Unused)
    0,   // 0x78 - (Unused)
    0,   // 0x79 - (Unused)
    0,   // 0x7A - (Unused)
    0,   // 0x7B - (Unused)
    0,   // 0x7C - (Unused)
    0,   // 0x7D - (Unused)
    0,   // 0x7E - (Unused)
    0    // 0x7F - (Unused)
};


#endif //SCANCODES_H