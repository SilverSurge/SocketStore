#ifndef APPLIER_HPP
#define APPLIER_HPP


#include "Schema.hpp"
#include "Dependencies.hpp"
#include "StatusCodes.hpp"

template<class T>
class Applier
{
// assert
static_assert(std::is_base_of<Schema, T>::value, "Applier<T> - T must be Schema or any of it's derived class");

public:
    // simple pure virtual method for some filtering [complete]
    virtual void apply(T* _rec) = 0;
};

#endif // APPLIER_HPP