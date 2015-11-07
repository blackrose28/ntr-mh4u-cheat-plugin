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
u8 chargeBladeSkillEnabled = 0;
u32 insectGlaiveEnabled = 0;
u32 rebirthEnable = 0;
u32 skillEditModeEnabled = 0;
u16 skillTest = 0;
u8 skillEditIndex = 0;
u8 skillEditInc = 0;
u8 skillEditDec = 0;
u8 skillEditInc10 = 0;
u8 skillEditDec10 = 0;

int isNewNtr = 0;
u8 longswordGauge = 0;
u32 Mpointer0 = 0x081C7D00;
u32 Mpointer1 = 0;
u32 Mpointer2 = 0;
u32 pointer0 = 0x081C7CD0;
u32 rewardAddress = 0x08364B14;
u32 reward = 0;
//u32 pointer0 = 0x081C739C;
//u32 pointer0 = 0x081C7CD4;
//u32 pointer1 = 0;
u32 start_offset = 0;
u32 itemOffset = 0;
u16 Sharpness = 0;
u8 frenzy = 0;
u32 wystone = 0;

u8 oneSecCounter = 0;

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
				waitKeyUp();
				break;
			case BUTTON_SE + BUTTON_DD:
				rebirthEnable = !rebirthEnable;
				reward = READU32(rewardAddress);
				waitKeyUp();
				break;
			case BUTTON_SE + BUTTON_DU + BUTTON_R:
				longswordGaugeEnabled = !longswordGaugeEnabled;
				waitKeyUp();
				break;
			case BUTTON_SE + BUTTON_DR + BUTTON_R:
				chargeBladeGaugeEnabled = !chargeBladeGaugeEnabled;
				waitKeyUp();
				break;
			case BUTTON_SE + BUTTON_DD + BUTTON_R:
				insectGlaiveEnabled = !insectGlaiveEnabled;
				waitKeyUp();
				break;
			case BUTTON_SE + BUTTON_R + BUTTON_L:
				skillEditModeEnabled = !skillEditModeEnabled;
				waitKeyUp();
				break;
			case BUTTON_SE + BUTTON_L + BUTTON_DR:
				chargeBladeSkillEnabled = !chargeBladeSkillEnabled;
				waitKeyUp();
				break;
		}
		if (skillEditModeEnabled){
			switch(key){
				case BUTTON_L:
					if (skillEditIndex == 0) skillEditIndex = 19;
					skillEditIndex--;
					waitKeyUp();
					break;
				case BUTTON_R:
					if (skillEditIndex == 19) skillEditIndex = 0;
					skillEditIndex++;
					waitKeyUp();
					break;
				case BUTTON_DU + BUTTON_Y:
					skillEditInc = 1;
					waitKeyUp();
					break;
				case BUTTON_DD + BUTTON_Y:
					skillEditDec = 1;
					waitKeyUp();
					break;
				case BUTTON_X + BUTTON_DU:
					skillEditInc10 = 1;
					waitKeyUp();
					break;
				case BUTTON_X + BUTTON_DD:
					skillEditDec10 = 1;
					waitKeyUp();
					break;														
			}
		}
		
		// if (key == BUTTON_SE) {
		// 	// toggle cheats when SELECT button pressed
		// 	cheatEnabled = !cheatEnabled;
		// 	// wait until key is up
		// 	waitKeyUp();
		// }
		if (cheatEnabled || longswordGaugeEnabled || chargeBladeGaugeEnabled || rebirthEnable || chargeBladeSkillEnabled){
			if (rebirthEnable){
				WRITEU32(rewardAddress,reward);
			}
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

					// Frenzy Virus
					frenzy = READU8(start_offset + 0x94C9);
					if (frenzy != 0){
						WRITEU32(start_offset + 0x94C8,0x010100);
					}

					// Wystone
					wystone = READU32(start_offset + 0x94F8);
					if (wystone != 0){
						WRITEU32(start_offset + 0x94F8,0x46000000);
					}

					wystone = READU32(start_offset + 0x94FC);
					if (wystone != 0){
						WRITEU32(start_offset + 0x94FC,0x46000000);
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
					if (skillEditModeEnabled){
						if (skillEditInc){
							skillTest = READU16(start_offset + 0xC4 + (skillEditIndex * 2));
							if (skillTest >= 277){
								skillTest = 1;
							}else{
								skillTest++;
							}
							WRITEU16(start_offset + 0xC4 + (skillEditIndex * 2),skillTest);
							skillEditInc = 0;
						}
						if (skillEditDec){
							skillTest = READU16(start_offset + 0xC4 + (skillEditIndex * 2));
							if (skillTest <= 1){
								skillTest = 277;
							}else{
								skillTest--;
							}
							WRITEU16(start_offset + 0xC4 + (skillEditIndex * 2),skillTest);
							skillEditDec = 0;
						}
						if (skillEditInc10){
							skillTest = READU16(start_offset + 0xC4 + (skillEditIndex * 2));
							if (skillTest >= 268){
								skillTest = 1;
							}else{
								skillTest = skillTest + 10;
							}
							WRITEU16(start_offset + 0xC4 + (skillEditIndex * 2),skillTest);
							skillEditInc10 = 0;
						}
						if (skillEditDec10){
							skillTest = READU16(start_offset + 0xC4 + (skillEditIndex * 2));
							if (skillTest <= 10){
								skillTest = 277;
							}else{
								skillTest = skillTest - 10;
							}
							WRITEU16(start_offset + 0xC4 + (skillEditIndex * 2),skillTest);
							skillEditDec10 = 0;
						}
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
				if (insectGlaiveEnabled){
					WRITEU16(start_offset + 0x9B72,0x4676);
					WRITEU16(start_offset + 0x9B7A,0x4676);
					WRITEU16(start_offset + 0x9B82,0x4676);
				}
				if (chargeBladeSkillEnabled){
					WRITEU16(start_offset + 0xC4,0x56);
					WRITEU16(start_offset + 0xC6,0x5E);
					WRITEU16(start_offset + 0xC8,0x26);
					WRITEU16(start_offset + 0xCA,0xBD);
					WRITEU16(start_offset + 0xCC,0x0B);
					WRITEU16(start_offset + 0xCE,0x0D);
					WRITEU16(start_offset + 0xD0,0x71);
					WRITEU16(start_offset + 0xD2,0x10F);
					WRITEU16(start_offset + 0xD4,0x62);
					WRITEU16(start_offset + 0xD6,0x05);
					WRITEU16(start_offset + 0xD8,0x03);
					WRITEU16(start_offset + 0xDA,0x0A);
					WRITEU16(start_offset + 0xDC,0x10E);
					WRITEU16(start_offset + 0xDE,0x110);
					WRITEU16(start_offset + 0xE0,0x111);
					WRITEU16(start_offset + 0xE2,0xCF);
					WRITEU16(start_offset + 0xE4,0xBF);
					WRITEU16(start_offset + 0xE6,0xFC);
					WRITEU16(start_offset + 0xE8,0x0C);
					WRITEU16(start_offset + 0xEA,0x2D);
				}
			}else{
				cheatEnabled = 0;
				rebirthEnable = 0;
				insectGlaiveEnabled = 0;
				longswordGaugeEnabled = 0;
				chargeBladeGaugeEnabled = 0;
				chargeBladeSkillEnabled = 0;
				skillEditModeEnabled = 0;
			}	
		}
		oneSecCounter++;
		if (oneSecCounter >= 10) oneSecCounter = 0;
	}
}