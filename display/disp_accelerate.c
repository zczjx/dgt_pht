/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: disp_accelerate.c 
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
#include "disp_accelerate.h"
#include "display.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct disp_buf_mem *buf_mem_h = NULL;


/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/

/*******************************************************************************
* @function name: alloc_disp_buf_mem    
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
int alloc_disp_buf_mem(int num)
{
	int i;
	int xres = 0;
	int yres = 0;
	int src_bpp  = 0;
	int buf_sz;
	int bytes_of_row;
	struct disp_buf_mem *new_buf;
	struct display_dev *dis_dev;
	

	/* 确定VideoMem的大小
	 */
	get_dis_dev_res("fb", &xres, &yres, &src_bpp);
	buf_sz 		 = xres * yres * src_bpp / 8;
	bytes_of_row = xres * src_bpp / 8;

	/* 先把设备本身的framebuffer放入链表
	 * 分配一个T_VideoMem结构体, 注意我们没有分配里面的tPixelDatas.aucPixelDatas
	 * 而是让tPixelDatas.aucPixelDatas指向显示设备的framebuffer
	 */
	new_buf = malloc(sizeof(struct disp_buf_mem));
	if (!new_buf){
		perror("not enough mem to alloc! \n");
		return -1;
	}
	dis_dev = get_disp_dev("fb");
	/* 指向framebuffer */
	printf("bf alloc dev fb to disp mem!\n");
	new_buf->mem = dis_dev->dfb_mem;
	new_buf->src_bpp = src_bpp;
	new_buf->id  = 0;
	new_buf->buf_use_flag = DISP_DEV;
	new_buf->mem_stat	  = MEM_FREE;
	new_buf->dat_stat	  = BD_BLANK;
	new_buf->buf_size	  = buf_sz;
	new_buf->bytes_of_fb_row = bytes_of_row;

	if (num != 0)
	new_buf->mem_stat	  = MEM_USED_FOR_CUR;
	
	/* 放入链表 */
	new_buf->next = buf_mem_h;
	buf_mem_h 	  = new_buf;

	/*
	 * 分配用于缓存的VideoMem
	 */
	printf("bf alloc normal disp mem!\n");
	for (i = 0; i < num; i++){
		/* 分配T_VideoMem结构体本身和"跟framebuffer同样大小的缓存" */
		new_buf = (struct disp_buf_mem *)malloc(sizeof(struct disp_buf_mem) + buf_sz);
		if (!new_buf){
		perror("not enough mem to alloc! \n");
		return -1;
		}
		printf("bf set 1st normal disp mem!\n");
		new_buf->mem = (__u8 *)(new_buf + 1);
		new_buf->id  = -1;
		new_buf->buf_use_flag = COMMON_BUF;
		new_buf->mem_stat	  = MEM_FREE;
		new_buf->dat_stat	  = BD_BLANK;
		new_buf->buf_size	  = buf_sz;
		new_buf->bytes_of_fb_row = bytes_of_row;
		new_buf->next = buf_mem_h;
		buf_mem_h 	  = new_buf;
	}
	return 0;
}

/*******************************************************************************
* @function name: get_disp_dev_mem    
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
struct disp_buf_mem * get_disp_dev_mem(void)
{
	
	struct disp_buf_mem *tmp_buf = buf_mem_h;
	
	while (tmp_buf){
		if (tmp_buf->buf_use_flag == DISP_DEV)
			return tmp_buf;
		tmp_buf = tmp_buf->next;
	}
	return NULL;
}


/*******************************************************************************
* @function name: get_com_disp_buf_mem    
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
struct disp_buf_mem * get_com_disp_buf_mem (int id, int prior)
{
	struct disp_buf_mem *tmp_buf = buf_mem_h;
	
	/* 1. 优先: 取出空闲的、ID相同的videomem */
	while (tmp_buf){
		if ((tmp_buf->mem_stat == MEM_FREE) && (tmp_buf->id == id)){
			tmp_buf->mem_stat  = (prior == _PRIOR_HIGH)? MEM_USED_FOR_CUR : MEM_USED_FOR_PREPARE;
			return tmp_buf;
		}
		tmp_buf = tmp_buf->next;
	}

	/* 2. 如果前面不成功, 取出一个空闲的并且里面没有数据(ptVideoMem->ePicState = PS_BLANK)的VideoMem */
	tmp_buf = buf_mem_h;
	while (tmp_buf){
		if ((tmp_buf->mem_stat == MEM_FREE) && (tmp_buf->dat_stat== BD_BLANK)){
			tmp_buf->id = id;
			tmp_buf->mem_stat  = (prior == _PRIOR_HIGH)? MEM_USED_FOR_CUR : MEM_USED_FOR_PREPARE;
			return tmp_buf;
		}
		tmp_buf = tmp_buf->next;
	}	
	/* 3. 如果前面不成功: 取出任意一个空闲的VideoMem */
	tmp_buf = buf_mem_h;
	while (tmp_buf){
		if (tmp_buf->mem_stat == MEM_FREE){
			tmp_buf->id = id;
			tmp_buf->dat_stat = BD_BLANK;
			tmp_buf->mem_stat  = (prior == _PRIOR_HIGH)? MEM_USED_FOR_CUR : MEM_USED_FOR_PREPARE;
			return tmp_buf;
		}
		tmp_buf = tmp_buf->next;
	}

    /* 4. 如果没有空闲的VideoMem并且bCur为1, 则取出任意一个VideoMem(不管它是否空闲) */
    if (prior == _PRIOR_HIGH){
    	tmp_buf = buf_mem_h;
    	tmp_buf->id = id;
		tmp_buf->dat_stat = BD_BLANK;
		tmp_buf->mem_stat  = (prior == _PRIOR_HIGH)? MEM_USED_FOR_CUR : MEM_USED_FOR_PREPARE;
		return tmp_buf;
    }
	return NULL;
}

/*******************************************************************************
* @function name: put_disp_buf_mem    
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
void put_disp_buf_mem(struct disp_buf_mem *mem)
{
	if(!mem)
		return;
	mem->mem_stat = MEM_FREE;
	if(mem->id == -1)
		mem->dat_stat = BD_BLANK;
	return;
}

/*******************************************************************************
* @function name: flush_disp_buf_mem    
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
int flush_disp_buf_mem(struct disp_buf_mem *dmem)
{
	struct display_dev *dis_dev;
	struct disp_buf_mem *dev_buf_mem = NULL;
	if(!dmem)
		return -1;
	dev_buf_mem = get_disp_dev_mem();
	if(!dev_buf_mem)
		return -1;
	memcpy(dev_buf_mem->mem, dmem->mem, dmem->buf_size);
	return 0;
}

