//
// Copyright (c) 2004 K. Wilkins
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

//////////////////////////////////////////////////////////////////////////////
//                       Handy - An Atari Lynx Emulator                     //
//                          Copyright (c) 1996,1997                         //
//                                 K. Wilkins                               //
//////////////////////////////////////////////////////////////////////////////
// ROM emulation class                                                      //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This class emulates the system ROM (512B), the interface is pretty       //
// simple: constructor, reset, peek, poke.                                  //
//                                                                          //
//    K. Wilkins                                                            //
// August 1997                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// Revision History:                                                        //
// -----------------                                                        //
//                                                                          //
// 01Aug1997 KW Document header added & class documented.                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define ROM_CPP

//#include <crtdbg.h>
//#define   TRACE_ROM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "rom.h"
#include <psp2/types.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>

CRom::CRom(const char *romfile)
{
	mWriteEnable = false;
	mValid = false;
	strncpy(mFileName,romfile,1024);
	Reset();

	// Initialise ROM
	for(int loop=0;loop<ROM_SIZE;loop++) mRomData[loop]=DEFAULT_ROM_CONTENTS;

	// Load up the file

	SceUID	fp;

	if((fp=sceIoOpen(mFileName,SCE_O_RDONLY,0777))<=0)
	{
		printf( "Invalid ROM.\n");
		return;
	}else{
		printf( "ROM valid : %d",fp);
	}

	// Read in the 512 bytes
	if(sceIoRead(fp,mRomData,ROM_SIZE)!=ROM_SIZE)
	{
		printf( "Invalid ROM.\n");
		sceIoClose(fp);
		return;
	}

	sceIoClose(fp);

	// Check the code that has been loaded and report an error if its a
	// fake version of the bootrom

	UBYTE mRomCheck[16]={0x38,0x80,0x0A,0x90,0x04,0x8E,0x8B,0xFD,
						 0x18,0xE8,0x8E,0x87,0xFD,0xA2,0x02,0x8E};

	static bool firsttime=true;

	if(firsttime)
	{
		firsttime=false;
		for(ULONG loop=0;loop<16;loop++)
		{
			if(mRomCheck[loop]!=mRomData[loop])
			{
				/*mSystem.gError->Warning("FAKE LYNXBOOT.IMG - CARTRIDGES WILL NOT WORK\n\n"
								"PLEASE READ THE ACCOMPANYING README.TXT FILE\n\n"
								"(Do not email the author asking for this image)\n");*/
				return;
			}
		}
	}

	mValid = true;
}

void CRom::Reset(void)
{
	// Nothing to do here
}

bool CRom::ContextSave(FILE *fp)
{
	if(!fprintf(fp,"CRom::ContextSave")) return 0;
	if(!fwrite(mRomData,sizeof(UBYTE),ROM_SIZE,fp)) return 0;
	return 1;
}

bool CRom::ContextLoad(LSS_FILE *fp)
{
	char teststr[100]="XXXXXXXXXXXXXXXXX";
	if(!lss_read(teststr,sizeof(char),17,fp)) return 0;
	if(strcmp(teststr,"CRom::ContextSave")!=0) return 0;

	if(!lss_read(mRomData,sizeof(UBYTE),ROM_SIZE,fp)) return 0;
	return 1;
}


//END OF FILE
