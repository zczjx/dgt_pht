/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: image_render.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-16    
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
#include "image_render.h"
#include "display.h"
#include "com_file_opr.h"
#include <stdlib.h>
#include <string.h>
/*******************************************************************************
* @global static variable:	   
*					 
*******************************************************************************/


/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
static int invert_blk_region(int top_x_left, int top_y_left, 
							   	  int bot_x_right, int bot_y_right);


/*******************************************************************************
* @function name: create_image_obj    
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
int create_image_obj(char *path_name, struct image_obj *obj, 
							enum IMAGE_TYP type, int canvas_flag, int ui_id)
{
	struct common_file img_f;
	int ret;
	int xres, yres, scr_bpp;
    struct image_codec *pcodec;
	if(!obj)
		return -1;
	switch(type){
		case TYP_TRUE_IMAGE:
			obj->type = TYP_TRUE_IMAGE;
			strncpy(img_f.pathname, path_name, 256);
			img_f.pathname[255] = '\0';
			ret = map_file_ro(&img_f);
			if (ret){
				perror("MapFile  error!\n");
				return -1;
			}

    		pcodec = get_fmt_match_image_codec(&img_f);
			if (!pcodec){
       			unmap_file(&img_f);
				perror("no match image codec!\n");
				return -1;
			}
			get_dis_dev_res("fb", &xres, &yres, &scr_bpp);
			pcodec->image_obj.scr_bpp = scr_bpp;
			ret = pcodec->get_pix_dat(&img_f, &pcodec->image_obj);
			if (ret){
			unmap_file(&img_f);
			perror("get pix error!\n");
			return -1;
			}
			obj->path_name = (char *) malloc(strlen(img_f.pathname));
			if(!obj->path_name){
				unmap_file(&img_f);
				perror("can't malloc mem!\n");
				return -1;
			}
			obj->img = (struct image_dsc *) malloc(sizeof(struct image_dsc));
			if(!obj->img){
				unmap_file(&img_f);
				perror("can't malloc mem!\n");
				return -1;
			}
			memcpy(obj->img, &pcodec->image_obj, sizeof(struct image_dsc));
			if(canvas_flag){
				obj->dis_mem = get_com_disp_buf_mem (ui_id, _PRIOR_HIGH);
				if(!obj->dis_mem){
					unmap_file(&img_f);
					perror("can't get dis_mem!\n");
					return -1;
				}
			}else
				obj->dis_mem = NULL;
			unmap_file(&img_f);
			break;
		case TYP_VIRT_IMAGE:
			obj->type = TYP_VIRT_IMAGE;
			if(canvas_flag){
				obj->dis_mem = get_com_disp_buf_mem (ui_id, _PRIOR_HIGH);
				if(!obj->dis_mem){
					perror("can't get dis_mem!\n");
					return -1;
				}
				obj->path_name = NULL;
				obj->img = NULL;
			}else{
				obj->dis_mem = NULL;
				obj->path_name = NULL;
				obj->img = (struct image_dsc *) malloc(sizeof(struct image_dsc));
				if(!obj->img){
				perror("can't malloc mem!\n");
				return -1;
				}
				get_dis_dev_res("fb", &xres, &yres, &scr_bpp);
				obj->img->scr_bpp = scr_bpp;
				obj->img->total_bytes = xres * yres * scr_bpp / 8;
			}
			break;
		default:
			return -1;
	}
	return 0;
}
/*******************************************************************************
* @function name: destroy_image_obj    
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
int destroy_image_obj(struct image_obj *obj)
{
	if(!obj)
		return -1;
	if(obj->path_name){
		free(obj->path_name);
		obj->path_name = NULL;
	}
	if(obj->img){
		if(obj->img->pix_dat){
			free(obj->img->pix_dat);
			obj->img->pix_dat = NULL;
		}
		free(obj->img);
		obj->img = NULL;
	}
	if(obj->dis_mem){
		put_disp_buf_mem(obj->dis_mem);
		obj->dis_mem = NULL;
	}
	return 0;
}
/*******************************************************************************
* @function name: flush_canvas_display    
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
int flush_canvas_display(struct image_obj *obj)
{
	if((!obj) || (!obj->dis_mem))
		return -1;
	return flush_disp_buf_mem(obj->dis_mem);
}


/*******************************************************************************
* @function name: image_zoom    
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
struct image_obj *gen_zoom_image(struct image_obj *orgin, float wid_scale, float heig_scale)
{
	
    __u32 *src_x_tlb;
	__u32 xpos;
	__u32 ypos;
	__u32 src_y;
	__u8 *dst;
	__u8 *src;
	__u32 pix_bytes = orgin->img->img_bpp / 8;
	__u32 dst_pix_of_ln = orgin->img->pix_of_row * wid_scale;
	__u32 dst_pix_of_col = orgin->img->pix_of_col * heig_scale;
	struct image_obj *dst_img;
	
    src_x_tlb = malloc(sizeof(__u32) * dst_pix_of_ln);
	dst_img	  = malloc(1 + sizeof(struct image_obj) + (
					   pix_bytes * dst_pix_of_ln * dst_pix_of_col ));
	dst_img->img  = malloc(sizeof(struct image_dsc));
    if ((!src_x_tlb) || (!dst_img) || (!dst_img->img)){
        perror("no mem to zoom!\n");
        return NULL;
    }
	printf("bf zoom tmp set val!\n");
	dst_img->path_name = "tmp_img_obj";
	dst_img->type = orgin->type;
	dst_img->img->pix_of_row = dst_pix_of_ln;
	dst_img->img->pix_of_col = dst_pix_of_col;
	dst_img->img->img_bpp = orgin->img->img_bpp;
	dst_img->img->scr_bpp = orgin->img->scr_bpp;
	dst_img->img->bytes_of_row = pix_bytes * dst_img->img->pix_of_row;
	dst_img->img->total_bytes  = dst_img->img->bytes_of_row * dst_img->img->pix_of_col;
	dst_img->img->pix_dat = (__u8 *)(dst_img + 1);
	dst_img->dis_mem = NULL;
	printf("bf src_x_tlb set val!\n");
	//memset(dst_img->img->pix_dat, 0x00 ,dst_img->img->total_bytes);
    for (xpos = 0; xpos < dst_pix_of_ln; xpos++)  //pdwSrcXTable
    	src_x_tlb[xpos] = (__u32)(xpos * 1 / wid_scale);

	printf("bf tmp zoom img cpy val!\n");
    for (ypos = 0; ypos < dst_pix_of_col; ypos++){			
        src_y = (__u32) (ypos * 1 / heig_scale);
		dst = dst_img->img->pix_dat + ypos * dst_img->img->bytes_of_row;
		src = orgin->img->pix_dat + src_y * orgin->img->bytes_of_row;
        for (xpos = 0; xpos < dst_pix_of_ln; xpos++)
			memcpy(dst + xpos * pix_bytes, src + src_x_tlb[xpos] * pix_bytes,
					pix_bytes); 
    }

    free(src_x_tlb);
	return dst_img;
}
/*******************************************************************************
* @function name: merge_image_to_large    
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
int merge_image_to_large(struct image_obj *sml, struct image_obj *larg, int x, int y)
{
	/*bug to be resoleve*/
	int i, j;
	__u8 *src;
	__u8 *dst;
	int larg_bytes_of_row;
	int larg_bpp;
	__u32 larg_pix_of_ln;
	__u32 larg_pix_of_col;
	
	if((!larg) || (!sml)){
		printf("null in merge!\n");
		return -1;
		}

	if(larg->dis_mem){
		/*canvas*/
		larg_bytes_of_row = larg->dis_mem->bytes_of_fb_row;
		larg_bpp		  = larg->dis_mem->src_bpp;
		larg_pix_of_ln	  = larg->dis_mem->xres;
		larg_pix_of_col	  = larg->dis_mem->yres;
		dst = larg->dis_mem->mem;
	}else{
		/*normal img*/
		larg_bytes_of_row = larg->img->bytes_of_row;
		larg_bpp		  = larg->img->img_bpp;
		larg_pix_of_ln	  = larg->img->pix_of_row;
		larg_pix_of_col	  = larg->img->pix_of_col;
		dst = larg->img->pix_dat;
		if(sml->img->img_bpp != larg_bpp)
			return -1;
	}
	
	printf("large bpp is %d!\n", larg_bpp);
	printf("sml bpp is %d!\n", sml->img->img_bpp);
	if ((sml->img->pix_of_row > larg_pix_of_ln)  ||
		(sml->img->pix_of_col > larg_pix_of_col)){
			printf("largbpp err!\n");
			return -1;

		}
	
	src = sml->img->pix_dat;
	dst = dst + y * larg_bytes_of_row + x * larg_bpp / 8;

	if(larg->dis_mem){
		/*canvas*/
		__u8 *tmp_src;
		__u8 *tmp_dst;
		switch(larg_bpp){
			case 32:
				for (i = 0; i < sml->img->pix_of_col; i++){
					tmp_src = src; 
					tmp_dst = dst;	
					for(j = 0; j < sml->img->pix_of_row; j++){
						tmp_dst[0] = tmp_src[0];
						tmp_dst[1] = tmp_src[1];
						tmp_dst[2] = tmp_src[2];
						tmp_dst[3] = 0x00;
						tmp_src += 3;
						tmp_dst += 4;
					}
					src += sml->img->bytes_of_row;
					dst += larg_bytes_of_row;
				}
				break;
			case 24:
				for (i = 0; i < sml->img->pix_of_col; i++){
					memcpy(dst, src, sml->img->pix_of_row);
					src += sml->img->bytes_of_row;
					dst += larg_bytes_of_row;
				}
				break;

			default:
				return -1;
		}
	
	}
	else{
		for (i = 0; i < sml->img->pix_of_col; i++){
			memcpy(dst, src, sml->img->pix_of_row);
			src += sml->img->bytes_of_row;
			dst += larg_bytes_of_row;
		}
	}
	return 0;
}
/*******************************************************************************
* @function name: merge_part_image_to_large    
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
int merge_part_image_to_large(struct image_obj *sml,int sx, int wid, int sy, int heig, 
										struct image_obj *larg, int lx, int ly)
{
	int i;
	__u8 *src;
	__u8 *dst;
	int larg_bytes_of_row;
	int larg_bpp;
	__u32 larg_pix_of_ln;
	__u32 larg_pix_of_col;
	int ln_bytes_of_region;
	if((!larg) || (!sml))
		return -1;
	if(larg->dis_mem){
		/*canvas*/
		int xres,yres,bpp;
		get_dis_dev_res("fb", &xres, &yres, &bpp);
		larg_bytes_of_row = larg->dis_mem->bytes_of_fb_row;
		larg_bpp		  = larg->dis_mem->src_bpp;
		larg_pix_of_ln	  = xres;
		larg_pix_of_col	  = yres;
		dst = larg->dis_mem->mem;
	}else{
		/*normal img*/
		larg_bytes_of_row = larg->img->bytes_of_row;
		larg_bpp		  = larg->img->img_bpp;
		larg_pix_of_ln	  = larg->img->pix_of_row;
		larg_pix_of_col	  = larg->img->pix_of_col;
		dst = larg->img->pix_dat;
	}
	if ((lx < 0 || lx >= larg_pix_of_ln) || \
        (ly < 0 || ly >= larg_pix_of_col) || \
        (sx < 0 || sx >= sml->img->pix_of_row) || \
        (sy < 0 || sy >= sml->img->pix_of_col))
			return -1;		
	ln_bytes_of_region = wid * larg_bpp / 8;
	src = sml->img->pix_dat + sy * sml->img->bytes_of_row + sx * sml->img->img_bpp;
	dst = dst + ly * larg_bytes_of_row + lx * larg_bpp / 8;
	for (i = 0; i < heig; i++){
		memcpy(dst, src, ln_bytes_of_region);
		src += sml->img->bytes_of_row;
		dst += larg_bytes_of_row;
	}
	return 0;
}


