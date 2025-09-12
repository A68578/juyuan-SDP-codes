#include "bl_manager.h"
#include "sourceswc.h"
#if 0
int main(void)
{
	//		FLASH_ERASE_SECTOR(0x10000000);
	fblmain();

	return 0;
}
#endif


int main()
{
	sourcetask();
}