/*
 * Timer.h
 *
 */

#include "test.h"

extern "C" {
	void app_main(void);
}

void app_main()
{
	vTaskDelay(8);
	Stopwatch s(100);
	for(;;)
	{
		printf("A: %d; ", s.get_ms());
		vTaskDelay(5);
		printf("B: %d; ", s.get_ms());
		printf("C: %d; ", s.waitForNext());
		printf("D: %d\n", s.get_ms());
	}
}
