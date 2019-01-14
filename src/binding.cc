#include <node.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "binding.h"

namespace ngo
{

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;
using v8::Value;

void *open_handle(const char *path)
{
    void *handle;
    handle = dlopen(path, RTLD_LAZY);
    if (!handle)
    {
        fputs(dlerror(), stderr);
        exit(1);
    }
    return handle;
}

void close(void *handle)
{
    dlclose(handle);
}

char *call(void *handle, const char *method, const char *payload)
{
    char *error;
    void *ptr = dlsym(handle, method);
    if ((error = dlerror()) != NULL)
    {
        fputs(error, stderr);
        exit(1);
    }
    GoFunc func = (GoFunc)ptr;
    return func(payload);
}

void Initialize(Local<Object> exports)
{
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

} // namespace ngo