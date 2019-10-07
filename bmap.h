/************************************************************************


			bmap.h

Copyright (c) Kuninori Morimoto <morimoto.kuninori@renesas.com>

2009/09  morimoto
************************************************************************/

#ifndef _BMAP_H_
#define _BMAP_H_

#include <linux/fb.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#define u32 unsigned int
#define u16 unsigned short
#define u8  unsigned char

//=====================================
//
//	version
//
//=====================================
#define VERSION "0.0.2"  // see VERSION file for detail

//=====================================
//
//	BitMap
//
//=====================================
#pragma pack(2)
struct BitMapInfo {
	u16 type;
	u32 size;
	u32 reserved;
	u32 offset;
};

struct BitMapHeader {
	u32  size;
	u32  width;
	u32  height;
	u16  planes;
	u16  bit_count;
	u32  compression;
	u32  image_size;
	long x_pels_per_meter;
	long y_pels_per_meter;
	u32  colors_used;
	u32  colors_imported;
};

struct BitMap {
	int id;
	size_t size;
	struct BitMapInfo   *pinfo;
	struct BitMapHeader *phead;
	void (*getcolor)(struct BitMap *pBit, u8 *pRed, u8 *pGreen, u8 *pBlue);
	uintptr_t map;
	u32 seek;
};

void CloseBitMap(struct BitMap *pBit);
struct BitMap *OpenBitMap(const char *pFile);
const u8 *GetBitMapLine(int y, struct BitMap *pBit);
bool BitMapSeek(u32 nY, struct BitMap *pBit);

//=====================================
//
//	Frame Buffer
//
//=====================================

void CloseFrameBuffer(void);
bool OpenFrameBuffer(const char *pFrame);
void DrawBitMap(struct BitMap *pBit, u32 startx, u32 starty);

#endif /* _BMAP_H_ */
