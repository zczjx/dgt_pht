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
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct input_dev *idev_h = NULL;

#ifdef  CONFIG_INPUT_SELECT
#include <sys/select.h>
static fd_set idev_fd_set;
static int max_fd_val = -1;
#endif 

#ifdef CONFIG_INPUT_THREAD
#include <pthread.h>
static pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  input_cond  = PTHREAD_COND_INITIALIZER;
static struct input_ev sh_iev;
#endif 

/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/
#ifdef CONFIG_INPUT_THREAD
static void * input_pthd_hld_func(void *arg);
#endif 
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
	int ret;
	struct input_dev *tmp_dev = idev_h;
	fd_set tmp_rd_set = idev_fd_set;
	ret = select(max_fd_val, &tmp_rd_set, NULL, NULL, NULL);
	if(ret > 0){
		while(tmp_dev){
			if(FD_ISSET(tmp_dev->mthd.fd, &tmp_rd_set))
				if(tmp_dev->get_input_ev(pev) == 0)
					return 0;
			tmp_dev = tmp_dev->next;
		}
	}	
	return -1;

#endif 
#ifdef CONFIG_INPUT_THREAD 
	pthread_mutex_lock(&input_mutex);
	pthread_cond_wait(&input_cond, &input_mutex);
	memcpy(pev, &sh_iev, sizeof(struct input_ev));
	pthread_mutex_unlock(&input_mutex);
	return 0;

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
	int ret;
	struct input_dev *tmp_dev = idev_h;
	while(tmp_dev){
		int i;
		for(i = 0; dev_ls[i] != NULL; i++){
			if(strcmp(tmp_dev->name, dev_ls[i]) == 0){
				ret = tmp_dev->init_dev();
				if(!ret){
					FD_SET(tmp_dev->mthd.fd, &idev_fd_set);
					if(max_fd_val < tmp_dev->mthd.fd)
						max_fd_val = tmp_dev->mthd.fd;
				}else{
					printf("can't enable input_dev %s! \n",tmp_dev->name);
					return -1;
				}	
			}
			
		}
		tmp_dev = tmp_dev->next;
	}
	max_fd_val++;
	return 0;				
#endif 
#ifdef CONFIG_INPUT_THREAD 
	int ret;
	struct input_dev *tmp_dev = idev_h;
	while(tmp_dev){
		int i;
		for(i = 0; dev_ls[i] != NULL; i++){
			if(strcmp(tmp_dev->name, dev_ls[i]) == 0){
				ret = tmp_dev->init_dev();
				if(!ret)
					ret = pthread_create(&tmp_dev->mthd.tid,NULL,
									input_pthd_hld_func,tmp_dev->get_input_ev);				
				else{
					printf("can't enable input_dev %s! \n",tmp_dev->name);
					return -1;
				}	
			}
			
		}
		tmp_dev = tmp_dev->next;
	}
	return ret;			

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
#ifdef CONFIG_INPUT_THREAD
/*need do pthread join and exit*/
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


}


#ifdef CONFIG_INPUT_THREAD
/*******************************************************************************
* @function name: input_pthd_hld_func    
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
static void * input_pthd_hld_func(void *arg)
{
	struct input_ev iev;
	int (*get_input_ev)(struct input_ev *pev);
	get_input_ev = (int (*)(struct input_ev *)) arg;
	while(1){
		if(get_input_ev(&iev) == 0){
		pthread_mutex_lock(&input_mutex);
		memcpy(&sh_iev, &iev, sizeof(struct input_ev));
		pthread_cond_signal(&input_cond);
		pthread_mutex_unlock(&input_mutex);

		}
	}
	return NULL;

}
#endif 


