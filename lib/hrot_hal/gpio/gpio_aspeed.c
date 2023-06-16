/*
 * Copyright (c) 2022 ASPEED Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#include <logging/log.h>
#include <drivers/flash.h>
#include <drivers/spi_nor.h>
#include <kernel.h>
#include <sys/util.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <drivers/gpio.h>
#include "gpio_aspeed.h"

#define LOG_MODULE_NAME gpio_api

#if !DT_NODE_HAS_STATUS(DT_INST(0, aspeed_pfr_gpio_common), okay)
#error "no correct pfr gpio device"
#endif

LOG_MODULE_REGISTER(LOG_MODULE_NAME);
static bool first_time_boot = true;

static void bmc_srst_enable_ctrl(bool enable)
{
	int ret;
	const struct gpio_dt_spec gpio_m5 =
		GPIO_DT_SPEC_GET_BY_IDX(DT_INST(0, aspeed_pfr_gpio_common),
						bmc_srst_ctrl_out_gpios, 0);

	if (enable)
		gpio_pin_set(gpio_m5.port, gpio_m5.pin, 0);
	else
		gpio_pin_set(gpio_m5.port, gpio_m5.pin, 1);

	ret = gpio_pin_configure_dt(&gpio_m5, GPIO_OUTPUT);
	if (ret)
		return;

	k_busy_wait(10000); /* 10ms */
}

static void bmc_extrst_enable_ctrl(bool enable)
{
	int ret;
	const struct gpio_dt_spec gpio_h2 =
		GPIO_DT_SPEC_GET_BY_IDX(DT_INST(0, aspeed_pfr_gpio_common),
						bmc_extrst_ctrl_out_gpios, 0);

	if (enable)
		gpio_pin_set(gpio_h2.port, gpio_h2.pin, 0);
	else
		gpio_pin_set(gpio_h2.port, gpio_h2.pin, 1);

	ret = gpio_pin_configure_dt(&gpio_h2, GPIO_OUTPUT);
	if (ret)
		return;

	k_busy_wait(10000); /* 10ms */
}

static void pch_rst_enable_ctrl(bool enable)
{
	int ret;
	const struct gpio_dt_spec gpio_m2 =
		GPIO_DT_SPEC_GET_BY_IDX(DT_INST(0, aspeed_pfr_gpio_common),
						pch_rst_ctrl_out_gpios, 0);

	if (enable)
		gpio_pin_set(gpio_m2.port, gpio_m2.pin, 0);
	else
		gpio_pin_set(gpio_m2.port, gpio_m2.pin, 1);

	ret = gpio_pin_configure_dt(&gpio_m2, GPIO_OUTPUT);
	if (ret)
		return;

	k_busy_wait(10000); /* 10ms */
}

