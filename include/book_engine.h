/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: book_engine.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-12    
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
#ifndef _BOOK_ENGINE_H_
#define _BOOK_ENGINE_H_
#include "config.h"
#include "display.h"
#include "encode.h"
#include "font.h"
#include "myinput.h"
#include <linux/types.h>

typedef struct page_dsc{
	__u32 pg_nr;
	__u8 *curr_pg_st_addr_at_book_buf;
	__u8 *next_pg_st_addr_at_book_buf;
	struct page_dsc *pre_pg;
	struct page_dsc *next_pg;
}page_dsc;

typedef struct book_buf{
	int fd;
	__u8 *file_st_addr;
	__u8 *file_end_addr;
	__u8 *st_pg_valid_addr;
	__u8 *next_pg_valid_addr;
}book_buf;

typedef struct text{
	struct encode *encode_fmt;
	char *font_file_path;
	char *font_name;
	__u32 font_size;
}text;

typedef struct disp{
	struct display_dev *dev;
	__u32 back_color;
	__u32 font_color;
}disp;

typedef struct book_dsc{
	char *book_path_name;
	char **input_dev_ls; /*stdin / tscreen*/
	struct book_buf  bk_buf_attr;
	struct text 	 text_attr;
	struct disp      disp_attr;
	struct page_dsc  *fisrt_pg_h;
	struct page_dsc  *curr_pg;
	int (*show_next_pg)(struct book_dsc *self);
	int (*show_pre_pg) (struct book_dsc *self);
	struct input_ev   rt_ev;
	int (*get_rt_input_ev)(struct input_ev *pev);
}book_dsc;

/*creat book dsc and load and init sub system */
extern int init_book_load_sys(struct book_dsc *pdsc, const char *book_path);

/*if back_color || font_color == 0 stand for default color config */
extern int set_book_disp_attr(struct book_dsc *pdsc ,const char *devname,
										__u32 back_color, __u32 font_color);

extern int set_book_text_attr(struct book_dsc *pdsc ,const char *font_name,
									__u32 font_size );


#endif /* ifndef _BOOK_ENGINE_H_.2015-12-12 ÏÂÎç 12:46:38 zcz */













