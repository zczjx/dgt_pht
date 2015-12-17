/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: font.c 
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
#include "config.h"
#include "font.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct font *fobj_h = NULL;

/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/

/*******************************************************************************
* @extern function implement in sub module:     
*                    
*******************************************************************************/
#ifdef CONFIG_ASCII_FONT
extern int load_ascii_font_md(void);
#endif 
#ifdef CONFIG_HZK_FONT
extern int load_hzk_font_md(void);
#endif 
#ifdef CONFIG_FREETYPE_FONT
extern int load_freetype_font_md(void);
#endif 
/*******************************************************************************
* @function name:     
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

int load_font_md(void)
{
	int ret;
#ifdef CONFIG_ASCII_FONT
	ret = load_ascii_font_md();
	if(ret){
		printf("can't load asccii font!\n");
		return -1;
	}	
#endif 
#ifdef CONFIG_HZK_FONT
	ret = load_hzk_font_md();
	if(ret){
		printf("can't load hzk16 font!\n");
		return -1;
	}
#endif 
#ifdef CONFIG_FREETYPE_FONT
	ret = load_freetype_font_md();
	if(ret){
		printf("can't load freetype font!\n");
		return -1;
	}
#endif 
	return ret;

}


/*******************************************************************************
* @function name: register_font_md    
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
int register_font_md(struct font *fobj)

{
	struct font *tmp_font;
	if(!fobj)
		return -1;
	if(!fobj_h){ /*first item in this list*/
		fobj_h = fobj;
		fobj_h->next = NULL;
		return 0;
	}
	else{
		tmp_font = fobj_h;
		while(tmp_font->next)
			tmp_font = tmp_font->next;
		tmp_font->next = fobj;
		fobj->next = NULL;
	}
	return 0;

}


/*******************************************************************************
* @function name: print_font_md_lst    
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
void print_font_md_lst(void)
{
	struct font *tmp_font = fobj_h;
	__u32 i = 0;
	while(tmp_font){
	printf("the NO.%d module is %s\n", i++, tmp_font->name);
	tmp_font = tmp_font->next;
	}

}

/*******************************************************************************
* @function name: get_font_dev    
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
struct font *get_font_obj(char *name)
{
	struct font *tmp_font = fobj_h;
	if(!name)
		return NULL;
	while(tmp_font){
		if(strcmp(tmp_font->name, name) == 0)
			return tmp_font;
		tmp_font = tmp_font->next;

	}
	return NULL;

}

