/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: encode.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-7    
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
#ifndef _ENCODE_H_
#define _ENCODE_H_
#include <linux/types.h>
#include "font.h"
typedef struct encode_attr{
	int head_len;
	struct font *suppt_font_h;
}encode_attr;

typedef struct encode{
	char *name;
	struct encode_attr attr;
	int (*is_support)(char *file_buf_head);
	int (*get_fmtcode_frm_buf)(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode);
	struct encode *next;
}encode;


extern int load_encode_md(void);
extern int register_encode_md(struct encode *eobj);
extern void print_encode_md_lst(void);
extern struct encode *select_encode_for_file(__u8 *file_buf_h); 

/*The below API just use for internal encode module obj*/
int add_font_for_encode(struct encode *eobj, struct font *fobj);
int del_font_frm_encode(struct encode *eobj, struct font *fobj);
extern const char utf8_pre[];
extern const char utf16le_pre[];
extern const char utf16be_pre[];





#endif /* ifndef _ENCODE_H_.2015-12-7 ÏÂÎç 8:09:16 zcz */







