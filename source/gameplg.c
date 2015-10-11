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
u16 monsterHP;
u16 monster2HP;

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

int checkStartOffsetValid(pointer){
	if ((pointer > 0x08920000) && (pointer < 0x08930000)){
		u8 check = READU8(pointer - 0x1E0);
		if (check != 0) return true;
	}
	return false;
}

u32 cheatEnabled = 0;
u32 longswordGaugeEnabled = 0;
u32 chargeBladeGaugeEnabled = 0;
int isNewNtr = 0;
u8 longswordGauge = 0;
u32 Mpointer0 = 0x081C7D00;
u32 Mpointer1 = 0;
u32 Mpointer2 = 0;
u32 pointer0 = 0x081C7CD0;
//u32 pointer0 = 0x081C739C;
//u32 pointer0 = 0x081C7CD4;
//u32 pointer1 = 0;
u32 start_offset = 0;
u32 itemOffset = 0;
u16 Sharpness = 0;

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
	//monsterHP = 0;
	while (1) {
		svc_sleepThread(100000000);
		
		key = getKey();
		switch(key){
			case BUTTON_SE + BUTTON_DU:
				cheatEnabled = !cheatEnabled;
				break;
			case BUTTON_SE + BUTTON_DU + BUTTON_R:
				longswordGaugeEnabled = !longswordGaugeEnabled;
				break;
			case BUTTON_SE + BUTTON_DR + BUTTON_R:
				chargeBladeGaugeEnabled = !chargeBladeGaugeEnabled;
				break;
		}
		waitKeyUp();
		// if (key == BUTTON_SE) {
		// 	// toggle cheats when SELECT button pressed
		// 	cheatEnabled = !cheatEnabled;
		// 	// wait until key is up
		// 	waitKeyUp();
		// }
		if (cheatEnabled || longswordGaugeEnabled || chargeBladeGaugeEnabled){
			start_offset = READU32(pointer0) + 0x1070;
			if (checkStartOffsetValid(start_offset)){
				if (cheatEnabled){
					monsterHP = 0;
					Mpointer1 = READU32(Mpointer0) + 0xE28;
					if (checkPointerValid(Mpointer1)){
						Mpointer2 = READU32(Mpointer1) + 0x3E8;
						if (checkPointerValid(Mpointer2)){
							monsterHP = READU16(Mpointer2);
						}
					}
					//monsterHP
					//monsterHP = READU16(start_offset + 0x14418);
					//monster2HP = READU16(start_offset + 0x20AE8);
					//Sharpness
					Sharpness = READU16(start_offset + 0x94C6);
					WRITEU16(start_offset + 0x94C4,Sharpness);
					//Stamina

					WRITEU16(start_offset + 0x11E,0x4461);
					WRITEU16(start_offset + 0x122,0x4461);
					
					//Item
					itemOffset = start_offset;
					int i = 0;
					for (i = 0;i < 32;i++){
						if (i == 8){
							if (monsterHP > 0){
								WRITEU16(itemOffset,monsterHP);	
							}else{
								WRITEU8(itemOffset,0x63);
							}
						}else{
							//if (i == 9){
								// if (monster2HP > 0){
								// 	WRITEU16(start_offset,monster2HP);
								// }else{
								//	WRITEU8(start_offset,0x63);
								//}
							//}else{
								WRITEU8(itemOffset,0x63);
							//}
						}
						itemOffset = itemOffset + 4;
					}
				}
				if (longswordGaugeEnabled){
					//Long Sword
					// longswordGauge = READU8(start_offset + 0x134);
					// if (longswordGauge != 0){
						WRITEU8(start_offset + 0x134,0x64);
						WRITEU8(start_offset + 0x136,0x03);
					//}
				}
				if (chargeBladeGaugeEnabled){
					WRITEU8(start_offset + 0x134,50);
					WRITEU16(start_offset + 0x9292,0x4676);
					WRITEU16(start_offset + 0x9744,0x0606);
				}
				
			}else{
				cheatEnabled = 0;
				longswordGaugeEnabled = 0;
				chargeBladeGaugeEnabled = 0;
			}	
		}
	}
}