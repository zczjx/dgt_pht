/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: display.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version: v1.0.0
*                
* @date: 2015-12-3    
*                
* @brief: API file for display system in digital book         
*                  
*                  
* @details:        
*                 
*    
*    
* @comment           
*******************************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <linux/types.h>

/*************color attr macro************************/
/*24 bit true color val*/
#define COLOR_BLACK  		0 
#define COLOR_WHITE  		0xffffff
#define COLOR_GREEN  		0x00ff00
#define COLOR_BLUE   		0x0000ff
#define COLOR_RED    		0xff0000
#define COLOR_YELLOWISH     0xE7DBB5  /* 泛黄的纸 */
#define COLOR_BROWN   		0x514438  /* 褐色字体 */
/*************************************/
typedef struct disp_dev_attr{
		__u32 xres;
		__u32 yres;
		__u32 bpp;
}disp_dev_attr;
typedef struct display_dev{
	char *name;
	struct disp_dev_attr attr;
	int (*dev_init)(void);
	int (*clr_screen)(__u32 back_color);
	int (*disp_pixel)(int px, int py, __u32 color);
	struct display_dev *next;
}display_dev;

extern int load_display_md(void);
extern int register_display_md(struct display_dev *pdev);
extern void print_disp_md_lst(void);
extern int select_main_scr_dev(char *name); /*fb/crt */
extern struct display_dev *get_disp_dev(char *name); 

extern int get_dis_dev_res(const char *dev_name, int *xres, int *yres);




#endif /* ifndef _DISPLAY_H_.2015-12-3 下午 10:12:15 zcz */