int BMCBootHold(void)
{
	const struct device *dev_m = NULL;
	const struct device *flash_dev = NULL;

	/* Hold BMC Reset */
	bmc_extrst_enable_ctrl(true);
	// Only pull-up/down SRST in first bootup. Pull-up/down this pin in runtime will affect host
	// VGA function.
	if (first_time_boot)
		bmc_srst_enable_ctrl(true);
	dev_m = device_get_binding(BMC_SPI_MONITOR);
	spim_passthrough_config(dev_m, 0, false);
	/* config spi monitor as master mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_ROT);
	flash_dev = device_get_binding("spi1_cs0");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi1_cs0");
	}
#if defined(CONFIG_BMC_DUAL_FLASH)
	dev_m = device_get_binding(BMC_SPI_MONITOR_2);
	spim_passthrough_config(dev_m, 0, false);
	/* config spi monitor as master mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_ROT);
	flash_dev = device_get_binding("spi1_cs1");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi1_cs1");
	}
#endif
	LOG_INF("hold BMC");
	return 0;
}

int PCHBootHold(void)
{
	const struct device *dev_m = NULL;
	const struct device *flash_dev = NULL;

	/* Hold PCH Reset */
	pch_rst_enable_ctrl(true);

	dev_m = device_get_binding(PCH_SPI_MONITOR);
	spim_passthrough_config(dev_m, 0, false);
	/* config spi monitor as master mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_ROT);
	flash_dev = device_get_binding("spi2_cs0");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi2_cs0");
	}

#if defined(CONFIG_CPU_DUAL_FLASH)
	dev_m = device_get_binding(PCH_SPI_MONITOR_2);
	spim_passthrough_config(dev_m, 0, false);
	/* config spi monitor as master mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_ROT);
	flash_dev = device_get_binding("spi2_cs1");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi2_cs1");
	}
#endif
	LOG_INF("hold PCH");
	return 0;
}

int BMCBootRelease(void)
{
	const struct device *dev_m = NULL;
	const struct device *flash_dev = NULL;

	flash_dev = device_get_binding("spi1_cs0");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi1_cs0");
	}
	dev_m = device_get_binding(BMC_SPI_MONITOR);
	spim_passthrough_config(dev_m, 0, false);
	aspeed_spi_monitor_sw_rst(dev_m);
	/* config spi monitor as monitor mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_BMC_PCH);
#if defined(CONFIG_BMC_DUAL_FLASH)
	flash_dev = device_get_binding("spi1_cs1");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi1_cs1");
	}
	dev_m = device_get_binding(BMC_SPI_MONITOR_2);
	spim_passthrough_config(dev_m, 0, false);
	aspeed_spi_monitor_sw_rst(dev_m);
	/* config spi monitor as monitor mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_BMC_PCH);
#endif

	if (first_time_boot) {
		bmc_srst_enable_ctrl(false);
		first_time_boot = false;
	}

	bmc_extrst_enable_ctrl(false);
	LOG_INF("release BMC");
	return 0;
}

int PCHBootRelease(void)
{
	const struct device *dev_m = NULL;
	const struct device *flash_dev = NULL;

	flash_dev = device_get_binding("spi2_cs0");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi2_cs0");
	}
	dev_m = device_get_binding(PCH_SPI_MONITOR);
	spim_passthrough_config(dev_m, 0, false);
	aspeed_spi_monitor_sw_rst(dev_m);
	/* config spi monitor as monitor mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_BMC_PCH);

#if defined(CONFIG_CPU_DUAL_FLASH)
	flash_dev = device_get_binding("spi2_cs1");
	if (flash_dev) {
		spi_nor_rst_by_cmd(flash_dev);
	} else {
		LOG_ERR("Failed to bind spi2_cs1");
	}
	dev_m = device_get_binding(PCH_SPI_MONITOR_2);
	spim_passthrough_config(dev_m, 0, false);
	aspeed_spi_monitor_sw_rst(dev_m);
	/* config spi monitor as monitor mode */
	spim_ext_mux_config(dev_m, SPIM_EXT_MUX_BMC_PCH);
#endif

	pch_rst_enable_ctrl(false);
	LOG_INF("release PCH");
	return 0;
}

#if defined(CONFIG_PFR_MCTP_I3C)
#if !defined(CONFIG_I3C_SLAVE)
static int i3c_mng_mux_owner = I3C_MNG_OWNER_BMC;
void switch_i3c_mng_owner(int owner)
{
#if DT_NODE_HAS_STATUS(DT_INST(0, aspeed_pfr_gpio_bhs), okay)
// BHS only
	const struct gpio_dt_spec i3c_mng_owner =
		GPIO_DT_SPEC_GET_BY_IDX(DT_INST(0, aspeed_pfr_gpio_bhs),
						i3c_mng_mux_sel_out_gpios, 0);
	gpio_pin_set(i3c_mng_owner.port, i3c_mng_owner.pin, owner);
#endif
	i3c_mng_mux_owner = owner;
}

int get_i3c_mng_owner(void)
{
	return i3c_mng_mux_owner;
}
#endif
#endif
