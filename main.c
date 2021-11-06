// This minimal Azure Sphere app repeatedly toggles an LED. Use this app to test that
// installation of the device and SDK succeeded, and that you can build, deploy, and debug an app.

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <hw/demo_appliance.h>

#include "applibs_versions.h"
#include <applibs/eventloop.h>
#include <applibs/gpio.h>
#include <applibs/wificonfig.h>
#include <applibs/log.h>
#include <applibs/networking.h>
#include <applibs/i2c.h>
#include <applibs/powermanagement.h>

#include "i2c.h"
#include "oled.h"
//#include "sd1306.h"
#include "eventloop_timer_utilities.h"
#include "exit_codes.h"

#define JSON_BUFFER_SIZE 512

// Global variable to hold wifi network configuration data
network_var network_data;

float altitude;

// Timer / polling
EventLoop* eventLoop = NULL;
static EventLoopTimer* buttonPollTimer = NULL;

// Variables used to update the polling time between sending telemetry data
EventLoopTimer* sensorPollTimer = NULL;
int readSensorPeriod = SENSOR_READ_PERIOD_SECONDS;

#ifdef OLED_SD1306
static EventLoopTimer* oledUpdateTimer = NULL;
#endif 

#ifdef IOT_HUB_APPLICATION
static EventLoopTimer* azureTimer = NULL;

// Azure IoT poll periods
static const int AzureIoTDefaultPollPeriodSeconds = 1;        // poll azure iot every second
static const int AzureIoTMinReconnectPeriodSeconds = 60;      // back off when reconnecting
static const int AzureIoTMaxReconnectPeriodSeconds = 10 * 60; // back off limit

static int azureIoTPollPeriodSeconds = -1;

// Declare a timer and handler for the force reboot Direct Method
EventLoopTimer* rebootDeviceTimer = NULL;
static void RebootDeviceEventHandler(EventLoopTimer* timer);

#endif // IOT_HUB_APPLICATION

// State variables
static GPIO_Value_Type buttonAState = GPIO_Value_High;
static GPIO_Value_Type buttonBState = GPIO_Value_High;

#ifdef IOT_HUB_APPLICATION
// Usage text for command line arguments in application manifest.
static const char* cmdLineArgsUsageText =
"DPS connection type: \" CmdArgs \": [\"--ConnectionType\", \"DPS\", \"--ScopeID\", "
"\"<scope_id>\"]\n"
#ifdef USE_PNP     
"PnP connection type: \" CmdArgs \": [\"--ConnectionType\", \"PnP\", \"--ScopeID\", "
"\"<scope_id>\"]\n"
#endif     
"Direction connection type: \" CmdArgs \": [\"--ConnectionType\", \"Direct\", "
"\"--Hostname\", \"<azureiothub_hostname>\"]\n "
"IoTEdge connection type: \" CmdArgs \": [\"--ConnectionType\", \"IoTEdge\", "
"\"--Hostname\", \"<iotedgedevice_hostname>\", \"--IoTEdgeRootCAPath\", "
"\"certs/<iotedgedevice_cert_name>\"]\n";
#endif // IOT_HUB_APPLICATION

// Buttons
static int buttonAgpioFd = -1;
static int buttonBgpioFd = -1;

// GPIO File descriptors
int userLedRedFd = -1;
int userLedGreenFd = -1;
int userLedBlueFd = -1;
int appLedFd = -1;
int wifiLedFd = -1;
int clickSocket1Relay1Fd = -1;
int clickSocket1Relay2Fd = -1;

static void ReadWifiConfig(bool);
static void ButtonPollTimerEventHandler(EventLoopTimer* timer);
static bool ButtonStateChanged(int fd, GPIO_Value_Type* oldState);
static void ReadSensorTimerEventHandler(EventLoopTimer* timer);
#ifdef OLED_SD1306
static void UpdateOledEventHandler(EventLoopTimer* timer);
#endif


// forward references
static void TerminationHandler(int signalNumber);
static void ClosePeripheralsAndHandlers(void);

// Termination state
static volatile sig_atomic_t exitCode = ExitCode_Success;

/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
    exitCode = ExitCode_TermHandler_SigTerm;
}

