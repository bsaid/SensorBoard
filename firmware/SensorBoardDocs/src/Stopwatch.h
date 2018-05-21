/*
 * Timer.h
 *
 */

#ifndef MAIN_STOPWATCH_H_
#define MAIN_STOPWATCH_H_

#include <esp_types.h>
#include "driver/timer.h"

#include <esp_types.h>
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class Stopwatch
{
	uint32_t interval = 0;
	uint32_t cycles = 0;

	timer_group_t timer_group = TIMER_GROUP_0;
	timer_idx_t timer_idx = TIMER_0;
	timer_config_t config;

	/**
	 * Initialize stopwatch.
	 */
	void init()
	{
		config.alarm_en = 0;
		config.auto_reload = 0;
		config.counter_dir = TIMER_COUNT_UP;
		config.divider = 16;
		config.intr_type = TIMER_INTR_LEVEL;
		config.counter_en = TIMER_PAUSE;
		// Configure timer
		timer_init(timer_group, timer_idx, &config);
		reset();
	}

public:
	/**
	 * Constructor.
	 * @param ms Set one lap time
	 */
	Stopwatch(uint32_t ms)
	{
		init();
		setInterval(ms);
	}

	Stopwatch()
	{
		init();
	}

	/**
	 * Reset the stopwatch.
	 */
	void reset()
	{
		// Stop timer counter
		timer_pause(timer_group, timer_idx);
		// Load counter value
		timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
		// Start timer counter
		timer_start(timer_group, timer_idx);

		cycles = 0;
	}

	/**
	 * Get time in milliseconds.
	 * @return milliseconds
	 */
	uint32_t get_ms()
	{
		double sec = 0;
		timer_get_counter_time_sec(timer_group, timer_idx, &sec);
		return 1000*sec;
	}

	/**
	 * Set one lap watch dog interval
	 * @param ms [description]
	 */
	void setInterval(uint32_t ms)
	{
		interval = ms;
	}

	/**
	 * Wait for the next lap.
	 * @return number of milliseconds of waiting
	 */
	int waitForNext()
	{
		cycles++;
		if(get_ms() > interval*cycles+4)
			return (get_ms() - interval*cycles);
		while(get_ms()+4 < interval*cycles)
		{
			int diff = interval*cycles - get_ms();
			if(diff<5)
				diff = 0;
			vTaskDelay( diff / portTICK_RATE_MS );
		}
		return 0;
	}

	/**
	 * Get number of past laps.
	 * @return number of past laps
	 */
	uint32_t getCycles()
	{
		return cycles;
	}
};

#endif /* MAIN_STOPWATCH_H_ */
