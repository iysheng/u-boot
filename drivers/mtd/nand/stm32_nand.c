/*
 * Copyright 2018 Sheng Yang, <iysheng@163.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <nand.h>
#include <linux/err.h>
#include <asm/io.h>

#include "stm32_nand.h"

#define READ_ID         0x90
#define NAND_RESET      0xFF
#define NAND_READSTA    0x70
#define NAND_READY      0x40
#define NAND_ADDR       0x80000000

static int ready_stm32nand(void)
{
    int times = 0;
    unsigned char data = 0;
 #define READY_TRY_TIMES        20
 do {
    *(volatile unsigned char *)(NAND_ADDR | 1 << 16) = NAND_READSTA;
    data = *(volatile unsigned char *)(NAND_ADDR);
    if (data & NAND_READY)
        return 0;
    times++;
    } while(times < READY_TRY_TIMES);
    return -EINVAL;
}
static int reset_stm32nand(void)
{
    volatile unsigned char data = 0;
    int ret = 0;
    debug("iysheng nand reseting\n");
    *(volatile unsigned char *)(NAND_ADDR | 1 << 16) = NAND_RESET;  
    return ready_stm32nand();
}
static int read_stm32nand_id(void)
{
    unsigned int nand_id, i = 0;
    unsigned char temp[5] = {0};
    *(volatile unsigned char *)(NAND_ADDR | (1 << 16)) = READ_ID;
    *(volatile unsigned char *)(NAND_ADDR | (1 << 17)) = 0x00;
    for (; i<5; i++)
    {
        temp[i] = *(volatile unsigned char *)(NAND_ADDR);
    }
    memcpy(&nand_id, &temp[1], sizeof(unsigned int));
    return nand_id;
}

int board_nand_init(struct nand_chip *nand)
{
    int ret = 0;
    debug("iysheng %s %p\n", __func__, &FMC_NAND_BASE->pcr);
    writel(FMC_PWID_8 << FMC_PWID_SHIFT
        | FMC_TCLR_6 << FMC_TCLR_SHIFT
        | FMC_TAR_6 << FMC_TAR_SHIFT
        | FMC_ECCPS_512 << FMC_ECCPS_SHIFT, &FMC_NAND_BASE->pcr);
    writel(MEMSET_VALE_2 << MEMSET_SHIFT 
        | MEMWAIT_VALE_4 << MEMWAIT_SHIFT
        | MEMHOLD_VALE_2 << MEMHOLD_SHIFT
        | MEMHIZ_VALE_2 << MEMHIZ_SHIFT, &FMC_NAND_BASE->pmem);
    writel(readl(&FMC_NAND_BASE->pcr) | FMC_NAND_MEM_EN, &FMC_NAND_BASE->pcr);

    ret = reset_stm32nand();
    if (!ret)
    {
        debug("iysheng nand reset success nand_id=%08x!!!\n", read_stm32nand_id());
        if (__cpu_to_le32(MT29F4G08ABADA) == read_stm32nand_id())
        {
            nand->mtd.size = 0x10000000;
        }
    }
    return -ENODEV;
}

