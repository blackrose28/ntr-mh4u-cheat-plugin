#include "global.h"

#define WRITEU8(addr, data) *(vu8*)(addr) = data
#define WRITEU16(addr, data) *(vu16*)(addr) = data
#define WRITEU32(addr, data) *(vu32*)(addr) = data
#define READU8(addr) *(vu8*)(addr)
#define READU16(addr) *(vu16*)(addr)
#define READU32(addr) *(vu32*)(addr)

u32 threadStack[0x1000];
Handle fsUserHandle;
FS_archive sdmcArchive;
u32 monsterHP;

u32 plgGetIoBase(u32 IoType);
#define IO_BASE_PAD		1
#define IO_BASE_LCD		2
#define IO_BASE_PDC		3
#define IO_BASE_GSPHEAP		4

u32 IoBasePad = 0xFFFD4000;

u32 getKey() {
	return (*(vu32*)(IoBasePad) ^ 0xFFF) & 0xFFF;
}

void waitKeyUp() {
	while (getKey() != 0) {
		svc_sleepThread(100000000);
	}
}

int checkPointerValid(pointer){
	return (pointer > 0x08000000) && (pointer < 0x0A000000);
}

 // void itoa_reverse(u8 s[])
 // {
 //   int c, i, j;
 
 //   for (i = 0, j = strlen(s)-1; i < j; i++ , j--)
 //   {
 //     c = s[i];
 //     s[i] = s[j];
 //     s[j] = c;
 //   }
 // }  

 // void my_itoa_2(u16 n, u8 s[])
 // {
 //   int i;
 
 //   i = 0;
 //   do 
 //   {
 //     s[i++] = n % 10 + '0';
 //   }
 //   while ((n /= 10) >= 1);
 //   if ( i == 1)
 //     s[i++] = '0';
 //   s[i] = '\0'; 
 //   itoa_reverse(s);
 // }
 
 
 
 //------------------------------ string reverse function -----------------------------//
 


// u32 showInfo(){
// 	u8 str[15];
// 	my_itoa_2(monsterHP,str);
// 	showDbg("Monster HP:", 0, 0);
// 	showDbg(str,0,0);
// 	return 0;
// }

u32 cheatEnabled = 0;
int isNewNtr = 0;

void gamePluginEntry() {
	u32 ret, key;
	INIT_SHARED_FUNC(plgGetIoBase, 8);
	//plgGetSharedServiceHandle("fs:USER", &fsUserHandle);
	
	// wait for game starts up (5 seconds)
	svc_sleepThread(30000000000);

	//plgRegisterMenuEntry(1, "MH4U Info", showInfo);

	if (((NS_CONFIG*)(NS_CONFIGURE_ADDR))->sharedFunc[8]) {
		isNewNtr = 1;
	} else {
		isNewNtr = 0;
	}
	
	if (isNewNtr) {
		IoBasePad = plgGetIoBase(IO_BASE_PAD);
	}
	monsterHP = 0;
	while (1) {
		svc_sleepThread(100000000);
		
		key = getKey();
		if (key == BUTTON_SE) {
			// toggle cheats when SELECT button pressed
			cheatEnabled = !cheatEnabled;
			// wait until key is up
			waitKeyUp();
		}
		if (cheatEnabled) {
			u32 pointer0 = 0x081C7D00;
			u32 pointer1 = READU32(pointer0);
			if (checkPointerValid(pointer1)){
				u32 start_offset = READU32(pointer1 + 0xE28) - 0x14030;
				if (checkPointerValid(start_offset)){
					int i = 0;
					monsterHP = READU16(start_offset + 0x14418);
					for (i = 0;i < 32;i++){
						if (i == 8){
							if (monsterHP != 0){
								WRITEU16(start_offset,monsterHP);	
							}else{
								WRITEU8(start_offset,0x63);
							}
						}else{
							WRITEU8(start_offset,0x63);
						}
						start_offset = start_offset + 4;
					}
				}else{
					cheatEnabled = 0;
				}	
			}else{
				cheatEnabled = 0;
			}
		}
	}
}