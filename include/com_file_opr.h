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
 * �������ƣ� GetFilesIndir
 * ���������� ��������ȵķ�ʽ���Ŀ¼�µ��ļ� 
 *            ��: �Ȼ�ö���Ŀ¼�µ��ļ�, �ٽ���һ����Ŀ¼A
 *                �ٻ��һ����Ŀ¼A�µ��ļ�, �ٽ��������Ŀ¼AA, ...
 *                ������һ����Ŀ¼A��, �ٽ���һ����Ŀ¼B
 *
 * "����ģʽ"�µ��øú������Ҫ��ʾ���ļ�
 * �����ַ��������Щ�ļ�:
 * 1. ���Ȱ������ļ������ֱ��浽ĳ����������
 * 2. �õ�ʱ��ȥ����ȡ�����ɸ��ļ���
 * ��1�ַ����Ƚϼ�,���ǵ��ļ��ܶ�ʱ�п��ܵ����ڴ治��.
 * ����ʹ�õ�2�ַ���:
 * ����ĳĿ¼(����������Ŀ¼)�����е��ļ���������һ����
 *
 * ���������strDirName            : Ҫ����ĸ�Ŀ¼�µ����� 
 *           piStartNumberToRecord : �ӵڼ����ļ���ʼȡ�����ǵ�����
 *           iFileCountTotal       : �ܹ�Ҫȡ�����ٸ��ļ�������
 * ���������piFileCountHaveGet    : �Ѿ��õ��˶��ٸ��ļ�������
 *           apstrFileNames[][256] : �����洢���������ļ���
 * ���/���������
 *           piCurFileNumber       : ��ǰ���������ļ����
 * �� �� ֵ��0 - �ɹ�
 *           1 - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
// extern int recur_ls_file_in_dir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);



#endif /* ifndef _COM_FILE_OPR_H_.2016-1-10 ���� 5:56:50 zcz */

