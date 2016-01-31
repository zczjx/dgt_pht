/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: com_file_opr.h 
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
#ifndef _COM_FILE_OPR_H_
#define _COM_FILE_OPR_H_
#include <stdio.h>
#include <linux/types.h>

typedef enum FILE_TYP {
	TYP_FILE = 0,    
	TYP_DIR  = 1,     
} FILE_TYP;

typedef struct subfile_idx {
	char filename[256];  
	enum FILE_TYP type;  
} subfile_idx;

/*common file : file or directory*/
typedef struct common_file {
	char pathname[256]; 
	enum FILE_TYP file_typ;
	FILE *fhld; 
	int  file_size;
	__u8 *fmem;
	int subfile_cnt;
	struct subfile_idx **dir_lst;
} common_file;

extern int map_file_ro(struct common_file *fobj);

extern int unmap_file(struct common_file *fobj);

extern int list_dir_cont(struct common_file *fobj, int *file_nr);

extern int clear_dir_list(struct common_file *fobj);

/**********************************************************************
 * 函数名称： GetFilesIndir
 * 功能描述： 以深度优先的方式获得目录下的文件 
 *            即: 先获得顶层目录下的文件, 再进入一级子目录A
 *                再获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *                处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先把所有文件的名字保存到某个缓冲区中
 * 2. 用到时再去搜索取出若干个文件名
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 *
 * 输入参数：strDirName            : 要获得哪个目录下的内容 
 *           piStartNumberToRecord : 从第几个文件开始取出它们的名字
 *           iFileCountTotal       : 总共要取出多少个文件的名字
 * 输出参数：piFileCountHaveGet    : 已经得到了多少个文件的名字
 *           apstrFileNames[][256] : 用来存储搜索到的文件名
 * 输出/输出参数：
 *           piCurFileNumber       : 当前搜索到的文件编号
 * 返 回 值：0 - 成功
 *           1 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
// extern int recur_ls_file_in_dir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);



#endif /* ifndef _COM_FILE_OPR_H_.2016-1-10 下午 5:56:50 zcz */