/// <summary>
///     Button timer event:  Check the status of the button
/// </summary>
static void ButtonPollTimerEventHandler(EventLoopTimer* timer)
{
#ifdef IOT_HUB_APPLICATION    
    // Flags used to dertermine if we need to send a telemetry update or not
    bool sendTelemetryButtonA = false;
    bool sendTelemetryButtonB = false;
#endif     

    if (ConsumeEventLoopTimerEvent(timer) != 0) {
        exitCode = ExitCode_ButtonTimer_Consume;
        return;
    }

    if (ButtonStateChanged(buttonAgpioFd, &buttonAState)) {

        if (buttonAState == GPIO_Value_Low) {
            Log_Debug("Button A pressed!\n");
#ifdef IOT_HUB_APPLICATION    	    	
            sendTelemetryButtonA = true;
#endif 
#ifdef OLED_SD1306
            // Use buttonB presses to drive OLED to display the last screen
            oled_state--;

            if (oled_state < 0)
            {
                oled_state = OLED_NUM_SCREEN;
            }

            Log_Debug("OledState: %d\n", oled_state);
#endif 
        }
        else {
            Log_Debug("Button A released!\n");
#ifdef IOT_HUB_APPLICATION    		
            sendTelemetryButtonA = true;
#endif             

        }

    }

    // If the B button has just been pressed/released, send a telemetry message
    // The button has GPIO_Value_Low when pressed and GPIO_Value_High when released
    if (ButtonStateChanged(buttonBgpioFd, &buttonBState)) {

        if (buttonBState == GPIO_Value_Low) {
            Log_Debug("Button B pressed!\n");
#ifdef IOT_HUB_APPLICATION    		
            sendTelemetryButtonB = true;
#endif             

#ifdef OLED_SD1306
            // Use buttonB presses to drive OLED to display the next screen
            oled_state++;

            if (oled_state > OLED_NUM_SCREEN)
            {
                oled_state = 0;
            }
            Log_Debug("OledState: %d\n", oled_state);
#endif 
        }
        else {
            Log_Debug("Button B released!\n");
#ifdef IOT_HUB_APPLICATION    		
            sendTelemetryButtonB = true;
#endif             

        }
    }

#ifdef IOT_HUB_APPLICATION    	

    // If either button was pressed, then enter the code to send the telemetry message
    if (sendTelemetryButtonA || sendTelemetryButtonB) {

        char* pjsonBuffer = (char*)malloc(JSON_BUFFER_SIZE);
        if (pjsonBuffer == NULL) {
            Log_Debug("ERROR: not enough memory to send telemetry");
            exitCode = ExitCode_Button_Telemetry_Malloc_Failed;
        }

        if (sendTelemetryButtonA) {
            // construct the telemetry message  for Button A
            snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonInteger, "buttonA", !buttonAState);
        }

        if (sendTelemetryButtonB) {
            // construct the telemetry message for Button B
            snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonInteger, "buttonB", !buttonBState);

        }

        Log_Debug("\n[Info] Sending telemetry %s\n", pjsonBuffer);
        SendTelemetry(pjsonBuffer, true);
        free(pjsonBuffer);
    }
#endif // IOT_HUB_APPLICATION
}

#ifdef OLED_SD1306
/// <summary>
///     Button timer event:  Check the status of the button
/// </summary>
static void UpdateOledEventHandler(EventLoopTimer* timer)
{

    if (ConsumeEventLoopTimerEvent(timer) != 0) {
        exitCode = ExitCode_ButtonTimer_Consume;
        return;
    }

    // Update/refresh the OLED data
    update_oled();
}

#endif 

