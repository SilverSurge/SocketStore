#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include "Schema.hpp"
#include "RecordLL.hpp"
#include "Applier.hpp"
#include "Dependencies.hpp"
#include "StatusCodes.hpp"

template<class T>
class Collection
{
// assert
static_assert(std::is_base_of<Schema, T>::value, "Collection<T> - T must be Schema or any of it's derived class");
 
private:
    RecordLL<T> *head;
    RecordLL<T> *tail;
public:

    // constructor [complete]
    Collection()
    {
        head = nullptr;
        tail = nullptr;
    }

    // add record at the end [complete]
    void add(RecordLL<T> *_rec_ll);

    // apply some method over the applier [complete]
    void iterate(Applier<T> *_applier);
    
    // free the complete linked list [complete]
    void free();
    // NOTE we can't free a void* ptr so the user needs to create a delete applier

    // number of records [complete]
    int count();
};

#include "Collection.inl"

#endif // COLLECTION_HPP