/*******************************************************************************
* @function name: press_blk_region    
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
int press_blk_region(int top_x_left, int top_y_left, 
						   int bot_x_right, int bot_y_right)
{
	return invert_blk_region(top_x_left, top_y_left, 
							 bot_x_right, bot_y_right);


}

/*******************************************************************************
* @function name: release_blk_region    
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
int release_blk_region(int top_x_left, int top_y_left,
							 int bot_x_right, int bot_y_right)
{
	return invert_blk_region(top_x_left, top_y_left, 
							 bot_x_right, bot_y_right);
}

/*******************************************************************************
* @function name: clear_full_image_obj    
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
void clear_full_image_obj(struct image_obj *obj, __u32 color, int canvas_flag)
{
	__u8  *disbuf = NULL;
	__u16 *disbuf16bpp;
	__u32 *disbuf32bpp;
	__u16 color16bpp; /* 565 */
	int red,green,blue;
	int xres,yres,src_bpp;
	int i = 0;
	if(!obj)
		return;
	if(canvas_flag){
			if(!obj->dis_mem->mem)
				return;
			disbuf	   = obj->dis_mem->mem;
			disbuf16bpp  = (__u16 *)disbuf;
			disbuf32bpp  = (__u32 *)disbuf;
			get_dis_dev_res("fb", &xres, &yres, &src_bpp);
			switch (src_bpp){
				case 8:
					memset(disbuf, color, obj->dis_mem->buf_size);
					break;
				case 16:
					/* 先根据32位的dwColor构造出16位的wColor16bpp */
					red   = (color>> (16+3)) & 0x1f;
					green = (color >> (8+2)) & 0x3f;
					blue  = (color >> 3) & 0x1f;
					color16bpp = (red << 11) | (green << 5) | blue;
					while (i < obj->dis_mem->buf_size){
						*disbuf16bpp	= color16bpp;
						disbuf16bpp++;
						i += 2;
					}
					break;
				case 32:
					while (i < obj->dis_mem->buf_size){
						*disbuf32bpp = color;
						disbuf32bpp++;
						i += 4;
					}
					break;
				default :
					perror("can't support this src bpp!\n");
					return;
			}
	}
	else{
		if((!obj->img) || (!obj->img->pix_dat))
			return;
		disbuf	   = obj->img->pix_dat;
		disbuf16bpp  = (__u16 *)disbuf;
		disbuf32bpp  = (__u32 *)disbuf;
		get_dis_dev_res("fb", &xres, &yres, &src_bpp);
		obj->img->scr_bpp = src_bpp;
		switch (src_bpp){
			case 8:
				memset(disbuf, color, obj->img->total_bytes);
				break;
			case 16:
					/* 先根据32位的dwColor构造出16位的wColor16bpp */
				red   = (color>> (16+3)) & 0x1f;
				green = (color >> (8+2)) & 0x3f;
				blue  = (color >> 3) & 0x1f;
				color16bpp = (red << 11) | (green << 5) | blue;
				while (i <  obj->img->total_bytes){
					*disbuf16bpp	= color16bpp;
					disbuf16bpp++;
					i += 2;
				}
				break;
			case 32:
				while (i <  obj->img->total_bytes){
					*disbuf32bpp = color;
					disbuf32bpp++;
					i += 4;
				}
				break;
			default :
				perror("can't support this src bpp!\n");
				return;
			}

	}
	return;

}