/// <summary>
///     Senspr timer event:  Read the sensors
/// </summary>
static void ReadSensorTimerEventHandler(EventLoopTimer* timer)
{

    if (ConsumeEventLoopTimerEvent(timer) != 0) {
        exitCode = ExitCode_ButtonTimer_Consume;
        return;
    }

    acceleration_g = lp_get_acceleration();
    Log_Debug("\nLSM6DSO: Acceleration [g]  : %.4lf, %.4lf, %.4lf\n", acceleration_g.x, acceleration_g.y, acceleration_g.z);

    angular_rate_dps = lp_get_angular_rate();
    Log_Debug("LSM6DSO: Angular rate [dps] : %4.2f, %4.2f, %4.2f\n", angular_rate_dps.x, angular_rate_dps.y, angular_rate_dps.z);

    lsm6dso_temperature = lp_get_temperature();
    Log_Debug("LSM6DSO: Temperature1 [degC]: %.2f\n", lsm6dso_temperature);

    if (lps22hhDetected) {
        pressure_kPa = lp_get_pressure();
        lps22hh_temperature = lp_get_temperature_lps22h();

        Log_Debug("LPS22HH: Pressure     [kPa] : %.2f\n", pressure_kPa);
        Log_Debug("LPS22HH: Temperature2 [degC]: %.2f\n", lps22hh_temperature);
    }
    // LPS22HH was not detected
    else {
        Log_Debug("LPS22HH: Pressure     [kPa] : Not read!\n");
        Log_Debug("LPS22HH: Temperature  [degC]: Not read!\n");
    }

#ifdef M4_INTERCORE_COMMS
    Log_Debug("ALSPT19: Ambient Light[Lux] : %.2f\r\n", light_sensor);
#endif

    // Read the current wifi configuration
    ReadWifiConfig(false);

    // The ALTITUDE value calculated is actually "Pressure Altitude". This lacks correction for
    // temperature (and humidity)
    // "pressure altitude" calculator located at:
    // https://www.weather.gov/epz/wxcalc_pressurealtitude "pressure altitude" formula is defined
    // at: https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf altitude in feet =
    // 145366.45 * (1 - (hPa / 1013.25) ^ 0.190284) feet altitude in meters = 145366.45 * 0.3048 *
    // (1 - (hPa / 1013.25) ^ 0.190284) meters
    //
    // weather.com formula
    // altitude = 44307.69396 * (1 - powf((atm / 1013.25), 0.190284));  // pressure altitude in
    // meters
    // Bosch's formula
    altitude = 44330 * (1 - powf(((float)(pressure_kPa * 1000 / 1013.25)),
        (float)(1 / 5.255))); // pressure altitude in meters

#ifdef IOT_HUB_APPLICATION
#ifdef USE_IOT_CONNECT
    // If we have not completed the IoTConnect connect sequence, then don't send telemetry
    if (IoTCConnected) {
#endif         

        // Keep track of the first time through this code.  The LSMD6S0 returns bad data the first time
        // we read it.  Don't send that data up in case we're charting the data.
        static bool firstPass = true;

        if (!firstPass) {

            // Allocate memory for a telemetry message to Azure
            char* pjsonBuffer = (char*)malloc(JSON_BUFFER_SIZE);
            if (pjsonBuffer == NULL) {
                Log_Debug("ERROR: not enough memory to send telemetry");
            }

            // {"gX":%.2lf, "gY":%.2lf, "gZ":%.2lf, "aX": %.2f, "aY": " "%.2f,
            //  "aZ": %.2f, "pressure": %.2f, "light_intensity": %.2f, "
            //  "altitude": %.2f, "temp": %.2f,  "rssi": %d}

            snprintf(pjsonBuffer, JSON_BUFFER_SIZE,
                "{\"gX\":%.2lf, \"gY\":%.2lf, \"gZ\":%.2lf, \"aX\": %.2f, \"aY\": "
                "%.2f, \"aZ\": %.2f, \"pressure\": %.2f, \"light_intensity\": %.2f, "
                "\"altitude\": %.2f, \"temp\": %.2f,  \"rssi\": %d}",
                acceleration_g.x, acceleration_g.y, acceleration_g.z, angular_rate_dps.x,
                angular_rate_dps.y, angular_rate_dps.z, pressure_kPa, light_sensor, altitude,
                lsm6dso_temperature, network_data.rssi);

            Log_Debug("\n[Info] Sending telemetry: %s\n", pjsonBuffer);
            SendTelemetry(pjsonBuffer, true);
            free(pjsonBuffer);
        }
        else {
            // It is the first pass, flip the flag
            firstPass = false;

            // On the first pass set the OLED screen to the Avnet graphic!
#ifdef OLED_SD1306
            oled_state = OLED_NUM_SCREEN;
#endif 

        }
#ifdef USE_IOT_CONNECT        
    }
#endif // USE_IOT_CONNECT
#endif // IOT_HUB_APPLICATION    
}

