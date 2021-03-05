#ifndef REDISNATIVE_VARIANTHELPER_H
#define REDISNATIVE_VARIANTHELPER_H

#include <AddInDefBase.h>
#include <IMemoryManager.h>
#include <string>
#include "memoryHelper.h"

namespace extension {
    bool setValue(IMemoryManager* manager, tVariant* variable, const std::string& value) {
        auto utf16 = extension::ToUtf16(value);
        variable->vt = ENUMVAR::VTYPE_PWSTR;
        variable->wstrLen = utf16.size();
        return copyString(manager, &variable->pwstrVal, (WCHAR_T*)utf16.data(), variable->wstrLen);
    }

    bool setValue(tVariant* variable) {
        variable->vt = ENUMVAR::VTYPE_EMPTY;
        return true;
    }

    bool setValue(tVariant* variable, bool value) {
        variable->vt = ENUMVAR::VTYPE_BOOL;
        variable->bVal = value;
        return true;
    }

    bool setValue(tVariant* variable, long value) {
        variable->vt = ENUMVAR::VTYPE_I8;
        variable->lVal = value;
        return true;
    }

    bool setValue(tVariant* variable, double value) {
        variable->vt = ENUMVAR::VTYPE_R8;
        variable->dblVal = value;
        return true;
    }

    std::shared_ptr<tVariant> makeValue(IMemoryManager* manager, const std::string& value) {
        auto utf16 = extension::ToUtf16(value);
        auto variant = std::shared_ptr<tVariant>(new tVariant);
        variant->vt = ENUMVAR::VTYPE_PWSTR;
        variant->wstrLen = utf16.size();
        if (copyString(manager, &variant->pwstrVal, (WCHAR_T*)utf16.data(), variant->wstrLen))
            return variant;
        return nullptr;
    }

    std::shared_ptr<tVariant> makeValue(bool value) {
        auto variant = std::shared_ptr<tVariant>(new tVariant);
        variant->vt = ENUMVAR::VTYPE_BOOL;
        variant->bVal = value;
        return variant;
    }

    std::shared_ptr<tVariant> makeValue(long value) {
        auto variant = std::shared_ptr<tVariant>(new tVariant);
        variant->vt = ENUMVAR::VTYPE_I8;
        variant->lVal = value;
        return variant;
    }

    std::shared_ptr<tVariant> makeValue(double value) {
        auto variant = std::shared_ptr<tVariant>(new tVariant);
        variant->vt = ENUMVAR::VTYPE_R8;
        variant->dblVal = value;
        return variant;
    }
}

#endif //REDISNATIVE_VARIANTHELPER_H
