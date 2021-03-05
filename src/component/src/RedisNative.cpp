#include "../include/RedisNative.h"
#include "../include/extension.h"
#include "../include/memoryHelper.h"
#include "../include/VariantHelper.h"
#include "../include/TypeHelper.h"

std::vector<MethodDescriptor> RedisNative::m_methods = {
        // Ложь - не удалось подключиться, Истина - подключение успешно установлено
        MethodDescriptor(L"Connect", L"Подключиться" , 4, true, RedisNative::eConnect),
        // Ложь - не удалось авторизоваться, Истина - успешная авторизация
        MethodDescriptor(L"Auth", L"Авторизация" , 2, true, RedisNative::eAuth),
        // Ложь - не удалось добавить элемент, Истина - элемент успешно добавлен
        MethodDescriptor(L"LPush", L"ДобавитьЭлементВНачалоСписка" , 2, true, RedisNative::eLPush),
        // Неопределено - не удалось получить элемент, Значение - полученный элемент
        MethodDescriptor(L"LPop", L"ПолучитьЭлементНачалаСписка" , 1, true, RedisNative::eLPop),
        // Ложь - не удалось добавить элемент, Истина - элемент успешно добавлен
        MethodDescriptor(L"RPush", L"ДобавитьЭлементВКонецСписка" , 2, true, RedisNative::eRPush),
        // Неопределено - не удалось получить элемент, Значение - полученный элемент
        MethodDescriptor(L"RPop", L"ПолучитьЭлементКонцаСписка" , 1, true, RedisNative::eRPop),
        // -1 - не удалось получить размер списка, Число - размер списка
        MethodDescriptor(L"ListSize", L"РазмерСписка" , 1, true, RedisNative::eListSize),
        // Ложь - не удалось установить ключ, Истина - ключ установлен
        MethodDescriptor(L"SetKey", L"УстановитьЗначениеКлюча", 5, true, RedisNative::eSetKey),
        // Неопределено - не удалось получить значение ключа, Значение - значение ключа
        MethodDescriptor(L"GetKey", L"ПолучитьЗначениеКлюча", 1, true, RedisNative::eGetKey),
        // Ложь - не удалось удалить ключ, Истина - ключ удален
        MethodDescriptor(L"RemoveKey", L"УдалитьКлюч", 1, true, RedisNative::eRemoveKey),
        // Ложь - не удалось установить таймаут, Истина - таймаут установлен
        MethodDescriptor(L"SetTimeOut", L"УстановитьТаймаутОпераций", 2, true, RedisNative::eSetTimeOut),
};

std::vector<PropertyDescriptor> RedisNative::m_props = {
        // empty
        PropertyDescriptor(L"ErrorCode", L"КодОшибки", PropertyDescriptor::READABLE, ENUMVAR::VTYPE_R8, RedisNative::eFirstProperty),
        PropertyDescriptor(L"ErrorMsg", L"ТекстОшибки", PropertyDescriptor::READABLE, ENUMVAR::VTYPE_PWSTR, RedisNative::eSecondProperty)
};

bool RedisNative::RegisterExtensionAs(wchar_t **wsExtName)  {
    const WCHAR_T* wsExtension = L"Redis";
    const size_t wsSize = wcslen(wsExtension) + 1;

    return copyString(m_iMemory, wsExtName, wsExtension, wsSize);
}

void RedisNative::Done() {
    if (m_connection != nullptr) {
        delete m_connection;
        m_connection = nullptr;
    }
}

long RedisNative::GetNProps()  {
    return RedisNative::m_props.size();
}

const std::vector<PropertyDescriptor> & RedisNative::GetPropsDescriptors()  {
    return RedisNative::m_props;
}

long RedisNative::GetNMethods() {
    return RedisNative::m_methods.size();
}

const std::vector<MethodDescriptor> & RedisNative::GetMethodsDescriptors()  {
    return RedisNative::m_methods;
}

bool RedisNative::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue) {
    auto type = m_methods[lMethodNum].method;
    if (type == eSetKey && lParamNum > 1 && lParamNum < 6) {
        if (lParamNum == 2) extension::setValue(pvarParamDefValue, (long)0);
        if (lParamNum == 3) extension::setValue(pvarParamDefValue, (long)0);
        if (lParamNum == 4) extension::setValue(pvarParamDefValue, false);
        if (lParamNum == 5) extension::setValue(this->m_iMemory, pvarParamDefValue, "");
    }
    else if (type == eSetTimeOut && lParamNum >= 0 && lParamNum <= 1) {
        if (lParamNum == 0) extension::setValue(pvarParamDefValue, (long)0);
        if (lParamNum == 1) extension::setValue(pvarParamDefValue, (long)0);
    }
    else if ((type == eLPop || type == eRPop) && lParamNum == 1) {
        extension::setValue(pvarParamDefValue);
    }
    else if (type == eConnect && (lParamNum == 2 || lParamNum == 3)) {
        if (lParamNum == 2) extension::setValue(pvarParamDefValue, false);
        if (lParamNum == 3) extension::setValue(pvarParamDefValue, (long)0);
    }
    else return false;

    return true;
}

