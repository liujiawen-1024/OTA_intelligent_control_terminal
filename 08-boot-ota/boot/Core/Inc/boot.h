#ifndef __BOOT_H_
#define __BOOT_H_
#define PageSize FLASH_PAGE_SIZE // 1K

/*=====用户配置(根据自己的分区进行配置)=====*/
// 0x400 1K
// 0x800 2K
// 0x1000 4K
// 0x2000 8K
// 0x7000 4K*7 = 28K

#define BootLoader_Size 0x2000U // BootLoader的大小 8K
#define Application_Size 0x7000U // APP 应用程序的大小 28K
#define Application_1_Addr 0x08002000U // 应用程序1的首地址 大小 29K 0x08002000 + 0x7000
#define Application_2_Addr 0x08009000U // 应用程序2的首地址 大小 29K 0x08009000 + 0x7000

/*==========================================*/

/* 启动的步骤 */
#define Startup_Normol 0xFFFFFFFF // 正常启动
#define Startup_Update 0xAAAAAAAA // 升级再启动

void Start_BootLoader(void);

#endif
