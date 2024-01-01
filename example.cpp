#include "miniDB/miniDB.hpp"

#include <stdio.h>

// 1. Create Your Own Schema
// NOTE: KEEP THE SCHEMA OF FIXED LENGTH
class MySchema:public Schema
{
private:
    int price;
public:
    // NOTE: always keep empty constructor
    MySchema():Schema(){}
    MySchema(int _id, bool _is_valid, int _price): Schema(_id, _is_valid)
    {
        price = _price;
    }
    int get_price()
    {
        return price;
    }
    void set_price(int _price)
    {
        price = _price;
    }
    void display() 
    {
        if(is_valid)
        {
            printf("---------------------------\n");
            printf("id: %d\n", id);
            printf("price: %d\n", price);
            printf("---------------------------\n");
        }
    }
};

// 2. Create Filters
template <class T>
class Filter_Allower: public Filter<T>
{
public:
    bool filter(T* _rec) override
    {
        return true;
    }
};

class MySchema_Filter_EvenIndex: public Filter<MySchema> 
{
public:
    bool filter(MySchema* _rec) override
    {
        if(_rec->get_id() % 2 == 0)
            return true;
        return false;
    }
};

// 3. Create Appliers
class MySchema_Applier_Printer: public Applier<MySchema>
{
public:
    void apply(MySchema* _rec) override
    {
        _rec->display();
    }
};


// 3. Modifiers
class MySchema_Modifier_Doubler: public Modifier<MySchema>
{
public:
    void modify(MySchema* _in_rec, MySchema* _out_rec) override
    {
        // don't free the _out_rec
        _out_rec->set_price(_in_rec->get_price() * 2);
    }
};

int main()
{
    //-------------------------------------------------
    // NOTE There are 8 Sections below follow them one by one
    // uncomment a section [while keeping other sections commented]
    // ONE and Only One Section should be uncommented at a time
    // build the executable using
    // $ g++ main.cpp
    // run a.out using
    // $ a.out
    //-------------------------------------------------

    //-------------------------------------------------
    // 1. create a class MySchema extending Schema
    // 2. create Filters, Modifiers, Appliers for your Application
    // 3. start using miniDB.
    //-------------------------------------------------

    // creating a relation (basically a table)
    int key = 11;                           // should be unique for each relation
    char name[] = "MySchema";               // should be unique for each relation, fix this
    Relation<MySchema> relation(key, name); 
    // connect to the database
    relation.connect();         // everything is done between a [connect, disconnect] block


    /*
    // 1. Create Records - A

    // create some instances
    int id = 1;
    MySchema obj1(id++, true, 10);
    MySchema obj2(id++, true, 20);
    MySchema obj3(id++, true, 30);
    MySchema obj4(id++, true, 40);

    // create_records
    relation.create_record(&obj1);
    relation.create_record(&obj2);
    relation.create_record(&obj3);
    relation.create_record(&obj4);

    // note in ./relations, on looking on the file sizes using 
    // $ll relations
    // db file has 96 and backup file has 0, this means nothing has been saved
    // that is until commit()
    */


    
    /*
    // 2. Create Records - B

    // create some instances
    int id = 1;
    MySchema obj1(id++, true, 10);
    MySchema obj2(id++, true, 20);
    MySchema obj3(id++, true, 30);
    MySchema obj4(id++, true, 40);

    // create_records
    relation.create_record(&obj1);
    relation.create_record(&obj2);
    relation.create_record(&obj3);
    relation.create_record(&obj4);

    // commit
    relation.commit();

    // note in ./relations, on looking on the file sizes
    // db file has 96 and backup also has 96,
    // this means changes have been saved
    */

    /*
    // 3. Retrieve Records 

    // things for retireval 
    Filter_Allower<MySchema> allower;       // a filter
    Collection<MySchema> collection;        // a collection
    relation.retrieve_record(&allower, &collection);

    MySchema_Applier_Printer printer;       // an applier
    collection.iterate(&printer);           // iterate over the filtered records
    collection.free();                      // free the memory if not required

    */

    /*
    // 4. Updating Records - A

    // things for modifying
    // a filter
    MySchema_Filter_EvenIndex evenidx;      // a filter
    MySchema_Modifier_Doubler dbl;          // a modifier
    relation.update_record(&evenidx, &dbl);
    relation.commit();

    // note if you don't commit it wont save things 
    // the changes will last until the execution completes,
    // if you disconnect and then connect again, everything will be lost.

    */

    // 5. Updating Records - B
    // run the Retrieving Records again. you will see a difference in the output


    /*
    // 6. Deleting Records - A

    // things for deleting
    MySchema_Filter_EvenIndex evenidx;      // a filter
    relation.delete_record(&evenidx);
    relation.commit();

    // note if you don't commit it wont save things 
    // the changes will last until the execution completes,
    // if you disconnect and then connect again, everything will be lost.

    // note the sizes of files in ./relations
    // db - 96, backup - 96, it seems that nothing has been deleted,
    // as i use a deletion marker for deleting. but stay with me

    */

    // 7. Deleting Records - B
    // run the Retrieving Records again. you will see a difference in the output
    
    /*
    // 8. Thrashing
    // this slams all the records together, and
    // compresses the size of db file by removing the deleted records
    relation.thrash();
    relation.commit(); // remember the importance of commit
    */

    // disconnect
    relation.disconnect();
    return 0;
}
