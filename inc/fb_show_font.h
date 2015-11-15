#include <linux/fb.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define FONTDATAMAX 4096
#define BYTE_OF_CHAR 16

#define COLOR_BLACK 0 
#define COLOR_WHITE  0xffffff
#define COLOR_GREEN  0x00ff00
#define COLOR_BLUE   0x0000ff
#define COLOR_RED    0xff0000



typedef struct lcd_scr{
	unsigned int line_bytes;
	unsigned int pixel_bytes;
	unsigned int scr_bytes;
	unsigned char *fbmem;
	unsigned char *hzk_mem;
	unsigned char *ascii_mem;
	} lcd_scr;
	
#define GET_BYTES_OF_LINE(var) (var.xres * var.bits_per_pixel / 8)	
#define GET_BYTES_OF_PIXEL(var) (var.bits_per_pixel / 8)	
#define GET_BYTES_OF_SCR(var) (var.xres * var.yres * var.bits_per_pixel / 8)	
#define CLR_LCD_FB(fbmem,size) do{memset(fbmem, 0, size);}while(0)	
extern struct fb_var_screeninfo var;
extern struct fb_fix_screeninfo fix;
//extern const unsigned char fontdata_8x16[FONTDATAMAX];
int put_lcd_pxl(struct lcd_scr *lcd_obj ,int x, int y, unsigned int color);

int lcd_put_chinese(struct lcd_scr *lcd_obj, int x, int y, unsigned char *chstr);

int lcd_put_char_ascii(struct lcd_scr *lcd_obj, int x, int y, unsigned char c);
