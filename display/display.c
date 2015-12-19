/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: display.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version: v1.0.0
*                
* @date: 2015-12-4    
*                
* @brief: Implementation for display system in digital book            
*                  
*                  
* @details:        
*                 
*    
*    
* @comment           
*******************************************************************************/
#include "config.h"
#include "display.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>





/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct display_dev *disp_dev_h = NULL;

/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/

/*******************************************************************************
* @extern function implement in sub module:     
*                    
*******************************************************************************/
#ifdef CONFIG_FB
extern int load_fb_md(void);
#endif 

#ifdef CONFIG_CRT
extern int load_crt_md(void);
#endif 



/*******************************************************************************
* @function name: load_display_md    
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
int load_display_md(void)
{
	int ret;
#ifdef CONFIG_FB
	ret = load_fb_md();
#endif 

#ifdef CONFIG_CRT
	ret = load_crt_md();
#endif 

	return ret;

}

/*******************************************************************************
* @function name: register_display_md    
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
int register_display_md(struct display_dev *pdev)
{
	struct display_dev *tmp_dev;
	if(!pdev)
		return -1;
	if(!disp_dev_h){ /*first item in this list*/
		disp_dev_h = pdev;
		disp_dev_h->next = NULL;
		return 0;
	}
	else{
		tmp_dev = disp_dev_h;
		while(tmp_dev->next)
			tmp_dev = tmp_dev->next;
		tmp_dev->next = pdev;
		pdev->next = NULL;
	}
	return 0;

}

/*******************************************************************************
* @function name: print_disp_md_lst    
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
void print_disp_md_lst(void)
{
	struct display_dev *tmp_dev = disp_dev_h;
	__u32 i = 0;
	while(tmp_dev){
	printf("the NO.%d module is %s\n", i++, tmp_dev->name);
	tmp_dev = tmp_dev->next;
	}


	
}

/*******************************************************************************
* @function name: select_main_scr_dev    
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
int select_main_scr_dev(char *name)
{
	struct display_dev *tmp_dev;
	tmp_dev = get_disp_dev(name);
	if(!tmp_dev)
		return -1;
	tmp_dev->dev_init();
	return 0;

}
/*******************************************************************************
* @function name: get_disp_dev    
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
struct display_dev *get_disp_dev(char *name)
{
	struct display_dev *tmp_dev = disp_dev_h;
	if(!name)
		return NULL;
	while(tmp_dev){
		if(strcmp(tmp_dev->name, name) == 0)
			return tmp_dev;
		tmp_dev = tmp_dev->next;

	}
	return NULL;

}


/*******************************************************************************
* @function name: get_dis_dev_res    
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
int get_dis_dev_res(const char *dev_name, int *xres, int *yres)
{
	struct display_dev *dis_dev = NULL;
	dis_dev = get_disp_dev(dev_name);
	if(!dis_dev)
		return -1;
	*xres   = (int)dis_dev->attr.xres;
	*yres   = (int)dis_dev->attr.yres;
	return 0;
}


