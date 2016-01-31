/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: image_codec.h 
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
#ifndef _IMAGE_CODEC_H_
#define _IMAGE_CODEC_H_
#include <linux/types.h>
#include "com_file_opr.h"
typedef struct image_dsc{
	int pix_of_row;   /* ���: һ���ж��ٸ����� */
	int pix_of_col;  /* �߶�: һ���ж��ٸ����� */
	int img_bpp;     /*image origin bpp */
	int scr_bpp;	/*screen fix bpp*/
	int bytes_of_row;  /* һ�������ж����ֽ� */
	int total_bytes; /* �����ֽ��� */ 
	__u8 *pix_dat;  /* �������ݴ洢�ĵط� */
} image_dsc;

typedef struct image_codec {
	char *codec_name;
	struct image_dsc image_obj;
	int (*is_support_fmt)(struct common_file *pfile);  /* �Ƿ�֧��ĳ�ļ� */
	int (*get_pix_dat)(struct common_file *pfile, struct image_dsc *img);  /* ���ļ��н�����ͼ����������� */
	int (*free_pix_dat)(struct image_dsc *img);  
    struct image_codec *next; 
} image_codec ;

extern int load_image_codec (void);
extern int register_image_codec (struct image_codec *pcodec);
extern void print_image_codec_lst(void);

extern struct image_codec * get_image_codec_by_name(char *name);
extern struct image_codec * get_fmt_match_image_codec(struct common_file *pfile);


#endif /* ifndef _IMAGE_CODEC_H_.2016-1-10 ���� 5:11:22 zcz */

