/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: image_render.h 
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
#ifndef _IMAGE_RENDER_H_
#define _IMAGE_RENDER_H_
#include "ui_config.h"
#include "image_codec.h"
#include "disp_accelerate.h"

typedef enum IMAGE_TYP {
	TYP_TRUE_IMAGE = 0,    
	TYP_VIRT_IMAGE = 1,     
}IMAGE_TYP;

typedef struct image_obj{
	enum IMAGE_TYP type;
	char *path_name;
	int	 canvas_flg;
	struct image_dsc *img;
	struct disp_buf_mem *dis_mem;
}image_obj;

extern int create_image_obj(char *path_name, struct image_obj *obj, 
								  enum IMAGE_TYP type, int canvas_flag, int ui_id);
extern int destroy_image_obj(struct image_obj *obj);
extern int flush_canvas_display(struct image_obj *obj);


extern void clear_full_image_obj(struct image_obj *obj, __u32 color, int canvas_flag);

extern void clear_region_image_obj(struct image_obj *obj, 
											int top_x_left, int top_y_left,
								  			int bot_x_right, int bot_y_right, 
								  			__u32 color, int canvas_flag);

extern struct image_obj* gen_zoom_image(struct image_obj *orgin, float wid_scale, float heig_scale);
extern int merge_image_to_large(struct image_obj *sml, struct image_obj *larg, int x, int y);
extern int merge_part_image_to_large(struct image_obj *sml,int sx, int wid, int sy, int heig, 
											   struct image_obj *larg, int lx, int ly);


extern int press_blk_region(int top_x_left, int top_y_left,
								  int bot_x_right, int bot_y_right);
	
extern int release_blk_region(int top_x_left, int top_y_left,
								  int bot_x_right, int bot_y_right);




#endif /* ifndef _IMAGE_RENDER_H_.2016-1-16 ÏÂÎç 12:11:01 zcz */

