#include <common.h>
#include <asm/io.h>
#include <asm/arch/ldo.h>
#include <asm/arch/sprd_reg_ahb.h>
#include <asm/arch/regs_ahb.h>
#include <asm/arch/common.h>
#include <asm/arch/adi_hal_internal.h>
#include <asm/u-boot.h>
#include <part.h>
#include <sdhci.h>
#include <asm/arch/mfp.h>
#include <linux/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pinmap.h>
DECLARE_GLOBAL_DATA_PTR;

extern void sprd_gpio_init(void);
extern void ADI_init (void);
extern int LDO_Init(void);
extern void ADC_Init(void);
extern int sound_init(void);

#ifdef CONFIG_GENERIC_MMC
int mv_sdh_init(u32 regbase, u32 max_clk, u32 min_clk, u32 quirks);
int mmc_sdcard_init();

int board_mmc_init(bd_t *bd)
{
	mmc_sdcard_init();

	mv_sdh_init(CONFIG_SYS_SD_BASE, SDIO_BASE_CLK_192M,
			SDIO_CLK_250K, 0);

	return 0;
}
#endif

extern struct eic_gpio_resource sprd_gpio_resource[];

int board_init()
{
	gd->bd->bi_arch_number = MACH_TYPE_OPENPHONE;
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;
	ADI_init();
	misc_init();
	LDO_Init();
	ADC_Init();
	pin_init();
	sprd_eic_init();
	sprd_gpio_init();
	sound_init();
	init_ldo_sleep_gr();
	//TDPllRefConfig(1);

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((volatile void *)PHYS_SDRAM_1,
			PHYS_SDRAM_1_SIZE);
	return 0;
}

void fdt_fixup_chosen_bootargs_board(char *buf, const char *boot_mode, int calibration_mode)
{
	char *p = buf;
	/**
	 * Because of in u-boot, we can't find FDT chosen remove function
	 * and samsung only uses uart to do calibration,
	 * so in samsung board .dts, we remove the "console=ttyS1,115200n8" in chosen node by default.
	 * so in normal mode, we need to append console
	 */
	if (!calibration_mode) {
		p += sprintf(p, "console=ttyS1,115200n8 no_console_suspend");
	}
}
