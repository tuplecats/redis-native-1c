#include "../include/RedisNative.h"
#include "../include/extension.h"

static const WCHAR_T* classNames = L"RedisNative";

const WCHAR_T* GetClassNames() {
    return classNames;
}

long GetClassObject(const WCHAR_T* clsName, IComponentBase** pIntf) {

    if (*pIntf) return 0;

    auto clsName_utf8 = extension::to_utf8(const_cast<wchar_t*>(clsName));
    if (std::strcmp(clsName_utf8.data(), "RedisNative") == 0) {
        *pIntf = new RedisNative;
        return (long)*pIntf;
    }
    return 0;
}

long DestroyObject(IComponentBase** pIntf) {
    delete *pIntf;
    return 0;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities) {
    return eAppCapabilitiesLast;
}