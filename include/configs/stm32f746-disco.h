/*
 * Copyright (C) 2016, STMicroelectronics - All Rights Reserved
 * Author(s): Vikas Manocha, <vikas.manocha@st.com> for STMicroelectronics.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_SYS_FLASH_BASE		0x08000000
#define CONFIG_SYS_INIT_SP_ADDR		0x20050000

#ifdef CONFIG_SUPPORT_SPL
/*#define CONFIG_SYS_LOAD_ADDR		0x08008000*/
#define CONFIG_SYS_LOAD_ADDR		0x20030000
#else
/*#define CONFIG_SYS_LOAD_ADDR		0xC0400000 */
/*#define CONFIG_LOADADDR			0xC0400000 */
#define CONFIG_SYS_LOAD_ADDR		0x20030000
#define CONFIG_LOADADDR			0x20030000

#endif

/*
 * Configuration of the external SDRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1

#define CONFIG_SYS_MAX_FLASH_SECT	8
#define CONFIG_SYS_MAX_FLASH_BANKS	1

#define CONFIG_ENV_SIZE			(8 << 10)

#define CONFIG_STM32_FLASH

#define CONFIG_DW_GMAC_DEFAULT_DMA_PBL	(8)
#define CONFIG_DW_ALTDESCRIPTOR
#define CONFIG_MII
#define CONFIG_PHY_SMSC

#define CONFIG_SYS_CLK_FREQ		100000000 /* 100 MHz */
#define CONFIG_SYS_HZ_CLOCK		1000000	/* Timer is clocked at 1MHz */

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_SYS_CBSIZE		1024

#define CONFIG_SYS_MALLOC_LEN		(1 * 1024 * 1024)

#define CONFIG_BOOTCOMMAND						\
	"run bootcmd_romfs"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd_romfs=setenv bootargs ${bootargs};fatload mmc 0 0xc0008000 yyfish.bin;" \
	"bootm 0xc0008000\0"


/*
 * Command line configuration.
 */
#define CONFIG_CMD_CACHE
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_DISPLAY_BOARDINFO

/* For SPL */
#ifdef CONFIG_SUPPORT_SPL
#define CONFIG_SPL_STACK		CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SPL_TEXT_BASE		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)
#define CONFIG_SYS_SPL_LEN		0x00008000
#define CONFIG_SYS_UBOOT_START		0x080083FD
#define CONFIG_SYS_UBOOT_BASE		(CONFIG_SYS_FLASH_BASE + \
					CONFIG_SYS_SPL_LEN)

/* DT blob (fdt) address */
#define CONFIG_SYS_FDT_BASE		(CONFIG_SYS_FLASH_BASE + \
					0x1C0000)
#endif
/* For SPL ends */

#define YYFISH_LED
#define YYFISH_BOARD
#define CONFIG_MACH_TYPE        MACH_TYPE_STM32F2

#ifndef CONFIG_DMAMEM_TAG
#define CONFIG_DMAMEM_TAG
#endif

#define CONFIG_SYS_NAND_MAX_CHIPS       1
#define CONFIG_SYS_MAX_NAND_DEVICE      CONFIG_SYS_NAND_MAX_CHIPS   
#define CONFIG_SYS_NAND_BASE_LIST 	    {0x80000000}
#define CONFIG_CLOCKS

/* #define DEBUG */
#endif /* __CONFIG_H */
