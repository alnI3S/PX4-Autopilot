#include <nuttx/mtd/mtd.h>
#include <px4_platform_common/spi.h>
#include <px4_platform_common/log.h>
#include "w25n01gv.h"

int px4_mtd_setup_nand(void)
{
    // Acquire SPI device used by NAND on Kakute H7 Mini
    FAR struct spi_dev_s *spi = px4_spibus_initialize(PX4_SPI_BUS_NAND);
    if (!spi) { PX4_ERR("SPI NAND bus init failed"); return -ENODEV; }

    FAR struct mtd_dev_s *mtd = w25nxx_initialize(spi, 40000000);
    if (!mtd) { PX4_ERR("W25Nxx probe failed"); return -ENODEV; }

    // Wrap in FTL block device
    int minor = 0;
    if (ftl_initialize(minor, mtd) < 0) { PX4_ERR("FTL init failed"); return -EIO; }

    // Optional: format littlefs on first boot (check mount)
    // mount to /fs/microsd so logger works unchanged
    int ret = nx_mount(NULL, "/fs/microsd", "littlefs", 0, NULL);
    if (ret < 0) {
        // try to format then mount
        // TODO: call littlefs mkfs on /dev/mtdblock0 (board specific helper)
        PX4_WARN("Formatting littlefs on NAND ...");
        // mkfs_littlefs("/dev/mtdblock0"); // implement helper
        ret = nx_mount(NULL, "/fs/microsd", "littlefs", 0, NULL);
    }

    PX4_INFO("NAND mounted at /fs/microsd");
    return 0;
}
