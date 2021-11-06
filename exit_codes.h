#ifndef _EXIT_CODES_H
#define _EXIT_CODES_H

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,

    ExitCode_TermHandler_SigTerm,

    ExitCode_Main_EventLoopFail,

    ExitCode_ButtonTimer_Consume,

    ExitCode_AzureTimer_Consume,

    ExitCode_Init_EventLoop,
    ExitCode_Init_ButtonA,
    ExitCode_Init_ButtonB,
    ExitCode_Init_ButtonPollTimer,
    ExitCode_Init_AzureTimer,
    ExitCode_Init_Led,

	ExitCode_IsButtonPressed_GetValue,

    ExitCode_Validate_ConnectionType,
    ExitCode_Validate_ScopeId,
    ExitCode_Validate_Hostname,
    ExitCode_Validate_IoTEdgeCAPath,
    ExitCode_InterfaceConnectionStatus_Failed,

    ExitCode_IoTEdgeRootCa_Open_Failed,
    ExitCode_IoTEdgeRootCa_LSeek_Failed,
    ExitCode_IoTEdgeRootCa_FileSize_Invalid,
    ExitCode_IoTEdgeRootCa_FileSize_TooLarge,
    ExitCode_IoTEdgeRootCa_FileRead_Failed,

    ExitCode_PayloadSize_TooLarge,
    ExitCode_Init_sensorPollTimer,

    ExitCode_SetGPIO_Failed,
    ExitCode_Button_Telemetry_Malloc_Failed,
    ExitCode_RebootDevice_Malloc_failed,
    ExitCode_SetPollTime_Malloc_failed,
    ExitCode_NoMethodFound_Malloc_failed,
    ExitCode_DirectMethod_InvalidPayload_Malloc_failed,
    ExitCode_DirectMethod_RebootExectued,
    ExitCode_Init_OledUpdateTimer,

    // IoTConnect exit codes
    ExitCode_IoTCTimer_Consume,
    ExitCode_Init_IoTCTimer,
    ExitCode_IoTCMalloc_Failed,

    // Mutable Storage exit codes
    ExitCode_WriteFile_OpenMutableFile,
    ExitCode_WriteFile_Write,
    ExitCode_ReadFile_OpenMutableFile,
    ExitCode_ReadFile_Read,
    ExitCode_SendTelemetryMemoryError,
    
    // M4 intercore comms exit codes
    ExitCode_Init_RegisterIo,
    ExitCode_Init_Rt_PollTimer,
    ExitCode_Read_RT_Socket,
    ExitCode_Write_RT_Socket,
    ExitCode_RT_Timer_Consume,

    // Reboot exit codes
    ExitCode_TriggerReboot_Success,
    ExitCode_UpdateCallback_Reboot,

    ExitCode_SettxInterval_Malloc_failed,
    ExitCode_DirectMethodError_Malloc_failed,
    ExitCode_DirectMethodResponse_Malloc_failed,
    ExitCode_Init_RebootTimer,
} ExitCode;

#endif 