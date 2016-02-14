/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: disp_accelerate.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-16    
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
#ifndef _DISP_ACCELERATE_H_
#define _DISP_ACCELERATE_H_
#include <linux/types.h>
#include "display.h"

typedef enum BUF_USE_FLG{
	DISP_DEV = 0,
	COMMON_BUF = 1,	
}BUF_USE_FLG;


/* VideoMem的状态:
 * 空闲/用于预先准备显示内容/用于当前线程
 */
typedef enum DIS_MEM_STAT {
	MEM_FREE = 0,
	MEM_USED_FOR_PREPARE = 1,
	MEM_USED_FOR_CUR = 2,	
}DIS_MEM_STAT;

/* VideoMem中内存里图片的状态:
 * 空白/正在生成/已经生成
 */
typedef enum BUF_DAT_STAT {
	BD_BLANK = 0,
	BD_GENERATING = 1,
	BD_GENERATED = 2,	
}BUF_DAT_STAT;

typedef struct disp_buf_mem{
	int id;                        /* ID值,用于标识不同的页面 */
	enum BUF_USE_FLG buf_use_flag;            /* 1: 这个VideoMem是显示设备的显存; 0: 只是一个普通缓存 */
	enum DIS_MEM_STAT mem_stat; /* 这个VideoMem的状态 */
	enum BUF_DAT_STAT dat_stat;           /* VideoMem中内存里图片的状态 */
	int xres;
	int yres;
	int buf_size;
	int bytes_of_fb_row;
	int src_bpp;
	__u8 *mem;       /* 内存: 用来存储图像 */
	struct disp_buf_mem *next;        /* 链表 */
}disp_buf_mem;

extern int alloc_disp_buf_mem(int num);

extern struct disp_buf_mem * get_disp_dev_mem(void);

#define _PRIOR_HIGH 1
#define _PRIOR_LOW  2
extern struct disp_buf_mem * get_com_disp_buf_mem (int id, int prior);

extern void put_disp_buf_mem(struct disp_buf_mem *mem);

extern int flush_disp_buf_mem(struct disp_buf_mem *dmem);

#endif /* ifndef _DISP_ACCELERATE_H_.2016-1-16 下午 9:01:18 zcz */


