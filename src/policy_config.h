#pragma once
#include <mmdeviceapi.h>

/**
 * IPolicyConfigVista is an undocumented interface, originally reverse engineered by EreTIk,
 * that allows us to change the default audio output device.
 *
 * Only the methods we call are typed, the others keep the vtable layout intact.
 */

DEFINE_GUID(CLSID_PolicyConfig, 0x294935CE, 0xF637, 0x4E7C, 0xA4, 0x1B, 0xAB, 0x25, 0x54, 0x60, 0xB8, 0x62);
DEFINE_GUID(IID_IPolicyConfig, 0x568B9108, 0x44BF, 0x40B4, 0x90, 0x06, 0x86, 0xAF, 0xE5, 0xB5, 0xA6, 0x20);

typedef struct IPolicyConfig IPolicyConfig;

typedef struct
{
    void *QueryInterface;
    void *AddRef;
    ULONG(STDMETHODCALLTYPE *Release)(IPolicyConfig *);
    void *GetMixFormat;
    void *GetDeviceFormat;
    void *SetDeviceFormat;
    void *GetProcessingPeriod;
    void *SetProcessingPeriod;
    void *GetShareMode;
    void *SetShareMode;
    void *GetPropertyValue;
    void *SetPropertyValue;
    HRESULT(STDMETHODCALLTYPE *SetDefaultEndpoint)(IPolicyConfig *, PCWSTR, ERole);
} IPolicyConfigVtbl;

struct IPolicyConfig
{
    IPolicyConfigVtbl *lpVtbl;
};