/// <summary>
///     Set up SIGTERM termination handler, initialize peripherals, and set up event handlers.
/// </summary>
/// <returns>
///     ExitCode_Success if all resources were allocated successfully; otherwise another
///     ExitCode value which indicates the specific failure.
/// </returns>
static ExitCode InitPeripheralsAndHandlers(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

    eventLoop = EventLoop_Create();
    if (eventLoop == NULL) {
        Log_Debug("Could not create event loop.\n");
        return ExitCode_Init_EventLoop;
    }

    // Open SAMPLE_BUTTON_1 GPIO as input (ButtonA)
    Log_Debug("Opening SAMPLE_BUTTON_1 as input.\n");
    buttonAgpioFd = GPIO_OpenAsInput(SAMPLE_BUTTON_1);
    if (buttonAgpioFd == -1) {
        Log_Debug("ERROR: Could not open SAMPLE_BUTTON_1: %s (%d).\n", strerror(errno), errno);
        return ExitCode_Init_ButtonA;
    }

    // Open SAMPLE_BUTTON_2 GPIO as input (ButtonB)
    Log_Debug("Opening SAMPLE_BUTTON_2 as input.\n");
    buttonBgpioFd = GPIO_OpenAsInput(SAMPLE_BUTTON_2);
    if (buttonBgpioFd == -1) {
        Log_Debug("ERROR: Could not open SAMPLE_BUTTON_2: %s (%d).\n", strerror(errno), errno);
        return ExitCode_Init_ButtonB;
    }

    // Set up a timer to poll for button events.
    static const struct timespec buttonPressCheckPeriod = { .tv_sec = 0, .tv_nsec = 1000 * 1000 };
    buttonPollTimer = CreateEventLoopPeriodicTimer(eventLoop, &ButtonPollTimerEventHandler,
        &buttonPressCheckPeriod);
    if (buttonPollTimer == NULL) {
        return ExitCode_Init_ButtonPollTimer;
    }

#ifdef OLED_SD1306
    // Set up a timer to drive quick oled updates.
    static const struct timespec oledUpdatePeriod = { .tv_sec = 0, .tv_nsec = 100 * 1000 * 1000 };
    oledUpdateTimer = CreateEventLoopPeriodicTimer(eventLoop, &UpdateOledEventHandler,
        &oledUpdatePeriod);
    if (oledUpdateTimer == NULL) {
        return ExitCode_Init_OledUpdateTimer;
    }
#endif 

    // Iterate across all the device twin items and open any File Descriptors
    //deviceTwinOpenFDs();

    // Set up a timer to poll the sensors.  SENSOR_READ_PERIOD_SECONDS is defined in CMakeLists.txt
    static const struct timespec readSensorPeriod = { .tv_sec = SENSOR_READ_PERIOD_SECONDS,
                                                     .tv_nsec = SENSOR_READ_PERIOD_NANO_SECONDS };
    sensorPollTimer = CreateEventLoopPeriodicTimer(eventLoop, &ReadSensorTimerEventHandler, &readSensorPeriod);
    if (sensorPollTimer == NULL) {
        return ExitCode_Init_sensorPollTimer;
    }

#ifdef IOT_HUB_APPLICATION
    // Initialize the direct method handler
    ExitCode result = InitDirectMethods();
    if (result != ExitCode_Success) {
        return result;
    }

    azureIoTPollPeriodSeconds = AzureIoTDefaultPollPeriodSeconds;
    struct timespec azureTelemetryPeriod = { .tv_sec = azureIoTPollPeriodSeconds, .tv_nsec = 0 };
    azureTimer =
        CreateEventLoopPeriodicTimer(eventLoop, &AzureTimerEventHandler, &azureTelemetryPeriod);
    if (azureTimer == NULL) {
        return ExitCode_Init_AzureTimer;
    }

    // Setup the halt application handler and timer.  This is disarmed and will only fire
    // if we receive a halt application direct method call
    rebootDeviceTimer = CreateEventLoopDisarmedTimer(eventLoop, RebootDeviceEventHandler);
#endif // IOT_HUB_APPLICATION

#ifdef USE_IOT_CONNECT
    if (IoTConnectInit() != ExitCode_Success) {
        return ExitCode_Init_IoTCTimer;
    }
#endif

    // Initialize the i2c sensors
    lp_imu_initialize();

#ifdef M4_INTERCORE_COMMS
    //// ADC connection

    // Open connection to real-time capable application.
    sockFd = Application_Connect(rtAppComponentId);
    if (sockFd == -1)
    {
        Log_Debug("ERROR: Unable to create socket: %d (%s)\n", errno, strerror(errno));
        Log_Debug("Real Time Core disabled or Component Id is not correct.\n");
        Log_Debug("The program will continue without showing light sensor data.\n");
        // Communication with RT core error
        RTCore_status = 1;
    }
    else
    {
        // Communication with RT core success
        RTCore_status = 0;
        // Set timeout, to handle case where real-time capable application does not respond.
        static const struct timeval recvTimeout = { .tv_sec = 5,.tv_usec = 0 };
        int result = setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout));
        if (result == -1)
        {
            Log_Debug("ERROR: Unable to set socket timeout: %d (%s)\n", errno, strerror(errno));
            return -1;
        }

        // Register handler for incoming messages from real-time capable application.
        rtAppEventReg = EventLoop_RegisterIo(eventLoop, sockFd, EventLoop_Input, SocketEventHandler, NULL);
        if (rtAppEventReg == NULL) {
            return ExitCode_Init_RegisterIo;
        }

        // Register one second timer to send a message to the real-time core.
        static const struct timespec M4PollPeriod = { .tv_sec = M4_READ_PERIOD_SECONDS,.tv_nsec = M4_READ_PERIOD_NANO_SECONDS };
        M4PollTimer = CreateEventLoopPeriodicTimer(eventLoop, &M4TimerEventHandler, &M4PollPeriod);
        if (M4PollTimer == NULL) {
            return ExitCode_Init_Rt_PollTimer;
        }
    }

    //// end ADC Connection
