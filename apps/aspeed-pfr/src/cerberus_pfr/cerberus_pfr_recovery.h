/*
 * Copyright (c) 2022 ASPEED Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define DUAL_SPI 0

#include <stdint.h>
#include "manifest/pfm/pfm_manager.h"
#include "recovery/recovery_image.h"
#include "pfr/pfr_common.h"

struct recovery_header {
	uint16_t header_length;
	uint16_t format;
	uint32_t magic_number;
	uint8_t version_id[32];
	uint32_t image_length;
	uint32_t sign_length;
};

struct recovery_section {
	uint16_t header_length;
	uint16_t format;
	uint32_t magic_number;
	uint32_t start_addr;
	uint32_t section_length;
};

int cerberus_pfr_recovery_verify(struct recovery_image *image, struct hash_engine *hash,
			      struct signature_verification *verification, uint8_t *hash_out, size_t hash_length,
			      struct pfm_manager *pfm);

int recovery_verify(struct recovery_image *image, struct hash_engine *hash,
		    struct signature_verification *verification, uint8_t *hash_out,
		    size_t hash_length, struct pfm_manager *pfm);
int recovery_apply_to_flash(struct recovery_image *image, struct spi_flash *flash);
int pfr_staging_pch_staging(struct pfr_manifest *manifest);
int pfr_recover_active_region(struct pfr_manifest *manifest);
int does_staged_fw_image_match_active_fw_image(struct pfr_manifest *manifest);

