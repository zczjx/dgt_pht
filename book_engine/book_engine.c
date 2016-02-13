/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: book_engine.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-12    
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
#include "book_engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
/*******************************************************************************
* @global static variable:	   
*					 
*******************************************************************************/

static char *def_input_ls[] = {
#ifdef CONFIG_STDIN_DEV
								"stdin_dev",
#endif 

#ifdef CONFIG_TSC_DEV
								"tscreen",							
#endif 
								NULL};

/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
static int show_next_pg(struct book_dsc *self);
static int show_pre_pg(struct book_dsc *self);

static int show_one_pg(struct book_dsc *self, __u8 *pg_st_addr);
static int show_one_font(struct disp *disp_attr, struct font_bitmap_attr  *bit_map);
static int update_global_pg_list(struct book_dsc *self, struct page_dsc *pg_new);
static int inc_lcd_x(struct disp *disp_attr, int x);
static int inc_lcd_y(struct disp *disp_attr, int y, __u32 font_size);
static int relocate_font_dis_pos(struct disp *disp_attr ,struct font_bitmap_attr *bitmap, __u32 font_size);


/*******************************************************************************
* @function name: creat_init_book_dsc    
*                
* @brief:    creat book dsc and init input_dev & buf mem       
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
int init_book_load_sys(struct book_dsc *pdsc, const char *book_path)
{
	int ret;
	struct stat bk_stat;
	void *addr;
	if((!pdsc) || (!book_path))
		return -1;
	/*1. init book file mem mapping*/
	pdsc->bk_buf_attr.fd = open(book_path,O_RDONLY);
	if(pdsc->bk_buf_attr.fd < 0){
		perror("can't open this book file\n");
		return -1;
	}
	if(fstat(pdsc->bk_buf_attr.fd,&bk_stat)){
		perror("can't get book file stat\n");
		return -1;
	}
	addr = mmap(NULL, bk_stat.st_size, 
				PROT_READ, MAP_SHARED, 
				pdsc->bk_buf_attr.fd, 0);
	if(addr == MAP_FAILED){
		perror("can't mmap this file\n");
		return -1;
	}
	pdsc->bk_buf_attr.file_st_addr = (__u8 *)addr;
	pdsc->bk_buf_attr.file_end_addr = pdsc->bk_buf_attr.file_st_addr + bk_stat.st_size;
	/*2. Load subsystem*/
	ret = load_display_md();
	if(ret == -1){
		perror("can't load display module! \n");
		return -1;
	}
	ret = load_font_md();
	if(ret == -1){
		perror("can't load font module! \n");
		return -1;
	}
	ret = load_input_md();
	if(ret == -1){
		perror("can't load input  module! \n");
		return -1;
	}
	ret = load_encode_md();
	if(ret == -1){
		perror("can't load encode module! \n");
		return -1;
	}
	/*3. recognize file readable format*/

	pdsc->text_attr.encode_fmt = select_encode_for_file(pdsc->bk_buf_attr.file_st_addr);
	printf("fmt head len: %d \n", pdsc->text_attr.encode_fmt->attr.head_len);
	if(pdsc->text_attr.encode_fmt){
		pdsc->bk_buf_attr.st_pg_valid_addr = pdsc->bk_buf_attr.file_st_addr
										   + pdsc->text_attr.encode_fmt->attr.head_len;

		pdsc->bk_buf_attr.next_pg_valid_addr = NULL;
	}
		
	else{
		perror("can't recognize this book file encode format! \n");
		return -1;
	}
	
	/*4. enbale input dev*/
	pdsc->input_dev_ls = def_input_ls; 
	ret = enable_input_dev_set(def_input_ls);
	if(ret){
		perror("can't enable this input dev! \n");
		return -1;
	}
	pdsc->get_rt_input_ev = get_rt_input_ev;
	/*5. init page function and global/current page list state */
	pdsc->show_next_pg = show_next_pg;
	pdsc->show_pre_pg  = show_pre_pg;
	pdsc->curr_pg = NULL;
	pdsc->fisrt_pg_h = NULL;
	return 0;	
}


