#pragma once

#include <redis_connection.h>
#include <ComponentBase.h>
#include <AddInDefBase.h>
#include <IMemoryManager.h>
#include <map>

class CAddInNative: public IComponentBase {

    friend struct MethodDescriptor;
    friend struct PropertyDescriptor;

public:

    CAddInNative() = default;

    virtual ~CAddInNative() {}

    bool ADDIN_API Init(void* disp);

    bool ADDIN_API setMemManager(void* memManager);

    long ADDIN_API GetInfo();

    virtual void ADDIN_API Done() = 0;

    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsExtName) = 0;

    virtual long ADDIN_API GetNProps() = 0;

    virtual const std::vector<PropertyDescriptor>& GetPropsDescriptors() = 0;

    long ADDIN_API FindProp(const WCHAR_T* wsPropName);

    const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);

    bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);

    bool ADDIN_API SetPropVal(const long lPropNum, tVariant* pvarPropVal);

    bool ADDIN_API IsPropReadable(const long lPropNum);

    bool ADDIN_API IsPropWritable(const long lPropNum);

    virtual long ADDIN_API GetNMethods() = 0;

    virtual const std::vector<MethodDescriptor>& GetMethodsDescriptors() = 0;

    long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);

    const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias);

    long ADDIN_API GetNParams(const long lMethodNum);

    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue) = 0;

    bool ADDIN_API HasRetVal(const long lMethodNum);

    virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) = 0;

    virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) = 0;

    void ADDIN_API SetLocale(const WCHAR_T* wsLocale);

protected:

    bool set_property_value(short, tVariant*);

    IAddInDefBase* m_iConnect;
    IMemoryManager* m_iMemory;

    std::map<long, tVariant> propertiesValues;
};

