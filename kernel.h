
/* various shorthand types */
#define byte unsigned char
#define word unsigned short
#define dword unsigned int
#define qword unsigned long
#define shortbuf unsigned short int *

/* extern functions: devices.h */

/* VGA screen & colors */
#define vga_width 80
#define vga_height 25
#define vga_mem (unsigned short*)0xB8000
#define vga_color(fg,bg) ( ((byte)fg) | ((byte)bg) << 4 )
#define vga_entry(ch,col) ( ((word)ch) | ((word)col) << 8 )
#define vga_pitch 160 /* bytes until term[x][y+1] */
#define vga_depth 2 /* bytes per entry */
#define vga_black 0
#define vga_blue 1
#define vga_green 2
#define vga_cyan 3
#define vga_red 4
#define vga_magenta 5
#define vga_brown 6
#define vga_lightgrey 7
#define vga_darkgrey 8
#define vga_lightblue 9
#define vga_lightgreen 10
#define vga_lightcyan 11
#define vga_lightred 12
#define vga_lightmagenta 13
#define vga_lightbrown 14
#define vga_white 15

/* utility functions */
#define i2c(i) ('0'+i)
#define checkb(num,b) (((num) & (1<<b))>>b)
#define assert(x) do { if(!(x)) printerr('a'); } while(0)

/* device drivers: -> devices.h */

/* standard integers */
#ifdef _stdint_types
#define uint8_t unsigned char
#define int8_t signed char
#define uint16_t unsigned short int
#define int16_t signed short int
#define uint32_t unsigned int
#define int32_t signed int
#define uint64_t unsigned long int
#define int64_t signed long int
#endif

/* strict functional types */
#ifdef _pure_functional
#define c_int const int
#define mut_int int
#define c_char const char
#define mut_char char
#endif