#endif 

    return ExitCode_Success;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    DisposeEventLoopTimer(buttonPollTimer);
    DisposeEventLoopTimer(sensorPollTimer);


#ifdef M4_INTERCORE_COMMS    
    DisposeEventLoopTimer(M4PollTimer);
#endif 
#ifdef OLED_SD1306
    DisposeEventLoopTimer(oledUpdateTimer);
#endif 
#ifdef IOT_HUB_APPLICATION    
    DisposeEventLoopTimer(azureTimer);

    // Cleanup andy resources allocated by the direct method handlers
    CleanupDirectMethods();

#endif // IOT_HUB_APPLICATION

    EventLoop_Close(eventLoop);

    //Log_Debug("Closing file descriptors\n");
    //CloseFdAndPrintError(buttonAgpioFd, "ButtonA Fd");
    //CloseFdAndPrintError(buttonBgpioFd, "ButtonB Fd");

    //// Close all the FD's associated with device twins
    //deviceTwinCloseFDs();

    // Close the i2c interface
    lp_imu_close();
}

/// <summary>
///     Check whether a given button has just been pressed/released.
/// </summary>
/// <param name="fd">The button file descriptor</param>
/// <param name="oldState">Old state of the button (pressed or released)</param>
/// <returns>true if button state has changed, false otherwise</returns>
static bool ButtonStateChanged(int fd, GPIO_Value_Type* oldState)
{
    bool didButtonStateChange = false;
    GPIO_Value_Type newState;
    int result = GPIO_GetValue(fd, &newState);
    if (result != 0) {
        Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
        exitCode = ExitCode_IsButtonPressed_GetValue;
    }
    else {
        // Button is pressed if it is low and different than last known state.
        didButtonStateChange = (newState != *oldState);
        *oldState = newState;
    }

    return didButtonStateChange;
}