bool RedisNative::CallAsProc(const long lMethodNum, tVariant *paParams, const long lSizeArray) {
    return true;
}

bool RedisNative::CallAsFunc(const long lMethodNum, tVariant *pvarRetValue, tVariant *paParams, const long lSizeArray)  {
    set_error_props(0, "");

    auto type = m_methods[lMethodNum].method;

    if (type == eConnect) {
        return connect(pvarRetValue, paParams);
    }
    else if (type == eAuth) {
        return auth(pvarRetValue, paParams);
    }
    else if (type == eLPush) {
        return lpush(pvarRetValue, paParams);
    }
    else if (type == eLPop) {
        return lpop(pvarRetValue, paParams);
    }
    else if (type == eRPush) {
        return rpush(pvarRetValue, paParams);
    }
    else if (type == eRPop) {
        return rpop(pvarRetValue, paParams);
    }
    else if (type == eListSize) {
        return get_length(pvarRetValue, paParams);
    }
    else if (type == eSetKey) {
        return set(pvarRetValue, paParams);
    }
    else if (type == eGetKey) {
        return get(pvarRetValue, paParams);
    }
    else if (type == eRemoveKey) {
        return del(pvarRetValue, paParams);
    }
    else if (type == eSetTimeOut) {
        return set_timeout(pvarRetValue, paParams);
    }

    return true;
}

bool RedisNative::connect(tVariant *pvarRetValue, tVariant *paParams) {

    if (m_connection != nullptr) {
        delete m_connection;
        m_connection = nullptr;
    }

    std::string address = extension::to_utf8(paParams->pwstrVal);
    int port = static_cast<int>(get_number(&paParams[1]));
    bool use_tls = paParams[2].bVal;
    int timeout = static_cast<int>(get_number(&paParams[3]));

    m_connection = redis_connection::make_connection(address, std::to_string(port), use_tls, timeout, m_error);
    bool result = (m_connection != nullptr);

    extension::setValue(pvarRetValue, result);
    if (!result) {
        set_error_props(m_error.type, m_error.description);
    }

    return true;
}

bool RedisNative::lpush(tVariant *pvarRetValue, tVariant *paParams) {

    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string queue = extension::ToUtf8(paParams[0].pwstrVal);
    std::string value = extension::ToUtf8(paParams[1].pwstrVal);
    auto command = redis_cli("lpush", queue, value);
    auto response = m_connection->executeCommand(command, m_error);

    if (response.has_value()) {
        if (response.value().type == redis_object::tError) {
            pvarRetValue->vt = ENUMVAR::VTYPE_EMPTY;
            set_error_props(-2, std::get<std::string>(response.value().value));
            return true;
        }
        return extension::setValue(pvarRetValue, true);
    }

    extension::setValue(pvarRetValue, false);
    set_error_props(m_error.type ? -4 : -1, m_error.description);

    return true;

}

bool RedisNative::rpush(tVariant *pvarRetValue, tVariant *paParams) {

    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string queue = extension::ToUtf8(paParams[0].pwstrVal);
    std::string value = extension::ToUtf8(paParams[1].pwstrVal);
    auto command = redis_cli("rpush", queue, value);
    auto response = m_connection->executeCommand(command, m_error);

    if (response.has_value()) {
        if (response.value().type == redis_object::tError) {
            pvarRetValue->vt = ENUMVAR::VTYPE_EMPTY;
            set_error_props(-2, std::get<std::string>(response.value().value));
            return true;
        }
        return extension::setValue(pvarRetValue, true);
    }

    extension::setValue(pvarRetValue, false);
    set_error_props(m_error.type ? -4 : -1, m_error.description);

    return true;

}

bool RedisNative::lpop(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string queue = extension::to_utf8(paParams[0].pwstrVal);
    auto command = redis_cli("lpop", queue);
    auto response = m_connection->executeCommand(command, m_error);

    if (!response.has_value()) {
        extension::setValue(pvarRetValue);
        set_error_props(m_error.type, m_error.description);
    } else if (response.value().type == redis_object::tString) {
        return extension::setValue(m_iMemory, pvarRetValue, std::get<1>(response.value().value));
    } else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue);
        set_error_props(-2, std::get<std::string>(response.value().value));
    } else if (response.value().type == redis_object::tNullString) {
        extension::setValue(pvarRetValue);
        set_error_props(-3, std::string("Нет данных в очереди: ") + queue);
    }

    return true;
}

bool RedisNative::rpop(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string queue = extension::to_utf8(paParams[0].pwstrVal);
    auto command = redis_cli("rpop", queue);
    auto response = m_connection->executeCommand(command, m_error);

    if (!response.has_value()) {
        extension::setValue(pvarRetValue);
        set_error_props(m_error.type, m_error.description);
    } else if (response.value().type == redis_object::tString) {
        return extension::setValue(m_iMemory, pvarRetValue, std::get<1>(response.value().value));
    } else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue);
        set_error_props(-2, std::get<std::string>(response.value().value));
    } else if (response.value().type == redis_object::tNullString) {
        extension::setValue(pvarRetValue);
        set_error_props(-3, std::string("Нет данных в очереди: ") + queue);
    }

    return true;
}

