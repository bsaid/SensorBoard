#include "catch.hpp"
#include "test.h"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

//TEST_CASE( "Factorials are computed", "[factorial]" ) {
//    REQUIRE( Factorial(1) == 1 );
//    REQUIRE( Factorial(2) == 2 );
//    REQUIRE( Factorial(3) == 6 );
//    REQUIRE( Factorial(10) == 3628800 );
//}

extern "C" {
	void app_main(void);
}

void app_main()
{
	WiFi wifi;
	wifi.wifi_init_softap();
	wifi.create_tcp_server();
	for(;;)
	{
		wifi.send_data();
		vTaskDelay(100);
	}

	// Self-test
	//int result = Catch::Session().run();
	//TODO: handle test results

	/*vTaskDelay(200);
	Stopwatch s(10);
	for(;;)
	{
		printf("A: %d; ", s.get_ms());
		vTaskDelay(1);
		printf("B: %d; ", s.get_ms());
		printf("C: %d; ", s.waitForNext());
		printf("D: %d\n", s.get_ms());
	}*/
}
