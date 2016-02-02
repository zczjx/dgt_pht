/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: app_ui_server.c 
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

#include "ui_config.h"
#include "ui_frame.h"
#include "app_ui_server.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>


/*******************************************************************************
* @global static variable:	   
*					 
*******************************************************************************/


/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/

/*******************************************************************************
* @function name: load_init_ui_app    
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
int load_init_ui_sys(struct ui_server *server, int argc, char **argv)
{
	int ret;
	char *dev_ls[] = DEFAULT_DEV_LIST;
	ret = load_display_md();
	ret = select_main_scr_dev("fb");
	ret = alloc_disp_buf_mem(CONFIG_ACC_MEM_NR);
	ret = load_input_md();
	ret = enable_input_dev_set(dev_ls);
	ret = load_image_codec ();
	ret = server->init(server, argc, argv);
	return ret;
}
	
/*******************************************************************************
* @function name: start_ui_server    
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
int start_ui_server(struct ui_server *server, int argc, char **argv)
{
	int ret;
	pthread_create(&server->ser_tid, NULL,
					server->run, server);
	
	ret = pthread_join(server->ser_tid, NULL);

	return ret;

}
	
/*******************************************************************************
* @function name: activate_ui    
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
int activate_ui(struct ui_server *server, char *act_ui_name)
{
	int id;
	int hash_len;
	int p_rd_fd[2];
	struct m_ui_obj **ui_hash;
	struct m_ui_obj *tmp_ui;
	if((!server) || (!act_ui_name))
		return -1;
	hash_len = server->hash_len;
	ui_hash  = server->ui_hash;
	id = ui_name_to_hash_id(act_ui_name, hash_len);
	if((id < 0) || (id > hash_len))
		return -1;
	tmp_ui = ui_hash[id];
	if(!tmp_ui)
		return -1;
	
	while(tmp_ui){
		if(strcmp(tmp_ui->name, act_ui_name) == 0){
			if(!server->act_ui){
				/*first startup ui*/
				printf("bf 1st active ui!\n");
				if(pipe(p_rd_fd) < 0)
					return -1;
				server->pfd[SERVER_PFD_RD] = p_rd_fd[0];
				tmp_ui->pfd[UI_PFD_WR]	   = p_rd_fd[1];
				tmp_ui->app_com_attr	   = server->com_attr;
				tmp_ui->state			   = ACTIVE;
				pthread_create(&server->ui_tid, NULL,
								tmp_ui->run,tmp_ui);
				server->act_ui = tmp_ui;
				printf("ui name is %s\n", server->act_ui->name);
				printf("ui_tid: %d, ser_tid: %d\n", server->ui_tid,server->ser_tid);
				return 0;
			}else{	
				printf("bf normal active ui!\n");
				close(server->pfd[SERVER_PFD_RD]);
				close(server->act_ui->pfd[UI_PFD_WR]);
				server->act_ui->state = INACTIVE;
				pthread_join(server->ui_tid, NULL);
				if(pipe(p_rd_fd) < 0)
					return -1;
				server->pfd[SERVER_PFD_RD] = p_rd_fd[0];
				tmp_ui->pfd[UI_PFD_WR]	   = p_rd_fd[1];
				tmp_ui->app_com_attr	   = server->com_attr;
				tmp_ui->state			   = ACTIVE;
				pthread_create(&server->ui_tid, NULL,
								tmp_ui->run,tmp_ui);
				server->act_ui = tmp_ui;
				return 0;
			}

		}
		tmp_ui = tmp_ui->next;
	}
	return -1;

}


