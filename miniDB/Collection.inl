#include "Collection.hpp"

#include <stdio.h>

template <class T>
void Collection<T>::add(RecordLL<T> *_rec_ll)
{
    if(_rec_ll == nullptr)      // trying to add nullptr
        printf("Collection -- Add -- 1");
    else 
    {
        // case of first element
        if(head == nullptr)
        {
            head = _rec_ll;
            tail = _rec_ll;
        }
        else 
        {
            // add the element to the tail
            tail->next = _rec_ll;
            tail = tail->next;
        }
    }
}
template <class T>
void Collection<T>::iterate(Applier<T> *_applier)
{
    // iterate over all the elements
    RecordLL<T> *h = head;
    while(h)
    {
        _applier->apply(h->record);
        h = h->next;
    }
}

template <class T>
void Collection<T>::free()
{
    // iterating over the list and free every element
    RecordLL<T> *p = head, *q = nullptr;
    if(p == nullptr)
        return;
    while(p != nullptr)
    {
        q = p;
        p = p->next;
        delete q->record;
        delete q;
    }

    head = nullptr;
    tail = nullptr;
}

template <class T>
int Collection<T>::count()
{
    // traverse the list to find the count
    int cnt = 0;
    RecordLL<T> *h = head;
    while(h)
    {
        cnt++;
        h = h->next;
    }
    return cnt;
}