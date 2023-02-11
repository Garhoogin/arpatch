#include <nds.h>

#define reg_KEYINPUT (*(vu16 *)0x04000130)

extern void driver_2068A64(void *driver, int a2, int a3);

void RaceVBlankIntrHook(void) {
	void *driver = *(void **) 0x0217D028;
	
	if ((~reg_KEYINPUT) & KEY_SELECT) {
		driver_2068A64(driver, 2, 1);
	} else {
		driver_2068A64(driver, 0, 1);
	}
}
