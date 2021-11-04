// This minimal Azure Sphere app repeatedly toggles an LED. Use this app to test that
// installation of the device and SDK succeeded, and that you can build, deploy, and debug an app.

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/gpio.h>
#include <applibs/i2c.h>

#include "SSD1306.h"

// The following #include imports a "template appliance" definition. This app comes with multiple
// implementations of the template appliance, each in a separate directory, which allow the code
// to run unchanged on different hardware.
//
// By default, this app targets hardware that follows the MT3620 Reference Development Board (RDB)
// specification, such as the MT3620 Dev Kit from Seeed Studio.
//
// To target different hardware, you'll need to update CMakeLists.txt.  For example, to target the
// Avnet MT3620 Starter Kit, make this update: azsphere_target_hardware_definition(${PROJECT_NAME}
// TARGET_DIRECTORY "HardwareDefinitions/avnet_mt3620_sk" TARGET_DEFINITION
// "template_appliance.json")
//
// See https://aka.ms/AzureSphereHardwareDefinitions for more details.
#include <signal.h>
#include <hw/demo_appliance.h>

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,
    ExitCode_TermHandler_SigTerm,
    ExitCode_Main_Led
} ExitCode;

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

    return ExitCode_Success;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    /*
    DisposeEventLoopTimer(accelTimer);
    EventLoop_Close(eventLoop);

    Log_Debug("Closing file descriptors.\n");
    CloseFdAndPrintError(i2cFd, "i2c");
    */
}

int main(void)
{
    Log_Debug("\nVisit https://github.com/Azure/azure-sphere-samples for extensible samples to use as a starting point for full applications.\n");
    exitCode = InitPeripheralsAndHandlers();

    const int fdRed = GPIO_OpenAsOutput(SAMPLE_RGBLED_RED, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fdRed < 0) {
        Log_Debug("Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n", strerror(errno), errno);
        return ExitCode_Main_Led;
    }
    const int fdGreen = GPIO_OpenAsOutput(SAMPLE_RGBLED_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fdGreen < 0) {
        Log_Debug("Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n", strerror(errno), errno);
        return ExitCode_Main_Led;
    }
    const int fdBlue = GPIO_OpenAsOutput(SAMPLE_RGBLED_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fdBlue < 0) {
        Log_Debug("Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n", strerror(errno), errno);
        return ExitCode_Main_Led;
    }

    const struct timespec sleepTime = {.tv_sec = 1, .tv_nsec = 0};
    while (exitCode == ExitCode_Success)
    {
        GPIO_SetValue(fdRed, GPIO_Value_Low);
        nanosleep(&sleepTime, NULL);
        GPIO_SetValue(fdRed, GPIO_Value_High);
        nanosleep(&sleepTime, NULL);

    	GPIO_SetValue(fdGreen, GPIO_Value_Low);
        nanosleep(&sleepTime, NULL);
        GPIO_SetValue(fdGreen, GPIO_Value_High);
        nanosleep(&sleepTime, NULL);

        GPIO_SetValue(fdBlue, GPIO_Value_Low);
        nanosleep(&sleepTime, NULL);
        GPIO_SetValue(fdBlue, GPIO_Value_High);
        nanosleep(&sleepTime, NULL);
    }

    ClosePeripheralsAndHandlers();
    Log_Debug("Application exiting.\n");
    return exitCode;
}
