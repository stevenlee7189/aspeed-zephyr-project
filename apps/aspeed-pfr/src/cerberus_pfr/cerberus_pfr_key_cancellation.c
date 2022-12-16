/*
 * Copyright (c) 2022 ASPEED Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(CONFIG_CERBERUS_PFR)
#include <logging/log.h>

#include "pfr/pfr_common.h"
#include "pfr/pfr_util.h"
#include "pfr/pfr_ufm.h"
#include "cerberus_pfr_provision.h"
#include "cerberus_pfr_verification.h"
#include "cerberus_pfr_key_cancellation.h"
#include "AspeedStateMachine/common_smc.h"

LOG_MODULE_DECLARE(pfr, CONFIG_LOG_DEFAULT_LEVEL);

int get_cancellation_policy_offset(uint32_t pc_type)
{
	if ((pc_type == CPLD_CAPSULE_CANCELLATION) || (pc_type == PFR_CPLD_UPDATE_CAPSULE))
		return KEY_CANCELLATION_POLICY_FOR_SIGNING_CPLD_UPDATE_CAPSULE;

	else if ((pc_type == PCH_PFM_CANCELLATION) || (pc_type == PFR_PCH_PFM))
		return KEY_CANCELLATION_POLICY_FOR_SIGNING_PCH_PFM;

	else if ((pc_type == PCH_CAPSULE_CANCELLATION) || (pc_type == PFR_PCH_UPDATE_CAPSULE))
		return KEY_CANCELLATION_POLICY_FOR_SIGNING_PCH_UPDATE_CAPSULE;

	else if ((pc_type == BMC_PFM_CANCELLATION) || (pc_type == PFR_BMC_PFM))
		return KEY_CANCELLATION_POLICY_FOR_SIGNING_BMC_PFM;

	else if ((pc_type == BMC_CAPSULE_CANCELLATION) || (pc_type == PFR_BMC_UPDATE_CAPSULE) || (pc_type == DECOMMISSION_CAPSULE))
		return KEY_CANCELLATION_POLICY_FOR_SIGNING_BMC_UPDATE_CAPSULE;

	return 0;
}

int verify_csk_key_id(struct pfr_manifest *manifest, uint8_t key_id)
{

	uint32_t ufm_offset = get_cancellation_policy_offset(manifest->pc_type);
	uint32_t policy_data;
	uint32_t bit_offset;
	int status = 0;

	if (!ufm_offset) {
		LOG_ERR("%s: Invalid provisioned UFM offset for key cancellation", __func__);
		return Failure;
	}

	// key id must be within 0-127
	if (key_id > KEY_CANCELLATION_MAX_KEY_ID) {
		LOG_ERR("%s: Invalid key Id: %d", __func__, key_id);
		return Failure;
	}

	ufm_offset += (key_id / 32) * 4;
	// bit little endian
	bit_offset = 31 - (key_id % 32);

	status = ufm_read(PROVISION_UFM, ufm_offset, (uint8_t *)&policy_data, sizeof(policy_data));
	if (status != Success) {
		LOG_ERR("%s: Read cancellation policy status from UFM failed", __func__);
		return Failure;
	}

	if (!(policy_data & (0x01 << bit_offset))) {
		LOG_ERR("This CSK key was cancelled..! Can't Proceed with verify with this key Id: %d", key_id);
		return Failure;
	}

	return Success;
}

int cancel_csk_key_id(struct pfr_manifest *manifest, uint8_t key_id)
{
	uint32_t ufm_offset = get_cancellation_policy_offset(manifest->pc_type);
	uint32_t policy_data;
	uint32_t bit_offset;
	int status = 0;

	if (!ufm_offset) {
		LOG_ERR("%s: Invalid provisioned UFM offset for key cancellation", __func__);
		return Failure;
	}

	// key id must be within 0-127
	if (key_id > KEY_CANCELLATION_MAX_KEY_ID) {
		LOG_ERR("%s: Invalid key Id: %d", __func__, key_id);
		return Failure;
	}

	ufm_offset += (key_id / 32) * 4;
	// bit little endian
	bit_offset = 31 - (key_id % 32);

	// store policy data from flash part
	status = ufm_read(PROVISION_UFM, ufm_offset, (uint8_t *)&policy_data, sizeof(policy_data));
	if (status != Success) {
		LOG_ERR("%s: Read cancellation policy status from UFM failed", __func__);
		return Failure;
	}

	policy_data &= ~(0x01 << bit_offset);

	status = ufm_write(PROVISION_UFM, ufm_offset, (uint8_t *)&policy_data, sizeof(policy_data));
	if (status != Success) {
		LOG_ERR("Write cancellation policy status to UFM failed, offset = %x, data = %x", ufm_offset, policy_data);
		return Failure;
	}

	return Success;
}

#endif // CONFIG_CERBERUS_PFR
