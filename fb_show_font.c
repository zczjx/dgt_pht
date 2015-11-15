#include <stdio.h>
#include <fb_show_font.h>


struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;

/********************************************
int main(int argc,int *argv[])
{
	int fb_fd ,fd_hzk16,i;
	struct stat hzk_stat;
	unsigned char chstr[] = "÷‹≥ø÷”";
	const char *str = argv[1];

	struct lcd_scr mylcd;
	fb_fd = open("/dev/fb0", O_RDWR);
	if (fb_fd < 0){
		printf("can't open /dev/fb0\n");
		return -1;
	}
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var)){
		printf("can't get var-info of screen! \n");
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix)){
		printf("can't get fix-info of screen\n");
		return -1;
	}
	mylcd.line_bytes = GET_BYTES_OF_LINE(var);
	mylcd.pixel_bytes = GET_BYTES_OF_PIXEL(var);
	mylcd.scr_bytes = GET_BYTES_OF_SCR(var);
	mylcd.fbmem = (unsigned char *)mmap(NULL, mylcd.scr_bytes,
										PROT_READ | PROT_WRITE, MAP_SHARED,
										fb_fd, 0);
	if(mylcd.fbmem < 0){
		printf("can't mmap\n");
		return -1;
	}
	//mylcd.ascii_mem = fontdata_8x16;
	fd_hzk16 = open("HZK16", O_RDONLY);
	if (fd_hzk16 < 0){
		printf("can't open HZK16\n");
		return -1;
	}
	if(fstat(fd_hzk16, &hzk_stat)){
		printf("can't get fstat\n");
		return -1;
	}
	
	mylcd.hzk_mem = (unsigned char *)mmap(NULL, hzk_stat.st_size,
										PROT_READ, MAP_SHARED,
										fd_hzk16, 0);
	

	CLR_LCD_FB(mylcd.fbmem,mylcd.scr_bytes);
	for(i = 0; str[i] != '\0'; i++ )
		lcd_put_char_ascii(&mylcd, var.xres/2 + (i * 8), var.yres/2, str[i]); 
	lcd_put_chinese(&mylcd, var.xres/2 + 8 * (strlen(str) + 1) , var.yres/2, chstr);	
	return 0;

}
********************/


int put_lcd_pxl(struct lcd_scr *lcd_obj ,int x, int y, unsigned int color)
{
	unsigned char  *pen_8 = lcd_obj->fbmem + lcd_obj->line_bytes * y + x * lcd_obj->pixel_bytes;
	unsigned short *pen_16;	
	unsigned int   *pen_32;	

	unsigned int red, green, blue;	

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (var.bits_per_pixel)
	{
		case 8:
		{
			*pen_8 = color;
			break;
		}
		case 16:
		{
			/* 565 */
			red   = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue  = (color >> 0) & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", var.bits_per_pixel);
			break;
		}
	}
	return 0;	
}

int lcd_put_chinese(struct lcd_scr *lcd_obj, int x, int y, unsigned char *chstr)
{
	unsigned int area;
	unsigned int where;
	unsigned char *dots;
	unsigned char byte;
	int n;
	int i, j, b;
	for(n = 0;chstr[n] != '\0'; n += 2 ){
		area   = chstr[n] - 0xA1;
		where  = chstr[n + 1] - 0xA1;
		dots   = lcd_obj->hzk_mem + (area * 94 + where) * 32;
		for (i = 0; i < 16; i++)/*16 * 2bytes*/
			for (j = 0; j < 2; j++)
			{
				byte = dots[i*2 + j]; /*0,2,4,6...... nr of byte in every dot line*/
				for (b = 7; b >=0; b--)
				{
					if (byte & (1<<b))
					{
						/* show */
						put_lcd_pxl(lcd_obj, n *8 + x + j*8+7-b, y + i, COLOR_GREEN);
					}
					else
					{
					/* hide */
						put_lcd_pxl(lcd_obj, n *8 + x + j*8 + 7-b, y + i, COLOR_BLACK);
					}
				
				}
			}
	}
	
	return 0;
}

int lcd_put_char_ascii(struct lcd_scr *lcd_obj, int x, int y, unsigned char c)
{
	unsigned char *dots = (unsigned char *)&(lcd_obj->ascii_mem[c * BYTE_OF_CHAR]);
	int i, b;
	unsigned char byte;

	for (i = 0; i < BYTE_OF_CHAR; i++)
	{
		byte = dots[i];
		for (b = 7; b >= 0; b--)
		{
			if (byte & (1 << b))
				/* on */
				put_lcd_pxl(lcd_obj, x + 7 - b, y + i, COLOR_GREEN);
			else
				/* off */
				put_lcd_pxl(lcd_obj, x + 7 - b, y + i, COLOR_BLACK);

		}
	}
	return 0;	
	
}
