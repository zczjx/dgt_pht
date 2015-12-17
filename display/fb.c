/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: fb.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-4    
*                
* @brief:          
*                  
*                  
* @details:        
*                 
*    
*    
* @comment           
*******************************************************************************/
#include "display.h"
#include <linux/fb.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/*******************************************************************************
* @Macro:     
*                         
*                     
*******************************************************************************/
typedef struct lcd_fb{
		int fb_fd;
		__u32 line_bytes;
		__u32 pixel_bytes;
		__u32 scr_bytes; /*bytes of one screen*/
		unsigned char *fbmem;
}lcd_fb;
#define GET_BYTES_OF_LINE(var) (var.xres * var.bits_per_pixel / 8)	
#define GET_BYTES_OF_PIXEL(var) (var.bits_per_pixel / 8)	
#define GET_BYTES_OF_SCR(var) (var.xres * var.yres * var.bits_per_pixel / 8)	
#define CLR_LCD_FB(fbmem,size,color) do{memset(fbmem, color, size);}while(0)



/*******************************************************************************
* @global static func:     
*                      
*******************************************************************************/
static int fb_dev_init(void);
static int fb_clr_screen(__u32 back_color);
static int fb_disp_pixel(int px, int py, __u32 color);


/*******************************************************************************
* @global var:     
*                    
*******************************************************************************/
static struct fb_var_screeninfo var;
static struct fb_fix_screeninfo fix;
static struct lcd_fb g_lcd;

static struct display_dev fb_dev = {
			.name 		= "fb",
			.dev_init 	= fb_dev_init,
			.clr_screen = fb_clr_screen,
			.disp_pixel = fb_disp_pixel,
};




/*******************************************************************************
* @function name: load_fb_md    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
int load_fb_md(void)
{
	return register_display_md(&fb_dev);
}

/*******************************************************************************
* @function name: fb_dev_init    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
static int fb_dev_init(void)
{

	g_lcd.fb_fd = open("/dev/fb0", O_RDWR);
	if (g_lcd.fb_fd < 0){
		printf("can't open /dev/fb0\n");
		return -1;
	}
	if (ioctl(g_lcd.fb_fd, FBIOGET_VSCREENINFO, &var)){
		printf("can't get var-info of screen! \n");
		return -1;
	}

	if (ioctl(g_lcd.fb_fd, FBIOGET_FSCREENINFO, &fix)){
		printf("can't get fix-info of screen\n");
		return -1;
	}
	g_lcd.line_bytes = GET_BYTES_OF_LINE(var);
	g_lcd.pixel_bytes = GET_BYTES_OF_PIXEL(var);
	g_lcd.scr_bytes = GET_BYTES_OF_SCR(var);
	g_lcd.fbmem = (unsigned char *)mmap(NULL, g_lcd.scr_bytes,
										PROT_READ | PROT_WRITE, MAP_SHARED,
										g_lcd.fb_fd, 0);
	if(g_lcd.fbmem < 0){
		printf("can't mmap\n");
		return -1;
	}
	/* init the resolution and color bit infomation for high level display dev*/
	fb_dev.attr.xres = var.xres;
	fb_dev.attr.yres = var.yres;
	fb_dev.attr.bpp	=  var.bits_per_pixel;
	printf("fb_dev.attr.bpp is:  %d \n", fb_dev.attr.bpp);
	return 0;
}
/*******************************************************************************
* @function name: fb_clr_screen    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
static int fb_clr_screen(__u32 back_color)
{
	__u8  	*fb_8bpp  = g_lcd.fbmem;
	__u16 	*fb_16bpp = (__u16 *)fb_8bpp;	
	__u32   *fb_32bpp = (__u32 *)fb_8bpp;	
	__u32 	red, green, blue;
	__u8    color_8bpp = (__u8)back_color;
	__u16   color_16bpp;
	__u32   color_32bpp = back_color;
	int i = 0;
	switch (var.bits_per_pixel)
	{
		case 8:
				CLR_LCD_FB(fb_8bpp, g_lcd.scr_bytes, color_8bpp);
				break;
		
		case 16:
				/* 565 */
				red   = (back_color >> 16) & 0xff;
				green = (back_color >> 8) & 0xff;
				blue  = (back_color >> 0) & 0xff;
				color_16bpp = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
				for(i = 0;i < g_lcd.scr_bytes;i += 2 ,fb_16bpp ++)
					*fb_16bpp = color_16bpp;
				break;
		case 32:
				for(i = 0;i < g_lcd.scr_bytes;i += 4 ,fb_32bpp ++)
					*fb_32bpp = color_32bpp;
				break;
		default:
				printf("can't surport %dbpp\n", var.bits_per_pixel);
				break;
	}
		
	return 0;	
}
/*******************************************************************************
* @function name: fb_disp_pixel    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
static int fb_disp_pixel(int px, int py, __u32 color)
{
	__u8  	*fb_8bpp  = g_lcd.fbmem;
	__u16 	*fb_16bpp = (__u16 *)fb_8bpp;	
	__u32   *fb_32bpp = (__u32 *)fb_8bpp;	
	__u32 	red, green, blue;
	int 	i = 0;
	if((px >= var.xres) || (py >= var.yres)){
		printf("pixel out of screen!\n");
		return -1;
	}
		
	fb_8bpp  = g_lcd.fbmem 
			 + g_lcd.pixel_bytes * px
			 + g_lcd.line_bytes  * py; 

	fb_16bpp = (__u16 *)fb_8bpp;	
	fb_32bpp = (__u32 *)fb_8bpp;
	//printf("word color is 0x%x !\n",color);
	switch (var.bits_per_pixel)
	{
		case 8:
				*fb_8bpp = color;
				break;
		case 16:
			/* 565 */
				red   = (color >> 16) & 0xff;
				green = (color >> 8) & 0xff;
				blue  = (color >> 0) & 0xff;
				color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
				*fb_16bpp = color;
				break;
		case 32:
				*fb_32bpp = color;
				break;
		default:
				printf("can't surport %dbpp\n", var.bits_per_pixel);
				return -1;
	}
	return 0;	
}