/*******************************************************************************
* @function name: set_book_disp_attr    
*                
* @brief:          
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:  
			if back_color || font_color == 0 stand for default color config
*******************************************************************************/
int set_book_disp_attr(struct book_dsc *pdsc, const char *devname,
										__u32 back_color, __u32 font_color)
{
	int ret;
	pdsc->disp_attr.dev = get_disp_dev(devname);
	if(!pdsc->disp_attr.dev)
		return -1;
	
	ret = select_main_scr_dev(devname);
	if(ret)
		return -1;
	if((back_color || font_color) == 0){ /*default color*/
		pdsc->disp_attr.back_color = COLOR_YELLOWISH;
		pdsc->disp_attr.font_color = COLOR_BROWN;
	}else{
		pdsc->disp_attr.back_color = back_color;
		pdsc->disp_attr.font_color = font_color;
		}
	return 0;
}

/*******************************************************************************
* @function name: set_book_text_attr    
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
#define FONT_ASCII 		0
#define FONT_HZK   		1
#define FONT_FREETYPE   2
#define FONT_UNKOWN   	0xFF
int set_book_text_attr(struct book_dsc *pdsc, char *font_name,
							__u32 font_size )
{
	__u8 font_nr;
	int ret = 0;
	struct font *tmp_ft = pdsc->text_attr.encode_fmt->attr.suppt_font_h;
	while(tmp_ft){
		if(strcmp(tmp_ft->name, font_name) == 0){
			if(strcmp(font_name, "ascii") == 0)
				font_nr = FONT_ASCII;
			else if(strcmp(font_name, "hzk") == 0)
				font_nr = FONT_HZK;
			else if(strcmp(font_name, "freetype") == 0)
				font_nr = FONT_FREETYPE;
			else{ 
				font_nr = FONT_UNKOWN;
				perror("can't no this font file now! \n");
				return -1;
			}
			switch(font_nr){
				case FONT_ASCII:
							pdsc->text_attr.font_file_path = NULL;
							pdsc->text_attr.font_name = font_name;
							pdsc->text_attr.font_size = font_size;
							ret = tmp_ft->font_init(pdsc->text_attr.font_file_path, 
											  pdsc->text_attr.font_size);
							break;
				case FONT_HZK:
							pdsc->text_attr.font_file_path = "./data/HZK16";
							pdsc->text_attr.font_name = font_name;
							pdsc->text_attr.font_size = font_size;
							ret = tmp_ft->font_init(pdsc->text_attr.font_file_path, 
											  pdsc->text_attr.font_size);
							break;
				case FONT_FREETYPE:
							pdsc->text_attr.font_file_path = "./data/simkai.ttf";
							pdsc->text_attr.font_name = font_name;
							pdsc->text_attr.font_size = font_size;
							ret = tmp_ft->font_init(pdsc->text_attr.font_file_path, 
											  pdsc->text_attr.font_size);
							break;
				default:
							break;
				}
		}
		else
			del_font_frm_encode(pdsc->text_attr.encode_fmt, tmp_ft);
		tmp_ft = tmp_ft->next;
	}
	return ret;
}

/*******************************************************************************
* @function name: show_next_pg    
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
static int show_next_pg(struct book_dsc *self)
{
	int ret;
	struct page_dsc *tmp_pg;
	__u8 *show_pg_addr;
	if(!self)
		return -1;
	/*1.show next page first */
	if(self->curr_pg)
		show_pg_addr = self->curr_pg->next_pg_st_addr_at_book_buf;
	else 
		/*page is not in show page list now*/
		show_pg_addr = self->bk_buf_attr.st_pg_valid_addr;
	ret = show_one_pg(self,show_pg_addr);
	if(ret){
		perror("can't show this page\n");
		return -1;
	}
	/*2.update current page data and state */
	if(self->curr_pg && self->curr_pg->next_pg){
		self->curr_pg = self->curr_pg->next_pg;
		printf("current page %d\n", self->curr_pg->pg_nr);
		return 0;
	}
	tmp_pg = (struct page_dsc *)malloc(sizeof(struct page_dsc));
	if(tmp_pg){
		tmp_pg->curr_pg_st_addr_at_book_buf = show_pg_addr;
		tmp_pg->next_pg_st_addr_at_book_buf = self->bk_buf_attr.next_pg_valid_addr;
		tmp_pg->next_pg = NULL;
		tmp_pg->pre_pg= NULL;
		ret = update_global_pg_list(self,tmp_pg);
		printf("current page %d\n", self->curr_pg->pg_nr);
	}
	else{ 
		perror("can't alloc head mem!\n");
		return -1;
	}
	return ret;

}
/*******************************************************************************
* @function name: show_pre_pg    
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
static int show_pre_pg(struct book_dsc *self)

{
	int ret;
	if((!self) || (!self->curr_pg) || (!self->curr_pg->pre_pg)){
		perror("pre-page is not exit!\n");
		return -1;
	}
	ret = show_one_pg(self,self->curr_pg->pre_pg->curr_pg_st_addr_at_book_buf);
	if(ret){
		perror("can't show pre-page!\n");
		return -1;
	}
	self->curr_pg = self->curr_pg->pre_pg;
	printf("current page %d\n", self->curr_pg->pg_nr);
	return 0;
}


/*******************************************************************************
* @function name: show_one_pg    
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
static int show_one_pg(struct book_dsc *self, __u8 *pg_st_addr)
{
	int ret;
	int rd_bytes;
	__u32 wcode;
	struct font *show_font;
	int has_rd_code_flg = 0;
	int has_clr_scr_flg = 0;
	__u8 *char_buf_pos = pg_st_addr;
	
	show_font = self->text_attr.encode_fmt->attr.suppt_font_h;
	if(!show_font){
		perror("no support font!\n");
		return -1;
	}
	show_font->attr.xCur_begin = 0;
	show_font->attr.yCur_begin = self->text_attr.font_size;
	/*1. update point position*/
	while(1){
		
		rd_bytes = self->text_attr.encode_fmt->get_fmtcode_frm_buf(char_buf_pos, 
																   self->bk_buf_attr.file_end_addr,
																   &wcode);
						
		if(rd_bytes){/*have read encode*/
			has_rd_code_flg = 1;
			char_buf_pos += rd_bytes;
		}else{/*has no code to read*/
			if(has_rd_code_flg)
				return 0; /*read EOF*/
			else
				return -1;
		}
	/*skip the space and enter line code*/	
		switch((char) wcode){
			case '\n':
					self->bk_buf_attr.next_pg_valid_addr = char_buf_pos;
					show_font->attr.xCur_begin = 0;
					show_font->attr.yCur_begin = inc_lcd_y(&self->disp_attr, 
														  	show_font->attr.yCur_begin,
														  	self->text_attr.font_size);
					if(show_font->attr.yCur_begin == 0)
						return 0;
					break;
			case '\r':
					break;
			case '\t':
					/*tab key*/
					wcode = ' ';
					break;
			default:
					break;
		}
	/*2. get encode process it and get bimap*/
		ret = show_font->get_font_bitmap(wcode, &show_font->attr);
		if(ret){
			perror("can't get this font bitmap!\n");
			return -1;
		}
	/*3. show bitmap and continue update position until screen full*/
		ret = relocate_font_dis_pos(&self->disp_attr, 
									&show_font->attr, 
									self->text_attr.font_size);
		//printf("have relocated done! ret is %d \n",ret);	
		if(ret) 	/*end of this lcd screen*/
			return 0;
		if(!has_clr_scr_flg){
			self->disp_attr.dev->clr_screen(self->disp_attr.back_color);
			has_clr_scr_flg = 1;
			//printf("back color is 0x%x \n",self->disp_attr.back_color);
			//printf("have clr the screen!\n");	
		}
		ret = show_one_font(&self->disp_attr, &show_font->attr);
		if(ret)
			return -1;
		show_font->attr.xCur_begin = show_font->attr.xNext_begin;
		show_font->attr.yCur_begin = show_font->attr.yNext_begin;
		self->bk_buf_attr.next_pg_valid_addr = char_buf_pos;
	}
	return 0;
}
/*******************************************************************************
* @function name: show_one_font    
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
static int show_one_font(struct disp *disp_attr, struct font_bitmap_attr  *bit_map)
{
	int xpos;
	int ypos;
	__u8 color_byte = 0;
	int i = 0;
	int bit;
	
	if (bit_map->f_bpp == 1){
		for (ypos = bit_map->yTop; ypos < bit_map->yRim; ypos++){
			i = (ypos - bit_map->yTop) * bit_map->pitch;
			for (xpos = bit_map->xLeft, bit = 7; xpos < bit_map->xRim; xpos++){
				if (bit == 7)
					color_byte = bit_map->bitmap_buffer[i++];
				if (color_byte & (1 << bit))
					disp_attr->dev->disp_pixel(xpos, ypos, disp_attr->font_color);
					
					/*when else do nothing*/
				bit--;
				if (bit == -1)
					bit = 7;
			}
		}
	}else if (bit_map->f_bpp == 8){
		for (ypos = bit_map->yTop; ypos < bit_map->yRim; ypos++)
			for (xpos = bit_map->xLeft; xpos < bit_map->xRim; xpos++)
				if (bit_map->bitmap_buffer[i++])
					disp_attr->dev->disp_pixel(xpos, ypos, disp_attr->font_color);
	}else{
		perror("show_one_Font error, can't support this bpp\n");
		return -1;
	}
	return 0;
}
/*******************************************************************************
* @function name: update_curr_pg_stat    
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
static int update_global_pg_list(struct book_dsc *self, struct page_dsc *pg_new)
{
	int ret;
	if((!self) || (!pg_new))
		return -1;
	if(!self->fisrt_pg_h){/*first open this book*/
		pg_new->pg_nr = 1;
		self->fisrt_pg_h = pg_new;
		self->curr_pg = pg_new;
		return 0;
	}else{
		pg_new->pg_nr = self->curr_pg->pg_nr + 1;
		pg_new->pre_pg = self->curr_pg;
		self->curr_pg->next_pg = pg_new;
		self->curr_pg = pg_new;
		self->curr_pg->next_pg = NULL;
		return 0;
	}

}

