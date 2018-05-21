#include <Arduino.h>

#include <FileSystem.h>
#include <GlobalSettings.h>

#include "WiFi.h"
#include "RuntimeSettings.h"
#include "ComputingTask.h"

// Filesystem initialization
extern FileSystem fs;
extern GlobalSettings settings;
extern RuntimeSettings machineState;
extern QueueHandle_t queue;

/**
 * Manages incomming connection to TCP server.
 * The function is called as FreeRTOS thread.
 * @param param client socket
 */
void manage_connection(void *param)
{
	int cs = *((int*)param);
	char recv_buf[8];
	while(true)
	{
		int r = recv(cs, recv_buf, sizeof(recv_buf)-1,0);
		// Any command received ftom TCP client
		if(r>0)
		{
			char ch = recv_buf[0];
			if(ch == 'r')
				esp_restart();
			if(ch == 'p')
				write(cs , "Ping received.\n" , strlen("Ping received.\n"));
			if(ch == 'h')
			{
				machineState.isHorseAnalysis = !machineState.isHorseAnalysis;
				const char msgT[] = "Horse analysis started.\n";
				const char msgF[] = "Horse analysis stopped.\n";
				if(machineState.isHorseAnalysis)
					write(cs , msgT , strlen(msgT));
				else
					write(cs , msgF , strlen(msgF));
			}
			if(ch == 's')
			{
				machineState.isStreaming = !machineState.isStreaming;
				const char msgT[] = "Streaming started.\n";
				const char msgF[] = "Streaming stopped.\n";
				if(machineState.isStreaming)
					write(cs , msgT , strlen(msgT));
				else
					write(cs , msgF , strlen(msgF));
			}
			if(ch == 'l')
			{
				fs.increaseLogCounter();
				machineState.isLogging = !machineState.isLogging;
				char msgT[] = "Logging started.\n";
				char msgF[] = "Logging stopped.\n";
				if(machineState.isLogging)
					write(cs , msgT , strlen(msgT));
				else
					write(cs , msgF , strlen(msgF));
			}
		}
		// Process queue of data awaiting to send to client
		else if(uxQueueMessagesWaiting(queue) > 0)
		{
			char element;
			xQueueReceive(queue, &element, portMAX_DELAY);
			write(cs , &element , sizeof(char));
		}
		else
			vTaskDelay(10);
	}
	close(cs);
}

/**
 * Setup is like main function here. The main function calles so many
 * initialization procedures.
 */
void setup()
{
	printf("Started...\n");
	queue = xQueueCreate( 1024, sizeof(char) );

	// Filesystem initialization
	fs.init();
	settings.loadFile(fs.getConfigFile());

	// Runtime configuration
	machineState.isLogging = settings.startLoggingImmediately;
	machineState.isStreaming = settings.isStreaming;

	// Create only one periodic task for reading the sensors and computing data from them
	xTaskCreatePinnedToCore(
	    dataLoopTask,   // Function to implement the task
	    "dataLoopTask", // Name of the task
	    10000,          // Stack size in words
	    NULL,           // Task input parameter
	    1,              // Priority of the task
	    NULL,           // Task handle.
	    0               // Core where the task should run, other than main loop
	);

 	printf("Computing task created...\n");

	// WiFi connection initialization and TCP server FreeRTOS (sporadic) task creation
	WiFi wifi(settings.isWiFiClient, settings.isWiFiAP, settings.WiFiSSID, settings.WiFiPassword);

    // Enter main loop, listens to USB connection
	for(;;)
	{
		// Read and process commands
		if(!feof(stdin))
		{
			char ch = getchar();
			if(ch == 'r')
				break;
			if(ch == 'p')
				printf("Ping received.\n");
			if(ch == 'h')
				machineState.isHorseAnalysis = !machineState.isHorseAnalysis;
			if(ch == 's')
				machineState.isStreaming = !machineState.isStreaming;
			if(ch == 'l')
			{
				fs.increaseLogCounter();
				machineState.isLogging = !machineState.isLogging;
			}
		}
		else
			vTaskDelay(10);
	}

	printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

// Just for feeding Arduino
void loop() {}
