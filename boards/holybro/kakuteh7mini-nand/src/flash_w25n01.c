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

/**
 * @file flash_w25n01.c
 *
 * Board-specific external flash W25N01GV functions.
 */
#include "board_config.h"
#include "qspi.h"
#include "arm_internal.h"
#include <assert.h>
#include <debug.h>

#include <nuttx/config.h>

#include <nuttx/drivers/drivers.h>
#include <nuttx/fs/fs.h>
#include <nuttx/fs/fat.h>

#if defined(CONFIG_MTD)
#include <nuttx/mtd/mtd.h>
#endif

#if defined(CONFIG_STM32H7_QUADSPI)
#include <nuttx/spi/qspi.h>
#include "qspi.h" // cf sam_qspi.h
#endif


/* This type represents the state of the MTD device.  The struct mtd_dev_s must
 * appear at the beginning of the definition so that you can freely cast between
 * pointers to struct mtd_dev_s and struct n25qxxx_dev_s.
 */

// struct w25n01gv_dev_s w25n01_dev;
// struct qspi_dev_s *qspi;

/************************************************************************************
 * Public Functions
 ************************************************************************************/

void flash_w25n01_init(void)
{
	struct qspi_dev_s *qspi;
	/* Get the SPI port */
    syslog(LOG_INFO, "[boot] Initializing QuadSPI port 0\n");
	// int ret = OK;
	qspi = stm32h7_qspi_initialize(0); // only one QSPI interface on KakuteH7Mini
	if (!qspi) {
		syslog(LOG_ERR, "[boot] ERROR: Failed to initialize SPI port 1\n");
		// led_on(LED_BLUE);
		return;
	} else {
		struct mtd_dev_s *mtd;
		syslog(LOG_INFO, "[boot] Binding QSPI to the W25N01GV MTD driver\n");

		mtd = w25n01gv_initialize(qspi, true);

		if (!mtd) {
			syslog(LOG_ERR, "[boot] ERROR: Failed to bind QSPI port 0 to the W25N01GV MTD driver\n");
			// led_on(LED_BLUE);
			return;
		} else {
			syslog(LOG_INFO, "[boot] Successfully bound QSPI port 0 to the W25N01GV MTD driver\n");

			/* Initialize FTL */
			int ret = ftl_initialize(0, mtd);
			if (ret < 0) {
				syslog(LOG_ERR, "[boot] ERROR: Failed to initialize the FTL layer: %d\n", ret);
				// led_on(LED_BLUE);
				return;
			}
		}
	}

}
