#pragma once
#include "core.hpp"
#include "ps2.hpp"
#include "term.hpp"

namespace kbd {

string<80> LINE_BUFFER = {0};

enum special_ascii : char { ASCII_END_OF_TEXT = 0x3, ASCII_BACKSPACE=0x8 };

optional<char> get_ascii() {
    static bool ctrl_pressed = false;
    auto a = ps2::get_output();
    // scan codes (set 2)
    // http://www.techtoys.com.hk/Downloads/Download/Microchip/PS2_driver/ScanCode.pdf
    switch (a) {
    case 0xF0: {
        // break codes
        auto b = ps2::get_output();
        switch (b) {
        case 0x14:
            ctrl_pressed = false;
            break;
        }
        break;
    }
    case 0x14:
        ctrl_pressed = true;
        break;
    case 0x66:
        return ASCII_BACKSPACE;
    case 0x1C:
        return 'a';
    case 0x32:
        return 'b';
    case 0x21:
        if (ctrl_pressed)
            return ASCII_END_OF_TEXT;
        return 'c';
    case 0x23:
        return 'd';
    case 0x24:
        return 'e';
    case 0x2B:
        return 'f';
    case 0x34:
        return 'g';
    case 0x33:
        return 'h';
    case 0x43:
        return 'i';
    case 0x3B:
        return 'j';
    case 0x42:
        return 'k';
    case 0x4B:
        return 'l';
    case 0x3A:
        return 'm';
    case 0x31:
        return 'n';
    case 0x44:
        return 'o';
    case 0x4D:
        return 'p';
    case 0x15:
        return 'q';
    case 0x2D:
        return 'r';
    case 0x1B:
        return 's';
    case 0x2C:
        return 't';
    case 0x3C:
        return 'u';
    case 0x2A:
        return 'v';
    case 0x1D:
        return 'w';
    case 0x22:
        return 'x';
    case 0x35:
        return 'y';
    case 0x1A:
        return 'z';
    case 0x45:
        return '0';
    case 0x16:
        return '1';
    case 0x1E:
        return '2';
    case 0x26:
        return '3';
    case 0x25:
        return '4';
    case 0x2E:
        return '5';
    case 0x36:
        return '6';
    case 0x3D:
        return '7';
    case 0x3E:
        return '8';
    case 0x46:
        return '9';
    case 0x29:
        return ' ';
    case 0x0D:
        return '\t';
    case 0x5A:
        return '\n';
    case 0x55:
        return '=';
    case 0x4E:
        return '-';
    case 0x49:
        return '.';
    case 0x4A:
        return '/';
    //  case 0xE0: {
    //      // read another byte
    //      auto b = ps2::get_output();
    //      //...
    //      break;
    //  }
    default:
        break; // unhandled make/break code
    }
    return {};
}

auto &get_line(bool echo = true) {
    int b = 0;
    while (b < LINE_BUFFER.size() - 1) {
        while (!ps2::has_output_data())
            ;
        auto _a = get_ascii();
        if (!_a)
            continue;
        char a = _a.value;

        if(a==ASCII_BACKSPACE) {
            b--;
            term::Term.col-=1;
            term::write(" ");
            term::Term.col-=1;
            continue;
        }
        if (echo) {
            term::write(a);
        }
        if (a == '\n')
            break;
        if (a == ASCII_END_OF_TEXT) {
            term::write('\n');
            LINE_BUFFER[0] = 0;
            return LINE_BUFFER;
        }
        
        LINE_BUFFER[b++] = a;
    }
    LINE_BUFFER[b] = 0;
    return LINE_BUFFER;
}

} // namespace kbd
