#include "miniDB/miniDB.hpp"
#include "SocketStoreDB.hpp"


/*
file to initialize the database,
adding some products and creating and admin

$ g++ initialize.cpp -o initialize
$ ./initialize

*/
int main()
{
    // creating relations
    char authdb[] = "AuthDB";
    Relation<AuthSchema> auth_relation(11, authdb);

    char productdb[] = "ProductDB";
    Relation<ProductSchema> product_relation(12, productdb);

    // creating some products
    product_relation.connect();

    int id = 1;
    ProductSchema product_1(id++, true, 10, 10, (char*)"lays_10");
    ProductSchema product_2(id++, true, 12, 18, (char*)"coffee");
    ProductSchema product_3(id++, true, 15, 12, (char*)"britannia cake");
    ProductSchema product_4(id++, true, 20, 12, (char*)"lays_20");
    ProductSchema product_5(id++, true, 20, 16, (char*)"omlette");
    ProductSchema product_6(id++, true, 30, 24, (char*)"paneer sandwich");
    ProductSchema product_7(id++, true, 55, 16, (char*)"paneer chilli");
    ProductSchema product_8(id++, true, 80, 15, (char*)"biryani");

    product_relation.create_record(&product_1);
    product_relation.create_record(&product_2);
    product_relation.create_record(&product_3);
    product_relation.create_record(&product_4);
    product_relation.create_record(&product_5);
    product_relation.create_record(&product_6);
    product_relation.create_record(&product_7);
    product_relation.create_record(&product_8);

    product_relation.commit();
    product_relation.disconnect();


    // creating an admin
    auth_relation.connect();    

    char adm[] = "admin";
    AuthSchema admin(0, true, adm, adm);

    auth_relation.create_record(&admin);

    auth_relation.commit();
    auth_relation.disconnect();

    return 0;
}