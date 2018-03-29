/*
 * Copyright 2018 Sheng Yang, <iysheng@163.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __STM32_NAND_H
#define __STM32_NAND_H

#define FMC_PWAIT_EN        1
#define FMC_PWAIT_DIS       0

#define FMC_PWID_8          0

#define FMC_ECC_EN          1
#define FMC_ECC_DIS         0

#define FMC_TCLR_6          (6 - 1)

#define FMC_TAR_6           (6 - 1)

#define FMC_ECCPS_512       1

#define MEMSET_VALE_2       (2 - 1)
#define MEMWAIT_VALE_4      (4 - 1)
#define MEMHOLD_VALE_2      (2 - 1)
#define MEMHIZ_VALE_2       (2 - 1)

#endif

