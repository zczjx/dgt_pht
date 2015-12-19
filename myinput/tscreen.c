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
#include "config.h"
#include "myinput.h"
#include "display.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "tslib.h"



/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/
static int init_tscreen_dev(void);
static int exit_tscreen_dev(void);
static int get_tscreen_ev(struct input_ev *pev);
static int is_out_of_time(struct timeval *pre_time, struct timeval *now_time,int dtime_ms);



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

static struct tsdev *ts;
static char  *tsdevice = NULL;


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
#define _BLK_MD_ 0
#define _NO_BLK_MD_ 1

int init_tscreen_dev(void)
{
#ifdef CONFIG_INPUT_SELECT
	return 0;
#else
	printf("init input dev: %s! \n",tscreen.name);
	if((tsdevice = getenv("TSLIB_TSDEVICE")) != NULL )
		ts = ts_open(tsdevice, _BLK_MD_);
	else 
		ts = ts_open("/dev/input/event0", _BLK_MD_);
	if (!ts){
		perror("ts_open");
		return -1;
	   }
	if (ts_config(ts)) {
		perror("ts_config");
		return -1;
	   }
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
#ifdef CONFIG_TSC_INPUT_CLICK
#if	defined(CONFIG_INPUT_QUERY) || defined(CONFIG_INPUT_SELECT) 
	static int get_res_flag = 0;
	static int g_xres;
	static int g_yres;
	static int up_scr = 0;
	static int down_scr = 0;
	struct ts_sample samp;
	int ret;
	static struct timeval pre_tv;
	if(!get_res_flag){
	ret = get_dis_dev_res("fb", &g_xres, &g_yres);
	printf("xres val is %d\n", g_xres);
	printf("yres val is %d\n", g_yres);
	up_scr = g_yres / 3;
	down_scr = (2 * g_yres) / 3;
	get_res_flag = 1;
	}

	//printf("before read tslib\n");
	ret = ts_read(ts, &samp, 1);
	if (ret < 0) 
		return -1;	
	if(is_out_of_time(&pre_tv, &samp.tv, 500)){
		if((samp.y <= 0) || (samp.y > g_yres))
			return -1;  /*discard val*/
		memcpy(&pre_tv, &samp.tv, sizeof(struct timeval));
		memcpy(&pev->time, &samp.tv, sizeof(struct timeval));
		pev->type = INPUT_TYPE_TSC;
		//printf("ts y val is %d\n", samp.y);
		if(samp.y < up_scr)
			pev->val = INPUT_VAL_UP ;
		else if(samp.y > down_scr)
			pev->val = INPUT_VAL_DOWN;
		else
			pev->val = INPUT_VAL_UNKNOWN;
		return 0;
	}else
		return -1;
#elif defined(CONFIG_INPUT_THREAD) || defined(CONFIG_INPUT_SLIP)

#endif 


#endif 

#ifdef CONFIG_TSC_INPUT_SLIDE
#if	defined(CONFIG_INPUT_QUERY) || defined(CONFIG_INPUT_SELECT) 
	int ret;
	static int get_res_flag = 0;
	static int g_xres;
	static int g_yres;
	static int slide_st = 0;
	static int r_slide = 0;
	static int l_slide = 0;
	static struct ts_sample pre_samp; /*press sample*/
	static struct ts_sample rls_samp;
	struct ts_sample samp;
	int ts_delta;
	if(!get_res_flag){
	ret = get_dis_dev_res("fb", &g_xres, &g_yres);
	printf("xres val is %d\n", g_xres);
	printf("yres val is %d\n", g_yres);
	r_slide = g_xres / 5;
	l_slide = 0 - r_slide;
	get_res_flag = 1;
	}

	ret = ts_read(ts, &samp, 1);
	if (ret < 0) 
		return -1;	
	if((samp.x <= 0) || (samp.x > g_xres))
		return -1;  /*discard val*/
	
	if((samp.pressure > 0) && (!slide_st)){
		/*press on tscreen*/
		memcpy(&pre_samp, &samp, sizeof(struct ts_sample));
		slide_st = 1;
	}
	if(samp.pressure <= 0){
		/*release the tscreen*/
		if(slide_st){
			slide_st = 0;
			memcpy(&rls_samp, &samp, sizeof(struct ts_sample));
			pev->type = INPUT_TYPE_TSC;
			memcpy(&pev->time, &rls_samp.tv, sizeof(struct timeval));
			ts_delta = rls_samp.x - pre_samp.x;
			if(ts_delta > r_slide)
				/*right slide next page*/
				pev->val = INPUT_VAL_DOWN;
			else if(ts_delta < l_slide)
				pev->val = INPUT_VAL_UP;
			else
				pev->val = INPUT_VAL_UNKNOWN;
			
			return 0;
		}else
			return -1;
	}
		
		return -1;
#elif defined(CONFIG_INPUT_THREAD) || defined(CONFIG_INPUT_SLIP)

#endif 


#endif 

}

/*******************************************************************************
* @function name: is_out_of_time    
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
static int is_out_of_time(struct timeval *pre_time, struct timeval *now_time, int dtime_ms)
{
	int pre_ms, now_ms;
	pre_ms = (pre_time->tv_sec * 1000) + (pre_time->tv_usec / 1000);
	now_ms = (now_time->tv_sec * 1000) + (now_time->tv_usec / 1000);
	return (now_ms >= (pre_ms + dtime_ms));

}



