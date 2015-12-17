/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: utf16_be.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-8    
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
static int is_utf16_be_encode(char *file_buf_head);
static int get_utf16_be_code_frm_buf(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode);
/*******************************************************************************
* @global var:	   
*					 
*******************************************************************************/
static struct encode utf16_be = {
			.name			 	 = "utf16_be",
			.is_support		 	 = is_utf16_be_encode,
			.get_fmtcode_frm_buf = get_utf16_be_code_frm_buf,
			.attr = {
						.head_len 			 = 2,
					 	.suppt_font_h	 = NULL,
					},	
};


/*******************************************************************************
* @function name: load_utf16_be_encode_md    
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
int load_utf16_be_encode_md(void)

{	
	int ret;
	ret = add_font_for_encode(&utf16_be, get_font_obj("ascii"));
	if(ret){
		printf("can't add ascii font!\n");
		return -1;
	}
	ret = add_font_for_encode(&utf16_be, get_font_obj("freetype"));
	if(ret){
		printf("can't add freetype font!\n");
		return -1;
	}
	return register_encode_md(&utf16_be);
}

/*******************************************************************************
* @function name: is_utf16_be_encode    
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
int is_utf16_be_encode(char *file_buf_head)
{
	if (strncmp((const char*)file_buf_head, utf16be_pre, 2) == 0)
			/* utf16-le */
			return 1;
	else
			return 0;

}

/*******************************************************************************
* @function name: get_utf16_be_code_frm_buf    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:   
*			
*******************************************************************************/
int get_utf16_be_code_frm_buf(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode)
{
	__u8 val;
	__u32 sum;
	int  i,nr;
	if( (curr_addr + 1) < buf_end){
		*fmtcode = (__u32)(curr_addr[0] << 8) + curr_addr[1];
		return 2;
	}
	else
		/*EOF*/
		return 0;
	
}





