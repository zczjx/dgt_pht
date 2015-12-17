/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: font.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-5    
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
#ifndef _FONT_H_
#define _FONT_H_
#include <linux/types.h>
typedef struct font_bitmap_attr{
	int f_bpp;
	int xLeft;
	int yTop;
	int xRim;
	int yRim;
	int pitch;       /* 对于单色位图, 两行象素之间的跨度 */
	int xCur_begin;
	int yCur_begin;
	int xNext_begin;
	int yNext_begin;
	unsigned char *bitmap_buffer;
}font_bitmap_attr;

typedef struct font{
	char *name;
	struct font_bitmap_attr attr;
	int (*font_init)(char *font_filename, __u32 font_size);
	int (*get_font_bitmap)(__u32 code, struct font_bitmap_attr *bitmap);
	struct font *next;
}font;

extern int load_font_md(void);
extern int register_font_md(struct font *fobj);
extern void print_font_md_lst(void);
extern struct font *get_font_obj(char *name); 
#endif /* ifndef _FONT_H_.2015-12-5 下午 7:28:11 zcz */



