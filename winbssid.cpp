#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
#include <guiddef.h>
#include <wlanapi.h>
#include <winnetwk.h>

#include "winbssid.hpp"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

int main(int argc, char *argv[], char *envp[]) {
    const char *action = argv[1];

    HANDLE hClientHandle = nullptr;
    winbssid_init(&hClientHandle);

    if (hClientHandle == nullptr)
    {
        std::cout << "Failed to initialize handle" << std::endl;
        ExitProcess(0);
    }

    if (strcmp(action, "connect") == 0) {
        const char *interfaceGuidStr = argv[2];
        const char *bssid = argv[3];
        const GUID *pInterfaceGuid = nullptr;
        winbssid_connect(&hClientHandle, pInterfaceGuid, bssid);
    } else if (strcmp(action, "disconnect") == 0) {
        const char *interfaceGuidStr = argv[2];
        const GUID *pInterfaceGuid = nullptr;
        const char *bssid = argv[2];
        winbssid_disconnect(hClientHandle, pInterfaceGuid, bssid);
    } else if (strcmp(action, "list-networks") == 0) {
        const char *interfaceGuidStr = argv[2];
        const GUID *pInterfaceGuid = nullptr;
        winbssid_list_networks(hClientHandle, pInterfaceGuid);
    } else if (strcmp(action, "list-interfaces") == 0) {
        winbssid_list_interfaces(hClientHandle);
    } else {
        winbssid_print_help();
    }
    winbssid_destroy(hClientHandle);
    ExitProcess(0);
}

void winbssid_init(PHANDLE phClientHandle) {
    DWORD dwClientVersion = 2; // Client for Windows Vista and higher
    DWORD dwNegotiatedVersion = dwClientVersion; // Highest version supported by both client and server
    DWORD wlanOpenHandleError = WlanOpenHandle(dwClientVersion, nullptr, &dwNegotiatedVersion, phClientHandle);
    if (wlanOpenHandleError != ERROR_SUCCESS) {
        std::cout << "Could not open handle" << std::endl;
        return;
    }
}

void winbssid_destroy(HANDLE hClientHandle) {
    if (hClientHandle != nullptr) {
        WlanCloseHandle(hClientHandle, nullptr);
        hClientHandle = nullptr;
    }
}

void winbssid_connect(PHANDLE phClientHandle, const GUID *pInterfaceGuid, const char *bssid) {
    WLAN_CONNECTION_PARAMETERS connectionParameters;
    DWORD wlanConnectError = WlanConnect(phClientHandle, pInterfaceGuid, &connectionParameters, nullptr);
    if (wlanConnectError != ERROR_SUCCESS) {
        std::cout << "Could not connect to " << bssid << std::endl;
        return;
    }
    std::cout << "Successfully connected to " << bssid << std::endl;
}

void winbssid_disconnect(HANDLE hClientHandle, const GUID *pInterfaceGuid, const char *bssid) {
    DWORD wlanDisconnectError = WlanDisconnect(hClientHandle, pInterfaceGuid, nullptr);
    if (wlanDisconnectError != ERROR_SUCCESS) {
        std::cout << "Could not disconnect" << std::endl;
        return;
    }
    std::cout << "Successfully disconnected from " << bssid << std::endl;
}

bool winbssid_is_connected() {
    return false;
}

void winbssid_list_networks(HANDLE hClientHandle, const GUID *pInterfaceGuid) {
    DWORD dwFlags = WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES |
                    WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES;
    PWLAN_AVAILABLE_NETWORK_LIST availableNetworkList = nullptr;
    DWORD wlanGetAvailableNetworkListError = WlanGetAvailableNetworkList(
            hClientHandle,
            pInterfaceGuid,
            dwFlags,
            nullptr,
            &availableNetworkList
    );

    if (wlanGetAvailableNetworkListError != ERROR_SUCCESS) {
        std::cout << "Could not get available networks " << wlanGetAvailableNetworkListError << std::endl;
        return;
    }

    for (int i = 0; i < availableNetworkList->dwNumberOfItems; i++)
    {
        WLAN_AVAILABLE_NETWORK network = availableNetworkList->Network[i];
        wprintf(L"Profile name: %ws\n", network.strProfileName);
    }

    if (availableNetworkList != nullptr) {
        WlanFreeMemory(availableNetworkList);
        availableNetworkList = nullptr;
    }
}

void winbssid_list_interfaces(HANDLE hClientHandle) {
    PWLAN_INTERFACE_INFO_LIST pp_interface_list = nullptr;
    DWORD wlanEnumInterfacesError = WlanEnumInterfaces(hClientHandle, nullptr, &pp_interface_list);
    if (wlanEnumInterfacesError != ERROR_SUCCESS) {
        std::cout << "Could not get available interfaces "  << wlanEnumInterfacesError << std::endl;
        return;
    }

    for (int i = 0; i < pp_interface_list->dwNumberOfItems; i++)
    {
        WLAN_INTERFACE_INFO pwInfo = pp_interface_list->InterfaceInfo[i];
        WCHAR guidStr[40];
        int err = StringFromGUID2(pwInfo.InterfaceGuid, (LPOLESTR) &guidStr, 39);
        if (err == 0)
        {
            std::cout << "Failed to read interface GUID" << std::endl;
        }
        else
        {
            wprintf(L"GUID: %ws\n", guidStr);
        }

        wprintf(L"Description: %ws\n", pwInfo.strInterfaceDescription);
    }

    if (pp_interface_list != nullptr) {
        WlanFreeMemory(pp_interface_list);
        pp_interface_list = nullptr;
    }
}

void winbssid_print_help() {
    std::cout << "WinBSSID [connect | disconnect] [BSSID]" << std::endl;
}