// Read the current wifi configuration, output it to debug and send it up as device twin data
static void ReadWifiConfig(bool outputDebug)
{
    char bssid[20];
#ifdef IOT_HUB_APPLICATION        
    static bool ssidChanged = false;
#endif     

    WifiConfig_ConnectedNetwork network;
    int result = WifiConfig_GetCurrentNetwork(&network);

    if (result < 0)
    {
        // Log_Debug("INFO: Not currently connected to a WiFi network.\n");
        strncpy(network_data.SSID, "Not Connected", 20);
        network_data.frequency_MHz = 0;
        network_data.rssi = 0;
    }
    else
    {
        network_data.frequency_MHz = network.frequencyMHz;
        network_data.rssi = network.signalRssi;
        snprintf(bssid, JSON_BUFFER_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
            network.bssid[0], network.bssid[1], network.bssid[2],
            network.bssid[3], network.bssid[4], network.bssid[5]);

        // Check to see if the SSID changed, if so update the SSID and send updated device twins properties
        if (strncmp(network_data.SSID, (char*)&network.ssid, network.ssidLength) != 0)
        {
#ifdef IOT_HUB_APPLICATION    
            // Set the flag to send ssid changes to the IoTHub
            ssidChanged = true;
#endif             

            // Clear the ssid array
            memset(network_data.SSID, 0, WIFICONFIG_SSID_MAX_LENGTH);
            strncpy(network_data.SSID, network.ssid, network.ssidLength);
        }

#ifdef IOT_HUB_APPLICATION
        if ((iothubClientHandle != NULL) && (ssidChanged)) {
            // Note that we send up this data to Azure if it changes, but the IoT Central Properties elements only 
            // show the data that was currenet when the device first connected to Azure.
            checkAndUpdateDeviceTwin("ssid", &network_data.SSID, TYPE_STRING, false);
            checkAndUpdateDeviceTwin("freq", &network_data.frequency_MHz, TYPE_INT, false);
            checkAndUpdateDeviceTwin("bssid", &bssid, TYPE_STRING, false);

            // Reset the flag 
            ssidChanged = false;
        }
#endif

        if (outputDebug) {
            Log_Debug("SSID: %s\n", network_data.SSID);
            Log_Debug("Frequency: %dMHz\n", network_data.frequency_MHz);
            Log_Debug("bssid: %s\n", bssid);
            Log_Debug("rssi: %d\n", network_data.rssi);
        }
    }
}

int main(void)
{
    Log_Debug("\nVisit https://github.com/Azure/azure-sphere-samples for extensible samples to use as a starting point for full applications.\n");

    ReadWifiConfig(true);

	exitCode = InitPeripheralsAndHandlers();

    const int fdRed = GPIO_OpenAsOutput(SAMPLE_RGBLED_RED, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fdRed < 0) {
        Log_Debug("Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n", strerror(errno), errno);
        return ExitCode_Init_Led;
    }
    const int fdGreen = GPIO_OpenAsOutput(SAMPLE_RGBLED_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fdGreen < 0) {
        Log_Debug("Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n", strerror(errno), errno);
        return ExitCode_Init_Led;
    }
    const int fdBlue = GPIO_OpenAsOutput(SAMPLE_RGBLED_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fdBlue < 0) {
        Log_Debug("Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n", strerror(errno), errno);
        return ExitCode_Init_Led;
    }

    //const struct timespec sleepTime = {.tv_sec = 1, .tv_nsec = 0};
    //while (exitCode == ExitCode_Success)
    //{
    //    GPIO_SetValue(fdRed, GPIO_Value_Low);
    //    nanosleep(&sleepTime, NULL);
    //    GPIO_SetValue(fdRed, GPIO_Value_High);
    //    nanosleep(&sleepTime, NULL);

    //	GPIO_SetValue(fdGreen, GPIO_Value_Low);
    //    nanosleep(&sleepTime, NULL);
    //    GPIO_SetValue(fdGreen, GPIO_Value_High);
    //    nanosleep(&sleepTime, NULL);

    //    GPIO_SetValue(fdBlue, GPIO_Value_Low);
    //    nanosleep(&sleepTime, NULL);
    //    GPIO_SetValue(fdBlue, GPIO_Value_High);
    //    nanosleep(&sleepTime, NULL);
    //}

	// Main loop
	while (exitCode == ExitCode_Success)
    {
        //GPIO_SetValue(fdRed, GPIO_Value_Low);

    	EventLoop_Run_Result result = EventLoop_Run(eventLoop, -1, true);
        // Continue if interrupted by signal, e.g. due to breakpoint being set.
        if (result == EventLoop_Run_Failed && errno != EINTR) {
            exitCode = ExitCode_Main_EventLoopFail;
        }

    	//GPIO_SetValue(fdRed, GPIO_Value_High);
    }

    ClosePeripheralsAndHandlers();

    Log_Debug("Application exiting.\n");

    //if (exitCode == ExitCode_TriggerReboot_Success) {
    //    TriggerReboot();
    //}

    return exitCode;
}
