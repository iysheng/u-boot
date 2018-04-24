/*
 * Copyright (C) 2016, STMicroelectronics - All Rights Reserved
 * Author(s): Vikas Manocha, <vikas.manocha@st.com> for STMicroelectronics.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <ram.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/armv7m.h>
#include <asm/arch/stm32.h>
#include <asm/arch/gpio.h>
#include <asm/arch/syscfg.h>
#include <asm/gpio.h>
#include <clk.h>

DECLARE_GLOBAL_DATA_PTR;

int get_memory_base_size(fdt_addr_t *mr_base, fdt_addr_t *mr_size)
{
	int mr_node;

	mr_node = fdt_path_offset(gd->fdt_blob, "/memory");
	if (mr_node < 0)
		return mr_node;
	*mr_base = fdtdec_get_addr_size_auto_noparent(gd->fdt_blob, mr_node,
						      "reg", 0, mr_size, false);
	debug("mr_base = %lx, mr_size= %lx\n", *mr_base, *mr_size);
    
	return 0;
}
int dram_init(void)
{
	int rv;
	fdt_addr_t mr_base, mr_size;
#undef CONFIG_SUPPORT_SPL
#ifndef CONFIG_SUPPORT_SPL
	struct udevice *dev;
	rv = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (rv) {
		debug("DRAM init failed: %d\n", rv);
		return rv;
	}

#endif
	rv = get_memory_base_size(&mr_base, &mr_size);

	if (rv)
		return rv;
	gd->ram_size = mr_size;
	gd->ram_top = mr_base;

	return rv;
}

int dram_init_banksize(void)
{
	fdt_addr_t mr_base, mr_size;
	get_memory_base_size(&mr_base, &mr_size);
	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = mr_base;
	gd->bd->bi_dram[0].size  = mr_size;
	return 0;
}

int board_early_init_f(void)
{
	return 0;
}

#ifdef CONFIG_SPL_BUILD
#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	debug("SPL: booting kernel\n");
	/* break into full u-boot on 'c' */
	return serial_tstc() && serial_getc() == 'c';
}
#endif

int spl_dram_init(void)
{
	struct udevice *dev;
	int rv;
	rv = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (rv)
		debug("DRAM init failed: %d\n", rv);
	return rv;
}
void spl_board_init(void)
{
	spl_dram_init();
	preloader_console_init();
	arch_cpu_init(); /* to configure mpu for sdram rw permissions */
}
u32 spl_boot_device(void)
{
	return BOOT_DEVICE_XIP;
}

#endif
u32 get_board_rev(void)
{
	return 0;
}

int board_late_init(void)
{
	struct gpio_desc gpio = {};
	int node;

	node = fdt_node_offset_by_compatible(gd->fdt_blob, 0, "st,led1");
	if (node < 0)
		return -1;

	gpio_request_by_name_nodev(offset_to_ofnode(node), "led-gpio", 0, &gpio,
				   GPIOD_IS_OUT);

	if (dm_gpio_is_valid(&gpio)) {
		dm_gpio_set_value(&gpio, 1);
		mdelay(10);
		dm_gpio_set_value(&gpio, 0);
	}

	/* read button 1*/
	node = fdt_node_offset_by_compatible(gd->fdt_blob, 0, "st,button1");
	if (node < 0)
		return -1;

	gpio_request_by_name_nodev(offset_to_ofnode(node), "button-gpio", 0,
				   &gpio, GPIOD_IS_IN);

	if (dm_gpio_is_valid(&gpio)) {
		if (dm_gpio_get_value(&gpio))
			puts("usr button is at HIGH LEVEL\n");
		else
			puts("usr button is at LOW LEVEL\n");
	}

	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = gd->bd->bi_dram[0].start + 0x100;

#ifdef CONFIG_ETH_DESIGNWARE
	/* Set >RMII mode */
	STM32_SYSCFG->pmc |= SYSCFG_PMC_MII_RMII_SEL;
#endif

	return 0;
}

int set_cpu_clk_info(void)
{
    int ret = 0;
    unsigned long freq = 0;
    struct clk clk_tmp;
    struct udevice *fixed_clock_dev = NULL;
    puts("CPU_CLK: ");
    ret = uclass_get_device_by_name(UCLASS_CLK, "stm32fx_rcc_clock",
					&fixed_clock_dev);
    if (ret < 0)
    {
        debug("Failed to get device clk-hse :%d\n", ret);
        return ret;
    }
    ret= clk_request(fixed_clock_dev, &clk_tmp);
    if (ret < 0)
    {
        debug("Failed to request clk :%d\n", ret);
        return ret;
    }
    clk_tmp.id = 0;
    freq = clk_get_rate(&clk_tmp);
    if (freq == 0)
    {
        debug("Failed to get rate\n");
        return ret;
    }
    gd->cpu_clk = gd->bd->bi_arm_freq = freq;
    printf("%lu MHz\n", freq/1000000);
    return 0;    
}

#define YYFISH_LED

#ifdef YYFISH_LED
#define GPIO_I_REG_BASE 0x40022000U
#define GPIO_B_REG_BASE 0x40020400U
#define RCC_REG_BASE 0x40023800U
#define RCC_CR 0X00U
#define RCC_PLLCFGR 0x04U
#define RCC_CFGR 0x08U
#define RCC_CIR 0x0cU

#define RCC_AHB1ENR_OFFSET 0X30U
#define RCC_APB1ENR_OFFSET 0x40U
#define PWR_REG_BASE 0x40007000U
#define PWD_CR1 0x00U
#define VTOR 0xe000ed08U
typedef volatile unsigned int * REG_PTR;

void yyfish_led_toggle(void)
{
    *(REG_PTR)(GPIO_I_REG_BASE + 0x14U) ^= (1 << 11);    
}

void yyfish_led_on(void)
{
    *(REG_PTR)(GPIO_I_REG_BASE + 0x14U) |= (1 << 11);
}
void yyfish_led_off(void)
{
    *(REG_PTR)(GPIO_I_REG_BASE + 0x14U) &= ~(1 << 11);
}
extern char __image_copy_start[];
extern char __image_copy_end[];
char * ptr_relocate = NULL;
char * dest;

void yyfish_relocate(void)
{
    dest = (char *)(gd->relocaddr);
    ptr_relocate = &__image_copy_start[0];
    printf("iysheng coming dest=%p ptr_relocate=%p __image_copy_end=%p %p\n",
        dest, ptr_relocate, &__image_copy_end[0], __image_copy_end);
    for ( ;(unsigned int)ptr_relocate <= (unsigned int)&__image_copy_end[0]; ptr_relocate++,dest++)
    {
        *dest = *ptr_relocate;
    }
    printf("%p <= %08x\n", ptr_relocate, (unsigned int)&__image_copy_end[0]);
}

void yyfish_put(int r0)
{
    int num = r0;
    printf("iysheng r0=%08x\n", (unsigned int)num);
}

void yyfish_nprintf(uchar * src, int num)
{
    int i=0;
    for (; i<num; i++);
        //printf("%02x ", src[i]);
    //puts("\r\n");
}
#endif

