/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: freetype.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-5    
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
#include "font.h"
#include <stdio.h>
#include <linux/types.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

	
/*******************************************************************************
* @Macro:	  
*						  
*					  
*******************************************************************************/

	
/*******************************************************************************
* @global static func:	   
*					   
*******************************************************************************/
static int ft_font_init(char *font_filename, __u32 font_size);
static int get_ft_font_bitmap(__u32 code,struct font_bitmap_attr *bitmap);

/*******************************************************************************
* @global var:	   
*					 
*******************************************************************************/
static struct font ft_font = {
			.name			 = "freetype",
			.font_init		 = ft_font_init,
			.get_font_bitmap = get_ft_font_bitmap,
};
static FT_Library	  ft_lib;
static FT_Face	   	  ft_face;
static FT_GlyphSlot   ft_slot;
/*******************************************************************************
* @function name: load_freetype_font_md    
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
int load_freetype_font_md(void)
{
	return register_font_md(&ft_font);
}

/*******************************************************************************
* @function name: ft_font_init    
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
static int ft_font_init(char *font_filename, __u32 font_size)
{
	int ret;
  	ret = FT_Init_FreeType(&ft_lib);              /* initialize library */
	if (ret){
		printf("FT_Init_FreeType failed\n");
		return -1;
	}
  
	ret = FT_New_Face(ft_lib, font_filename, 0, &ft_face); /* create face object */
 	if (ret){
		printf("FT_New_Face failed\n");
		return -1;
	}
	ret = FT_Set_Pixel_Sizes(ft_face, font_size, 0);
	if (ret){
			printf("FT_Set_Pixel_Sizes\n");
			return -1;
		}

	ft_slot = ft_face->glyph;
	return 0;
}
/*******************************************************************************
* @function name: get_ft_font_bitmap    
*                
* @brief: the function get one char/wchar font bitmap and refresh 
* 		  the next char/wchar pixel dot beginning position in display
*		  device
*                
* @param:        
*                
*                
* @return:        
*                
* @comment:        
*******************************************************************************/
static int get_ft_font_bitmap(__u32 code, struct font_bitmap_attr *bitmap)
{
	int ret;
	int x_begn = bitmap->xCur_begin;
	int y_begn = bitmap->yCur_begin;	

    /* load glyph image into the slot (erase previous one) */
	ret = FT_Load_Char(ft_face, code, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if (ret){
			printf("FT_Set_Pixel_Sizes\n");
			return -1;
		}

	/*refresh the font char/wchar attr statue in time */
	bitmap->f_bpp = 1;
	bitmap->xLeft = x_begn + ft_slot->bitmap_left;
	bitmap->yTop  = y_begn - ft_slot->bitmap_top;
	
	bitmap->xRim  = bitmap->xLeft + ft_slot->bitmap.width;
	bitmap->yRim  = bitmap->yTop + ft_slot->bitmap.rows;
	bitmap->pitch = ft_slot->bitmap.pitch;
	bitmap->bitmap_buffer = ft_slot->bitmap.buffer;
	bitmap->xNext_begin = x_begn + (ft_slot->advance.x / 64); /*1 step means a 1/64 point in advance*/
	bitmap->yNext_begin = y_begn;
	return 0;
}




