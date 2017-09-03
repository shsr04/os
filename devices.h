
/* extern functions */
extern void io_sendb(word port,byte data);
extern byte io_readb(word port);
extern void gen_int(byte code);
extern void load_gdt(word limit,word base);
extern void setup_idt(void);
extern void load_idt(word limit,word base);
extern int_hdl();

/* device drivers */
#define ps2_command 0x64
#define ps2_data 0x60
#define pic_command1 0x20
#define pic_command2 0xA0
#define pic_data1 0x21
#define pic_data2 0xA1
byte ps2_stat(void) { return io_readb(0x64); }
int ps2_obf(void) { return checkb(ps2_stat(),0); } /* does controller's out buffer have data? */
int ps2_ibf(void) { return checkb(ps2_stat(),1); } /* does controller's in buffer have data? */
int ps2_dst(void) { return checkb(ps2_stat(),3); } /* write to device or controller? */
int ps2_terr(void) { return checkb(ps2_stat(),6); } /* got time-out error? */
int ps2_perr(void) { return checkb(ps2_stat(),7); } /* got parity error? */
void ps2_rst(void) { io_sendb(0x64,0xD1); io_sendb(0x60,0xFE); }
void ps2_wto(void) { volatile a=0; for(a=0; !ps2_obf(); a++) if(a>100) return; } /* returns when data can be read */
void ps2_flo(void) { while(ps2_obf()) io_readb(0x60); } /* flush read buffer */
void ps2_wti(void) { volatile a=0; for(a=0; ps2_ibf(); a++) if(a>100) return; }
void ps2_dis1(void) { io_sendb(0x64,0xAD); }
void ps2_en1(void) { io_sendb(0x64,0xAE); }
void ps2_dis2(void) { io_sendb(0x64,0xA7); }
void ps2_en2(void) { io_sendb(0x64,0xA8); }
byte ps2_rcb(void) { io_sendb(0x64,0x20); return io_readb(0x60); } /* read the controller config. byte */
void ps2_scb(byte cb) { io_sendb(0x64,0x60); io_sendb(0x60,cb); }
byte ps2_tstc(void) { io_sendb(0x64,0xAA); return io_readb(0x60); } /* controller self test, assert 0x55 */

byte pic_mask1(void) { return io_readb(0x21); }
byte pic_mask2(void) { return io_readb(0xA1); }
void pic_ini() { io_sendb(0x20,0x11); io_sendb(0xA0,0x11); 
	io_sendb(0x21,0); io_sendb(0xA1,0); io_sendb(0x21,0x04); io_sendb(0xA1,0x04); io_sendb(0x21,0x01); io_sendb(0xA1,0x01); io_sendb(0x21,0); io_sendb(0xA1,0); }

