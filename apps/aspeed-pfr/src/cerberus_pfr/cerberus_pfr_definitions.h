/*
 * Copyright (c) 2022 ASPEED Technology Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

# if defined(CONFIG_CERBERUS_PFR)
#define BMC_FLASH_ID                0
#define PCH_FLASH_ID                2

#define BMC_TYPE                    0
#define PCH_TYPE                    2
#define KEY_CANCELLATION_TYPE       4
#define DECOMMISSION_TYPE           5

#define FALSE                       0
#define TRUE                        1
#define START                       2

#define BMC_SUPPORT                 1
// Debug configuration token
#define PF_STATUS_DEBUG             1
#define PF_VERIFY_DEBUG             1
#define PF_UPDATE_DEBUG             1
#define PFR_AUTHENTICATION_DEBUG    1
#define HROT_STATE_DEBUG            1
#define SMBUS_MAILBOX_DEBUG         1
#define CERBERUS_MANIFEST_DEBUG     1
#define EVERY_BOOT_SVN_VALIDATION   1
#define SMBUS_MAILBOX_SUPPORT       1

//Cerberus Content
#define RECOVERY_HEADER_MAGIC       0x8A147C29
#define RECOVERY_SECTION_MAGIC      0x4B172F31
#define CANCELLATION_HEADER_MAGIC   0xB6EAFD19

#define SHA256_SIGNATURE_LENGTH     256


// Hard-coded PFM offset
#define BMC_CPLD_STAGING_ADDRESS    CONFIG_BMC_PFR_STAGING_OFFSET
#define HROT_ACTIVE_AREA_SIZE   0x60000
#define PFM_RW_FLASH_REGION_LENGTH  12
#define TOC_ELEMENT_LENGTH_OFFSET   6
#define ALLOWABLE_FW_LIST_HEADER_LENGTH 4
#define ALLOWABLE_FW_LIST_VERSION_ADDR_LENGTH   4
#define ALLOWABLE_FW_LIST_ALLIGNMENT_LENGTH 2

// Will Remove after test
#define UFM0                        4
#define UFM1                        3
#define PROVISION_UFM               UFM0
#define UPDATE_STATUS_UFM           UFM1
#define ROT_TYPE                    3
#define UPDATE_STATUS_ADDRESS       0x00
#define CPLD_RELEASE_VERSION        1
#define CPLD_RoT_SVN                1
#define SHA256_DIGEST_LENGTH    32
#define SHA384_DIGEST_LENGTH    48
#define SHA512_DIGEST_LENGTH    64
#define FOUR_BYTE_ADDR_MODE     1
#define SVN_MAX                 63
#define MAX_READ_SIZE           0x1000
#define MAX_WRITE_SIZE          0x1000
#define PAGE_SIZE               0x1000
#define UFM_PAGE_SIZE           16
#define CSK_KEY_SIZE            16
#define ROOT_KEY_SIZE           64
#define ROOT_KEY_X_Y_SIZE_256       32
#define ROOT_KEY_X_Y_SIZE_384       48

enum Ecc_Curve {
	secp384r1 = 1,
	secp256r1,
};

typedef struct {
	uint8_t  ActiveRegion;
	uint8_t  Recoveryregion;
} UPD_REGION;

typedef struct{
	uint8_t CpldStatus;
	uint8_t BmcStatus;
	uint8_t PchStatus;
	UPD_REGION Region[3];
	uint8_t DecommissionFlag;
	uint8_t  CpldRecovery;
	uint8_t  BmcToPchStatus;
	uint8_t  Reserved[4];
} CPLD_STATUS;

#endif // CONFIG_CERBERUS_PFR
