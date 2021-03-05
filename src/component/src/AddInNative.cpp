#include "../include/AddInNative.h"
#include "../include/Descriptors.h"
#include "../include/memoryHelper.h"
#include "../include/TypeHelper.h"
#include "../include/extension.h"
#include <vector>

bool CAddInNative::Init(void *disp) {
    m_iConnect = (IAddInDefBase*)disp;
    return true;
}

bool CAddInNative::setMemManager(void *memManager) {
    m_iMemory = (IMemoryManager*)memManager;
    return true;
}

long CAddInNative::GetInfo() {
    return 1000;
}

long CAddInNative::GetNProps() {
    return GetPropsDescriptors().size();
}

long CAddInNative::FindProp(const WCHAR_T* wsPropName) {
    auto props_desc = GetPropsDescriptors();
    auto it = std::find_if(props_desc.begin(), props_desc.end(), [wsPropName](auto& value) {
        return strcmp((const char*)value.engName, (const char*)wsPropName) == 0 || strcmp((const char*)value.rusName, (const char*)wsPropName) == 0;
    });
    return it == props_desc.end() ? -1 : it - props_desc.begin();
}

const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias) {
    if (lPropNum >= GetPropsDescriptors().size()) {
        return nullptr;
    }

    WCHAR_T* allocation = nullptr;
    auto propName = (lPropAlias == 0 ? GetPropsDescriptors()[lPropNum].engName : GetPropsDescriptors()[lPropNum].rusName);

    copyString(m_iMemory, &allocation, propName, wcslen(propName) + 1);

    return allocation;
}

bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal) {
    auto it = propertiesValues.find(lPropNum);
    if (it == propertiesValues.end()) {
        pvarPropVal->vt = ENUMVAR::VTYPE_EMPTY;
    }
    else if (is_String(it->second.vt)) {
        pvarPropVal->vt = it->second.vt;
        pvarPropVal->wstrLen = it->second.wstrLen;

        if (!copyString(m_iMemory, &pvarPropVal->pwstrVal, it->second.pwstrVal, pvarPropVal->wstrLen)) {
            return false;
        }
    }
    else {
        memcpy_s(pvarPropVal, sizeof(tVariant), &it->second, sizeof(tVariant));
    }

    return true;
}

bool CAddInNative::SetPropVal(const long lPropNum, tVariant* pvarPropVal) {
    auto& descriptor = GetPropsDescriptors()[lPropNum];

    auto it = propertiesValues.find(lPropNum);
    if (it == propertiesValues.end()) {
        it = propertiesValues.insert({lPropNum, tVariant()}).first;
    }

    if (is_String(pvarPropVal->vt) && is_String(descriptor.type)) {
        tVariant variant;
        variant.vt = ENUMVAR::VTYPE_PWSTR;
        variant.pwstrVal = new WCHAR_T[pvarPropVal->wstrLen];
        variant.wstrLen = pvarPropVal->wstrLen;
        memcpy_s(variant.pwstrVal, pvarPropVal->wstrLen * sizeof(WCHAR_T), pvarPropVal->pwstrVal, pvarPropVal->wstrLen * sizeof(WCHAR_T));

        it->second = variant;
    }
    else if (is_Numeric(pvarPropVal->vt) && is_Numeric(descriptor.type))
    {
        it->second = *pvarPropVal;
        it->second.vt = descriptor.type;

        switch (pvarPropVal->vt) {
            case ENUMVAR::VTYPE_R4:
            case ENUMVAR::VTYPE_R8:
                break;
            default:
                it->second.dblVal = static_cast<double>(pvarPropVal->lVal);
                break;
        }
    }
    else if (is_Boolean(pvarPropVal->vt) && is_Boolean(descriptor.type)) {
        it->second = *pvarPropVal;
    }
    else {
        return false;
    }

    return true;
}

bool CAddInNative::IsPropReadable(const long lPropNum) {
    return GetPropsDescriptors()[lPropNum].rw & PropertyDescriptor::READABLE;
}

bool CAddInNative::IsPropWritable(const long lPropNum) {
    return GetPropsDescriptors()[lPropNum].rw & PropertyDescriptor::WRITEABLE;
}

long CAddInNative::GetNMethods() {
    return GetMethodsDescriptors().size();
}

long CAddInNative::FindMethod(const WCHAR_T* wsMethodName) {
    auto method_desc = GetMethodsDescriptors();
    auto it = std::find_if(method_desc.begin(), method_desc.end(), [wsMethodName](auto& value) {
        return strcmp((const char*)value.engName, (const char*)wsMethodName) == 0 || strcmp((const char*)value.rusName, (const char*)wsMethodName) == 0;
    });
    return it == method_desc.end() ? -1 : it - method_desc.begin();
}

const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias) {
    if (lMethodNum >= GetMethodsDescriptors().size()) {
        return nullptr;
    }
    WCHAR_T* allocation = nullptr;
    auto methodName = (lMethodAlias == 0 ? GetMethodsDescriptors()[lMethodNum].engName : GetMethodsDescriptors()[lMethodNum].rusName);

    copyString(m_iMemory, &allocation, methodName, wcslen(methodName) + 1);

    return allocation;
}

long CAddInNative::GetNParams(const long lMethodNum) {
    return GetMethodsDescriptors()[lMethodNum].paramCount;
}

bool CAddInNative::HasRetVal(const long lMethodNum) {
    return GetMethodsDescriptors()[lMethodNum].hasRetValue;
}

void CAddInNative::SetLocale(const WCHAR_T* wsLocale) {

}

bool CAddInNative::set_property_value(short eProp, tVariant* value) {
    auto property_it = std::find_if(GetPropsDescriptors().begin(), GetPropsDescriptors().end(), [eProp] (auto& val) {
        return val.prop == eProp;
    });

    if (property_it == GetPropsDescriptors().end()) {
        return false;
    }

    auto property_index = property_it - GetPropsDescriptors().begin();

    return SetPropVal(property_index, value);
}