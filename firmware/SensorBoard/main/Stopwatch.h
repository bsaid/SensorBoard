/*
 * Timer.h
 *
 */

#ifndef MAIN_STOPWATCH_H_
#define MAIN_STOPWATCH_H_

#include <esp_types.h>
#include "driver/timer.h"

class Timer
{
	uint32_t interval = 0;

	timer_group_t timer_group = TIMER_GROUP_0;
	timer_idx_t timer_idx = TIMER_0;
	timer_config_t config;

	void init()
	{
		config.alarm_en = 0;
		config.auto_reload = 0;
		config.counter_dir = TIMER_COUNT_UP;
		config.divider = 16;
		config.intr_type = 16;
		config.counter_en = TIMER_PAUSE;
		/*Configure timer*/
		timer_init(timer_group, timer_idx, &config);
		reset();
	}

public:
	Timer(uint32_t us)
	{
		init();
		setInterval(us);
	}

	Timer()
	{
		init();
	}

	void reset()
	{
		/*Stop timer counter*/
		timer_pause(timer_group, timer_idx);
		/*Load counter value */
		timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
		/*Start timer counter*/
		timer_start(timer_group, timer_idx);
	}

	uint32_t getMilliseconds()
	{
		//uint64_t timer_val;
		//timer_get_counter_value(evt.group, evt.idx, &timer_val);
		return 0;
	}

	void setInterval(uint32_t us)
	{
		interval = us;
	}

	bool waitForNext()
	{
		//
		return false;
	}
};

#endif /* MAIN_STOPWATCH_H_ */
