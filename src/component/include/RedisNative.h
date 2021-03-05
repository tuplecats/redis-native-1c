#pragma once

#include "AddInNative.h"
#include "Descriptors.h"

class RedisNative : public CAddInNative {

    enum Props {
        eFirstProperty = 0,
        eSecondProperty
    };

    enum Methods {
        eConnect = 0,
        eAuth,
        eLPush,
        eLPop,
        eRPush,
        eRPop,
        eListSize,
        eSetKey,
        eGetKey,
        eRemoveKey,
        eSetTimeOut
    };

public:

    bool RegisterExtensionAs(WCHAR_T **wsExtName) override;

    void Done() override;

    long GetNProps() override;

    const std::vector<PropertyDescriptor>& GetPropsDescriptors() override;

    long GetNMethods() override;

    const std::vector<MethodDescriptor>& GetMethodsDescriptors() override;

    bool GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue) override;

    bool CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) override;

    bool CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) override;

    // redis methods
    bool connect(tVariant* pvarRetValue, tVariant* paParams);

    bool lpush(tVariant* pvarRetValue, tVariant* paParams);

    bool rpush(tVariant* pvarRetValue, tVariant* paParams);

    bool lpop(tVariant* pvarRetValue, tVariant* paParams);

    bool rpop(tVariant* pvarRetValue, tVariant* paParams);

    bool get_length(tVariant* pvarRetValue, tVariant* paParams);

    bool auth(tVariant* pvarRetValue, tVariant* paParams);

    bool set(tVariant *pvarRetValue, tVariant *paParams);

    bool get(tVariant *pvarRetValue, tVariant *paParams);

    bool del(tVariant *pvarRetValue, tVariant *paParams);

    bool set_timeout(tVariant *pvarRetValue, tVariant *paParams);

private:

    void set_error_props(int code, const std::string& message);

    static std::vector<MethodDescriptor> m_methods;
    static std::vector<PropertyDescriptor> m_props;

    redis_connection* m_connection = nullptr;
    redis_error m_error;

};