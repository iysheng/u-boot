/*
 * Copyright 2018 Sheng Yang, <iysheng@163.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __STM32_NAND_H
#define __STM32_NAND_H


#define FMC_CONTROL_BASE    0xA0000000
#define FMC_CONTROL_NAND_BASE   (FMC_CONTROL_BASE + 0x80)

struct fmc_nand {
	uint32_t pcr; /* nand control register */
    uint32_t sr;
    uint32_t pmem;
    uint32_t patt;
    uint32_t reserved1[1];
    uint32_t eccr;
    uint32_t reserved2[27];
};

#define MT29F4G08ABADA			0XDC909556

#define FMC_PWID_8          0
#define FMC_PWID_SHIFT      4
#define FMC_PWID_MASK       ~(0X3 << FMC_TCLR_SHIFT)

#define FMC_TCLR_6          (6 - 1)
#define FMC_TCLR_SHIFT      9
#define FMC_TCLR_MASK       ~(0XF << FMC_TCLR_SHIFT)

#define FMC_TAR_6           (6 - 1)
#define FMC_TAR_SHIFT       13
#define FMC_TAR_MASK        ~(0XF << FMC_TAR_SHIFT)

#define FMC_ECCPS_512       1
#define FMC_ECCPS_SHIFT     17
#define FMC_ECCPS_MASK      ~(0X7 << FMC_ECCPS_SHIFT)

#define MEMSET_VALE_2       (2 - 1)
#define MEMSET_SHIFT        0
#define MEMWAIT_VALE_4      (4 - 1)
#define MEMWAIT_SHIFT       8
#define MEMHOLD_VALE_2      (2 - 1)
#define MEMHOLD_SHIFT       16
#define MEMHIZ_VALE_2       (2 - 1)
#define MEMHIZ_SHIFT        24

#define FMC_NAND_MEM_EN		BIT(2)
#define FMC_NAND_BASE       ((struct fmc_nand *)FMC_CONTROL_NAND_BASE)
#endif

