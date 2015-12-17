/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: tscreen.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-10    
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
#include "myinput.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/
static int init_tscreen_dev(void);
static int exit_tscreen_dev(void);
static int get_tscreen_ev(struct input_ev *pev);


/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct input_dev tscreen = {
			.name 		= "tscreen",
			.init_dev 	= init_tscreen_dev,
			.exit_dev	= exit_tscreen_dev,
			.get_input_ev = get_tscreen_ev,
};


/*******************************************************************************
* @function name: load_tscreen_md    
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
int load_tscreen_md(void)
{
	return  register_input_md(&tscreen);
}


/*******************************************************************************
* @function name: init_tscreen_dev    
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
int init_tscreen_dev(void)
{
#ifdef CONFIG_INPUT_SELECT
	return 0;
#else
	return 0;		
#endif 



}
/*******************************************************************************
* @function name: exit_tscreen_dev    
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
int exit_tscreen_dev(void)
{
	return 0;

}
/*******************************************************************************
* @function name: get_tscreen_ev    
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
int get_tscreen_ev(struct input_ev *pev)
{
#if	  defined(CONFIG_INPUT_QUERY) || defined(CONFIG_INPUT_SELECT) 
		return 0;
#elif defined(CONFIG_INPUT_THREAD) || defined(CONFIG_INPUT_SLIP)

#endif 

}


