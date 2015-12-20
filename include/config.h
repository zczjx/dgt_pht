/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: config.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-5    
*                
* @brief:    system config Macro in digital book      
*                  
*                  
* @details:        
*                 
*    
*    
* @comment           
*******************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

/*config display subsystem */
#define CONFIG_FB
// #define CONFIG_CRT

/*config Encode subsystem */
#define CONFIG_ASCII_GBK_ENCODE
#define CONFIG_UTF16_BE_ENCODE
#define CONFIG_UTF16_LE_ENCODE
#define CONFIG_UTF8_ENCODE
/*config Input dev component */
#define CONFIG_STDIN_DEV
#define CONFIG_TSC_DEV
/*tscreen input method**/
// #define CONFIG_TSC_INPUT_CLICK
#define CONFIG_TSC_INPUT_SLIDE




/*config Input Method */
// #define CONFIG_INPUT_QUERY
#define CONFIG_INPUT_SELECT
// #define CONFIG_INPUT_THREAD
// #define CONFIG_INPUT_SLIP

/*config Font subsystem */
#define CONFIG_ASCII_FONT
#define CONFIG_HZK_FONT
#define CONFIG_FREETYPE_FONT


#endif /* ifndef _CONFIG_H_.2015-12-5 ÉÏÎç 12:54:30 zcz */




