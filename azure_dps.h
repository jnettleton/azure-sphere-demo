/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

/// <summary>
/// Context data for required for provisioning and connection via DPS.
/// </summary>
typedef struct {
    const char* scopeId;
} Dps_Connection_Config; 

/// <summary>
/// Status of the connection attempt.
/// </summary>
typedef enum {
    Connection_NotStarted,
    Connection_Started,
    Connection_Complete,
    Connection_Failed
} Dps_Connection_Status;

typedef void (*Dps_Connection_StatusCallbackType)(Dps_Connection_Status status, IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubDeviceClientHandle);

static ExitCode Dps_Connection_Initialize(EventLoop* el, Dps_Connection_StatusCallbackType statusCallBack,
    ExitCode_CallbackType failureCallback, const char* modelId, void* context);

void Dps_Connection_Start(void);