bool RedisNative::get_length(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string queue = extension::to_utf8(paParams[0].pwstrVal);
    auto command = redis_cli("llen", queue);
    auto response = m_connection->executeCommand(command, m_error);
    if (!response.has_value()) {
        extension::setValue(pvarRetValue, (long)-1);
        set_error_props(m_error.type, m_error.description);
    } else if (response.has_value() && response.value().type == redis_object::tNumber) {
        return extension::setValue(pvarRetValue, static_cast<long>(std::get<2>(response.value().value)));
    } else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue, (long)-1);
        set_error_props(-2, std::get<std::string>(response.value().value));
    }

    return true;
}

bool RedisNative::auth(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string username = extension::to_utf8(paParams[0].pwstrVal);
    std::string password = extension::to_utf8(paParams[1].pwstrVal);
    auto command = redis_cli("auth", username, password);
    auto response = m_connection->executeCommand(command, m_error);
    if (!response.has_value()) {
        extension::setValue(pvarRetValue, false);
        set_error_props(m_error.type, m_error.description);
    } else if (response.has_value() && response.value().type == redis_object::tString) {
        return extension::setValue(pvarRetValue, true);
    } else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-2, std::get<std::string>(response.value().value));
    }

    return true;
}

bool RedisNative::set(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string key = extension::to_utf8(paParams[0].pwstrVal);
    std::string value = extension::to_utf8(paParams[1].pwstrVal);
    long timeout = static_cast<long>(get_number(&paParams[2])); // in milliseconds
    long nx_xx = static_cast<long>(get_number(&paParams[3])); // 1 - nx; 2 - xx
    bool keep_ttl = paParams[4].bVal;

    auto command = redis_cli("set", key, value);

    if (timeout > 0) {
        command.add_argument("PX");
        command.add_argument(std::to_string(timeout));
    }

    if (nx_xx == 1) command.add_argument("NX");
    if (nx_xx == 2) command.add_argument("XX");
    if (keep_ttl) command.add_argument("KEEPTTL");

    auto response = m_connection->executeCommand(command, m_error);
    if (!response.has_value()) {
        extension::setValue(pvarRetValue, false);
        set_error_props(m_error.type, m_error.description);
    } else if (response.value().type == redis_object::tString) {
        return extension::setValue(pvarRetValue, true);
    }
    else if (response.value().type == redis_object::tNullString) {
        extension::setValue(pvarRetValue, false);
        if (nx_xx == 2) {
            set_error_props(-2, "Не удалось установить значение: ключ не существует");
            return true;
        }
        set_error_props(-2, "Не удалось установить значение: ключ существует");
    }
    else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-2, std::get<std::string>(response.value().value));
    }

    return true;
}

bool RedisNative::get(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string key = extension::to_utf8(paParams[0].pwstrVal);

    auto command = redis_cli("get", key);
    auto response = m_connection->executeCommand(command, m_error);
    if (!response.has_value()) {
        extension::setValue(pvarRetValue);
        set_error_props(m_error.type ? -4 : -1, m_error.description);
    } else if (response.value().type == redis_object::tString) {
        return extension::setValue(m_iMemory, pvarRetValue, std::get<std::string>(response.value().value));
    }
    else if (response.value().type == redis_object::tNullString) {
        // ключ не существует
        extension::setValue(pvarRetValue);
        set_error_props(-3, "Не удалось получить значение: ключ не существует");
    }
    else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue);
        set_error_props(-2, std::get<std::string>(response.value().value));
    }

    return true;
}

bool RedisNative::del(tVariant *pvarRetValue, tVariant *paParams) {

    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    std::string key = extension::to_utf8(paParams[0].pwstrVal);

    auto command = redis_cli("del", key);
    auto response = m_connection->executeCommand(command, m_error);
    if (!response.has_value()) {
        extension::setValue(pvarRetValue, false);
        set_error_props(m_error.type ? -4 : -1, m_error.description);
    }
    else if (response.value().type == redis_object::tError) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-2, std::get<std::string>(response.value().value));
    }
    else {
        extension::setValue(pvarRetValue, true);
    }

    return true;
}

bool RedisNative::set_timeout(tVariant *pvarRetValue, tVariant *paParams) {
    if (m_connection == nullptr) {
        extension::setValue(pvarRetValue, false);
        set_error_props(-1, "Отсутствует подключение к Redis");
        return true;
    }

    int write_timeout = static_cast<int>(get_number(&paParams[0]));
    int read_timeout = static_cast<int>(get_number(&paParams[1]));

    if (write_timeout > 0) m_connection->set_read_timeout(write_timeout);
    if (read_timeout > 0) m_connection->set_write_timeout(read_timeout);

    extension::setValue(pvarRetValue, true);

    return true;
}

void RedisNative::set_error_props(int code, const std::string &message) {
    set_property_value(Props::eFirstProperty, extension::makeValue(static_cast<double>(code)).get());
    set_property_value(Props::eSecondProperty, extension::makeValue(m_iMemory, message).get());
}