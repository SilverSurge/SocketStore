#ifndef RECORD_LL_HPP
#define RECORD_LL_HPP


#include "Schema.hpp"
#include "Dependencies.hpp"
#include "StatusCodes.hpp"

template<class T>
class RecordLL 
{
// assert
static_assert(std::is_base_of<Schema, T>::value, "RecordLL<T> - T must be Schema or any of it's derived class");

public:
    // attributes
    T* record;
    RecordLL *next;

    // constructor [complete]
    RecordLL()
    {
        record = nullptr;
        next = nullptr;
    };
};

#endif // RECORD_LL_HPP