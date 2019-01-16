#include "library.hpp"

#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

namespace ngo
{
using namespace std;
using v8::Context;
using v8::Exception;
using v8::External;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Value;

typedef void (*NodeFunc)(const FunctionCallbackInfo<Value> &);
typedef char *(*GoFunc)(const char *);
typedef char *(*REGISTERFUNC)();

void ThrowError(Isolate *isolate, const char *msg)
{
    isolate->ThrowException(Exception::Error(
        String::NewFromUtf8(isolate,
                            msg,
                            NewStringType::kNormal)
            .ToLocalChecked()));
}

void ThrowTypeError(Isolate *isolate, const char *msg)
{
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate,
                            msg,
                            NewStringType::kNormal)
            .ToLocalChecked()));
}

vector<string> SplitString(string str, char delimeter)
{
    stringstream ss(str);
    string item;
    vector<string> result;
    while (getline(ss, item, delimeter))
    {
        result.push_back(item);
    }
    return result;
}

const char *ToCString(Isolate *isolate, Local<String> str)
{
    String::Utf8Value value(isolate, str);
    return *value;
}

void lib_invoke(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsString())
    {
        ThrowTypeError(isolate, "Wrong arguments");
        return;
    }

    auto context = Context::New(isolate);
    auto registry = Local<Object>::Cast(args.This()->Get(context, String::NewFromUtf8(isolate, "registry")).ToLocalChecked());
    auto func = (GoFunc)(Local<External>::Cast(registry->Get(args[0]->ToString()))->Value());
    auto payload = ToCString(isolate, args[1]->ToString());
    auto r = func(payload);
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, r));
}

void lib_close(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    auto isolate = args.GetIsolate();
    auto handle = Local<External>::Cast(args.This()->GetInternalField(0))->Value();
    CloseSharedLibrary(handle);
    args.This()->SetInternalField(0, External::New(isolate, nullptr));
}

void openlib(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    auto isolate = args.GetIsolate();
    if (args.Length() != 1 || !args[0]->IsString())
    {
        ThrowTypeError(isolate, "Wrong arguments");
        return;
    }

    auto handle = LoadSharedLibrary(ToCString(isolate, args[0]->ToString()));
    if (!handle)
    {
        ThrowError(isolate, "Could not load the shared library");
        return;
    }

    auto context = Context::New(isolate);
    auto libTemplate = ObjectTemplate::New(isolate);
    libTemplate->SetInternalFieldCount(1);
    auto lib = libTemplate->NewInstance(context).ToLocalChecked();
    lib->SetInternalField(0, External::New(isolate, handle));
    lib->Set(Nan::New("close").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(lib_close)->GetFunction());
    lib->Set(Nan::New("invoke").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(lib_invoke)->GetFunction());

    auto registry = Object::New(isolate);
    auto REGISTER = (REGISTERFUNC)LoadFunction(handle, "REGISTER");
    auto funcNames = REGISTER();
    auto functions = SplitString(string(funcNames), '|');
    for (auto funcName = functions.begin(); funcName != functions.end(); ++funcName)
    {
        auto func = LoadFunction(handle, funcName->c_str());
        registry->Set(String::NewFromUtf8(isolate, funcName->c_str()),
                      External::New(isolate, func));
    }
    lib->Set(String::NewFromUtf8(isolate, "registry"), registry);
    args.GetReturnValue().Set(lib);
}

void Initialize(Local<Object> exports, Local<Object> module)
{
    Nan::SetMethod(exports, "openlib", openlib);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
} // namespace ngo
