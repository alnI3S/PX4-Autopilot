/****************************************************************************
 *
 *   Copyright (C) 2020 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include <nuttx/spi/spi.h>
#include <px4_platform_common/px4_manifest.h>
//                                                              KiB BS    nB
static const px4_mft_device_t spi1 = {  // winbond W25N01GV Nand Flash: 1024 blocks of 128 KiB
	.bus_type = px4_mft_device_t::SPI,
	.devid    = SPIDEV_FLASH(0)
};

/* Partition mapped on the W25N01 SPI NAND:
 * Use the same partition type as before (MTD_PARAMETERS) and expose
 * the number of blocks in units compatible with CONFIG_RAMTRON_EMULATE_SECTOR_SHIFT.
 *
 * Total size = 128 MiB = 131072 KiB
 */
static const px4_mtd_entry_t w25n01_flash = {
    .device = &spi1,
    .npart = 8,
    .partd = {
        {
            .type = MTD_PARAMETERS,
            .path = "/fs/mtd_params",
            /* There is confusion terms mtd device block vs w25n01 page.
			 * So nblocks here is npages for w25n01.
			 * Total device erase blocks 65536 = 1024 blocks * 64 pages.
			 * device blocksize here is 2 KiB (w25n01 page size).
             */
            .nblocks = 64	// 1 block = 64 pages = 128 KiB
        },
		{
			.type = MTD_MFT_VER,
			.path = "/fs/mtd_mft_ver",
			.nblocks = 64
		},
		{
			.type = MTD_NET,
			.path = "/fs/mtd_net",
			.nblocks = 64

		},
		{
			.type = MTD_CALDATA,
			.path = "/fs/mtd_caldata",
			.nblocks = 64
		},
		{
			.type = MTD_MFT_REV,
			.path = "/fs/mtd_mft_rev",
			.nblocks = 64
		},
		{
			.type = MTD_ID,
			.path = "/fs/mtd_id",
			.nblocks = 64
		},
		{
			.type = MTD_WAYPOINTS,
			.path = "/fs/mtd_waypoints",
            .nblocks = 1024	// 2 MiB
		},
		{
            .type = MTD_PARAMETERS,
            .path = "/fs/mtd_storage",
            .nblocks = 62848	// 122 MiB
        }
		/* we reserve 1280=20*64 blocks (20 w25n blocks) for bad blocks and
		 * future use */
    },
};

static const px4_mtd_manifest_t board_mtd_config = {
    .nconfigs   = 1,
    .entries = {
        &w25n01_flash,
    }
};

static const px4_mft_entry_s mtd_mft = {
    .type = MTD,
    .pmft = (void *) &board_mtd_config,
};

static const px4_mft_s mft = {
    .nmft = 1,
    .mfts = {
        &mtd_mft
    }
};

const px4_mft_s *board_get_manifest(void)
{
    return &mft;
}
