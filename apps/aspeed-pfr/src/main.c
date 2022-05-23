/*
 * Copyright (c) 2022 ASPEED Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#include <logging/log.h>
#include <zephyr.h>

#include "state_machine.h"
#include "common_smc.h"
#include "common/common.h"
#include "include/SmbusMailBoxCom.h"
#include "Smbus_mailbox/Smbus_mailbox.h"
#include "intel_pfr/intel_pfr_verification.h"
#include "intel_pfr/intel_pfr_provision.h"
#include "pfr/pfr_common.h"
#include "intel_pfr/intel_pfr_definitions.h"
#include "intel_pfr/intel_pfr_pfm_manifest.h"
#include <logging/logging_wrapper.h>

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define DEBUG_HALT() {				  \
		volatile int halt = 1;		  \
		while (halt) {			  \
			__asm__ volatile ("nop"); \
		}				  \
}

void main(void)
{
	int status = 0;

	LOG_INF("*** ASPEED_PFR version 0.0.1 ***");

	status = initializeEngines();
	status = initializeManifestProcessor();
	debug_log_init();// State Machine log saving

	// DEBUG_HALT();
	BMCBootHold();
	PCHBootHold();

	// I2c_slave_dev_debug+>
	struct i2c_slave_interface *I2CSlaveEngine = getI2CSlaveEngineInstance();
	struct I2CSlave_engine_wrapper *I2cSlaveEngineWrapper;

	status = I2C_Slave_wrapper_init(getI2CSlaveEngineInstance());

#if SMBUS_MAILBOX_SUPPORT
	InitializeSmbusMailbox();
	SetPlatformState(ENTER_T_MINUS_1);
#endif
	StartHrotStateMachine();
}
