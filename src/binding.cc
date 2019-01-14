#include <node.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "binding.h"

namespace ngo
{

using v8::Context;
using v8::Exception;
using v8::External;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::Private;
using v8::String;
using v8::Value;

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

void lib_close(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    void *handle = Local<External>::Cast(args.This()->GetInternalField(0))->Value();
    if (handle != nullptr)
    {
        dlclose(handle);
        args.This()->SetInternalField(0, External::New(isolate, nullptr));
    }
}

void openlib(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    if (args.Length() != 1)
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments",
                                NewStringType::kNormal)
                .ToLocalChecked()));
        return;
    }

    if (!args[0]->IsString())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments",
                                NewStringType::kNormal)
                .ToLocalChecked()));
        return;
    }

    String::Utf8Value str(isolate, args[0]);
    const char *path = *str;

    void *handle;
    handle = dlopen(path, RTLD_LAZY);
    if (!handle)
    {
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate,
                                dlerror(),
                                NewStringType::kNormal)
                .ToLocalChecked()));
        return;
    }

    Local<Context> context = Context::New(isolate);

    Local<ObjectTemplate> libTemplate = ObjectTemplate::New(isolate);
    libTemplate->SetInternalFieldCount(1);
    MaybeLocal<Object> maybeLib = libTemplate->NewInstance(context);
    Local<Object> lib;
    if (!maybeLib.ToLocal(&lib))
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Unknown error",
                                NewStringType::kNormal)
                .ToLocalChecked()));
        return;
    }
    lib->SetInternalField(0, External::New(isolate, handle));

    Local<FunctionTemplate> funcTemplate = FunctionTemplate::New(isolate, lib_close);
    Local<Function> func = funcTemplate->GetFunction();
    func->SetName(String::NewFromUtf8(isolate, "close"));
    lib->Set(v8::String::NewFromUtf8(isolate, "close"), func);

    args.GetReturnValue().Set(lib);
}

void Initialize(Local<Object> exports)
{
    NODE_SET_METHOD(exports, "openlib", openlib);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

} // namespace ngo