#ifndef REDISNATIVE_TYPEHELPER_H
#define REDISNATIVE_TYPEHELPER_H
#include <types.h>

static bool is_float(TYPEVAR t) {
    return (t == ENUMVAR::VTYPE_R8 || t == ENUMVAR::VTYPE_R4);
}

static bool is_integer(TYPEVAR t) {
    return (t == ENUMVAR::VTYPE_I8 || t == ENUMVAR::VTYPE_I4 || t == ENUMVAR::VTYPE_I2 || t == ENUMVAR::VTYPE_I1
            || t == ENUMVAR::VTYPE_UI8 || t == ENUMVAR::VTYPE_UI4 || t == ENUMVAR::VTYPE_UI2
            || t == ENUMVAR::VTYPE_UI1);
}

static double get_number(tVariant* value) {
    if (is_integer(value->vt)) return value->lVal;
    if (is_float(value->vt)) return value->dblVal;
    return 0;
}


static bool is_Numeric(TYPEVAR t) {
    return is_float(t) || is_integer(t);
}

static bool is_String(TYPEVAR t) {
    return (t == ENUMVAR::VTYPE_PWSTR || t == ENUMVAR::VTYPE_PSTR);
}

static bool is_Boolean(TYPEVAR t) {
    return (t == ENUMVAR::VTYPE_BOOL);
}


#endif //REDISNATIVE_TYPEHELPER_H
