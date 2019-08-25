/*
 *
 *    Copyright (c) 2013-2017 Nest Labs, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      Implementation of the native methods expected by the Python
 *      version of Weave Device Manager.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <SystemLayer/SystemLayer.h>
#include <SystemLayer/SystemError.h>

#include <Weave/Core/WeaveCore.h>
#include <Weave/Support/CodeUtils.h>
#include <Weave/Core/WeaveEncoding.h>
#include <Weave/Profiles/WeaveProfiles.h>
#include <Weave/Support/logging/WeaveLogging.h>
#include <Weave/Profiles/common/CommonProfile.h>
#include <Weave/Profiles/network-provisioning/NetworkProvisioning.h>
#include <Weave/Support/ErrorStr.h>
#include <Weave/Support/NLDLLUtil.h>
#include <Weave/DeviceManager/WeaveDeviceManager.h>

#include <inttypes.h>
#include <net/if.h>

#if CONFIG_NETWORK_LAYER_BLE
#include "WeaveDeviceManager-BlePlatformDelegate.h"
#include "WeaveDeviceManager-BleApplicationDelegate.h"
#if WEAVE_ENABLE_WOBLE_TEST
#include "WoBleTest.h"
#endif
#endif /* CONFIG_NETWORK_LAYER_BLE */

using namespace nl::Inet;
using namespace nl::Weave;
using namespace nl::Weave::DeviceManager;
using namespace nl::Weave::Profiles::NetworkProvisioning;
using namespace nl::Weave::Profiles::DataManagement;
using DeviceDescription::IdentifyDeviceCriteria;

extern "C" {
typedef void * (*GetBleEventCBFunct)(void);
}

enum BleEventType
{
    kBleEventType_Rx        = 1,
    kBleEventType_Tx        = 2,
    kBleEventType_Subscribe = 3,
    kBleEventType_Disconnect = 4
};

enum BleSubscribeOperation
{
    kBleSubOp_Subscribe = 1,
    kBleSubOp_Unsubscribe = 2
};

class NL_DLL_EXPORT BleEventBase
{
public:
    int32_t eventType;
};

class NL_DLL_EXPORT BleRxEvent :
    public BleEventBase
{
public:
    void *   connObj;
    void *   svcId;
    void *   charId;
    void *   buffer;
    uint16_t length;
};

class NL_DLL_EXPORT BleTxEvent :
    public BleEventBase
{
public:
    void * connObj;
    void * svcId;
    void * charId;
    bool   status;
};

class NL_DLL_EXPORT BleSubscribeEvent :
    public BleEventBase
{
public:
    void *    connObj;
    void *    svcId;
    void *    charId;
    int32_t   operation;
    bool      status;
};

class NL_DLL_EXPORT BleDisconnectEvent :
    public BleEventBase
{
public:
    void *    connObj;
    int32_t   error;
};


static System::Layer sSystemLayer;
static InetLayer Inet;

#if CONFIG_NETWORK_LAYER_BLE
static BleLayer Ble;
static DeviceManager_BlePlatformDelegate sBlePlatformDelegate(&Ble);
static DeviceManager_BleApplicationDelegate sBleApplicationDelegate;

static volatile GetBleEventCBFunct GetBleEventCB = NULL;

static int BleWakePipe[2];
#endif /* CONFIG_NETWORK_LAYER_BLE */