/*******************************************************************************
* @function name: clear_region_image_obj    
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
void clear_region_image_obj(struct image_obj *obj, 
									int top_x_left, int top_y_left,
								  	int bot_x_right, int bot_y_right, 
								  	__u32 color, int canvas_flag)
{
		__u8  *disbuf = NULL;
		__u16 *disbuf16bpp;
		__u32 *disbuf32bpp;
		__u16 color16bpp; /* 565 */
		int red,green,blue;
		int xres,yres,bpp;
		int i = 0;
		int line_bytes;
		int bytes_of_full_row;
		int xpos;
		int ypos;
		if(canvas_flag){
			get_dis_dev_res("fb", &xres, &yres, &bpp);
			bytes_of_full_row = obj->dis_mem->bytes_of_fb_row;
			disbuf = obj->dis_mem->mem 
				   + top_y_left * obj->dis_mem->bytes_of_fb_row  
				   + top_x_left * bpp / 8;	
			line_bytes = (bot_x_right - top_x_left + 1) * bpp / 8;
		}
		else{
			bpp = obj->img->img_bpp;
			bytes_of_full_row = obj->img->bytes_of_row;
			disbuf = obj->img->pix_dat
				   + top_y_left * obj->img->bytes_of_row  
				   + top_x_left * bpp/ 8;
			
			line_bytes = (bot_x_right - top_x_left + 1) * bpp/ 8;
		}
		disbuf16bpp  = (__u16 *) disbuf;
		disbuf32bpp  = (__u32 *) disbuf;
		switch (bpp){
			case 8:
				for (ypos = top_y_left; ypos <= bot_y_right; ypos++){
					memset(disbuf, color, line_bytes);
					disbuf += bytes_of_full_row;
				}
				break;
			case 16:
				/* 先根据32位的dwColor构造出16位的wColor16bpp */
				red   = (color >> (16+3)) & 0x1f;
				green = (color>> (8+2)) & 0x3f;
				blue  = (color>> 3) & 0x1f;
				color16bpp = (red << 11) | (green << 5) | blue;
				for (ypos = top_y_left; ypos <= bot_y_right; ypos++){
					for (i=0, xpos = top_x_left; xpos <= bot_x_right; xpos++)
						disbuf16bpp[i++] = color16bpp;
					disbuf16bpp = (__u16 *)((unsigned int)disbuf16bpp + bytes_of_full_row);
				}
				break;
			case 32:
				for (ypos = top_y_left; ypos <= bot_y_right; ypos++){
					for (i=0, xpos = top_x_left; xpos <= bot_x_right; xpos++)
						disbuf32bpp[i++] = color;
					disbuf32bpp = (__u32 *)((unsigned int)disbuf32bpp + bytes_of_full_row);
				}
				break;
			default :
				perror("can't support this bpp now\n");
				return;
		}
}

/*******************************************************************************
* @function name: invert_blk_region    
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
static int invert_blk_region(int top_x_left, int top_y_left, 
							   	  int bot_x_right, int bot_y_right)
{
	int ypos;
	int i;
	int xres,yres,bpp;
	int blk_row_bytes;
	__u8 *dis_mem;
	struct disp_buf_mem *dev_buf_mem = get_disp_dev_mem();
	if(!dev_buf_mem)
		return -1;
	
	get_dis_dev_res("fb", &xres, &yres, &bpp);
	dis_mem = dev_buf_mem->mem;
	dis_mem += top_y_left * dev_buf_mem->bytes_of_fb_row 
			+  top_x_left * bpp / 8; /* 图标在Framebuffer中的地址 */
	blk_row_bytes = (bot_x_right - top_x_left  + 1) *bpp / 8;

	for (ypos = top_y_left; ypos <= bot_y_right; ypos++){
		for (i = 0; i < blk_row_bytes; i++)
			dis_mem[i] = ~dis_mem[i];  /* 取反 */
		dis_mem += dev_buf_mem->bytes_of_fb_row;
	}
	return 0;
}

