#ifndef REDISNATIVE_DESCRIPTORS_H
#define REDISNATIVE_DESCRIPTORS_H

#include <types.h>

struct MethodDescriptor {
    const WCHAR_T* engName = nullptr;
    const WCHAR_T* rusName = nullptr;
    size_t paramCount = 0;
    bool hasRetValue = false;
    short method;

    explicit MethodDescriptor(const WCHAR_T* _engName,  const WCHAR_T* _rusName, size_t _paramCount, bool _hasRetValue, short eMethod)
            : engName(_engName)
            , rusName(_rusName)
            , paramCount(_paramCount)
            , hasRetValue(_hasRetValue)
            , method(eMethod)
    {

    }
};

struct PropertyDescriptor {
    enum TYPE {
        READABLE = 1,
        WRITEABLE = 2,
    };

    const WCHAR_T* engName;
    const WCHAR_T* rusName;
    short rw = 0;
    TYPEVAR type = ENUMVAR::VTYPE_VARIANT;
    short prop;

    explicit PropertyDescriptor(const WCHAR_T* _engName,  const WCHAR_T* _rusName, short _rw, TYPEVAR _type, short eProp) noexcept
            : engName(_engName)
            , rusName(_rusName)
            , rw(_rw)
            , type(_type)
            , prop(eProp)
    {

    }

};

#endif //REDISNATIVE_DESCRIPTORS_H
