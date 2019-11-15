#pragma once
#include "core.hpp"

namespace io {
void send(uint16 port, uint8 byte) {
    asm volatile("outb %%al,%%dx" ::"d"(port), "a"(byte));
}

uint8 receive(uint16 port) {
    uint8 r;
    asm volatile("inb %%dx,%%al" : "=a"(r) : "d"(port));
    return r;
}
} // namespace io

namespace ps2 {
using namespace io;

/**
 * The PS/2 controller has two registers: one for command bytes and one to
 * read/write payload data.
 */
constexpr uint16 COMMAND = 0x64, DATA = 0x60;

/**
 * Specialized send/receive functions with timeout
 */
// void send(uint16 port, uint8 byte);
// uint8 receive(uint16 port);

uint8 read_config() {
    /*Bit     Meaning*/
    /*0 	    First PS/2 port interrupt (1 = enabled, 0 = disabled) */
    /*1 	    Second PS/2 port interrupt (1 = enabled, 0 = disabled, only
     * if 2 PS/2 ports supported)*/
    /*2 	    System Flag (1 = system passed POST, 0 = your OS shouldn't
     * be running)*/
    /*3 	    Should be zero*/
    /*4 	    First PS/2 port clock (1 = disabled, 0 = enabled)*/
    /*5 	    Second PS/2 port clock (1 = disabled, 0 = enabled, only if 2
     * PS/2 ports supported)*/
    /*6 	    First PS/2 port translation (1 = enabled, 0 = disabled)*/
    /*7       Must be zero*/
    send(COMMAND, 0x20);
    return receive(DATA);
}
void write_config(uint8 byte) {
    send(COMMAND, 0x60);
    send(DATA, byte);
}

uint8 status() { return receive(COMMAND); }
bool has_output_data() { return (status() & 0b00000001) != 0; }
bool has_input_data() { return (status() & 0b00000010) != 0; }
bool writing_to_controller() { return (status() & 0b00001000) != 0; }
bool got_timeout_error() { return (status() & 0b010000000) != 0; }
bool got_parity_error() { return (status() & 0b10000000) != 0; }

uint8 read_output_port() {
    send(COMMAND, 0xD0);
    return receive(DATA);
}

bool test_controller() {
    send(COMMAND, 0xAA);
    return receive(DATA) == 0x55;
}
bool test_port_1() {
    send(COMMAND, 0xAB);
    return receive(DATA) == 0x00;
}

void disable_1() { send(COMMAND, 0xAD); }
void enable_1() { send(COMMAND, 0xAE); }
void disable_2() { send(COMMAND, 0xA7); }
void enable_2() { send(COMMAND, 0xA8); }
bool reset_1() {
    send(DATA, 0xFF);
    receive(DATA); // this receive() should not be necessary...
    return receive(DATA) == 0xAA;
}
bool reset_2() {
    send(COMMAND, 0xD4);
    send(DATA, 0xFF);
    receive(DATA); // this receive() should not be necessary...
    return receive(DATA) == 0xAA;
}

void hard_reset() { send(COMMAND, 0xFE); }

uint8 get_output() { return receive(DATA); }

void flush_output() {
    while (has_output_data())
        receive(DATA);
}

bool startup(int device = 1) {
    ps2::disable_1();
    ps2::disable_2();
    ps2::flush_output();
    uint8 cb = ps2::read_config();
    cb &= 0b10111100; // disable interrupts and translation
    ps2::write_config(cb);
    if (!(ps2::test_controller() && ps2::test_port_1()))
        return false;
    ps2::enable_1();
    if (!ps2::reset_1())
        return false;
    if (device == 2) {
        ps2::enable_2();
        if (!ps2::reset_2())
            return false;
    }
    return true;
}

} // namespace ps2
