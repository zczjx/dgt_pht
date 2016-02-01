/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: jpeg.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-13    
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
#include <stdio.h>
#include <setjmp.h>
#include <jpeglib.h>
/*******************************************************************************
* @jpeg error handle:	   
*					 
*******************************************************************************/
typedef struct jpg_err{
	struct jpeg_error_mgr err;
	jmp_buf setjmp_buffer;
}jpg_err;

/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
static int is_jpeg_fmt(struct common_file *pfile); 
static int get_jpeg_pix_dat(struct common_file *pfile, struct image_dsc *img);
static int free_jpeg_pix_dat(struct image_dsc *img);  
static int cpy_one_jpeg_row(int pix_of_row, int img_bpp, int scr_bpp, __u8 *src, __u8 *dst);
static void jpeg_err_exit(j_common_ptr com_info);


/*******************************************************************************
* @global var:     
*                    
*******************************************************************************/
static struct image_codec jpeg_codec = {
		.codec_name 	= "jpeg",
		.is_support_fmt = is_jpeg_fmt,
		.get_pix_dat	= get_jpeg_pix_dat,
		.free_pix_dat	= free_jpeg_pix_dat,

};


/*******************************************************************************
* @function name: is_jpeg_fmt    
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
static int is_jpeg_fmt(struct common_file *pfile)
{
	struct jpeg_decompress_struct jinfo;

    /* 默认的错误处理函数是让程序退出
     * 我们参考libjpeg里的bmp.c编写自己的错误处理函数
     */
	//struct jpeg_error_mgr tJErr;   
	struct jpg_err jerr;
    int ret;
    fseek(pfile->fhld, 0, SEEK_SET);

	// 分配和初始化一个decompression结构体
	// jinfo.err = jpeg_std_error(&tJErr);
	jinfo.err              = jpeg_std_error(&jerr.err);
	jerr.err.error_exit    = jpeg_err_exit;

	if(setjmp(jerr.setjmp_buffer)){
		/* 如果程序能运行到这里, 表示JPEG解码出错 */
        jpeg_destroy_decompress(&jinfo);
		return 0;
	}
	
	jpeg_create_decompress(&jinfo);

	// 用jpeg_read_header获得jpg信息
	jpeg_stdio_src(&jinfo, pfile->fhld);

    ret = jpeg_read_header(&jinfo, TRUE);
	jpeg_abort_decompress(&jinfo);

    return (ret == JPEG_HEADER_OK);

}

/*******************************************************************************
* @function name: get_jpeg_pix_dat    
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
static int get_jpeg_pix_dat(struct common_file *pfile, struct image_dsc *img)
{
	struct jpeg_decompress_struct jinfo;
    int ret;
    int bytes_of_rowdat;
    __u8 *row_buf = NULL;
   	__u8 *dst_buf;
	struct jpg_err jerr;

    fseek(pfile->fhld, 0, SEEK_SET);

	// 分配和初始化一个decompression结构体
	//jinfo.err = jpeg_std_error(&tJErr);

	jinfo.err               = jpeg_std_error(&jerr.err);
	jerr.err.error_exit     = jpeg_err_exit;

	if(setjmp(jerr.setjmp_buffer)){
		/* 如果程序能运行到这里, 表示JPEG解码出错 */
        jpeg_destroy_decompress(&jinfo);
        if (row_buf)
            free(row_buf);
        if (img->pix_dat)
            free(img->pix_dat);
		return -1;
	}

	jpeg_create_decompress(&jinfo);
	// 用jpeg_read_header获得jpg信息
	jpeg_stdio_src(&jinfo, pfile->fhld);
    ret = jpeg_read_header(&jinfo, TRUE);
	/*original scale*/
    jinfo.scale_num   = 1;
    jinfo.scale_denom = 1; 
	// 启动解压：jpeg_start_decompress	
	jpeg_start_decompress(&jinfo);
    
	// 一行的数据长度
	bytes_of_rowdat = jinfo.output_width * jinfo.output_components;
	row_buf = (__u8 *) malloc(bytes_of_rowdat);
    if (!row_buf){
		perror("error: have no mem to malloc! \n");
		return -1;
    }

	img->pix_of_row   = jinfo.output_width;
	img->pix_of_col   = jinfo.output_height;
	//ptPixelDatas->iBpp    = iBpp;
	img->bytes_of_row = img->pix_of_row * img->scr_bpp / 8;
    img->total_bytes  = img->bytes_of_row * img->pix_of_col;
	img->pix_dat	  = (__u8 *) malloc(img->total_bytes);
	if (!img->pix_dat){
		perror("error: have no mem to malloc! \n");
		return -1;
	}


    dst_buf = img->pix_dat;

	// 循环调用jpeg_read_scanlines来一行一行地获得解压的数据
	while (jinfo.output_scanline < jinfo.output_height) {
        /* 得到一行数据,里面的颜色格式为0xRR, 0xGG, 0xBB */
		(void) jpeg_read_scanlines(&jinfo, &row_buf, 1);
		// 转到ptPixelDatas去
		cpy_one_jpeg_row(img->pix_of_row, 24,
						 img->scr_bpp, row_buf, 
						 dst_buf);
		dst_buf += img->bytes_of_row;
	}
	free(row_buf);
	jpeg_finish_decompress(&jinfo);
	jpeg_destroy_decompress(&jinfo);
    return 0;
}
/*******************************************************************************
* @function name: free_jpeg_pix_dat    
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
static int free_jpeg_pix_dat(struct image_dsc *img)
{
	if(!img){
		perror("can't free NULL pointer! \n ");
		return -1;
	}	
	free(img->pix_dat);
	img->pix_dat = NULL;
	//memset(img, 0, sizeof(struct image_dsc));
	return 0;


}

/*******************************************************************************
* @function name: cpy_one_jpeg_row    
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
static int cpy_one_jpeg_row(int pix_of_row, int img_bpp, int scr_bpp, __u8 *src, __u8 *dst)
{
	__u32 red;
	__u32 blue;
	__u32 green;
	__u32 color;
	__u16 *dst_dat_16bpp = (__u16 *) dst;
	__u32 *dst_dat_32bpp = (__u32 *) dst;
	int i;
	int pos = 0;

	if (img_bpp != 24)
		return -1;

	if (scr_bpp == 24)
		memcpy(dst, src, pix_of_row * 3);
	else{
		for (i = 0; i < pix_of_row; i++){
			blue  = src[pos++];
			green = src[pos++];
			red   = src[pos++];
			switch(scr_bpp){
				case 32:
						color = (red<< 16) | (green<< 8) | blue;
						*dst_dat_32bpp = color;
						dst_dat_32bpp ++;
						break;

				case 16:
						red   	= red>> 3;
						green 	= green >> 2;
						blue	= blue >> 3;
						color = (red << 11) | (green  << 5) | (blue);
						*dst_dat_16bpp = color;
						dst_dat_16bpp++;
						break;

				default:
						perror("no this format bpp! \n");
						return -1;
			}
			
		}
	}
	return 0;
}


/*******************************************************************************
* @function name: jpeg_err_exit    
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
static void jpeg_err_exit(j_common_ptr com_info)
{
	static char err_str[JMSG_LENGTH_MAX];
    
	struct jpg_err *myerr = (struct jpg_err *) com_info->err;

    /* Create the message */
    (*com_info->err->format_message) (com_info, err_str);
    printf("%s\n", err_str);

	longjmp(myerr->setjmp_buffer, 1);

}

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
int load_jpeg_codec(void)
{
	return register_image_codec(&jpeg_codec);
}


