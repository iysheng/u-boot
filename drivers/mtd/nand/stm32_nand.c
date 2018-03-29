/*
 * Copyright 2018 Sheng Yang, <iysheng@163.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <mtd.h>
#include <nand.h>
#include <linux/err.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <linux/mtd/mtd.h>

#include "stm32_nand.h"

struct stm32_fmc_nand_regs {
	uint32_t pcr; /* nand control register */
    uint32_t sr;
    uint32_t pmem;
    uint32_t patt;
    uint32_t reserved1[1];
    uint32_t eccr;
    uint32_t reserved2[27];
};

struct stm32_nand_control {
    uint8_t pwait;
    uint8_t pwid;
    uint8_t eccen;
    uint8_t tclr;
    uint8_t tar;
    uint8_t eccps;
};

struct stm32_nand_timing {
    uint8_t memset;
    uint8_t memwait;
    uint8_t memhold;
    uint8_t memhiz;    
};

struct stm32_fmc_nand_ranges {
    uint32_t addr;
    uint32_t cmd_pos;
    uint32_t data_pos;
};

struct stm32_nand_params {
    struct stm32_fmc_nand_regs *base;
    struct stm32_nand_control *nand_control;
    struct stm32_nand_timing *nand_timing;
    struct stm32_fmc_nand_ranges nand_ranges;
};

void stm32_nand_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
    struct nand_chip *this = mtd_to_nand(mtd);
    struct stm32_fmc_nand_ranges *rangs = (struct stm32_fmc_nand_ranges *)(this->priv);
    if (ctrl & NAND_CTRL_CHANGE) {
		ulong IO_ADDR_W = (ulong)(rangs->addr);
		IO_ADDR_W &= ~(rangs->cmd_pos
			     | rangs->data_pos);

		if (ctrl & NAND_CLE)
			IO_ADDR_W |= rangs->cmd_pos;
		else if (ctrl & NAND_ALE)
			IO_ADDR_W |= rangs->data_pos;

		this->IO_ADDR_W = (void *) IO_ADDR_W;
	}

	if (dat != NAND_CMD_NONE)
		writeb(dat, this->IO_ADDR_W);
}

static int stm32_nand_chip_bind(struct nand_chip *chip_ptr)
{
    if (!chip_ptr->cmd_ctrl)
        chip_ptr->cmd_ctrl = stm32_nand_cmd_ctrl;
    return 0;
}

int board_nand_init(struct nand_chip *nand)
{
    int ret = 0;
    struct udevice *nand_devp;
    struct stm32_nand_params *params; 
    /*
     * just call uclass_find_device_by_name is enough becase the stm32_nand device
     * maybe probe before relocate <iysheng@163.com>
     */
    ret = uclass_find_device_by_name(UCLASS_MTD, "stm32_nand", &nand_devp);
    if (!ret) {
        ret = nand_devp->driver->ofdata_to_platdata(nand_devp);
        if (ret)
            return ret;
        nand->bits_per_cell = 1;
        params = dev_get_platdata(nand_devp);
        nand->priv = (void *)(&params->nand_ranges);
        if (nand->priv) {
            stm32_nand_chip_bind(nand);
            return 0;
        }
    }
    return -ENODEV;
}

static int stm32_nand_ofdata_to_platdata(struct udevice *dev)
{
    struct stm32_nand_params *params = dev_get_platdata(dev);
    if (params) {
        params->nand_control =
    		(struct stm32_nand_control *)
    		 ofnode_read_u8_array_ptr(dev->node,
    					  "st,nand-control",
    					  sizeof(struct stm32_nand_control));
    	if (!params->nand_control) {
    		pr_err("st,nand-control not found for %s\n",
    		      ofnode_get_name(dev->node));
    		return -EINVAL;
    	}
    	params->nand_timing =
    		(struct stm32_nand_timing *)
    		 ofnode_read_u8_array_ptr(dev->node,
    					  "st,nand-timing",
    					  sizeof(struct stm32_nand_timing));
    	if (!params->nand_timing) {
    		pr_err("st,nand-timing not found for %s",
    		      ofnode_get_name(dev->node));
    		return -EINVAL;
        }
        int ret = 0;
        ret = ofnode_read_u32_array(dev->node, 
                        "st,nand-ranges",
                        (uint32_t *)(&params->nand_ranges),
                        sizeof(struct stm32_fmc_nand_ranges) / sizeof(uint32_t));
        if (ret) {
    		pr_err("st,nand-ranges not found for %s %d\n",
    		      ofnode_get_name(dev->node), ret);
    		return -EINVAL;
        }
        return 0;
    }
    return -EINVAL;
}

static int stm32_nand_init(struct stm32_nand_params *params)
{
    struct stm32_nand_control *control;
	struct stm32_nand_timing *timing;
	struct stm32_fmc_nand_regs *regs = params->base;

    control = params->nand_control;
    timing = params->nand_timing;
    
    writel(control->pwait << FMC_PWAIT_SHIFT
                | control->pwid << FMC_PWID_SHIFT
                | control->eccen << FMC_ECC_SHIFT
                | control->tclr << FMC_TCLR_SHIFT
                | control->tar << FMC_TAR_SHIFT
                | control->eccps << FMC_ECCPS_SHIFT,
                &regs->pcr);
    writel(timing->memset << MEMSET_SHIFT
			| timing->memwait << MEMWAIT_SHIFT
			| timing->memhold << MEMHOLD_SHIFT
			| timing->memhiz << MEMHIZ_SHIFT,
			&regs->pmem);
    writel(readl(&regs->pcr) | FMC_NAND_MEM_EN, &regs->pcr);
        
    return 0;
}

static int stm32_nand_probe(struct udevice *dev)
{
    int ret = 0;
    struct stm32_nand_params *params = dev_get_platdata(dev);
    if (params == NULL) 
        return -EINVAL;
    params->base = (struct stm32_fmc_nand_regs *)dev_read_addr(dev);
    ret = stm32_nand_init(params);
    
    return ret;
}

static const struct udevice_id stm32_nand_ids[] = {
    { .compatible = "st,nand-flash" },
    {}
};

U_BOOT_DRIVER(stm32_nand) = {
	.name = "stm32_nand",
	.id = UCLASS_MTD,
	.of_match = stm32_nand_ids,
	.ofdata_to_platdata = stm32_nand_ofdata_to_platdata,
	.probe = stm32_nand_probe,
	.platdata_auto_alloc_size = sizeof(struct stm32_nand_params),
};

