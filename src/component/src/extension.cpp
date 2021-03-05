#include <Windows.h>
#include "../include/extension.h"

std::wstring extension::ToUtf16(std::string str)
{
    std::wstring ret;
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    if (len > 0)
    {
        ret.resize(len);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &ret[0], len);
    }
    return ret;
}

std::string extension::ToUtf8(const wchar_t* str)
{
    std::string ret;
    int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (len > 0)
    {
        ret.resize(len - 1);
        WideCharToMultiByte(CP_UTF8, 0, str, -1, &ret[0], len - 1, NULL, NULL);
    }
    return ret;
}

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

std::string extension::to_utf8(wchar_t* value) {
    //return ToUtf8(value);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes((wchar_t*)value);
}

std::wstring extension::to_utf16(const std::string& value) {
    std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>> converter;
    return converter.from_bytes(value);
}
