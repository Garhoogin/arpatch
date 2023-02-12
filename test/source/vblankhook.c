#include <nitro.h>

extern void driver_2068A64(void *driver, int a2, int a3);

void RaceVBlankIntrHook(void) {
	void *driver = *(void **) 0x0217D028;
	
	if ((~reg_PAD_KEYINPUT) & REG_PAD_KEYINPUT_SEL_MASK) {
		driver_2068A64(driver, 2, 1);
	} else {
		driver_2068A64(driver, 0, 1);
	}
}
