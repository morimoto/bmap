/************************************************************************


                               frame.c

Copyright (c) Kuninori Morimoto <morimoto.kuninori@renesas.com>

2009/09  morimoto
************************************************************************/
#include "bmap.h"

//=====================================
//
//          struct
//
//=====================================
struct FramBuffer {
    int id;
    size_t size;
    u32 map;
    u32 seek;
    struct fb_var_screeninfo info;
    void (*drawcolor)( struct BitMap *pBit );
};

struct FramBuffer g_FB;

//=====================================
//
//          drawcolor
//
//=====================================
#define RGB16(r, g, b) ((u16)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3)))

static void drawcolor16( struct BitMap *pBit )
{
    u8 r, g, b;

    pBit->getcolor( pBit , &r , &g , &b );

    *((u16*)(g_FB.map + g_FB.seek)) = RGB16( r , g , b );

    g_FB.seek += 2;
}

static void drawcolor24( struct BitMap *pBit )
{
    u8 r, g, b;

    pBit->getcolor( pBit , &r , &g , &b );

    *((u8*)(g_FB.map + g_FB.seek++)) = r;
    *((u8*)(g_FB.map + g_FB.seek++)) = g;
    *((u8*)(g_FB.map + g_FB.seek++)) = b;
}

static void drawcolor32( struct BitMap *pBit )
{
    u8 r, g, b;

    pBit->getcolor( pBit , &r , &g , &b );

    *((u8*)(g_FB.map + g_FB.seek++)) = r;
    *((u8*)(g_FB.map + g_FB.seek++)) = g;
    *((u8*)(g_FB.map + g_FB.seek++)) = b;
    g_FB.seek++;
}

//=====================================
//
//          frambufferseek
//
//=====================================
static void frambufferseek ( u32 x, u32 y )
{
    u32 inc = g_FB.info.bits_per_pixel / 8;

    g_FB.seek =
        ( inc * x ) +
        ( inc * y * g_FB.info.xres_virtual );
}

/*======================================================================


                      global function


======================================================================*/
//=====================================
//
//          DrawBitMap
//
//=====================================
void DrawBitMap(struct BitMap *pBit , u32 startx, u32 starty )
{
    u32 height, width;
    u32 x, y;

    if (( startx < 0 ) ||
        ( starty < 0 ) ||
        ( startx > g_FB.info.xres_virtual ) ||
        ( starty > g_FB.info.yres_virtual )) {
        printf( "start position error\n"
                "frame buffer size is %d x %d\n",
                g_FB.info.xres_virtual,
                g_FB.info.yres_virtual );
        return ;
    }

    //----------------------
    // height
    //----------------------
    height = g_FB.info.yres_virtual;
    if ( pBit->phead->height < height )
        height = pBit->phead->height;

    if (( starty + height ) >
        g_FB.info.yres_virtual )
        height = g_FB.info.yres_virtual - starty;

    //----------------------
    // width
    //----------------------
    width = g_FB.info.xres_virtual;
    if ( pBit->phead->width < width )
        width = pBit->phead->width;

    if (( startx + width ) >
        g_FB.info.xres_virtual )
        width = g_FB.info.xres_virtual - startx;

    //----------------------
    // draw
    //----------------------
    for ( y=0 ; y<height ; y++ ) {
        BitMapSeek ( y, pBit );
        frambufferseek( startx, y + starty );
        for ( x=0 ; x<width ; x++ )
            g_FB.drawcolor( pBit );
    }
}

//=====================================
//
//          CloseFrameBuffer
//
//=====================================
void CloseFrameBuffer( void )
{
    if ( g_FB.map )
        munmap( (void*)g_FB.map, g_FB.size );

    if ( g_FB.id > 0 )
        close( g_FB.id );
}

//=====================================
//
//          OpenFrameBuffer
//
//=====================================
bool OpenFrameBuffer( const char *pFrame )
{
    //----------------------
    // frame buffer open
    //----------------------
    g_FB.id = open( pFrame, O_RDWR );
    if( g_FB.id < 0 ) {
        printf( "Error opening framebuffer\n" );
        goto open_err;
    }

    if ( ioctl(g_FB.id, FBIOGET_VSCREENINFO, &g_FB.info)) {
        printf("Error reading variable information.\n");
        goto open_err;
    }

    switch ( g_FB.info.bits_per_pixel ) {
    case 16 : g_FB.drawcolor = drawcolor16; break;
    case 24 : g_FB.drawcolor = drawcolor24; break;
    case 32 : g_FB.drawcolor = drawcolor32; break;
    default:
        printf( "Error frame buffer pixel support\n" );
        goto open_err;
    }

    //----------------------
    // get mmap
    //----------------------
    g_FB.size = g_FB.info.xres_virtual *
                g_FB.info.yres_virtual *
                g_FB.info.bits_per_pixel / 8;

    g_FB.map = (u32)mmap( NULL, g_FB.size,
                          PROT_READ | PROT_WRITE, MAP_SHARED,
                          g_FB.id, 0 );

    if ( MAP_FAILED == (void*)g_FB.map ) {
        g_FB.map = 0;
        printf( "Error frame buffer mmap\n" );
        goto open_err;
    }

    return true;

open_err:
    CloseFrameBuffer( );
    return false;
}
