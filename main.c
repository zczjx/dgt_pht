/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

#include <fb_show_font.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H



#define WIDTH   800
#define HEIGHT  480


/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];


/* Replace this function with something useful. */

void
draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;


  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= WIDTH || j >= HEIGHT )
        continue;

      image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    }
  }
}


void
show_image( void )
{
  int  i, j;
  int fd_fb;
  struct lcd_scr mylcd;
  fd_fb = open("/dev/fb0", O_RDWR);
	if (fd_fb < 0){
		printf("can't open /dev/fb0\n");
		return -1;
	}
	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var)){
		printf("can't get var-info of screen! \n");
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix)){
		printf("can't get fix-info of screen\n");
		return -1;
	}
	printf("var.xres = %d,var.yres = %d\n",var.xres,var.yres);
	mylcd.line_bytes = GET_BYTES_OF_LINE(var);
	mylcd.pixel_bytes = GET_BYTES_OF_PIXEL(var);
	mylcd.scr_bytes = GET_BYTES_OF_SCR(var);
	mylcd.fbmem = (unsigned char *)mmap(NULL, mylcd.scr_bytes,
										PROT_READ | PROT_WRITE, MAP_SHARED,
										fd_fb, 0);
	if(mylcd.fbmem < 0){
		printf("can't mmap\n");
		return -1;
	}
	CLR_LCD_FB(mylcd.fbmem,mylcd.scr_bytes);

  for ( i = 0; i < HEIGHT; i++ )
  {
	for ( j = 0; j < WIDTH; j++ )
		if (image[i][j])
		put_lcd_pxl(&mylcd,j, i, COLOR_RED);
		else
		put_lcd_pxl(&mylcd,j, i, COLOR_BLACK);
	
 
  }
}


int
main( int     argc,
      char**  argv )
{
  FT_Library    library;
  FT_Face       face;

  FT_GlyphSlot  slot;
  FT_Matrix     matrix;                 /* transformation matrix */
  FT_Vector     pen;                    /* untransformed origin  */
  FT_Error      error;

  char*         filename;
  char*         text;

  double        angle;
  int           target_height;
  int           n, num_chars;
  wchar_t *chstr = L"Clarence.Chow Love Sharping";
  FT_BBox bbox;
  FT_Glyph  glyph;

  if ( argc != 3 )
  {
    fprintf ( stderr, "usage: %s font sample-text\n", argv[0] );
    exit( 1 );
  }

  filename      = argv[1];                           /* first argument     */
  text          = argv[2];                           /* second argument    */
  num_chars     = strlen( text );
  angle         = ( 0 / 360 ) * 3.14159 * 2;      /* use 25 degrees     */
  target_height = HEIGHT;

  error = FT_Init_FreeType( &library );              /* initialize library */
  /* error handling omitted */

  error = FT_New_Face( library, argv[1], 0, &face ); /* create face object */
  /* error handling omitted */

#if 0
  /* use 50pt at 100dpi */
  error = FT_Set_Char_Size( face, 50 * 64, 0,
                            100, 0 );                /* set character size */
#endif
	FT_Set_Pixel_Sizes(face, 24, 0);
  /* error handling omitted */

  slot = face->glyph;

  /* set up matrix */
  matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

  /* the pen position in 26.6 cartesian space coordinates; */
  /* start at (0,40) relative to the upper left corner  */
  pen.x = 0 * 64;
  pen.y = ( target_height - 64 ) * 64;

  for ( n = 0; n < wcslen(chstr); n++ )
  {
    /* set transformation */
    FT_Set_Transform( face, &matrix, &pen );

    /* load glyph image into the slot (erase previous one) */
    error = FT_Load_Char( face, chstr[n], FT_LOAD_RENDER );
    if ( error )
      continue;                 /* ignore errors */

	error = FT_Get_Glyph( face->glyph, &glyph );
	if (error)
	{
		printf("FT_Get_Glyph error!\n");
		return -1;
	}

    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_TRUNCATE, &bbox );
    /* now, draw to our target surface (convert position) */
    draw_bitmap( &slot->bitmap,
                 slot->bitmap_left,
                 target_height - slot->bitmap_top );
	printf("Unicode: 0x%x\n", chstr[n]);
	printf("origin.x/64 = %d, origin.y/64 = %d\n", pen.x/64, pen.y/64);
	printf("xMin = %d, xMax = %d, yMin = %d, yMax = %d\n", bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);
    printf("slot->advance.x/64 = %d, slot->advance.y/64 = %d\n", slot->advance.x/64, slot->advance.y/64);

    /* increment pen position */
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }

  show_image();

  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  return 0;
}

/* EOF */




