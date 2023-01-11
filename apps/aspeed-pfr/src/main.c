/*
 * Copyright (c) 2022 ASPEED Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#include <logging/log.h>
#include <zephyr.h>
#include <build_config.h>

#include "common/common.h"
#include "include/SmbusMailBoxCom.h"
#include "Smbus_mailbox/Smbus_mailbox.h"
#if defined(CONFIG_INTEL_PFR)
#include "intel_pfr/intel_pfr_verification.h"
#include "intel_pfr/intel_pfr_provision.h"
#include "intel_pfr/intel_pfr_definitions.h"
#include "intel_pfr/intel_pfr_pfm_manifest.h"
#endif
#if defined(CONFIG_CERBERUS_PFR)
#include "cerberus_pfr/cerberus_pfr_verification.h"
#include "cerberus_pfr/cerberus_pfr_provision.h"
#include "cerberus_pfr/cerberus_pfr_definitions.h"
#endif
#include "pfr/pfr_common.h"
#include "AspeedStateMachine/AspeedStateMachine.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define DEBUG_HALT() {				  \
		volatile int halt = 1;		  \
		while (halt) {			  \
			__asm__ volatile ("nop"); \
		}				  \
}

extern void aspeed_print_sysrst_info(void);

void main(void)
{
	LOG_INF("*** ASPEED_PFR version v%02d.%02d Board:%s ***", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, CONFIG_BOARD);

	aspeed_print_sysrst_info();

	AspeedStateMachine();
}
