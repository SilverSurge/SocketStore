#ifndef FILTER_HPP
#define FILTER_HPP

#include "Schema.hpp"
#include "Dependencies.hpp"
#include "StatusCodes.hpp"

template<class T>
class Filter 
{
// assert
static_assert(std::is_base_of<Schema, T>::value, "Filter<T> - T must be Schema or any of it's derived class");

public:
    // simple pure virtual method for some filtering [complete]
    virtual bool filter(T* _rec) = 0;
};

#endif // FILTER_HPP