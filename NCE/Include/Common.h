#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <typeinfo>

using ComponentHandle = uint32_t;
using ComponentIndex = uint32_t;
using EntityHandle = uint32_t;
using ComponentType = const char*;

template<class T>
ComponentType GetTypeName()
{
    return typeid(T).name();
}


#endif