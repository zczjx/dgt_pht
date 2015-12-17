/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: crt.c 
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
#include "display.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/*******************************************************************************
* @Macro:     
*                         
*                     
*******************************************************************************/
/*24 bit true color val*/
#define COLOR_BLACK  		0 
#define COLOR_WHITE  		0xffffff
#define COLOR_GREEN  		0x00ff00
#define COLOR_BLUE   		0x0000ff
#define COLOR_RED    		0xff0000
#define COLOR_YELLOWISH     0xE7DBB5  /* ·º»ÆµÄÖ½ */
#define COLOR_BROWN   		0x514438  /* ºÖÉ«×ÖÌå */

/*******************************************************************************
* @global static func:     
*                      
*******************************************************************************/
static int crt_dev_init(void);
static int crt_clr_screen(__u32 back_color);
static int crt_disp_pixel(__u32 px, __u32 py, __u32 color);


/*******************************************************************************
* @global var:     
*                    
*******************************************************************************/

static struct display_dev crt_dev = {
			.name 		= "crt",
			.dev_init 	= crt_dev_init,
			.clr_screen = crt_clr_screen,
			.disp_pixel = crt_disp_pixel,
};




/*******************************************************************************
* @function name: load_crt_md    
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
int load_crt_md(void)
{
	return register_display_md(&crt_dev);
}

/*******************************************************************************
* @function name: crt_dev_init    
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
static int crt_dev_init(void)
{
	return 0;
}
/*******************************************************************************
* @function name: crt_clr_screen    
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
static int crt_clr_screen(__u32 back_color)
{
	return 0;	
}
/*******************************************************************************
* @function name: crt_disp_pixel    
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
static int crt_disp_pixel(__u32 px, __u32 py, __u32 color)
{
	
	return 0;	
}


