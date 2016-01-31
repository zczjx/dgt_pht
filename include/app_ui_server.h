/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: app_ui_server.h 
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
#ifndef _APP_UI_SERVER_H_
#define _APP_UI_SERVER_H_
#include "ui_config.h"
#include "ui_frame.h"
#include <pthread.h>

#define SERVER_PFD_RD 0
#define SERVER_PFD_WR 1
typedef struct ui_server{
	pthread_t ser_tid;
	void *com_attr;
	int    pfd[2]; /*IPC */
	struct m_ui_obj *act_ui;
	pthread_t 		ui_tid;
	struct m_ui_obj **ui_hash;
	int hash_len;
	int   (*init)(struct ui_server *self,int argc, char **argv);
	void  (*run)(struct ui_server *self);	
} ui_server;

extern int load_init_ui_sys(struct ui_server *server, int argc, char **argv);

extern int start_ui_server(struct ui_server *server, int argc, char **argv);

extern int activate_ui(struct ui_server *server, char *act_ui_name);




#endif /* ifndef _APP_UI_SERVER_H_.2016-1-5 ÏÂÎç 10:31:22 zcz */















