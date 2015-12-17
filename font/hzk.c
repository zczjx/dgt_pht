/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: hzk.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-6    
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
#include "font.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

/*******************************************************************************
* @Macro:	  
*						  
*					  
*******************************************************************************/
typedef struct hzk_dsc{
		int fd;
		__u8 *start_mem;
		__u8 *end_mem;
}hzk_dsc;

/*******************************************************************************
* @global static func:	   
*					   
*******************************************************************************/
static int hzk_font_init(char *font_filename, __u32 font_size);
static int get_hzk_font_bitmap(__u32 code,struct font_bitmap_attr *bitmap);

/*******************************************************************************
* @global var:	   
*					 
*******************************************************************************/
static struct font hzk_font = {
			.name			 = "hzk",
			.font_init		 = hzk_font_init,
			.get_font_bitmap = get_hzk_font_bitmap,
};
static struct hzk_dsc hzk_obj;

/*******************************************************************************
* @function name: load_hzk_font_md    
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
int load_hzk_font_md(void)
{
	return register_font_md(&hzk_font);
}

/*******************************************************************************
* @function name: hzk_font_init    
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
static int hzk_font_init(char *font_filename, __u32 font_size)
{
	struct stat hzkstat;
	if(font_size != 16){
		printf("!error: hzk font only support 16 pixels font size \n");
		return -1;
	}
	hzk_obj.fd = open(font_filename, O_RDONLY);
	if (hzk_obj.fd < 0){
		printf("can't open %s\n",font_filename);
		return -1;
	}
	if(fstat(hzk_obj.fd, &hzkstat)){
		printf("can't get fstat\n");
		return -1;
	}
	
	hzk_obj.start_mem = (__u8 *)mmap(NULL, hzkstat.st_size,
										PROT_READ, MAP_SHARED,
										hzk_obj.fd, 0);
	hzk_obj.end_mem = hzk_obj.start_mem + hzkstat.st_size;
	return 0;
}
/*******************************************************************************
* @function name: get_hzk_font_bitmap    
*                
* @brief: the function get one char/wchar font bitmap and refresh 
* 		  the next char/wchar pixel dot beginning position in display
*		  device
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
static int get_hzk_font_bitmap(__u32 code, struct font_bitmap_attr *bitmap)
{
	int ret;
	int x_begn = bitmap->xCur_begin;
	int y_begn = bitmap->yCur_begin;
	int area;
	int where;

	if(code & 0xffff0000){
		printf("!error: out of hzk code \n");
		return -1;
	}
	area   = (int)(code & 0x000000ff) - 0xA1; /*code is wchar */
	where  = (int)((code >> 8) & 0x000000ff) - 0xA1;
	bitmap->bitmap_buffer = hzk_obj.start_mem + (area * 94 + where) * 32;
	if (bitmap->bitmap_buffer >= hzk_obj.end_mem){
		printf("!error: out of hzk lib mem \n");
		return -1;
	}

	/*refresh the font char/wchar attr statue in time */
	bitmap->f_bpp = 1;
	bitmap->xLeft = x_begn;
	bitmap->yTop  = y_begn - 16; /*hzk  x and y large is stable*/
	bitmap->xRim  = x_begn + 16;
	bitmap->yRim  = y_begn;
	bitmap->pitch = 2;
	bitmap->xNext_begin = x_begn + 16;
	bitmap->yNext_begin = y_begn;
	return 0;
}




