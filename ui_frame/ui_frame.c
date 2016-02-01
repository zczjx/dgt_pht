/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: ui_frame.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-5    
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
#include "image_render.h"
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


/*******************************************************************************
* @function name: ui_name_to_hash_id    
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
int ui_name_to_hash_id(char *name, int hash_len)
{
	int has_sum,i ,max;
	if((!name) || (hash_len <= 0))
		return -1;
	max = strlen(name) % CONFIG_MAX_NAME;
	for(has_sum = 0, i = 0; i < max; i++)
		has_sum += name[i];
	return (has_sum % hash_len);
}

/*******************************************************************************
* @function name: load_ui_sys    
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
int load_ui_sys(struct m_ui_obj *ui_hash[], int hash_len)
{
	int i,ret;
	struct m_ui_obj *item;
	if((!ui_hash) || (hash_len <= 0))
		return -1;
	for(i = 0; i < hash_len; i++){
		item = ui_hash[i];
		if(!item)
			continue;
		else {
			for(; item != NULL; item = item->next){
				ret = item->ui_init(item);
				if(ret){
					perror("init ui error!\n");
					return -1;
				}
			}
		}
	}
	return 0;

}

/*******************************************************************************
* @function name: register_ui_obj    
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
int register_ui_obj(struct m_ui_obj *ui_hash[], int hash_len, struct m_ui_obj *pui)
{
	struct m_ui_obj *item;
	if((!ui_hash) || (hash_len <= 0) || (!pui))
		return -1;
	pui->hash_id = ui_name_to_hash_id(pui->name, hash_len);
	item = ui_hash[pui->hash_id];
	if(!item){
		pui->next = NULL;
		ui_hash[pui->hash_id] = pui;
	}else{
		pui->next = item;
		ui_hash[pui->hash_id] = pui;
	}
	return 0;
}

/*******************************************************************************
* @function name: gen_ui_view    
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
int gen_static_ui_view(struct view *pv)
{
	struct image_obj tmp_org_img; 
	struct image_obj *tmp_zoom_img;
	struct blk_layout *img_set;
	char full_pathname[128];
	int ret;
	if(!pv->dyn_canvas.dis_mem)
		return -1;
	
	/* 描画数据: VideoMem中的页面数据未生成的情况下才执行下面操作 */
	if (pv->dyn_canvas.dis_mem->dat_stat != BD_GENERATED){
		printf("bf clear canvas full gen static ui!\n");
		clear_full_image_obj(&pv->dyn_canvas, CONFIG_COLOR_CANVAS, 1);
		if(pv->icon_rg){
			img_set = pv->icon_rg->blk_set;
			memcpy(full_pathname, CONFIG_ICON_DIR, strlen(CONFIG_ICON_DIR) + 1);
			while (img_set->blk_name){
				float wid_scale, heig_scale;
				ret = create_image_obj(strcat(full_pathname, img_set->blk_name), &tmp_org_img, 
									   TYP_TRUE_IMAGE, 0, NULL);
				if (ret){
					perror("can't create tmp image!\n");
                	destroy_image_obj(&tmp_org_img);
					return -1;
				}
				heig_scale = (img_set->bot_y_right - img_set->top_y_left + 1) 
							 / tmp_org_img.img->pix_of_col;
				wid_scale  = (img_set->bot_x_right- img_set->top_x_left + 1) 
							/ tmp_org_img.img->pix_of_row;
            /* 把原始的图标象素数据缩放为指定大小 */
 				tmp_zoom_img = gen_zoom_image(&tmp_org_img, wid_scale, heig_scale);
				if(!tmp_zoom_img){
					perror("can't zoom tmp image!\n");
                	destroy_image_obj(tmp_zoom_img);
					return -1;
				}

            /* 把缩放后的图标数据,合并到VideoMem的指定区域 */
				ret = merge_image_to_large(tmp_zoom_img, &pv->dyn_canvas, 
									 img_set->top_x_left, img_set->top_y_left);
            /* 释放原始的图标象素数据 */
 				destroy_image_obj(&tmp_org_img);
 				img_set++;
			}


		}
		if(pv->widget_rg){
			printf("bf gen widget in gen static ui!\n");
			img_set = pv->widget_rg->blk_set;
			memcpy(full_pathname, CONFIG_WIDGET_DIR, strlen(CONFIG_WIDGET_DIR) + 1);
			while (img_set->blk_name){
				float wid_scale, heig_scale;
				printf("bf gen widget create img obj in gen static ui!\n");
				ret = create_image_obj(strcat(full_pathname, img_set->blk_name), &tmp_org_img, 
									   TYP_TRUE_IMAGE, 0, NULL);
				if (ret){
					perror("can't create tmp image!\n");
                	destroy_image_obj(&tmp_org_img);
					return -1;
				}
				heig_scale = (img_set->bot_y_right - img_set->top_y_left + 1) 
							 / tmp_org_img.img->pix_of_col;
				wid_scale  = (img_set->bot_x_right- img_set->top_x_left + 1) 
							/ tmp_org_img.img->pix_of_row;
				printf("bf gen widget gen zoom img in gen static ui!\n");
 				tmp_zoom_img = gen_zoom_image(&tmp_org_img, wid_scale, heig_scale);
				if(!tmp_zoom_img){
					perror("can't zoom tmp image!\n");
                	destroy_image_obj(tmp_zoom_img);
					return -1;
				}
				printf("bf gen widget merge img in gen static ui!\n");
				ret = merge_image_to_large(tmp_zoom_img, &pv->dyn_canvas, 
									 img_set->top_x_left, img_set->top_y_left);
 				destroy_image_obj(&tmp_org_img);
 				img_set++;
			}
		}
		if(pv->content_rg){
			img_set = pv->content_rg->blk_set;
			while (img_set->blk_name){
				float wid_scale, heig_scale;
				ret = create_image_obj(img_set->blk_name, &tmp_org_img, 
									   TYP_TRUE_IMAGE, 0, NULL);
				if (ret){
					perror("can't create tmp image!\n");
                	destroy_image_obj(&tmp_org_img);
					return -1;
				}
				heig_scale = (img_set->bot_y_right - img_set->top_y_left + 1) 
							 / tmp_org_img.img->pix_of_col;
				wid_scale  = (img_set->bot_x_right- img_set->top_x_left + 1) 
							/ tmp_org_img.img->pix_of_row;
 				tmp_zoom_img = gen_zoom_image(&tmp_org_img, wid_scale, heig_scale);
				if(!tmp_zoom_img){
					perror("can't zoom tmp image!\n");
                	destroy_image_obj(tmp_zoom_img);
					return -1;
				}
				ret = merge_image_to_large(tmp_zoom_img, &pv->dyn_canvas, 
									 img_set->top_x_left, img_set->top_y_left);
 				destroy_image_obj(&tmp_org_img);
 				img_set++;
			}
		}
        /* 数据上面分配的临时内存 */
		destroy_image_obj(tmp_zoom_img);
		pv->dyn_canvas.dis_mem->dat_stat = BD_GENERATED;
	}
	return 0;
}
/*******************************************************************************
* @function name: show_static_ui_view    
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
int show_static_ui_view(struct view *pv, int ui_id, void *arg)
{
	int ret;
	printf("dsktp show static ui create img obj!\n");
	ret = create_image_obj(NULL, &pv->dyn_canvas, 
							TYP_VIRT_IMAGE, 1, ui_id);
	if(ret){
		perror("can't create canvas!\n");
		return -1;

	}
	/* 2. do layout process */
	printf("dsktp show static ui layout static ui!\n");
	if (!pv->layout_flg)
		pv->layout_static_ui(pv);
	printf("dsktp show static ui gen static ui!\n");
	ret = gen_static_ui_view(pv);
	printf("dsktp show static ui gen static ui!\n");
	if(pv->render_static_ui_hook)
		ret = pv->render_static_ui_hook(pv, arg);

	/* 3. 刷到设备上去 */
	ret = flush_canvas_display(&pv->dyn_canvas);

	/* 4. 解放显存 */
	destroy_image_obj(&pv->dyn_canvas);
	return 0;
}


/*******************************************************************************
* @function name: com_ui_blk_animation    
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
int com_ui_blk_animation(struct blk_layout *pblk, char *anim_name)
{
	if((!pblk) || (!anim_name))
		return -1;
	if(strcmp("press-botton",anim_name) == 0)
		return press_blk_region(pblk->top_x_left,  pblk->top_y_left,
								pblk->bot_x_right, pblk->bot_y_right);

	else if(strcmp("release-botton",anim_name) == 0) 
		return release_blk_region(pblk->top_x_left,  pblk->top_y_left,
								  pblk->bot_x_right, pblk->bot_y_right);

	else{
		perror("no this animation!\n");
		return -1;
	}

}


/*******************************************************************************
* @function name: get_ui_raw_input    
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
int get_ui_raw_input(struct c_input *c_ip)
{
	if(!c_ip)
		return -1;
	return get_rt_raw_input_val(&c_ip->raw_ival);
}


