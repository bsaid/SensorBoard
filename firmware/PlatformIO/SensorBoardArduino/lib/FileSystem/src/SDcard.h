/*
 * SDcard.h
 *
 */

#ifndef MAIN_SDCARD_H_
#define MAIN_SDCARD_H_

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <string>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "freertos/task.h"

class SDcard
{
	const std::string CARD_NAME = "/sdcard";
	const bool FORMAT_IF_MOUNTED = false;
	const int MAX_FILES = 5;
	const bool ONE_BIT_INTERFACE = false;
	const char *TAG = "example";

	bool initializedCard = false;

public:
	SDcard()
	{
		ESP_LOGI(TAG, "Initializing SD card");
		ESP_LOGI(TAG, "Using SDMMC peripheral");

		sdmmc_host_t host = SDMMC_HOST_DEFAULT();
		if(ONE_BIT_INTERFACE)
			host.flags = SDMMC_HOST_FLAG_1BIT;

		// This initializes the slot without card detect (CD) and write protect (WP) signals.
		sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

		// If format_if_mount_failed is set to true, SD card will be partitioned and
		// formatted in case when mounting fails.
		esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = FORMAT_IF_MOUNTED,
			.max_files = MAX_FILES
		};

		sdmmc_card_t* card;
		esp_err_t ret = esp_vfs_fat_sdmmc_mount(CARD_NAME.c_str(), &host, &slot_config, &mount_config, &card);

		initializedCard = (ret == ESP_OK);
		if(initializedCard) {
			// Card has been initialized, print its properties
			sdmmc_card_print_info(stdout, card);
		} else {
			if (ret == ESP_FAIL) {
				ESP_LOGE(TAG, "Failed to mount filesystem.");
			} else {
				ESP_LOGE(TAG, "Failed to initialize the card (%d). "
					"Make sure SD card lines have pull-up resistors in place.", ret);
			}
		}
	}

	bool isInitialized()
	{
		return initializedCard;
	}

	FILE* getFile(std::string name, std::string attributes)
	{
		return fopen((CARD_NAME + "/" + name).c_str(), attributes.c_str());
	}

	// fopen, fclose, fprintf, fscanf, getc, fgets, rename, copy

	~SDcard()
	{
		esp_vfs_fat_sdmmc_unmount();
		ESP_LOGI(TAG, "Card unmounted");
	}
};

#endif // MAIN_SDCARD_H_
