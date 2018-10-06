#pragma once

#include <winnt.h>
#include <guiddef.h>

void winbssid_init(PHANDLE phClientHandle);
void winbssid_destroy(HANDLE phClientHandle);

void winbssid_connect(PHANDLE phClientHandle, const GUID *pInterfaceGuid, const char *bssid);
void winbssid_disconnect(HANDLE hClientHandle, const GUID *pInterfaceGuid, const char *bssid);
bool winbssid_is_connected();

void winbssid_list_networks(HANDLE hClientHandle, const GUID *pInterfaceGuid);
void winbssid_list_interfaces(HANDLE hClientHandle);

void winbssid_print_help();