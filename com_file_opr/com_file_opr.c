/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: com_file_opr.c 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-1-10    
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
#include "com_file_opr.h"
#include "com_file_opr.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*******************************************************************************
* @global static variable:	   
*					 
*******************************************************************************/



/*******************************************************************************
* @global static func:	   
*					 
*******************************************************************************/
static int is_dir(char *path, char *filename);
static int is_regular_file(char *path, char *filename);




/*******************************************************************************
* @function name: map_file    
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
int map_file_ro(struct common_file *fobj)
{
	FILE *fp;
	int fd;
	struct stat fst;
	__u8 *mem;
	if(!fobj){
		perror("null pointer error! \n");
		return -1;
	}
	fp = fopen(fobj->pathname, "r+");
	if(!fp){
		perror("can't open this file! \n");
		return -1;
	}
	fobj->fhld = fp;
	fd = fileno(fobj->fhld);
	
	if(fstat(fd, &fst) == -1){
		perror("can't get file state! \n");
		return -1;
	}
	fobj->file_size = fst.st_size;
	mem = (__u8 *)mmap(NULL , fobj->file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if( mem == MAP_FAILED){
		perror("can't get mem now! \n");
		return -1;
	}
	fobj->fmem = mem;
	return 0;

}

/*******************************************************************************
* @function name: unmap_file    
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
int unmap_file(struct common_file *fobj)
{
	if(!fobj){
		perror("null pointer error! \n");
		return -1;
	}
	munmap(fobj->fmem, fobj->file_size);
	fclose(fobj->fhld);
	return 0;
	
}


/*******************************************************************************
* @function name: list_dir_cont    
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
int list_dir_cont(struct common_file *fobj, int *file_nr)
{
	struct dirent **dir_name_ls;
	struct subfile_idx **file_arr;
	int nr, i, j;

	nr = scandir(fobj->pathname, &dir_name_ls, 0, alphasort);
	if(nr < 0){
		perror("scandir  error! \n");
		return -1;
	}
	file_arr = malloc(sizeof(subfile_idx *) * (nr -2)); /*exclude .  ..*/
	if(!file_arr){
		perror("malloc  error! \n");
		return -1;
	}
	fobj->dir_lst = file_arr; 
	for(i = 0; i < (nr - 2); i++){
		file_arr[i] = malloc(sizeof(subfile_idx));
		if(!file_arr[i]){
			perror("malloc  error! \n");
			return -1;
		}
	}

	for (i = 0, j = 0; i < nr; i++)
	{
		 /*exclude .  ..*/
		if ((0 == strcmp(file_arr[i]->filename, ".")) || (0 == strcmp(file_arr[i]->filename, "..")))
			continue;
        if (is_dir(fobj->pathname, file_arr[i]->filename))
		{
			strncpy(file_arr[j]->filename, dir_name_ls[i]->d_name, 256);
			file_arr[j]->filename[255] = '\0';
			file_arr[j]->type		   = TYP_DIR;
            free(dir_name_ls[i]);
            dir_name_ls[i] = NULL;
			j++;
		}
	}

	/* 再把常规文件挑出来存入aptDirContents */
	for (i = 0; i < nr; i++)
	{
        if (!dir_name_ls[i])
            continue;
		/*exclude .  ..*/
		if ((0 == strcmp(dir_name_ls[i]->d_name, ".")) || (0 == strcmp(dir_name_ls[i]->d_name, "..")))
			continue;
        if (is_regular_file(fobj->pathname, file_arr[i]->filename)){
			strncpy(file_arr[j]->filename, dir_name_ls[i]->d_name, 256);
			file_arr[j]->filename[255] = '\0';
			file_arr[j]->type		   = TYP_FILE;
            free(dir_name_ls[i]);
			dir_name_ls[i] = NULL;
			j++;
		}
	}

	/* 释放file_arr中未使用的项 */
	for (i = j; i < nr - 2; i++)
		free(file_arr[i]);

	/* 释放scandir函数分配的内存 */
	for (i = 0; i < nr; i++)
        if (dir_name_ls[i])
    		free(dir_name_ls[i]);
		
	free(dir_name_ls);

	fobj->subfile_cnt = j;
	return 0;
}


/*******************************************************************************
* @function name: clear_dir_list    
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
int clear_dir_list(struct common_file *fobj)
{
	int i;
	if(!fobj)
		return 0;
	
	for (i = 0; i < fobj->subfile_cnt; i++)
		free(fobj->dir_lst[i]);
	
	free(fobj->dir_lst);
	return 0;


}



/*******************************************************************************
* @function name: recur_ls_file_in_dir    
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
// int recur_ls_file_in_dir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);


/*******************************************************************************
* @function name: is_dir    
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
static int is_dir(char *path, char *filename)
{
	char tmp_str[256];
    struct stat dstat;

    snprintf(tmp_str, 256, "%s/%s", path, filename);
    tmp_str[255] = '\0';

    if ((stat(tmp_str, &dstat) == 0) && S_ISDIR(dstat.st_mode))
  		return 1;
    else
        return 0;
	return 0;
}


/*******************************************************************************
* @function name: is_regular_file    
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
static int is_regular_file(char *path, char *filename)
{
	char tmp_str[256];
    struct stat fstat;

    snprintf(tmp_str, 256, "%s/%s", path, filename);
    tmp_str[255] = '\0';

    if ((stat(tmp_str, &fstat) == 0) && S_ISREG(fstat.st_mode))
  		return 1;
    else
        return 0;
	return 0;
}


