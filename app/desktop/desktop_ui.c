/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: desktop_ui.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-26    
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
#include "ui_frame.h"
#include "display.h"
#include "config_app.h"


/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
static void  dp_layout_static_ui(struct view *self);
static int   dp_render_static_ui_hook(struct view *self, void *arg);
static void  dp_show_ui_blk_animation(struct blk_layout *pblk, char *anim_name);
static int   dp_prepare_view_dat(void);
static int 	 dsktp_get_input_msg(struct c_input *self, struct input_msg *imsg);
static int   dsktp_snd_rqst(int pfd[2], void *p_request);
static void  dsktp_run(struct m_ui_obj *self);
static int   dsktp_ui_init(struct m_ui_obj *self);	

/*******************************************************************************
* @global static variable:	   
*					 
*******************************************************************************/
		/*V*/

static struct blk_layout widget_set[] = {
		{"browse_mode.bmp", 0, 0, 0, 0},
		{"continue_mod.bmp", 0, 0, 0, 0},
		{"setting.bmp", 0, 0, 0, 0},
		NULL,
};

static struct region_layout dp_widget_rg = {
	.rg_name = "widget",
	.blk_set = widget_set,
};

static struct view 	dsktp_view = {
	.icon_rg	= NULL,
	.widget_rg	= &dp_widget_rg,
	.content_rg = NULL,
	.layout_flg = 0,
	.layout_static_ui = dp_layout_static_ui,
	.render_static_ui_hook = dp_render_static_ui_hook,
	.show_ui_blk_animation = dp_show_ui_blk_animation,
	.prepare_view_dat = dp_prepare_view_dat,
};
		/*C*/
static struct c_input dsktp_input={
		.icon_rg	= NULL,
		.widget_rg	= &dp_widget_rg,
		.get_input_msg = dsktp_get_input_msg,
};

		/*M*/
static struct m_ui_obj desktop = {
		.name     = "desktop",
		.state    = INACTIVE,
		.ui_view  = &dsktp_view,
		.ui_input = &dsktp_input,
		.snd_rqst = dsktp_snd_rqst,
		.run	  = dsktp_run,
		.ui_init  = dsktp_ui_init,

};


/*******************************************************************************
* @function name: dsktp_snd_rqst    
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
static int   dsktp_snd_rqst(int pfd[2], void *p_request)
{
	/*ok*/
	return 0;
}
/*******************************************************************************
* @function name: dsktp_run    
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
static void  dsktp_run(struct m_ui_obj *self)
{
	int ret;
	while(1){
		sleep(2);
		ret =  show_static_ui_view(self->ui_view, self->hash_id, NULL);
		printf("dsktp show ui ret is %d\n", ret);
	}
	return 0;

}
/*******************************************************************************
* @function name: dsktp_ui_init    
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
static int   dsktp_ui_init(struct m_ui_obj *self)
{
	/*ok*/
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
static int 	dsktp_get_input_msg(struct c_input *self, struct input_msg *imsg)
{
	/*ok*/
	return 0;


}


/*******************************************************************************
* @function name: dp_layout_static_ui    
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
static void  dp_layout_static_ui(struct view *self)
{
	int iStartY;
	int iWidth;
	int iHeight;
	int xres, yres, src_bpp;
	int iTmpTotalBytes;
	struct blk_layout *blk_set;
	blk_set = self->widget_rg->blk_set;
	get_dis_dev_res("fb", &xres, &yres, &src_bpp);

	/*   
	 *    ----------------------
	 *                           1/2 * iHeight
	 *          browse_mode.bmp  iHeight
	 *                           1/2 * iHeight
	 *         continue_mod.bmp     iHeight
	 *                           1/2 * iHeight
	 *          setting.bmp       iHeight
	 *                           1/2 * iHeight
	 *    ----------------------
	 */
	 
	iHeight = yres * 2 / 10;
	iWidth  = iHeight;
	iStartY = iHeight / 2;

	blk_set[0].top_y_left  = iStartY;
	blk_set[0].bot_y_right = blk_set[0].top_y_left + iHeight - 1;
	blk_set[0].top_x_left  = (xres - iWidth * 2) / 2;
	blk_set[0].bot_x_right = blk_set[0].top_x_left + iWidth * 2 - 1;
	iTmpTotalBytes = (blk_set[0].bot_y_right - blk_set[0].top_x_left + 1) 
				   * (blk_set[0].bot_y_right - blk_set[0].top_y_left + 1) 
				   * src_bpp / 8;
	if (self->window_var_attr.max_bytes < iTmpTotalBytes)
		self->window_var_attr.max_bytes = iTmpTotalBytes;
	printf("dp y_left: %d \n", blk_set[0].top_y_left);
	printf("dp y_right: %d \n", blk_set[0].bot_y_right);
	printf("dp x_left: %d \n", blk_set[0].top_x_left);
	printf("dp x_right: %d \n", blk_set[0].bot_x_right);			
	printf("dp yres: %d \n", yres);
	printf("dp xres: %d \n", xres);

	blk_set[1].top_y_left  = blk_set[0].bot_y_right + iHeight / 2 + 1;
	blk_set[1].bot_y_right = blk_set[1].top_y_left + iHeight - 1;
	blk_set[1].top_x_left  = (xres - iWidth * 2) / 2;
	blk_set[1].bot_x_right = blk_set[1].top_x_left + iWidth * 2 - 1;
	iTmpTotalBytes = (blk_set[1].bot_y_right - blk_set[1].top_x_left + 1) 
				   * (blk_set[1].bot_y_right - blk_set[1].top_y_left + 1) 
				   * src_bpp / 8;
	if (self->window_var_attr.max_bytes < iTmpTotalBytes)
		self->window_var_attr.max_bytes = iTmpTotalBytes;


	blk_set[2].top_y_left  = blk_set[1].bot_y_right + iHeight / 2 + 1;
	blk_set[2].bot_y_right = blk_set[2].top_y_left + iHeight - 1;
	blk_set[2].top_x_left  = (xres - iWidth * 2) / 2;
	blk_set[2].bot_x_right = blk_set[2].top_x_left + iWidth * 2 - 1;
	iTmpTotalBytes = (blk_set[2].bot_y_right - blk_set[2].top_x_left + 1) 
				   * (blk_set[2].bot_y_right - blk_set[2].top_y_left + 1) 
				   * src_bpp / 8;
	if (self->window_var_attr.max_bytes < iTmpTotalBytes)
		self->window_var_attr.max_bytes = iTmpTotalBytes;

}
/*******************************************************************************
* @function name: dp_render_static_ui_hook    
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
static int   dp_render_static_ui_hook(struct view *self, void *arg)
{
	/*ok*/
	return 0;

}
/*******************************************************************************
* @function name: dp_show_ui_blk_animation    
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
static void  dp_show_ui_blk_animation(struct blk_layout *pblk, char *anim_name)
{
	int ret;
	ret = com_ui_blk_animation(pblk, anim_name);
	if(ret){
		;
	}
	
}

/*******************************************************************************
* @function name: dp_prepare_view_dat    
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
static int   dp_prepare_view_dat(void)
{
	/*ok*/
	return 0;

}

/*******************************************************************************
* @function name: load_desktop_ui    
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
int load_desktop_ui(struct m_ui_obj *ui_hash[], int hash_len, struct m_ui_obj *pui)
{
	return register_ui_obj(ui_hash, hash_len, &desktop);

}

