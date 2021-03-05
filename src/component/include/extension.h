#ifndef REDISNATIVE_EXTENSION_H
#define REDISNATIVE_EXTENSION_H

#include <locale>
#include <codecvt>
#include <string>
#include <AddInDefBase.h>

namespace extension {

    std::wstring ToUtf16(std::string str);

    std::string ToUtf8(const wchar_t* str);

    std::string to_utf8(wchar_t* value);

    std::wstring to_utf16(const std::string& value);

}

#endif //REDISNATIVE_EXTENSION_H
