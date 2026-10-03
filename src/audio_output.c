#include <initguid.h>
#include <mmdeviceapi.h>
#include "audio_output.h"
#include "policy_config.h"

#ifndef __MINGW32__
DEFINE_GUID(CLSID_MMDeviceEnumerator, 0xbcde0395, 0xe52f, 0x467c, 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e);
DEFINE_GUID(IID_IMMDeviceEnumerator, 0xa95664d2, 0x9614, 0x4f35, 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6);
#endif

static HRESULT SetDefaultOutput(LPCWSTR id)
{
    IPolicyConfig *policy;
    HRESULT hr = CoCreateInstance(&CLSID_PolicyConfig, NULL, CLSCTX_ALL, &IID_IPolicyConfig, (void **)&policy);
    if (FAILED(hr))
        return hr;

    hr = policy->lpVtbl->SetDefaultEndpoint(policy, id, eConsole);

    policy->lpVtbl->Release(policy);
    return hr;
}

HRESULT CycleAudioOutput(void)
{
    IMMDeviceEnumerator *enumerator = NULL;
    IMMDeviceCollection *devices = NULL;
    IMMDevice *current = NULL, *next = NULL;
    LPWSTR currentId = NULL, nextId = NULL;
    UINT count, nextIndex = 0;

    HRESULT hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&enumerator);
    if (FAILED(hr))
        goto cleanup;

    hr = enumerator->lpVtbl->GetDefaultAudioEndpoint(enumerator, eRender, eConsole, &current);
    if (FAILED(hr))
        goto cleanup;

    hr = current->lpVtbl->GetId(current, &currentId);
    if (FAILED(hr))
        goto cleanup;

    hr = enumerator->lpVtbl->EnumAudioEndpoints(enumerator, eRender, DEVICE_STATE_ACTIVE, &devices);
    if (FAILED(hr))
        goto cleanup;

    hr = devices->lpVtbl->GetCount(devices, &count);
    if (FAILED(hr))
        goto cleanup;

    if (count == 0)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto cleanup;
    }

    for (UINT i = 0; i < count; i++)
    {
        IMMDevice *device;
        hr = devices->lpVtbl->Item(devices, i, &device);
        if (FAILED(hr))
            goto cleanup;

        LPWSTR id;
        hr = device->lpVtbl->GetId(device, &id);
        device->lpVtbl->Release(device);
        if (FAILED(hr))
            goto cleanup;

        BOOL isCurrent = wcscmp(id, currentId) == 0;
        CoTaskMemFree(id);
        if (isCurrent)
        {
            nextIndex = (i + 1) % count;
            break;
        }
    }

    hr = devices->lpVtbl->Item(devices, nextIndex, &next);
    if (FAILED(hr))
        goto cleanup;

    hr = next->lpVtbl->GetId(next, &nextId);
    if (FAILED(hr))
        goto cleanup;

    hr = SetDefaultOutput(nextId);

cleanup:
    CoTaskMemFree(nextId);
    CoTaskMemFree(currentId);
    if (next)
        next->lpVtbl->Release(next);
    if (current)
        current->lpVtbl->Release(current);
    if (devices)
        devices->lpVtbl->Release(devices);
    if (enumerator)
        enumerator->lpVtbl->Release(enumerator);
    return hr;
}
