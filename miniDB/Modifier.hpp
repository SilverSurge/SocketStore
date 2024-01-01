#ifndef MODIFIER_HPP
#define MODIFIER_HPP

#include "Schema.hpp"
#include "Dependencies.hpp"
#include "StatusCodes.hpp"

template<class T>
class Modifier 
{
// assert
static_assert(std::is_base_of<Schema, T>::value, "Modifier<T> - T must be Schema or any of it's derived class");

public:
    // simple pure virtual method for some filtering [complete]
    virtual void modify(T* _in_rec, T* _out_rec) = 0;
};

#endif // MODIFIER_HPP