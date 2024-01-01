#ifndef RELATION_HPP
#define RELATION_HPP

#include "SemaphoreController.hpp"
#include "Schema.hpp"
#include "Filter.hpp"
#include "Modifier.hpp"
#include "Applier.hpp"
#include "Collection.hpp"

template<class T>
class Relation 
{
// assert
static_assert(std::is_base_of<Schema, T>::value, "Relation<T> - T must be Schema or any of it's derived class");

private:
    // attributes
    int key;
    int fd;
    // int fd_backup;
    char relation_name[32];
    int rec_size;
    SemaphoreController semctrl;
public:

    // constructor [complete]
    Relation(int _key, char _relation_name[]);
    
    // get the file descriptors [complete]
    int connect();

    // close the file descriptors [complete]
    int disconnect();

    // append a record to the end [complete]
    int create_record(T* _rec);

    // retrieve record according to a filter [complete]
    int retrieve_record(Filter<T>* _filter, Collection<T>* _collection);

    // filter record and them apply a modifier [complete]
    int update_record(Filter<T>* _filter, Modifier<T>* _modifier);

    // turn the deletion marker on [complete]
    int delete_record(Filter<T>* _filter);

    // thrashing for compressing memory [complete]
    int thrash();

    // backing up [complete]
    int commit();
};

#include "Relation.inl"

#endif // RELATION_HPP