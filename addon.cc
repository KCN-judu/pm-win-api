#define NAPI_VERSION 9

#include <node_api.h>
#include "InteractionQueue.h"
#include "MacroFileProc.h"

// globVars
InteractionQueue iq;
std::fstream file;

napi_value winOpenFile(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    size_t bufSize = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &bufSize);
    std::string buffer(bufSize + 1, '\0');
    napi_get_value_string_utf8(env, argv[0], &buffer[0], buffer.size(), nullptr);

    openFile(file, buffer);
    return nullptr;
}

napi_value winCloseFile(napi_env env, napi_callback_info info)
{
    closeFile(file);
    return nullptr;
}

napi_value winReadMacroFile(napi_env env, napi_callback_info info)
{
    readMacroFile(file, iq);
    return nullptr;
}

napi_value winAutoPerform(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_status status;

    status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok)
        return nullptr;

    int32_t msPerTick;
    status = napi_get_value_int32(env, argv[0], &msPerTick);
    if (status != napi_ok)
        return nullptr;

    while (!iq.empty())
    {
        iq.autoPerform();
        Sleep(msPerTick);
    }

    return nullptr;
}

napi_value Init(napi_env env, napi_value exports)
{
    // init sets for commands searching in readMacroFile()
    procInit();

    napi_status status;
    napi_value fn;

    status = napi_create_function(env, nullptr, 0, winOpenFile, nullptr, &fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_set_named_property(env, exports, "winOpenFile", fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_create_function(env, nullptr, 0, winCloseFile, nullptr, &fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_set_named_property(env, exports, "winCloseFile", fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_create_function(env, nullptr, 0, winReadMacroFile, nullptr, &fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_set_named_property(env, exports, "winReadMacroFile", fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_create_function(env, nullptr, 0, winAutoPerform, nullptr, &fn);
    if (status != napi_ok)
        return nullptr;

    status = napi_set_named_property(env, exports, "winAutoPerform", fn);
    if (status != napi_ok)
        return nullptr;

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)