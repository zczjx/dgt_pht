#include "config.h"
#include "book_engine.h"
#include "encode.h"
#include "font.h"
#include "display.h"
#include "myinput.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define _DEF_PATH_NAME_  "./testbook.txt"
#define _DEF_FONT_NAME_  "freetype"
#define _DEF_FONT_SZ_	 32



/* ./app [-s Size] [-l] [-f font_name] [-b book_path] */
int main(int argc, char *argv[])
{
	int   ret;
	struct book_dsc mybook;
	char  *path_name = NULL;
	char  *font_name = NULL;
	__u32 font_size  = 0;
	int bList = 0;
	while ((ret = getopt(argc, argv, "ls:b:f:")) != -1){
		switch(ret){
			case 'l':
				  bList = 1;
				  break;
			case 's':
				  font_size = strtoul(optarg, NULL, 0);
				  break;
			case 'b':
				  path_name = (char *)malloc(128 * sizeof(char));
				  if(!path_name){
					perror("no mem to use!\n");
					return -1;
				  }
				  strncpy(path_name, optarg, 128);
				  path_name[127] = '\0';
				  break;		
			case 'f':
					font_name = (char *)malloc(128 * sizeof(char));
					if(!font_name){
						perror("no mem to use!\n");
						return -1;
				  	}
					strncpy(font_name, optarg, 128);
					font_name[127] = '\0';
					break;
			default:
					printf("Usage: %s app [-s Size] [-l] [-f font_name] [-b book_path]\n", argv[0]);
					printf("Usage: %s -l\n", argv[0]);
					return -1;
					break;
		}
	}

	if(!path_name)
		init_book_load_sys(&mybook, _DEF_PATH_NAME_);	
	else
		init_book_load_sys(&mybook, path_name);	

	if((!font_name) || (!font_size))
		set_book_text_attr(&mybook, _DEF_FONT_NAME_, _DEF_FONT_SZ_);
	else
		set_book_text_attr(&mybook, font_name, font_size);
	set_book_disp_attr(&mybook, "fb", 0, 0);
	if (bList){
		printf("supported display:\n");
		print_disp_md_lst();
		printf("supported font:\n");
		print_font_md_lst();
		printf("supported encoding:\n");
		print_encode_md_lst();
		printf("supported input:\n");
		print_input_md_lst();
		return 0;
	}
	ret = mybook.show_next_pg(&mybook);
	if (ret){
		printf("Error to show first page\n");
		return -1;
	}
	printf("at show first page done!\n");
	printf("Enter 'n' to show next page, 'u' to show previous page, 'q' to exit: ");
	while (1){
		if (0 == mybook.get_rt_input_ev(&mybook.rt_ev)){
			if (mybook.rt_ev.val== INPUT_VAL_DOWN)
				mybook.show_next_pg(&mybook);
			else if (mybook.rt_ev.val == INPUT_VAL_UP )
				mybook.show_pre_pg(&mybook);		
			else if (mybook.rt_ev.val == INPUT_VAL_EXIT)
				return 0;
		}
	}
	return 0;
}




