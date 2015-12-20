/*******************************************************************************
* Copyright (C), 2000-2015,  Electronic Technology Co., Ltd.
*                
* @filename: stdin_dev.c 
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
#include <termios.h>
#include <unistd.h>

#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>



/*******************************************************************************
* @global static func:     
*                    
*******************************************************************************/
static int init_stdin_dev(void);
static int exit_stdin_dev(void);
static int get_stdin_dev_ev(struct input_ev *pev);


/*******************************************************************************
* @global static variable:     
*                    
*******************************************************************************/
static struct input_dev stdin_dev = {
			.name 		= "stdin_dev",
			.init_dev 	= init_stdin_dev,
			.exit_dev	= exit_stdin_dev,
			.get_input_ev = get_stdin_dev_ev,
};


/*******************************************************************************
* @function name: load_stdin_dev_md    
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
int load_stdin_dev_md(void)
{
	return  register_input_md(&stdin_dev);
}


/*******************************************************************************
* @function name: init_stdin_dev    
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
int init_stdin_dev(void)
{
#ifdef CONFIG_INPUT_SELECT
	struct termios tty_stat;
	printf("init input dev: %s! \n",stdin_dev.name);
	tcgetattr(STDIN_FILENO, &tty_stat);
	tty_stat.c_lflag &= ~ICANON; /*non-canon input model*/
	tty_stat.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_stat);
	stdin_dev.mthd.fd = STDIN_FILENO;
	return 0;		
#else
	struct termios tty_stat;
	printf("init input dev: %s! \n",stdin_dev.name);
	tcgetattr(STDIN_FILENO, &tty_stat);
	tty_stat.c_lflag &= ~ICANON; /*non-canon input model*/
	tty_stat.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_stat);
	return 0;		
#endif 



}
/*******************************************************************************
* @function name: exit_stdin_dev    
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
int exit_stdin_dev(void)
{
	struct termios tty_stat;
	tcgetattr(STDIN_FILENO, &tty_stat);
	tty_stat.c_lflag |= ~ICANON; /*reset canon input model*/
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_stat);
	return 0;

}
/*******************************************************************************
* @function name: get_stdin_dev_ev    
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
int get_stdin_dev_ev(struct input_ev *pev)
{
#if	defined(CONFIG_INPUT_QUERY) || defined(CONFIG_INPUT_SELECT) 
	struct timeval tm_out;
	int ret;
	fd_set stdfd;
	char c;
	tm_out.tv_sec  = 0;
	tm_out.tv_usec = 0;
	FD_ZERO(&stdfd);
	FD_SET(STDIN_FILENO,&stdfd);
	ret = select(STDIN_FILENO + 1, &stdfd, NULL, NULL, &tm_out);
	if(ret == -1){
		perror("Error occur when select stdin_dev!");
		return -1;
	}
	if(FD_ISSET(STDIN_FILENO,&stdfd)){
		pev->type = INPUT_TYPE_STDIN; 
		gettimeofday(&pev->time, NULL);
		c = fgetc(stdin);
		switch(c){
			case 'u':
					pev->val = INPUT_VAL_UP;
					break;
			case 'n':
					pev->val = INPUT_VAL_DOWN;
					break;
			case 'q':
					pev->val = INPUT_VAL_EXIT;
					break;
			default:
					pev->val = INPUT_VAL_EXIT;;
					break;
		}
		return 0;
		
	}
	else
		return -1;	
#elif defined(CONFIG_INPUT_THREAD) || defined(CONFIG_INPUT_SLIP)

#endif 

}


