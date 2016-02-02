/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: main.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-26    
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
#include "config_app.h"
#include <unistd.h>
#include <stdio.h>

/*******************************************************************************
* @global func:	   
*					 
*******************************************************************************/
extern int load_desktop_ui(struct m_ui_obj *ui_hash[], int hash_len, struct m_ui_obj *pui);
static int dtp_server_init(struct ui_server *self,int argc, char **argv);
static void  dtp_server_run(struct ui_server *self);


/*******************************************************************************
* @global variable:	   
*					 
*******************************************************************************/
static struct ui_server dtp_server = {
	.com_attr	= NULL,
	.act_ui		= NULL,
	.ui_hash	= NULL,
	.hash_len	= UI_HASH_LEN,
	.init 		= dtp_server_init,
	.run		= dtp_server_run,
};



/*******************************************************************************
* @global func:	   
*					 
*******************************************************************************/
extern int load_desktop_ui(struct m_ui_obj *ui_hash[], int hash_len, struct m_ui_obj *pui);


/*******************************************************************************
* @function name: dtp_server_init    
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
static int dtp_server_init(struct ui_server *self,int argc, char **argv)
{
	int ret;
	int nr_bytes; 
	nr_bytes = 	sizeof(struct m_ui_obj *) * self->hash_len;
	if(!self)
		return -1;
	self->ui_hash = malloc(nr_bytes);
	if(!self->ui_hash)
		return -1;
	memset(self->ui_hash, 0x00, nr_bytes);
	ret = load_desktop_ui(self->ui_hash, self->hash_len, NULL);
	return ret;

}
/*******************************************************************************
* @function name: dtp_server_run    
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
static void  dtp_server_run(struct ui_server *self)
{
	int ret;
	printf("bf active ui!\n");
	ret = activate_ui(self, "desktop");
	printf("active ui ret is %d \n", ret);
	while(1){
		
		//ret = activate_ui(self, "desktop");	
		//printf("active ui ret is %d\n", ret);
		sleep(100);
	}

}


int main(int argc, char *argv[])
{
	load_init_ui_sys(&dtp_server, argc, argv);
	start_ui_server(&dtp_server, argc, argv);
	while(1)
		sleep(100);
	return 0;
}

