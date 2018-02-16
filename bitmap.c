/************************************************************************


                             bitmap.c

Copyright (c) Kuninori Morimoto <morimoto.kuninori@renesas.com>

2009/09  morimoto
************************************************************************/
#include "bmap.h"

//======================================================================
//
//
//                      static function
//
//
//======================================================================
static u32 get_bitmapsize( const char *pFile )
{
    FILE *fp;
    struct BitMapInfo info;
    struct BitMapHeader head;
    u32 ret = 0;
    u8 *type;

    fp = fopen( pFile, "rb" );
    if ( !fp )
        return ret;

    if ( 1 != fread( &info, sizeof(info), 1, fp))
        goto size_err;

    if ( 1 != fread( &head, sizeof(head), 1, fp))
        goto size_err;

    type = (u8*)&info.type;
    if ( type[1] != 'M' || type[0] != 'B' )
        goto size_err;

    switch ( head.bit_count ) {
    case 16:
    case 24:
    case 32:
        break;
    default:
        goto size_err;
    }

    ret = info.size;

size_err:
    fclose( fp );
    return ret;
}

//=====================================
//
//          getcolor
//
//=====================================
static void getcolor16( struct BitMap *pBit ,
                        u8 *pRed , u8 *pGreen , u8 *pBlue )
{
    u16 rgb = *((u16*)(pBit->map + pBit->seek));

    pBit->seek += 2;

    *pRed   = (u8)((rgb & 0xF800) >> 8);
    *pGreen = (u8)((rgb & 0x07E0) >> 3);
    *pBlue  = (u8)((rgb & 0x001F) << 3);
}

static void getcolor24( struct BitMap *pBit ,
                        u8 *pRed , u8 *pGreen , u8 *pBlue )
{
    u8 *rgb = (u8*)(pBit->map + pBit->seek);

    pBit->seek += 3;

    *pRed   = rgb[2];
    *pGreen = rgb[1];
    *pBlue  = rgb[0];
}

static void getcolor32( struct BitMap *pBit ,
                        u8 *pRed , u8 *pGreen , u8 *pBlue )
{
    u8 *rgb = (u8*)(pBit->map + pBit->seek);

    pBit->seek += 4;

    *pRed   = rgb[3];
    *pGreen = rgb[2];
    *pBlue  = rgb[1];
}

//======================================================================
//
//
//                      global function
//
//
//======================================================================
bool BitMapSeek( u32 nY , struct BitMap *pBit )
{
    u32 pitch;
    u32 line;

    if ( !pBit )
        return false;

    pitch  = pBit->pinfo->size - pBit->pinfo->offset;
    pitch /= pBit->phead->height;

    if (pBit->phead->height > 0)
        line = pBit->phead->height - nY - 1;
    else
        line = nY;

    pBit->seek = pBit->pinfo->offset + (line * pitch);
    return true;
}

//=====================================
//
//          CloseBitMap
//
//=====================================
void CloseBitMap( struct BitMap *pBit )
{
    if ( !pBit )
        return;

    if ( pBit->map )
        munmap( (void*)pBit->map, pBit->size );

    if ( pBit->id > 0 )
        close( pBit->id );

    free ( pBit );

}

//=====================================
//
//          OpenBitMap
//
//=====================================
struct BitMap* OpenBitMap( const char *pFile )
{
    struct BitMap *pret;

    pret = (struct BitMap *)malloc( sizeof(struct BitMap) );
    if ( !pret ) {
        printf( "Error malloc\n" );
        return NULL;
    }

    //----------------------
    // get bitmap file size
    //----------------------
    pret->size = get_bitmapsize( pFile );
    if ( 0 == pret->size ) {
        printf( "Error can not get bitmap info\n" );
        goto open_err;
    }

    //----------------------
    // frame buffer open
    //----------------------
    pret->id = open( pFile, O_RDONLY );
    if( pret->id < 0 ) {
        printf( "Error opening bitmap\n" );
        goto open_err;
    }

    //----------------------
    // get mmap
    //----------------------
    pret->map = (uintptr_t)mmap( NULL, pret->size,
                           PROT_READ, MAP_SHARED,
                           pret->id, 0 );

    if ( MAP_FAILED == (void*)pret->map ) {
        pret->map = 0;
        printf( "Error bitmap mmap\n" );
        goto open_err;
    }

    pret->pinfo = (struct BitMapInfo *)pret->map;
    pret->phead = (struct BitMapHeader *)(pret->map + sizeof(struct BitMapInfo));

    switch ( pret->phead->bit_count ) {
    case 16 : pret->getcolor = getcolor16; break;
    case 24 : pret->getcolor = getcolor24; break;
    case 32 : pret->getcolor = getcolor32; break;
    default:
        printf( "Error un supported bitmap\n" );
        goto open_err;
    }

    return pret;

open_err:
    CloseBitMap( pret );
    return NULL;
}
