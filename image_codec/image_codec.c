/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: image_codec.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-10    
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
#include "ui_config.h"
#include "image_codec.h"
#include <string.h>


/*******************************************************************************
* @global static variable:	   
*					 
*******************************************************************************/
static struct image_codec *img_codec_h = NULL;

/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
	
/*******************************************************************************
* @extern function implement in sub module: 	
*					 
*******************************************************************************/
#ifdef CONFIG_JPEG_CODEC
extern int load_jpeg_codec(void);
#endif 
	
#ifdef CONFIG_BMP_CODEC
extern int load_bmp_codec(void);
#endif 
	

/*******************************************************************************
* @function name: load_image_codec    
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

int load_image_codec (void)
{
	int ret;
#ifdef CONFIG_JPEG_CODEC
	ret = load_jpeg_codec();
#endif 
		
#ifdef CONFIG_BMP_CODEC
	ret = load_bmp_codec();
#endif 
	return ret;

}

/*******************************************************************************
* @function name: register_image_codec    
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
int register_image_codec (struct image_codec *pcodec)
{
	struct image_codec *tmp_codec;
	if(!pcodec)
		return -1;
	if(!img_codec_h){ /*first item in this list*/
		img_codec_h = pcodec;
		img_codec_h->next = NULL;
		return 0;
	}
	else{
		tmp_codec = img_codec_h;
		while(tmp_codec->next)
			tmp_codec = tmp_codec->next;
		tmp_codec->next = pcodec;
		pcodec->next = NULL;
	}
	return 0;

}
/*******************************************************************************
* @function name: print_image_codec_lst    
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
void print_image_codec_lst(void)
{
	struct image_codec *tmp_codec = img_codec_h;
	__u32 i = 0;
	while(tmp_codec){
	printf("the NO.%d codec is %s\n", i++, tmp_codec->codec_name);
	tmp_codec = tmp_codec->next;
	}

}

/*******************************************************************************
* @function name: get_image_codec_by_name    
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
struct image_codec * get_image_codec_by_name(char *name)
{
	struct image_codec *tmp_codec = img_codec_h;
	while (tmp_codec){
		if (strcmp(tmp_codec->codec_name, name) == 0)
			return tmp_codec;
		tmp_codec = tmp_codec->next;
	}
	return NULL;
}
/*******************************************************************************
* @function name: get_fmt_match_image_codec    
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
struct image_codec * get_fmt_match_image_codec(struct common_file *pfile)
{
	struct image_codec *tmp_codec = img_codec_h;
	while (tmp_codec){
		if (tmp_codec->is_support_fmt(pfile))
			return tmp_codec;
		tmp_codec = tmp_codec->next;
	}
	return NULL;

}

