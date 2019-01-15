#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void *LoadSharedLibrary(const char *path)
{
#if _WIN32
    void *handle = (void *)LoadLibraryA(path);
#else
    void *handle = (void *)dlopen(path, RTLD_LAZY);
#endif
    if (!handle)
    {
        return nullptr;
    }
    return handle;
}

void CloseSharedLibrary(void *handle)
{
#if _WIN32
    if (!handle)
    {
        FreeLibrary((HINSTANCE)handle);
    }
#else
    if (!handle)
    {
        dlclose(handle);
    }
#endif
}

void *LoadFunction(void *handle, const char *name)
{
    if (!handle)
    {
        return nullptr;
    }
#if _WIN32
    void *func = (void *)GetProcAddress((HINSTANCE)handle, (LPCSTR)name);
#else
    void *func = (void *)dlsym(handle, *name);
#endif
    if (!func)
    {
        return nullptr;
    }
    return func;
}

#endif
