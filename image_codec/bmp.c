/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: bmp.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-12    
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


/*******************************************************************************
* @bmp file and info head tag:	   
*					 
*******************************************************************************/
typedef struct bmp_file_head { /* bmfh */
	unsigned short bfType; 
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
}  __attribute__ ((packed)) bmp_file_head ;

typedef struct bmp_info_head { /* bmih */
	unsigned long  biSize;
	unsigned long  biWidth;
	unsigned long  biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	unsigned long  biXPelsPerMeter;
	unsigned long  biYPelsPerMeter;
	unsigned long  biClrUsed;
	unsigned long  biClrImportant;
}  __attribute__ ((packed)) bmp_info_head;


/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
static int is_bmp_fmt(struct common_file *pfile); 
static int get_bmp_pix_dat(struct common_file *pfile, struct image_dsc *fmt_img);
static int free_bmp_pix_dat(struct image_dsc *img);  
static int cpy_one_bmp_row(int pix_of_row, int img_bpp, int scr_bpp, __u8 *src, __u8 *dst);


/*******************************************************************************
* @global var:     
*                    
*******************************************************************************/
static struct image_codec bmp_codec = {
		.codec_name 	= "bmp",
		.is_support_fmt = is_bmp_fmt,
		.get_pix_dat	= get_bmp_pix_dat,
		.free_pix_dat	= free_bmp_pix_dat,

};


/*******************************************************************************
* @function name: is_bmp_fmt    
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
static int is_bmp_fmt(struct common_file *pfile)
{
	__u8 *fhead = pfile->fmem;
	if ((fhead[0] != 'B') || (fhead[1] != 'M'))
		return 0;
	else
		return 1;

}

/*******************************************************************************
* @function name: get_bmp_pix_dat    
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
static int get_bmp_pix_dat(struct common_file *pfile, struct image_dsc *fmt_img)
{
	bmp_file_head *bmp_fhead;
	bmp_info_head *bmp_ihead;
    __u8 *fmem;

	int org_img_row_bytes;
	int org_img_row_align_bytes;
	__u8 *bmp_src;
	__u8 *bmp_dst;
	int y;


	
	int iLineWidthReal;

    fmem = pfile->fmem;

	bmp_fhead = (struct bmp_file_head *) fmem ;
	bmp_ihead = (struct bmp_info_head *)(fmem  + sizeof(struct bmp_file_head));

	fmt_img->pix_of_row = bmp_ihead->biWidth;
	fmt_img->pix_of_col = bmp_ihead->biHeight;
	fmt_img->img_bpp	= bmp_ihead->biBitCount;

	if (fmt_img->img_bpp != 24){
		perror("error: only support 24 bit bmp! \n");
		return -1;
	}

	fmt_img->bytes_of_row = fmt_img->pix_of_row * fmt_img->scr_bpp / 8;
	fmt_img->total_bytes  = fmt_img->bytes_of_row * fmt_img->pix_of_col;
	fmt_img->pix_dat	  = (__u8 *) malloc(fmt_img->total_bytes);
	if (!fmt_img->pix_dat){
		perror("error: have no mem to malloc! \n");
		return -1;
	}

	org_img_row_bytes = fmt_img->pix_of_row  * fmt_img->img_bpp / 8;
	org_img_row_align_bytes = (org_img_row_bytes + 3) & ~0x3;   /* Ïò4È¡Õû */
		
	bmp_src = fmem + bmp_fhead->bfOffBits;
	bmp_src = bmp_src + (fmt_img->pix_of_col - 1) * org_img_row_align_bytes; /*dat start frm last line*/

	bmp_dst = fmt_img->pix_dat;
	
	for (y = 0; y < fmt_img->pix_of_col; y++){		
		cpy_one_bmp_row(fmt_img->pix_of_row, fmt_img->img_bpp, 
						fmt_img->scr_bpp, bmp_src, bmp_dst);
		
		bmp_src	-= org_img_row_align_bytes;
		bmp_dst	+= fmt_img->bytes_of_row ;
	}
	return 0;	

}
/*******************************************************************************
* @function name: free_bmp_pix_dat    
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
static int free_bmp_pix_dat(struct image_dsc *img)
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
* @function name: cpy_one_bmp_row    
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
static int cpy_one_bmp_row(int pix_of_row, int img_bpp, int scr_bpp, __u8 *src, __u8 *dst)
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
int load_bmp_codec(void)
{
	return register_image_codec (&bmp_codec);

}

