/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: encode.c 
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
#include "config.h"
#include "encode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct encode *eobj_h = NULL;
const char utf8_pre[]	 = {0xEF, 0xBB, 0xBF, 0};
const char utf16le_pre[] = {0xFF, 0xFE, 0};
const char utf16be_pre[] = {0xFE, 0xFF, 0};


/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/

/*******************************************************************************
* @extern function implement in sub module:     
*                    
*******************************************************************************/
#ifdef CONFIG_ASCII_GBK_ENCODE
extern int load_ascii_gbk_encode_md(void);
#endif 
#ifdef CONFIG_UTF16_BE_ENCODE
extern int load_utf16_be_encode_md(void);
#endif 
#ifdef CONFIG_UTF16_LE_ENCODE
extern int load_utf16_le_encode_md(void);
#endif 
#ifdef CONFIG_UTF8_ENCODE
extern int load_utf8_encode_md(void);
#endif 

/*******************************************************************************
* @function name: load_encode_md    
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
int load_encode_md(void)
{
	int ret;
#ifdef CONFIG_ASCII_GBK_ENCODE
	ret = load_ascii_gbk_encode_md();
	if(ret){
		printf("can't load asccii font!\n");
		return -1;
	}	
#endif 
#ifdef CONFIG_UTF16_BE_ENCODE
	ret = load_utf16_be_encode_md();
	if(ret){
		printf("can't load asccii font!\n");
		return -1;
	}

#endif 
#ifdef CONFIG_UTF16_LE_ENCODE
	ret = load_utf16_le_encode_md();
	if(ret){
		printf("can't load asccii font!\n");
		return -1;
	}

#endif 
#ifdef CONFIG_UTF8_ENCODE
	ret = load_utf8_encode_md();
	if(ret){
		printf("can't load asccii font!\n");
		return -1;
	}
#endif 	
	return ret;

}
/*******************************************************************************
* @function name: register_encode_md    
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
int register_encode_md(struct encode *eobj)
{
	struct encode *tmp_encode;
	if(!eobj)
		return -1;
	if(!eobj_h){ /*first item in this list*/
		eobj_h = eobj;
		eobj_h->next = NULL;
		return 0;
	}
	else{
		tmp_encode = eobj_h;
		while(tmp_encode->next)
			tmp_encode = tmp_encode->next;
		tmp_encode->next = eobj;
		eobj->next = NULL;
	}
	return 0;

}

/*******************************************************************************
* @function name: print_encode_md_lst    
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
void print_encode_md_lst(void)
{
	
	struct encode *tmp_encode = eobj_h;
	__u32 i = 0;
	while(tmp_encode){
		printf("the NO.%d module is %s\n", i++, tmp_encode->name);
		tmp_encode = tmp_encode->next;
	}

}
/*******************************************************************************
* @function name: select_encode_for_file    
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
struct encode *select_encode_for_file(__u8 *file_buf_h)
{
	
	struct encode *tmp_encode = eobj_h;
	while(tmp_encode){
		if(tmp_encode->is_support(file_buf_h))
			return tmp_encode;
		else
			tmp_encode = tmp_encode->next;
	}
		return NULL;

}

/*The below API just use for internal encode module obj*/
/*******************************************************************************
* @function name: add_font_for_encode    
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
int add_font_for_encode(struct encode *eobj, struct font *fobj)
{
	struct font *tmp_font;
	if((!eobj) || (!fobj))
		return -1;
	tmp_font = (struct font *) malloc(sizeof(struct font));
	if(!tmp_font)
		return -1;
	memcpy(tmp_font, fobj, sizeof(struct font));
	tmp_font->next = eobj->attr.suppt_font_h;
	eobj->attr.suppt_font_h = tmp_font;
	return 0;
}
/*******************************************************************************
* @function name: del_font_frm_encode    
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
int del_font_frm_encode(struct encode *eobj, struct font *fobj)
{
	struct font *curr, *pre;
	if((!eobj) || (!fobj))
		return -1;
	curr = eobj->attr.suppt_font_h;
	/*del list head*/
	if(strcmp(curr->name, fobj->name) == 0){
		eobj->attr.suppt_font_h = eobj->attr.suppt_font_h->next;
		free(curr);
		return 0;
	}
	pre = curr;
	curr = curr->next;
	while(curr){
		if(strcmp(curr->name, fobj->name) == 0){
			pre->next = curr->next;
			free(curr);
			return 0;
		}
			
		pre = curr;
		curr = curr->next;
	}
	return -1;
}

