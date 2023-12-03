#include "audio_output.h"

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
    CoInitialize(NULL);

    IPolicyConfig *pPolicyConfig = NULL;
    HRESULT hr = CoCreateInstance(&CLSID_IPolicyConfig, NULL, CLSCTX_ALL, &IID_IPolicyConfig, (void **)&pPolicyConfig);

    if (FAILED(hr))
        return hr;

    hr = pPolicyConfig->lpVtbl->SetDefaultEndpoint(pPolicyConfig, devID, eConsole);

    if (FAILED(hr))
        return hr;

    pPolicyConfig->lpVtbl->Release(pPolicyConfig);

    CoUninitialize();

    return S_OK;
}

HRESULT setNextAudioDeviceAsDefault()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // MMDevice Enumerator
    IMMDeviceEnumerator *pEnum = NULL;
    hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&pEnum);
    if (FAILED(hr))
        return hr;

    // Get default audio endpoint
    IMMDevice *pDefDevice = NULL;
    hr = pEnum->lpVtbl->GetDefaultAudioEndpoint(pEnum, eRender, eMultimedia, &pDefDevice);
    if (FAILED(hr))
        return hr;

    LPWSTR defaultId;
    pDefDevice->lpVtbl->GetId(pDefDevice, &defaultId);

    // Get all active audio endpoints
    IMMDeviceCollection *pDevices;
    hr = pEnum->lpVtbl->EnumAudioEndpoints(pEnum, eRender, DEVICE_STATE_ACTIVE, &pDevices);
    if (FAILED(hr))
        return hr;

    // Loop over these endpoints, and find the default ones id
    UINT count;
    pDevices->lpVtbl->GetCount(pDevices, &count);
    int defDeviceIndex = -1;
    for (UINT i = 0; i < count; i++)
    {
        // Get device at index i
        IMMDevice *pDevice;
        hr = pDevices->lpVtbl->Item(pDevices, i, &pDevice);
        if (FAILED(hr))
            return hr;

        // Get device ID
        LPWSTR wstrID;
        hr = pDevice->lpVtbl->GetId(pDevice, &wstrID);
        if (FAILED(hr))
            return hr;

        // Check if its the default device
        if (wcscmp(wstrID, defaultId) == 0)
        {
            defDeviceIndex = i;
            break;
        }

        pDevice->lpVtbl->Release(pDevice);
    }

    // Get next device index
    defDeviceIndex = (defDeviceIndex + 1) % count;

    // Get next device
    IMMDevice *newDevice;
    hr = pDevices->lpVtbl->Item(pDevices, defDeviceIndex, &newDevice);
    if (FAILED(hr))
        return hr;

    // Get device id
    LPWSTR newDeviceID = NULL;
    hr = newDevice->lpVtbl->GetId(newDevice, &newDeviceID);
    if (FAILED(hr))
        return hr;

    // Set new device as default
    hr = SetDefaultAudioPlaybackDevice(newDeviceID);
    if (FAILED(hr))
        return hr;

    // Cleanup COM
    pDefDevice->lpVtbl->Release(pDefDevice);
    newDevice->lpVtbl->Release(newDevice);
    pDevices->lpVtbl->Release(pDevices);
    pEnum->lpVtbl->Release(pEnum);
    CoUninitialize();

    return S_OK;
}