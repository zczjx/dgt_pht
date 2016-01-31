/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: ui_frame.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-3    
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
#ifndef _UI_FRAME_H_
#define _UI_FRAME_H_
#include "myinput.h"
#include "image_render.h"
#include "ui_config.h"

/*view abstruct*/
typedef struct blk_layout {
	char *blk_name;
	int top_x_left;
	int top_y_left;
	int bot_x_right;
	int bot_y_right;	
}blk_layout;

typedef struct region_layout{
	char *rg_name;
	int top_x_left;
	int top_y_left;
	int bot_x_right;
	int bot_y_right;	
	struct blk_layout *blk_set;
}region_layout;

typedef struct window_attr{
	int top_x_left;
	int top_y_left;
	int bot_x_right;
	int bot_y_right;	
	int src_bpp;
	int xres;
	int yres;
	int max_bytes;
	int back_color;
}window_attr;

typedef struct view{
	struct region_layout *icon_rg;   /*shared with input controller*/
	struct region_layout *widget_rg;
	struct region_layout *content_rg;
	struct window_attr 	 window_var_attr;
	struct image_obj	 dyn_canvas;
	int    layout_flg;
	void (*layout_static_ui)(struct view *self);
	int  (*render_static_ui_hook)(struct view *self, void *arg);
	void (*show_ui_blk_animation)(struct blk_layout *pblk, char *anim_name);
	int  (*prepare_view_dat)(void);
}view;

/*Controller abstruct*/
typedef enum msg_src{
	ICON 	= 0,
	WIDGET  = 1,
	MSG_UNKNOW	= 2,
}msg_src;

typedef struct input_msg{
	enum msg_src src;
	int  val;
}input_msg;

typedef struct c_input{
	struct region_layout *icon_rg;   /*shared with input controller*/
	struct region_layout *widget_rg;
	struct raw_input_val  raw_ival;
	int (*get_input_msg)(struct c_input *self, struct input_msg *imsg); 
}c_input;


/*Model UI OBJ*/
typedef enum ui_stat{
	INACTIVE 	= 0,
	ACTIVE  	= 1,
	UI_UNKNOW		= 2,
}ui_stat;

#define UI_PFD_RD 0
#define UI_PFD_WR 1
typedef struct m_ui_obj{
	char  *name;
	int    hash_id;
	enum   ui_stat  state;
	struct view     *ui_view;
	struct c_input  *ui_input;
	struct input_msg imsg;
	void  *ui_priv_attr;
	void  *app_com_attr;
	int    pfd[2];
	int   (*snd_rqst)(int pfd[2], void *p_request);
	void  (*run)(struct m_ui_obj *self);
	int   (*ui_init)(struct m_ui_obj *self);
	struct m_ui_obj *next;
}m_ui_obj;

extern int ui_name_to_hash_id(char *name, int hash_len);

extern int load_ui_sys(struct m_ui_obj *ui_hash[], int hash_len);

extern int register_ui_obj(struct m_ui_obj *ui_hash[], int hash_len, struct m_ui_obj *pui);

extern int gen_static_ui_view(struct view *pv);

extern int show_static_ui_view(struct view *pv, int ui_id, void *arg);


extern int com_ui_blk_animation(struct blk_layout *pblk, char *anim_name);


extern int get_ui_raw_input(struct c_input *c_ip);


#endif /* ifndef _UI_FRAME_H_.2016-1-3 ÉÏÎç 10:02:53 zcz */













