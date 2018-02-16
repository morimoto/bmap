/*************************************************************************

                                   main

Copyright (c) Kuninori Morimoto <morimoto.kuninori@renesas.com>

2009/09  morimoto
*************************************************************************/
#include <stdio.h>
#include "bmap.h"

//=====================================
//
//          usage
//
//=====================================
static void usage( void )
{
    printf( "bmap version %s\n"
            "usage: bmap [-x arg][-y arg][device] file\n"
            "\n"
            "-x    : start position of x\n"
            "-y    : start position of y\n"
            "device: /dev/fbX\n"
            "file  : bitmap file\n",
            VERSION );
}

//=====================================
//
//          main
//
//=====================================
int main( int argc, char *argv[] )
{
    struct BitMap *pbit = NULL;
    int ch;
    u32 x = 0;
    u32 y = 0;
    const char *dev = "/dev/fb0";

    //----------------------
    // check option
    //----------------------
    while ( -1 != (ch = getopt(argc, argv, "x:y:"))) {
        switch (ch){
        case 'x': x = (u32)strtoul( optarg, NULL, 10 ); break;
        case 'y': y = (u32)strtoul( optarg, NULL, 10 ); break;
        default:
            usage( );
            exit ( 1 );
        }
    }

    argc -= optind;
    argv += optind;

    //----------------------
    // device name
    //----------------------
    if( 2 == argc ) {
        dev = argv[ 0 ];
        argv++;
    }

    if ( !OpenFrameBuffer( dev ))
        goto end;

    pbit = OpenBitMap( argv[ 0 ] );
    if ( !pbit )
        goto end;

    DrawBitMap( pbit , x , y );

    CloseBitMap( pbit );
 end:
    CloseFrameBuffer(  );

    return 0;
}
