/**
 * The IPolicyConfig interface was originally reverse engineered by EreTIk and
 * allows us to change the default audio output device.
 *
 * This is a C implementation of said interface.
 */

typedef struct IPolicyConfig IPolicyConfig;
struct DeviceShareMode;

DEFINE_GUID(CLSID_IPolicyConfig, 0x294935CE, 0xF637, 0x4E7C, 0xA4, 0x1B, 0xAB, 0x25, 0x54, 0x60, 0xB8, 0x62);
DEFINE_GUID(IID_IPolicyConfig, 0x568B9108, 0x44BF, 0x40B4, 0x90, 0x06, 0x86, 0xAF, 0xE5, 0xB5, 0xA6, 0x20);

typedef HRESULT STDMETHODCALLTYPE QueryInterfacePtr(IPolicyConfig *, REFIID, IPolicyConfig **);
typedef ULONG STDMETHODCALLTYPE AddRefPtr(IPolicyConfig *);
typedef ULONG STDMETHODCALLTYPE ReleasePtr(IPolicyConfig *);

typedef HRESULT GetMixFormatPtr(IPolicyConfig *, PCWSTR, WAVEFORMATEX **);
typedef HRESULT STDMETHODCALLTYPE GetDeviceFormatPtr(IPolicyConfig *, PCWSTR, INT, WAVEFORMATEX **);
typedef HRESULT STDMETHODCALLTYPE SetDeviceFormatPtr(IPolicyConfig *, PCWSTR, WAVEFORMATEX *, WAVEFORMATEX *);
typedef HRESULT STDMETHODCALLTYPE GetProcessingPeriodPtr(IPolicyConfig *, PCWSTR, INT, PINT64, PINT64);
typedef HRESULT STDMETHODCALLTYPE SetProcessingPeriodPtr(IPolicyConfig *, PCWSTR, PINT64);
typedef HRESULT STDMETHODCALLTYPE GetShareModePtr(IPolicyConfig *, PCWSTR, struct DeviceShareMode *);
typedef HRESULT STDMETHODCALLTYPE SetShareModePtr(IPolicyConfig *, PCWSTR, struct DeviceShareMode *);
typedef HRESULT STDMETHODCALLTYPE GetPropertyValuePtr(IPolicyConfig *, PCWSTR, const PROPERTYKEY *, PROPVARIANT *);
typedef HRESULT STDMETHODCALLTYPE SetPropertyValuePtr(IPolicyConfig *, PCWSTR, const PROPERTYKEY *, PROPVARIANT *);
typedef HRESULT STDMETHODCALLTYPE SetDefaultEndpointPtr(IPolicyConfig *, PCWSTR wszDeviceId, ERole eRole);
typedef HRESULT STDMETHODCALLTYPE SetEndpointVisibilityPtr(IPolicyConfig *, PCWSTR, INT);

typedef struct
{
    QueryInterfacePtr *QueryInterface;
    AddRefPtr *AddRef;
    ReleasePtr *Release;
    GetMixFormatPtr *GetMixFormat;
    GetDeviceFormatPtr *GetDeviceFormat;
    SetDeviceFormatPtr *SetDeviceFormat;
    GetProcessingPeriodPtr *GetProcessingPeriod;
    SetProcessingPeriodPtr *SetProcessingPeriod;
    GetShareModePtr *GetShareMode;
    SetShareModePtr *SetShareMode;
    GetPropertyValuePtr *GetPropertyValue;
    SetPropertyValuePtr *SetPropertyValue;
    SetDefaultEndpointPtr *SetDefaultEndpoint;
    SetEndpointVisibilityPtr *SetEndpointVisibility;
} IPolicyConfigVtbl;

struct IPolicyConfig
{
    IPolicyConfigVtbl *lpVtbl;
};