/*******************************************************************************
* @function name: inc_lcd_x    
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
static int inc_lcd_x(struct disp *disp_attr, int x)
{
	if((x + 1) < disp_attr->dev->attr.xres)
		return(x + 1);
	else
		return 0;

}
/*******************************************************************************
* @function name: inc_lcd_y    
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
static int inc_lcd_y(struct disp *disp_attr, int y, __u32 font_size)
{
	if((y + font_size) < disp_attr->dev->attr.yres)
		return(y + font_size);
	else
		return 0;
}
/*******************************************************************************
* @function name: relocate_font_dis_pos    
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
static int relocate_font_dis_pos(struct disp *disp_attr ,struct font_bitmap_attr *bitmap, __u32 font_size)
{
	int ypos;
	int dx,dy;
	if(bitmap->yRim > disp_attr->dev->attr.yres)
		/*page full*/
		return -1;
	if(bitmap->xRim > disp_attr->dev->attr.xres){
		/*it's time to change to next line*/
		ypos = inc_lcd_y(disp_attr, bitmap->yCur_begin, font_size);
		if(ypos){
			dx = 0 - bitmap->xCur_begin;
			dy = ypos - bitmap->yCur_begin;
			bitmap->xCur_begin += dx;
			bitmap->yCur_begin += dy;

			bitmap->xNext_begin+= dx;
			bitmap->yNext_begin+= dy;
			
			bitmap->xLeft += dx;
			bitmap->yTop  += dy;

			bitmap->xRim += dx;
			bitmap->yRim += dy;
		}else /*page full*/
			return -1;
	}
	
	return 0;
}


