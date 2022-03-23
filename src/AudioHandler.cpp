#include "Mmdeviceapi.h"
#include "PolicyConfig.h"

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
    IPolicyConfigVista *pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
                                  NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
    if (SUCCEEDED(hr))
    {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
        pPolicyConfig->Release();
    }
    return hr;
}

void setNextAudioDeviceAsDefault()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return;

    // MMDevice Enumerator
    IMMDeviceEnumerator *pEnum = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
    if (FAILED(hr))
        return;

    // Standaard uitvoerapparaat
    IMMDevice *pDefDevice = NULL;
    hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefDevice);
    if (FAILED(hr))
        return;

    LPWSTR defaultId;
    pDefDevice->GetId(&defaultId);

    // Active uitvoerapparaten
    IMMDeviceCollection *pDevices;
    hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
    if (FAILED(hr))
        return;

    // Loopt over uitvoerapparaten + zoekt index van standaardtoestel
    UINT count;
    pDevices->GetCount(&count);
    int defDeviceIndex = -1;
    for (UINT i = 0; i < count; i++)
    {
        // Neem audio device met index i
        IMMDevice *pDevice;
        hr = pDevices->Item(i, &pDevice);
        if (FAILED(hr))
            return;

        // Krijg zijn ID
        LPWSTR wstrID;
        hr = pDevice->GetId(&wstrID);
        if (FAILED(hr))
            return;

        // Check of dit het standaard-apparaat is
        if (wcscmp(wstrID, defaultId) == 0)
        {
            defDeviceIndex = i;
            break;
        }

        pDevice->Release();
    }

    // Neem volgende device-index
    defDeviceIndex = defDeviceIndex + 1 == count ? 0 : defDeviceIndex + 1;

    // Neem volgend device
    IMMDevice *newDevice;
    pDevices->Item(defDeviceIndex, &newDevice);

    // Neem volgend device ID
    LPWSTR newDeviceID = NULL;
    newDevice->GetId(&newDeviceID);

    // Zet standaard output
    SetDefaultAudioPlaybackDevice(newDeviceID);

    // Cleanup
    pDefDevice->Release();
    newDevice->Release();
    pDevices->Release();
    pEnum->Release();
    CoUninitialize();
}