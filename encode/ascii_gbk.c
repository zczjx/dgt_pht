/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: ascii_gbk.c 
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
#include "encode.h"
#include <stdio.h>
#include <string.h>
	
/*******************************************************************************
* @Macro:	  
*						  
*					  
*******************************************************************************/


/*******************************************************************************
* @global static func:	   
*					   
*******************************************************************************/
static int is_ascii_gbk_encode(char *file_buf_head);
static int get_ascii_gbk_code_frm_buf(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode);


/*******************************************************************************
* @global var:	   
*					 
*******************************************************************************/
static struct encode ascii_gbk = {
			.name			 	 = "ascii_gbk",
			.is_support		 	 = is_ascii_gbk_encode,
			.get_fmtcode_frm_buf = get_ascii_gbk_code_frm_buf,
			.attr = {
						.head_len 			 = 0,
					 	.suppt_font_h	 = NULL,
					},
};


/*******************************************************************************
* @function name: load_ascii_gbk_encode_md    
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
int load_ascii_gbk_encode_md(void)

{	
	int ret;
	ret = add_font_for_encode(&ascii_gbk, get_font_obj("ascii"));
	if(ret){
		printf("can't add ascii font!\n");
		return -1;
	}
	ret = add_font_for_encode(&ascii_gbk, get_font_obj("hzk"));
	if(ret){
		printf("can't add hzk font!\n");
		return -1;
	}
	ret = add_font_for_encode(&ascii_gbk, get_font_obj("freetype"));
	if(ret){
		printf("can't add freetype font!\n");
		return -1;
	}
	return register_encode_md(&ascii_gbk);
}

/*******************************************************************************
* @function name: is_ascii_gbk_encode    
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
int is_ascii_gbk_encode(char *file_buf_head)
{
	if (strncmp((const char*)file_buf_head, utf8_pre, 3) == 0)
			/* UTF-8 */
			return 0;
	else if (strncmp((const char*)file_buf_head, utf8_pre, 2) == 0)
			/* UTF-16 little endian */
			return 0;
	else if (strncmp((const char*)file_buf_head, utf16be_pre, 2) == 0)
			/* UTF-16 big endian */
			return 0;
	else
			return 1;

}

/*******************************************************************************
* @function name: get_ascii_gbk_code_frm_buf    
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
int get_ascii_gbk_code_frm_buf(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode)
{
	__u8 code = *curr_addr;
	if((curr_addr < buf_end) && (code < (__u8) 0x80)){

		*fmtcode = code;
		return 1;
	}

	else if(((curr_addr + 1) < buf_end) && (code >= (__u8) 0x80)){

		*fmtcode = curr_addr[0] | (((__u32)curr_addr[1]) << 8);
		return 2;
	}

	else if (curr_addr < buf_end){
		*fmtcode = code;
		return 3;

	}
	else
		return 0;
	
}






