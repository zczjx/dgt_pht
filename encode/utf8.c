/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: utf8.c 
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
static int is_utf8_encode(char *file_buf_head);
static int get_utf8_code_frm_buf(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode);

static int get_pre_one_bit_cnt(__u8 val);

/*******************************************************************************
* @global var:	   
*					 
*******************************************************************************/
static struct encode utf8 = {
			.name			 	 = "utf8",
			.is_support		 	 = is_utf8_encode,
			.get_fmtcode_frm_buf = get_utf8_code_frm_buf,
			.attr = {
						.head_len 			 = 3,
					 	.suppt_font_h	 = NULL,
					},	
};


/*******************************************************************************
* @function name: load_utf8_encode_md    
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
int load_utf8_encode_md(void)

{	
	int ret;
	ret = add_font_for_encode(&utf8, get_font_obj("ascii"));
	if(ret){
		printf("can't add ascii font!\n");
		return -1;
	}
	ret = add_font_for_encode(&utf8, get_font_obj("freetype"));
	if(ret){
		printf("can't add freetype font!\n");
		return -1;
	}
	return register_encode_md(&utf8);
}

/*******************************************************************************
* @function name: is_utf8_encode    
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
int is_utf8_encode(char *file_buf_head)
{
	if (strncmp((const char*)file_buf_head, utf8_pre, 3) == 0)
			/* UTF-8 */
			return 1;
	else
			return 0;

}

/*******************************************************************************
* @function name: get_utf8_code_frm_buf    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:   
*			����UTF-8�����е������ֽ�B�����B�ĵ�һλΪ0����BΪASCII�룬����B�����ı�ʾһ���ַ�;
*   			���B�ĵ�һλΪ1���ڶ�λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���е�һ���ֽڣ����Ҳ�Ϊ�ַ��ĵ�һ���ֽڱ���;
*    			���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ��������ֽڱ�ʾ;
*   			���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ��������ֽڱ�ʾ;
*    			���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ����ĸ��ֽڱ�ʾ;
*    			��ˣ���UTF-8�����е������ֽڣ����ݵ�һλ�����ж��Ƿ�ΪASCII�ַ�;
*   			����ǰ��λ�����жϸ��ֽ��Ƿ�Ϊһ���ַ�����ĵ�һ���ֽ�; 
*  			����ǰ��λ�����ǰ��λ��Ϊ1������ȷ�����ֽ�Ϊ�ַ�����ĵ�һ���ֽڣ����ҿ��ж϶�
*******************************************************************************/
int get_utf8_code_frm_buf(__u8 *curr_addr, __u8 *buf_end, __u32 *fmtcode)
{
	__u8 val;
	__u32 sum = 0;
	int  i,nr;

	if(curr_addr >= buf_end)
		/*EOF*/
		return 0;
	val = curr_addr[0];
	nr 	= get_pre_one_bit_cnt(val);
	if( nr == 0){
	/*pre one bit is 0  ASCII */
		*fmtcode = curr_addr[0];
		return 1;
	}
	else{
	/*other complex code such as CH*/
		val <<= nr;
		val >>= nr; /*del the redundancy pre bits one*/
		sum += val;
		for(i = 1; i < nr; i++){
			val = curr_addr[i] & 0x3f; 
			sum <<= 6;
			sum += val;
		}
		*fmtcode = sum;
		return nr;
	}
	
}


/*******************************************************************************
* @function name: get_pre_one_bit_cnt    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:    ���ǰ��Ϊ1��λ�ĸ���
* 				����������� 11001111 ��ǰ��1��2λ
*              		11100001 ��ǰ��1��3λ
*	
*******************************************************************************/
static int get_pre_one_bit_cnt(__u8 val)
{
	int cnt,i;
	for(i = 7,cnt = 0; i >= 0; i--)
		if(val & (1 << i))
			cnt ++;
		else
			break;
	
	return cnt;
}




