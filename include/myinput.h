/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: myinput.h 
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
#ifndef _MYINPUT_H_
#define _MYINPUT_H_
#include "ui_config.h"
#include <pthread.h>
#include <linux/types.h>
#include <sys/time.h>


/*****macro for input_ev*****************************/
#define INPUT_TYPE_STDIN        0
#define INPUT_TYPE_TSC  		1

#define INPUT_VAL_UP          	0   
#define INPUT_VAL_DOWN        	1
#define INPUT_VAL_EXIT       	2
#define INPUT_VAL_UNKNOWN      -1

typedef struct raw_input_val{
	struct timeval tval;   /* 发生这个输入事件时的时间 */
	int type;  /* 类别: stdin, touchsceen */
	int x;     /* X/Y座标 */
	int y;
	int key;   /* 按键值 */
	int pressure; /* 压力值 */
}raw_input_val;


typedef struct input_ev {
	struct timeval time;
	int type;  /* stdin, touchsceen */
	int val;   /*  */
}input_ev;

/************************************/
typedef union input_method{
#ifdef CONFIG_INPUT_QUERY
				int foo; /*no use only for fill*/
#endif 
#ifdef CONFIG_INPUT_SELECT
				int fd;
#endif 
#ifdef CONFIG_INPUT_THREAD

				pthread_t tid;
#endif 
}input_method;

typedef struct input_dev{
	char *name;
	union input_method mthd;
	int (*init_dev)(void);
	int (*exit_dev)(void);
	int (*get_input_ev)(struct input_ev *pev);
	int (*get_input_raw_val)(struct raw_input_val *pval);
	struct input_dev *next;
}input_dev;


extern int load_input_md(void);
extern int register_input_md(struct input_dev *pdev);
extern void print_input_md_lst(void);
extern int get_rt_input_ev(struct input_ev *pev);
extern int get_rt_raw_input_val(struct raw_input_val *pval);


/*en and disable dev*/
#define DEFAULT_DEV_LIST {"touchscreen",NULL}
extern int enable_input_dev_set(char *dev_ls[]);
extern int disable_input_dev_set(char *dev_ls[]); 




#endif /* ifndef _MYINPUT_H_.2015-12-9 下午 9:43:57 zcz */


















