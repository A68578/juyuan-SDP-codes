#include "bl_manager.h"
#define DEBUG 1
#define DEBUGMAIN main
typedef int DebugMainType;
#if 0
int main(void)
{
	//		FLASH_ERASE_SECTOR(0x10000000);
	fblmain();

	return 0;
}
#endif
#if DEBUG 
DebugMainType DEBUGMAIN()
{

}
#endif