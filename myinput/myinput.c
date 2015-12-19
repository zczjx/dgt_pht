/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: myinput.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2015-12-9    
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
#include "myinput.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct input_dev *idev_h = NULL;

/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/

/*******************************************************************************
* @extern function implement in sub module:     
*                    
*******************************************************************************/
#ifdef CONFIG_STDIN_DEV
extern int load_stdin_dev_md(void);
#endif 

#ifdef CONFIG_TSC_DEV
extern int load_tscreen_md(void);
#endif 



/*******************************************************************************
* @function name: load_input_md    
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
int load_input_md(void)
{
	int ret = 0;
#ifdef CONFIG_STDIN_DEV
	ret = load_stdin_dev_md();
	if(ret){
		printf("can't stdin dev!\n");
		return -1;
	}	

#endif 
#ifdef CONFIG_TSC_DEV
	ret = load_tscreen_md();
	if(ret){
		printf("can't load tscreen dev!\n");
		return -1;
	}	
#endif 
	return 0;
}
/*******************************************************************************
* @function name: register_input_md    
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
int register_input_md(struct input_dev *pdev)
{
	struct input_dev *tmp_dev;
	if(!pdev)
		return -1;
	if(!idev_h){ /*first item in this list*/
		idev_h = pdev;
		idev_h->next = NULL;
		return 0;
	}
	else{
		tmp_dev = idev_h;
		while(tmp_dev->next)
			tmp_dev = tmp_dev->next;
		tmp_dev->next = pdev;
		pdev->next = NULL;
	}
	return 0; 
	
}
/*******************************************************************************
* @function name: print_input_md_lst    
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
void print_input_md_lst(void)
{
	struct input_dev *tmp_dev = idev_h;
	__u32 i = 0;
	while(tmp_dev){
	printf("the NO.%d module is %s\n", i++, tmp_dev->name);
	tmp_dev = tmp_dev->next;
	}

}
/*******************************************************************************
* @function name: get_rt_input_ev    
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
int get_rt_input_ev(struct input_ev *pev)
{
#ifdef CONFIG_INPUT_QUERY
	int ret;
	struct input_dev *tmp_dev = idev_h;
	while(tmp_dev){
		//printf("use input dev: %s\n",tmp_dev->name);
		if(tmp_dev->get_input_ev(pev) == 0)
			return 0;
		tmp_dev = tmp_dev->next;
	}
	return -1;			
#endif 
#ifdef CONFIG_INPUT_SELECT

#endif 
#if CONFIG_INPUT_THREAD 

#endif 
#ifdef CONFIG_INPUT_SLIP

#endif 



}

/*******************************************************************************
* @function name: enable_input_dev_set    
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
int enable_input_dev_set(char *dev_ls[])
{
#ifdef CONFIG_INPUT_QUERY
	int ret;
	struct input_dev *tmp_dev = idev_h;
	while(tmp_dev){
		int i;
		for(i = 0; dev_ls[i] != NULL; i++){
			if(strcmp(tmp_dev->name, dev_ls[i]) == 0){
				ret = tmp_dev->init_dev();
				if(ret){
					printf("can't enable input_dev %s! \n",tmp_dev->name);
					return -1;
				}	
			}
			
		}
		tmp_dev = tmp_dev->next;
	}
	return 0;				
#endif 
#ifdef CONFIG_INPUT_SELECT
				
#endif 
#if defined(CONFIG_INPUT_THREAD) || defined(CONFIG_INPUT_SLIP)

#endif 



}
/*******************************************************************************
* @function name: disable_input_dev_set    
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
int disable_input_dev_set(char *dev_ls[])
{
#ifdef CONFIG_INPUT_QUERY
	int ret;
	struct input_dev *tmp_dev = idev_h;
	while(tmp_dev){
		int i;
		for(i = 0; dev_ls[i] != NULL; i++){
			if(strcmp(tmp_dev->name, dev_ls[i]) == 0){
				ret = tmp_dev->exit_dev();
				if(ret){
					printf("can't exit input_dev %s, it is busy! \n",tmp_dev->name);
					return -1;
				}
					
			}
		}
		tmp_dev = tmp_dev->next;
	}
	return 0;			
#endif 
#ifdef CONFIG_INPUT_SELECT
				
#endif 
#if defined(CONFIG_INPUT_THREAD) || defined(CONFIG_INPUT_SLIP)

				
#endif 


}