extern "C" {
    // Trampolined callback types
    typedef void (*DeviceEnumerationResponseScriptFunct)(WeaveDeviceManager *deviceMgr, const DeviceDescription::WeaveDeviceDescriptor *devdesc, const char *deviceAddrStr);

    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_Init();
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_Shutdown();
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_DriveIO(uint32_t sleepTimeMS);

#if CONFIG_NETWORK_LAYER_BLE
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_WakeForBleIO();
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetBleEventCB(GetBleEventCBFunct getBleEventCB);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetBleWriteCharacteristic(WriteBleCharacteristicCBFunct writeBleCharacteristicCB);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetBleSubscribeCharacteristic(SubscribeBleCharacteristicCBFunct subscribeBleCharacteristicCB);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetBleClose(CloseBleCBFunct closeBleCB);
#endif /* CONFIG_NETWORK_LAYER_BLE */

    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_NewDeviceManager(WeaveDeviceManager **outDevMgr);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_DeleteDeviceManager(WeaveDeviceManager *devMgr);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_StartDeviceEnumeration(WeaveDeviceManager *devMgr, const IdentifyDeviceCriteria *deviceCriteria,
            DeviceEnumerationResponseScriptFunct onResponse, ErrorFunct onError);
    NL_DLL_EXPORT void nl_Weave_DeviceManager_StopDeviceEnumeration(WeaveDeviceManager *devMgr);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ConnectDevice_NoAuth(WeaveDeviceManager *devMgr, uint64_t deviceId, const char *deviceAddrStr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ConnectDevice_PairingCode(WeaveDeviceManager *devMgr, uint64_t deviceId, const char *deviceAddrStr, const char *pairingCode,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ConnectDevice_AccessToken(WeaveDeviceManager *devMgr, uint64_t deviceId, const char *deviceAddrStr, const uint8_t *accessToken, uint32_t accessTokenLen,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RendezvousDevice_NoAuth(WeaveDeviceManager *devMgr, const IdentifyDeviceCriteria *deviceCriteria,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RendezvousDevice_PairingCode(WeaveDeviceManager *devMgr, const char *pairingCode,
            const IdentifyDeviceCriteria *deviceCriteria, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RendezvousDevice_AccessToken(WeaveDeviceManager *devMgr, const uint8_t *accessToken, uint32_t accessTokenLen,
            const IdentifyDeviceCriteria *deviceCriteria, CompleteFunct onComplete, ErrorFunct onError);

    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_PassiveRendezvousDevice_NoAuth(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_PassiveRendezvousDevice_PairingCode(WeaveDeviceManager *devMgr, const char *pairingCode, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_PassiveRendezvousDevice_AccessToken(WeaveDeviceManager *devMgr, const uint8_t *accessToken, uint32_t accessTokenLen, CompleteFunct onComplete, ErrorFunct onError);

#if CONFIG_NETWORK_LAYER_BLE
	NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_TestBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, CompleteFunct onComplete, ErrorFunct onError, uint32_t count, uint32_t duration, uint16_t delay, uint8_t ack, uint16_t size, bool rx);
	NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_TestResultBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, bool local);
	NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_TestAbortBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj);
	NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_TxTimingBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, bool enabled, bool remote);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ConnectBle_NoAuth(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ConnectBle_PairingCode(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, const char *pairingCode,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ConnectBle_AccessToken(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, const uint8_t *accessToken, uint32_t accessTokenLen,
            CompleteFunct onComplete, ErrorFunct onError);
#endif /* CONFIG_NETWORK_LAYER_BLE */

    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RemotePassiveRendezvous_CASEAuth(WeaveDeviceManager *devMgr, const char *rendezvousDeviceAddr,
            const char *accessToken, uint32_t accessTokenLen, const uint16_t rendezvousTimeout,
            const uint16_t inactivityTimeout, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RemotePassiveRendezvous_PASEAuth(WeaveDeviceManager *devMgr, const char *rendezvousDeviceAddr,
            const char *pairingCode, const uint16_t rendezvousTimeout, const uint16_t inactivityTimeout,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RemotePassiveRendezvous_NoAuth(WeaveDeviceManager *devMgr, const char *rendezvousDeviceAddr,
            const uint16_t rendezvousTimeout, const uint16_t inactivityTimeout, CompleteFunct onComplete,
            ErrorFunct onError);

    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ReconnectDevice(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_EnableConnectionMonitor(WeaveDeviceManager *devMgr, uint16_t interval, uint16_t timeout, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_DisableConnectionMonitor(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT void nl_Weave_DeviceManager_Close(WeaveDeviceManager *devMgr);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_IdentifyDevice(WeaveDeviceManager *devMgr, IdentifyDeviceCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ScanNetworks(WeaveDeviceManager *devMgr, NetworkType networkType, NetworkScanCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_AddNetwork(WeaveDeviceManager *devMgr, const NetworkInfo *netInfo, AddNetworkCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_UpdateNetwork(WeaveDeviceManager *devMgr, const NetworkInfo *netInfo, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RemoveNetwork(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_GetNetworks(WeaveDeviceManager *devMgr, uint8_t getFlags, GetNetworksCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_GetCameraAuthData(WeaveDeviceManager *devMgr, const char* nonce, GetCameraAuthDataCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_EnableNetwork(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_DisableNetwork(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_TestNetworkConnectivity(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_GetRendezvousMode(WeaveDeviceManager *devMgr, GetRendezvousModeCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetRendezvousMode(WeaveDeviceManager *devMgr, uint16_t modeFlags, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_GetLastNetworkProvisioningResult(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_CreateFabric(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_LeaveFabric(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_GetFabricConfig(WeaveDeviceManager *devMgr, GetFabricConfigCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_JoinExistingFabric(WeaveDeviceManager *devMgr, const uint8_t *fabricConfig, uint32_t fabricConfigLen, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_Ping(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetRendezvousAddress(WeaveDeviceManager *devMgr, const char *rendezvousAddr);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetAutoReconnect(WeaveDeviceManager *devMgr, bool autoReconnect);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetRendezvousLinkLocal(WeaveDeviceManager *devMgr, bool RendezvousLinkLocal);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetConnectTimeout(WeaveDeviceManager *devMgr, uint32_t timeoutMS);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_RegisterServicePairAccount(WeaveDeviceManager *devMgr, uint64_t serviceId, const char *accountId,
            const uint8_t *serviceConfig, uint16_t serviceConfigLen,
            const uint8_t *pairingToken, uint16_t pairingTokenLen,
            const uint8_t *pairingInitData, uint16_t pairingInitDataLen,
            CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_UpdateService(WeaveDeviceManager *devMgr, uint64_t serviceId, const uint8_t *serviceConfig, uint16_t serviceConfigLen,
                              CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_UnregisterService(WeaveDeviceManager *devMgr, uint64_t serviceId, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ArmFailSafe(WeaveDeviceManager *devMgr, uint8_t armMode, uint32_t failSafeToken, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_DisarmFailSafe(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_ResetConfig(WeaveDeviceManager *devMgr, uint16_t resetFlags, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetupDMSession(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_TearDownDMSession(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_SetActiveLocale(WeaveDeviceManager *devMgr, const char *aLocale, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_PairToken(WeaveDeviceManager *devMgr, const uint8_t *pairingToken, uint32_t pairingTokenLen, PairTokenCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_UnpairToken(WeaveDeviceManager *devMgr, UnpairTokenCompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_StartSystemTest(WeaveDeviceManager *devMgr, uint32_t profileId, uint32_t testId, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_StopSystemTest(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError);
    NL_DLL_EXPORT bool nl_Weave_DeviceManager_IsConnected(WeaveDeviceManager *devMgr);
    NL_DLL_EXPORT uint64_t nl_Weave_DeviceManager_DeviceId(WeaveDeviceManager *devMgr);
    NL_DLL_EXPORT const char *nl_Weave_DeviceManager_DeviceAddress(WeaveDeviceManager *devMgr);
    NL_DLL_EXPORT WEAVE_ERROR nl_Weave_DeviceManager_CloseEndpoints();
    NL_DLL_EXPORT const char *nl_Weave_DeviceManager_ErrorToString(WEAVE_ERROR err);
    NL_DLL_EXPORT const char *nl_Weave_DeviceManager_StatusReportToString(uint32_t profileId, uint16_t statusCode);
    NL_DLL_EXPORT uint8_t nl_Weave_DeviceManager_GetLogFilter();
    NL_DLL_EXPORT void nl_Weave_DeviceManager_SetLogFilter(uint8_t category);
}

static void DeviceEnumerationResponseFunctTrampoline(WeaveDeviceManager *deviceMgr, void *appReqState, const DeviceDescription::WeaveDeviceDescriptor *devdesc,
                                                     IPAddress deviceAddr, InterfaceId deviceIntf)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    DeviceEnumerationResponseScriptFunct scriptCallback = *((DeviceEnumerationResponseScriptFunct *)&appReqState);
    char deviceAddrStr[INET6_ADDRSTRLEN + IF_NAMESIZE + 2]; // Include space for "<addr>%<interface name>" and NULL terminator

    // Convert IPAddress to string for Python CLI layer
    VerifyOrExit(NULL != deviceAddr.ToString(&deviceAddrStr[0], INET6_ADDRSTRLEN), err = INET_ERROR_BAD_ARGS);

    // Add "%" separator character, with NULL terminator, per IETF RFC 4007: https://tools.ietf.org/html/rfc4007
    VerifyOrExit(snprintf(&(deviceAddrStr[strlen(deviceAddrStr)]), 2, "%%") > 0, err = System::MapErrorPOSIX(errno));

    // Concatenate zone index (aka interface name) to IP address, with NULL terminator, per IETF RFC 4007: https://tools.ietf.org/html/rfc4007
    err = nl::Inet::GetInterfaceName(deviceIntf, &(deviceAddrStr[strlen(deviceAddrStr)]), IF_NAMESIZE + 1);
    SuccessOrExit(err);

    // Fire script callback with IPAddress converted to C-string
    scriptCallback(deviceMgr, devdesc, deviceAddrStr);

exit:
    if (WEAVE_NO_ERROR != err)
    {
        WeaveLogError(DeviceManager, "DeviceEnumerationResponseFunctTrampoline failure, err = %d", err);
    }
}

WEAVE_ERROR nl_Weave_DeviceManager_Init()
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    WeaveMessageLayer::InitContext initContext;
#if WEAVE_SYSTEM_CONFIG_USE_SOCKETS && CONFIG_NETWORK_LAYER_BLE
    int flags;
#endif /* WEAVE_SYSTEM_CONFIG_USE_SOCKETS && CONFIG_NETWORK_LAYER_BLE */

#if !WEAVE_SYSTEM_CONFIG_USE_SOCKETS

    ExitNow(err = WEAVE_ERROR_NOT_IMPLEMENTED);

#else /* WEAVE_SYSTEM_CONFIG_USE_SOCKETS */

    // Initialize the underlying platform secure random source.
    err = nl::Weave::Platform::Security::InitSecureRandomDataSource(NULL, 64, NULL, 0);
    SuccessOrExit(err);

    if (sSystemLayer.State() == System::kLayerState_Initialized)
        ExitNow();

    err = sSystemLayer.Init(NULL);
    SuccessOrExit(err);

    if (Inet.State == InetLayer::kState_Initialized)
        ExitNow();

    // Initialize the InetLayer object.
    err = Inet.Init(sSystemLayer, NULL);
    SuccessOrExit(err);

#if CONFIG_NETWORK_LAYER_BLE
    // Initialize the BleLayer object. For now, assume Device Manager is always a central.
    err = Ble.Init(&sBlePlatformDelegate, &sBleApplicationDelegate, &sSystemLayer);
    SuccessOrExit(err);

    initContext.ble = &Ble;
    initContext.listenBLE = false;

    // Create BLE wake pipe and make it non-blocking.
    if (pipe(BleWakePipe) == -1)
    {
        err = System::MapErrorPOSIX(errno);
        ExitNow();
    }

    // Make read end non-blocking.
    flags = fcntl(BleWakePipe[0], F_GETFL);
    if (flags == -1)
    {
        err = System::MapErrorPOSIX(errno);
        ExitNow();
    }

    flags |= O_NONBLOCK;
    if (fcntl(BleWakePipe[0], F_SETFL, flags) == -1)
    {
        err = System::MapErrorPOSIX(errno);
        ExitNow();
    }

    // Make write end non-blocking.
    flags = fcntl(BleWakePipe[1], F_GETFL);
    if (flags == -1)
    {
        err = System::MapErrorPOSIX(errno);
        ExitNow();
    }

    flags |= O_NONBLOCK;
    if (fcntl(BleWakePipe[1], F_SETFL, flags) == -1)
    {
        err = System::MapErrorPOSIX(errno);
        ExitNow();
    }
#endif /* CONFIG_NETWORK_LAYER_BLE */

    // Initialize the FabricState object.
    err = FabricState.Init();
    SuccessOrExit(err);

    FabricState.FabricId = 0; // Not a member of any fabric
    FabricState.LocalNodeId = 1; // TODO: TEMPORARY HACK -- use a different default node id to avoid conflict with the mock device.

    // Initialize the WeaveMessageLayer object.
    initContext.systemLayer = &sSystemLayer;
    initContext.inet = &Inet;
    initContext.fabricState = &FabricState;
    initContext.listenTCP = false;
    initContext.listenUDP = true;

    err = MessageLayer.Init(&initContext);
    SuccessOrExit(err);

    // Initialize the Exchange Manager object.
    err = ExchangeMgr.Init(&MessageLayer);
    SuccessOrExit(err);

    // Initialize the Security Manager object.
    err = SecurityMgr.Init(ExchangeMgr, sSystemLayer);
    SuccessOrExit(err);

#endif /* WEAVE_SYSTEM_CONFIG_USE_SOCKETS */

exit:
    if (err != WEAVE_NO_ERROR)
        nl_Weave_DeviceManager_Shutdown();
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_Shutdown()
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    if (Inet.State == InetLayer::kState_NotInitialized)
        ExitNow();

    if (sSystemLayer.State() == System::kLayerState_NotInitialized)
        ExitNow();

    // TODO: implement this

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_NewDeviceManager(WeaveDeviceManager **outDevMgr)
{
    WEAVE_ERROR err;

    *outDevMgr = new WeaveDeviceManager();
    VerifyOrExit(*outDevMgr != NULL, err = WEAVE_ERROR_NO_MEMORY);

    err = (*outDevMgr)->Init(&ExchangeMgr, &SecurityMgr);
    SuccessOrExit(err);

    err = (*outDevMgr)->SetUseAccessToken(true);
    SuccessOrExit(err);

exit:
    if (err != WEAVE_NO_ERROR && *outDevMgr != NULL)
    {
        delete *outDevMgr;
        *outDevMgr = NULL;
    }
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_DeleteDeviceManager(WeaveDeviceManager *devMgr)
{
    if (devMgr != NULL)
    {
        devMgr->Shutdown();
        delete devMgr;
    }
    return WEAVE_NO_ERROR;
}

WEAVE_ERROR nl_Weave_DeviceManager_DriveIO(uint32_t sleepTimeMS)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

#if !WEAVE_SYSTEM_CONFIG_USE_SOCKETS

    ExitNow(err = WEAVE_ERROR_NOT_IMPLEMENTED);

#else /* WEAVE_SYSTEM_CONFIG_USE_SOCKETS */
    struct timeval sleepTime;
    fd_set readFDs, writeFDs, exceptFDs;
    int maxFDs = 0;
#if CONFIG_NETWORK_LAYER_BLE
    uint8_t bleWakeByte;
    bool result = false;
    System::PacketBuffer* msgBuf;
    WeaveBleUUID svcId, charId;
    union
    {
        const BleEventBase      *ev;
        const BleTxEvent        *txEv;
        const BleRxEvent        *rxEv;
        const BleSubscribeEvent *subscribeEv;
        const BleDisconnectEvent *dcEv;
    } evu;
#endif /* CONFIG_NETWORK_LAYER_BLE */

    FD_ZERO(&readFDs);
    FD_ZERO(&writeFDs);
    FD_ZERO(&exceptFDs);

    sleepTime.tv_sec = sleepTimeMS / 1000;
    sleepTime.tv_usec = (sleepTimeMS % 1000) * 1000;

    if (sSystemLayer.State() == System::kLayerState_Initialized)
        sSystemLayer.PrepareSelect(maxFDs, &readFDs, &writeFDs, &exceptFDs, sleepTime);

    if (Inet.State == InetLayer::kState_Initialized)
        Inet.PrepareSelect(maxFDs, &readFDs, &writeFDs, &exceptFDs, sleepTime);

#if CONFIG_NETWORK_LAYER_BLE
    // Add read end of BLE wake pipe to readFDs.
    FD_SET(BleWakePipe[0], &readFDs);

    if (BleWakePipe[0] + 1 > maxFDs)
        maxFDs = BleWakePipe[0] + 1;
#endif /* CONFIG_NETWORK_LAYER_BLE */

    int selectRes = select(maxFDs, &readFDs, &writeFDs, &exceptFDs, &sleepTime);
    VerifyOrExit(selectRes >= 0, err = System::MapErrorPOSIX(errno));

#if CONFIG_NETWORK_LAYER_BLE
    // Drive IO to InetLayer and/or BleLayer.
    if (FD_ISSET(BleWakePipe[0], &readFDs))
    {
        while (true)
        {
            if (read(BleWakePipe[0], &bleWakeByte, 1) == -1)
            {
                if (errno == EAGAIN)
                    break;
                else
                {
                    err = errno;
                    printf("bleWakePipe calling ExitNow()\n");
                    ExitNow();
                }
            }

            if (GetBleEventCB)
            {
                evu.ev = (const BleEventBase *)GetBleEventCB();

                if (evu.ev)
                {
                    switch (evu.ev->eventType)
                    {
                        case kBleEventType_Rx:
                            // build a packet buffer from the rxEv and send to blelayer.
                            msgBuf = System::PacketBuffer::New();
                            VerifyOrExit(msgBuf != NULL, err = WEAVE_ERROR_NO_MEMORY);

                            memcpy(msgBuf->Start(), evu.rxEv->buffer, evu.rxEv->length);
                            msgBuf->SetDataLength(evu.rxEv->length);

                            // copy the svcId and charId from the event.
                            memcpy(svcId.bytes, evu.rxEv->svcId, sizeof(svcId.bytes));
                            memcpy(charId.bytes, evu.rxEv->charId, sizeof(charId.bytes));

                            result = Ble.HandleIndicationReceived(evu.txEv->connObj, &svcId, &charId, msgBuf);

                            if (!result)
                            {
                                System::PacketBuffer::Free(msgBuf);
                            }

                            msgBuf = NULL;
                            break;

                        case kBleEventType_Tx:
                            // copy the svcId and charId from the event.
                            memcpy(svcId.bytes, evu.txEv->svcId, sizeof(svcId.bytes));
                            memcpy(charId.bytes, evu.txEv->charId, sizeof(charId.bytes));

                            result = Ble.HandleWriteConfirmation(evu.txEv->connObj, &svcId, &charId);
                            break;

                        case kBleEventType_Subscribe:
                            memcpy(svcId.bytes, evu.subscribeEv->svcId, sizeof(svcId.bytes));
                            memcpy(charId.bytes, evu.subscribeEv->charId, sizeof(charId.bytes));

                            switch (evu.subscribeEv->operation)
                            {
                                case kBleSubOp_Subscribe:
                                    if (evu.subscribeEv->status)
                                    {
                                        result = Ble.HandleSubscribeComplete(evu.subscribeEv->connObj, &svcId, &charId);
                                    }
                                    else
                                    {
                                        Ble.HandleConnectionError(evu.subscribeEv->connObj, BLE_ERROR_GATT_SUBSCRIBE_FAILED);
                                    }
                                    break;

                                case kBleSubOp_Unsubscribe:
                                    if (evu.subscribeEv->status)
                                    {
                                        result = Ble.HandleUnsubscribeComplete(evu.subscribeEv->connObj, &svcId, &charId);
                                    }
                                    else
                                    {
                                        Ble.HandleConnectionError(evu.subscribeEv->connObj, BLE_ERROR_GATT_UNSUBSCRIBE_FAILED);
                                    }
                                    break;

                                default:
                                    printf("Error: unhandled subscribe operation. Calling ExitNow()\n");
                                    ExitNow();
                                    break;
                            }
                            break;

                        case kBleEventType_Disconnect:
                            Ble.HandleConnectionError(evu.dcEv->connObj, evu.dcEv->error);
                            break;

                        default:
                            printf("Error: unhandled Ble EventType. Calling ExitNow()\n");
                            ExitNow();
                            break;
                    }
                }
                else
                {
                    printf("no event\n");
                }
            }
        }

        // Don't bother InetLayer if we only got BLE IO.
        selectRes--;
    }
#endif /* CONFIG_NETWORK_LAYER_BLE */

    if (sSystemLayer.State() == System::kLayerState_Initialized)
        sSystemLayer.HandleSelectResult(selectRes, &readFDs, &writeFDs, &exceptFDs);

    if (Inet.State == InetLayer::kState_Initialized)
        Inet.HandleSelectResult(selectRes, &readFDs, &writeFDs, &exceptFDs);

#endif /* WEAVE_SYSTEM_CONFIG_USE_SOCKETS */

exit:
    return err;
}

#if CONFIG_NETWORK_LAYER_BLE
WEAVE_ERROR nl_Weave_DeviceManager_WakeForBleIO()
{
    if (BleWakePipe[1] == 0)
    {
        return WEAVE_ERROR_INCORRECT_STATE;
    }
    // Write a single byte to the BLE wake pipe. This wakes the IO thread's select loop for BLE input.
    if (write(BleWakePipe[1], "x", 1) == -1 && errno != EAGAIN)
    {
        return errno;
    }

    return WEAVE_NO_ERROR;
}

WEAVE_ERROR nl_Weave_DeviceManager_SetBleEventCB(GetBleEventCBFunct getBleEventCB)
{
    GetBleEventCB = getBleEventCB;

    return WEAVE_NO_ERROR;
}

WEAVE_ERROR nl_Weave_DeviceManager_SetBleWriteCharacteristic(WriteBleCharacteristicCBFunct writeBleCharacteristicCB)
{
    sBlePlatformDelegate.SetWriteCharCB(writeBleCharacteristicCB);
    return WEAVE_NO_ERROR;
}

WEAVE_ERROR nl_Weave_DeviceManager_SetBleSubscribeCharacteristic(SubscribeBleCharacteristicCBFunct subscribeBleCharacteristicCB)
{
    sBlePlatformDelegate.SetSubscribeCharCB(subscribeBleCharacteristicCB);
    return WEAVE_NO_ERROR;
}

WEAVE_ERROR nl_Weave_DeviceManager_SetBleClose(CloseBleCBFunct closeBleCB)
{
    sBlePlatformDelegate.SetCloseCB(closeBleCB);
    return WEAVE_NO_ERROR;
}

#endif /* CONFIG_NETWORK_LAYER_BLE */

void nl_Weave_DeviceManager_Close(WeaveDeviceManager *devMgr)
{
    devMgr->Close();
}

WEAVE_ERROR nl_Weave_DeviceManager_IdentifyDevice(WeaveDeviceManager *devMgr, IdentifyDeviceCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->IdentifyDevice(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_PairToken(WeaveDeviceManager *devMgr, const uint8_t *pairingToken, uint32_t pairingTokenLen, PairTokenCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->PairToken(pairingToken, pairingTokenLen, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_UnpairToken(WeaveDeviceManager *devMgr, UnpairTokenCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->UnpairToken(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_StartDeviceEnumeration(WeaveDeviceManager *devMgr, const IdentifyDeviceCriteria *deviceCriteria,
    DeviceEnumerationResponseScriptFunct onResponse, ErrorFunct onError)
{
    if (NULL == deviceCriteria || NULL == devMgr)
    {
        return WEAVE_ERROR_INVALID_ARGUMENT;
    }

    return devMgr->StartDeviceEnumeration(*(void **)&onResponse, (*deviceCriteria), DeviceEnumerationResponseFunctTrampoline, onError);
}

void nl_Weave_DeviceManager_StopDeviceEnumeration(WeaveDeviceManager *devMgr)
{
    devMgr->StopDeviceEnumeration();
}

WEAVE_ERROR nl_Weave_DeviceManager_ConnectDevice_NoAuth(WeaveDeviceManager *devMgr, uint64_t deviceId, const char *deviceAddrStr, CompleteFunct onComplete, ErrorFunct onError)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    IPAddress deviceAddr;

    if (deviceAddrStr != NULL)
    {
        if (!IPAddress::FromString(deviceAddrStr, deviceAddr))
            ExitNow(err = WEAVE_ERROR_INVALID_ADDRESS);
    }
    else
        deviceAddr = IPAddress::Any;

    return devMgr->ConnectDevice(deviceId, deviceAddr, NULL, onComplete, onError);

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_ConnectDevice_PairingCode(WeaveDeviceManager *devMgr, uint64_t deviceId, const char *deviceAddrStr, const char *pairingCode, CompleteFunct onComplete, ErrorFunct onError)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    IPAddress deviceAddr;

    if (deviceAddrStr != NULL)
    {
        if (!IPAddress::FromString(deviceAddrStr, deviceAddr))
            ExitNow(err = WEAVE_ERROR_INVALID_ADDRESS);
    }
    else
        deviceAddr = IPAddress::Any;

    return devMgr->ConnectDevice(deviceId, deviceAddr, pairingCode, NULL, onComplete, onError);

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_ConnectDevice_AccessToken(WeaveDeviceManager *devMgr, uint64_t deviceId, const char *deviceAddrStr,
        const uint8_t *accessToken, uint32_t accessTokenLen, CompleteFunct onComplete, ErrorFunct onError)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    IPAddress deviceAddr;

    if (deviceAddrStr != NULL)
    {
        if (!IPAddress::FromString(deviceAddrStr, deviceAddr))
            ExitNow(err = WEAVE_ERROR_INVALID_ADDRESS);
    }
    else
        deviceAddr = IPAddress::Any;

    return devMgr->ConnectDevice(deviceId, deviceAddr, accessToken, accessTokenLen, NULL, onComplete, onError);

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_RendezvousDevice_NoAuth(WeaveDeviceManager *devMgr,
        const IdentifyDeviceCriteria *deviceCriteria, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->RendezvousDevice(*deviceCriteria, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_RendezvousDevice_PairingCode(WeaveDeviceManager *devMgr, const char *pairingCode,
        const IdentifyDeviceCriteria *deviceCriteria, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->RendezvousDevice(pairingCode, *deviceCriteria, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_RendezvousDevice_AccessToken(WeaveDeviceManager *devMgr, const uint8_t *accessToken, uint32_t accessTokenLen,
        const IdentifyDeviceCriteria *deviceCriteria, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->RendezvousDevice(accessToken, accessTokenLen, *deviceCriteria, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_PassiveRendezvousDevice_NoAuth(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->PassiveRendezvousDevice(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_PassiveRendezvousDevice_PairingCode(WeaveDeviceManager *devMgr, const char *pairingCode, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->PassiveRendezvousDevice(pairingCode, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_PassiveRendezvousDevice_AccessToken(WeaveDeviceManager *devMgr, const uint8_t *accessToken, uint32_t accessTokenLen, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->PassiveRendezvousDevice(accessToken, accessTokenLen, NULL, onComplete, onError);
}

#if CONFIG_NETWORK_LAYER_BLE
WEAVE_ERROR nl_Weave_DeviceManager_TestBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, CompleteFunct onComplete, ErrorFunct onError, uint32_t count, uint32_t duration, uint16_t delay, uint8_t ack, uint16_t size, bool rx)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

#if WEAVE_ENABLE_WOBLE_TEST
    if (connObj == NULL)
    {
        WeaveLogError(DeviceManager, "%s: Invalid connObj = %u", __FUNCTION__, connObj);
        err = WEAVE_ERROR_INVALID_ARGUMENT;
    }
    else
        err = HandleCommandTest((void *)&Ble, connObj, count, duration, delay, ack, size, rx);
#else
    err = WEAVE_ERROR_NOT_IMPLEMENTED;
    WeaveLogError(DeviceManager, "%s: Not a WoBle Test Build!", __FUNCTION__);
#endif /* WEAVE_ENABLE_WOBLE_TEST */

    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_TestResultBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, bool local)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

#if WEAVE_ENABLE_WOBLE_TEST
    if (connObj == NULL)
    {
        WeaveLogError(DeviceManager, "%s: Invalid connObj = %u", __FUNCTION__, connObj);
        err = WEAVE_ERROR_INVALID_ARGUMENT;
    }
    else
        err = HandleCommandTestResult((void *)&Ble, connObj, local);
#else
    WeaveLogError(DeviceManager, "%s: Not a WoBle Test Build!", __FUNCTION__);
    // Returns NO_ERROR, so it can be used to check the test build
#endif /* WEAVE_ENABLE_WOBLE_TEST */

    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_TestAbortBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

#if WEAVE_ENABLE_WOBLE_TEST
    if (connObj == NULL)
    {
        WeaveLogError(DeviceManager, "%s: Invalid connObj = %u", __FUNCTION__, connObj);
        err = WEAVE_ERROR_INVALID_ARGUMENT;
    }
    else
        err = HandleCommandTestAbort((void *)&Ble, connObj);
#else
    WeaveLogError(DeviceManager, "%s: Not a WoBle Test Build!", __FUNCTION__);
    err = WEAVE_ERROR_NOT_IMPLEMENTED;
#endif /* WEAVE_ENABLE_WOBLE_TEST */

    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_TxTimingBle(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, bool enabled, bool remote)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

#if WEAVE_ENABLE_WOBLE_TEST
    if (connObj == NULL)
    {
        WeaveLogError(DeviceManager, "%s: Invalid connObj = %u", __FUNCTION__, connObj);
        err = WEAVE_ERROR_INVALID_ARGUMENT;
    }
    else
        err = HandleCommandTxTiming((void *)&Ble, connObj, enabled, remote);
#else
    WeaveLogError(DeviceManager, "%s: Not a WoBle Test Build!", __FUNCTION__);
    // Returns NO_ERROR, so it can be used to check the test build
#endif /* WEAVE_ENABLE_WOBLE_TEST */

    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_ConnectBle_NoAuth(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj,
            CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ConnectBle(connObj, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_ConnectBle_PairingCode(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, const char *pairingCode,
            CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ConnectBle(connObj, pairingCode, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_ConnectBle_AccessToken(WeaveDeviceManager *devMgr, BLE_CONNECTION_OBJECT connObj, const uint8_t *accessToken, uint32_t accessTokenLen,
            CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ConnectBle(connObj, accessToken, accessTokenLen, NULL, onComplete, onError);
}

#endif /* CONFIG_NETWORK_LAYER_BLE */

WEAVE_ERROR nl_Weave_DeviceManager_RemotePassiveRendezvous_CASEAuth(WeaveDeviceManager *devMgr, const char *rendezvousDeviceAddrStr,
            const char *accessToken, uint32_t accessTokenLen, const uint16_t rendezvousTimeout,
            const uint16_t inactivityTimeout, CompleteFunct onComplete, ErrorFunct onError)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    IPAddress rendezvousDeviceAddr;

    if (!IPAddress::FromString(rendezvousDeviceAddrStr, rendezvousDeviceAddr))
        ExitNow(err = WEAVE_ERROR_INVALID_ADDRESS);

    err = devMgr->RemotePassiveRendezvous(rendezvousDeviceAddr, (const uint8_t *)accessToken, accessTokenLen, rendezvousTimeout,
            inactivityTimeout, NULL, onComplete, onError);

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_RemotePassiveRendezvous_PASEAuth(WeaveDeviceManager *devMgr, const char *rendezvousDeviceAddrStr,
        const char *pairingCode, const uint16_t rendezvousTimeout, const uint16_t inactivityTimeout,
        CompleteFunct onComplete, ErrorFunct onError)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    IPAddress rendezvousDeviceAddr;

    if (!IPAddress::FromString(rendezvousDeviceAddrStr, rendezvousDeviceAddr))
        ExitNow(err = WEAVE_ERROR_INVALID_ADDRESS);

    err = devMgr->RemotePassiveRendezvous(rendezvousDeviceAddr, pairingCode, rendezvousTimeout, inactivityTimeout,
            NULL, onComplete, onError);

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_RemotePassiveRendezvous_NoAuth(WeaveDeviceManager *devMgr, const char *rendezvousDeviceAddrStr,
        const uint16_t rendezvousTimeout, const uint16_t inactivityTimeout, CompleteFunct onComplete,
        ErrorFunct onError)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;
    IPAddress rendezvousDeviceAddr;

    if (!IPAddress::FromString(rendezvousDeviceAddrStr, rendezvousDeviceAddr))
        ExitNow(err = WEAVE_ERROR_INVALID_ADDRESS);

    err = devMgr->RemotePassiveRendezvous(rendezvousDeviceAddr, rendezvousTimeout, inactivityTimeout,
            NULL, onComplete, onError);

exit:
    return err;
}

WEAVE_ERROR nl_Weave_DeviceManager_ReconnectDevice(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ReconnectDevice(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_EnableConnectionMonitor(WeaveDeviceManager *devMgr, uint16_t interval, uint16_t timeout, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->EnableConnectionMonitor(interval, timeout, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_DisableConnectionMonitor(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->DisableConnectionMonitor(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_ScanNetworks(WeaveDeviceManager *devMgr, NetworkType networkType, NetworkScanCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ScanNetworks(networkType, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_GetCameraAuthData(WeaveDeviceManager *devMgr, const char* nonce, GetCameraAuthDataCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->GetCameraAuthData(nonce, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_GetNetworks(WeaveDeviceManager *devMgr, uint8_t getFlags, GetNetworksCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->GetNetworks(getFlags, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_AddNetwork(WeaveDeviceManager *devMgr, const NetworkInfo *netInfo, AddNetworkCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->AddNetwork(netInfo, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_UpdateNetwork(WeaveDeviceManager *devMgr, const NetworkInfo *netInfo, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->UpdateNetwork(netInfo, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_RemoveNetwork(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->RemoveNetwork(networkId, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_EnableNetwork(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->EnableNetwork(networkId, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_DisableNetwork(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->DisableNetwork(networkId, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_TestNetworkConnectivity(WeaveDeviceManager *devMgr, uint32_t networkId, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->TestNetworkConnectivity(networkId, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_GetRendezvousMode(WeaveDeviceManager *devMgr, GetRendezvousModeCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->GetRendezvousMode(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetRendezvousMode(WeaveDeviceManager *devMgr, uint16_t modeFlags, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->SetRendezvousMode(modeFlags, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_GetLastNetworkProvisioningResult(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->GetLastNetworkProvisioningResult(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_CreateFabric(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->CreateFabric(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_LeaveFabric(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->LeaveFabric(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_GetFabricConfig(WeaveDeviceManager *devMgr, GetFabricConfigCompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->GetFabricConfig(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_JoinExistingFabric(WeaveDeviceManager *devMgr, const uint8_t *fabricConfig, uint32_t fabricConfigLen, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->JoinExistingFabric(fabricConfig, fabricConfigLen, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_Ping(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->Ping(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetRendezvousAddress(WeaveDeviceManager *devMgr, const char *rendezvousAddrStr)
{
    IPAddress rendezvousAddr;

    if (!IPAddress::FromString(rendezvousAddrStr, rendezvousAddr))
        return WEAVE_ERROR_INVALID_ADDRESS;

    return devMgr->SetWiFiRendezvousAddress(rendezvousAddr);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetAutoReconnect(WeaveDeviceManager *devMgr, bool autoReconnect)
{
    return devMgr->SetAutoReconnect(autoReconnect);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetRendezvousLinkLocal(WeaveDeviceManager *devMgr, bool RendezvousLinkLocal)
{
    return devMgr->SetRendezvousLinkLocal(RendezvousLinkLocal);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetConnectTimeout(WeaveDeviceManager *devMgr, uint32_t timeoutMS)
{
    return devMgr->SetConnectTimeout(timeoutMS);
}

WEAVE_ERROR nl_Weave_DeviceManager_RegisterServicePairAccount(WeaveDeviceManager *devMgr, uint64_t serviceId, const char *accountId,
        const uint8_t *serviceConfig, uint16_t serviceConfigLen,
        const uint8_t *pairingToken, uint16_t pairingTokenLen,
        const uint8_t *pairingInitData, uint16_t pairingInitDataLen,
        CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->RegisterServicePairAccount(serviceId, accountId, serviceConfig, serviceConfigLen, pairingToken,
            pairingTokenLen, pairingInitData, pairingInitDataLen, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_UpdateService(WeaveDeviceManager *devMgr, uint64_t serviceId, const uint8_t *serviceConfig, uint16_t serviceConfigLen,
                          CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->UpdateService(serviceId, serviceConfig, serviceConfigLen, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_UnregisterService(WeaveDeviceManager *devMgr, uint64_t serviceId, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->UnregisterService(serviceId, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_ArmFailSafe(WeaveDeviceManager *devMgr, uint8_t armMode, uint32_t failSafeToken, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ArmFailSafe(armMode, failSafeToken, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_DisarmFailSafe(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->DisarmFailSafe(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_ResetConfig(WeaveDeviceManager *devMgr, uint16_t resetFlags, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->ResetConfig(resetFlags, NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetupDMSession(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->SetupDMSession(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_TearDownDMSession(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->TearDownDMSession(NULL, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_SetActiveLocale(WeaveDeviceManager *devMgr, const char *aLocale, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->SetActiveLocale(NULL, aLocale, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_StartSystemTest(WeaveDeviceManager *devMgr, uint32_t profileId, uint32_t testId, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->StartSystemTest(NULL, profileId, testId, onComplete, onError);
}

WEAVE_ERROR nl_Weave_DeviceManager_StopSystemTest(WeaveDeviceManager *devMgr, CompleteFunct onComplete, ErrorFunct onError)
{
    return devMgr->StopSystemTest(NULL, onComplete, onError);
}

bool nl_Weave_DeviceManager_IsConnected(WeaveDeviceManager *devMgr)
{
    return devMgr->IsConnected();
}

uint64_t nl_Weave_DeviceManager_DeviceId(WeaveDeviceManager *devMgr)
{
    uint64_t deviceId = 0;
    devMgr->GetDeviceId(deviceId);
    return deviceId;
}

const char *nl_Weave_DeviceManager_DeviceAddress(WeaveDeviceManager *devMgr)
{
    IPAddress devAddr;
    static char devAddrStr[INET6_ADDRSTRLEN];

    if (devMgr->GetDeviceAddress(devAddr) == WEAVE_NO_ERROR)
    {
        devAddr.ToString(devAddrStr, sizeof(devAddrStr));
        return devAddrStr;
    }
    else
        return NULL;
}

WEAVE_ERROR nl_Weave_DeviceManager_CloseEndpoints()
{
    if (Inet.State != InetLayer::kState_Initialized)
        return WEAVE_ERROR_INCORRECT_STATE;

    return MessageLayer.CloseEndpoints();
}

const char *nl_Weave_DeviceManager_ErrorToString(WEAVE_ERROR err)
{
    return nl::ErrorStr(err);
}

const char *nl_Weave_DeviceManager_StatusReportToString(uint32_t profileId, uint16_t statusCode)
{
    return nl::StatusReportStr(profileId, statusCode);
}

uint8_t nl_Weave_DeviceManager_GetLogFilter()
{
    return nl::Weave::Logging::GetLogFilter();
}

void nl_Weave_DeviceManager_SetLogFilter(uint8_t category)
{
    nl::Weave::Logging::SetLogFilter(category);
}


namespace nl {
namespace Weave {
namespace Platform {
namespace PersistedStorage {

/*
 * Dummy implementations of PersistedStorage platform methods. These aren't
 * used in the context of the Python DeviceManager, but are required to satisfy
 * the linker.
 */

WEAVE_ERROR Read(const char *aKey, uint32_t &aValue)
{
    return WEAVE_NO_ERROR;
}

WEAVE_ERROR Write(const char *aKey, uint32_t aValue)
{
    return WEAVE_NO_ERROR;
}

} // PersistentStorage
} // Platform
} // Weave
} // nl
