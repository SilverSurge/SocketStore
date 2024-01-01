#ifndef SOCKET_STORE_DP_HPP
#define SOCKET_STORE_DP_HPP

#include "miniDB/miniDB.hpp"

// 1. Schemas
class AuthSchema: public Schema
{
private:
    // attributes
    char username[32];
    char password[32];
public:
    // doc says so
    AuthSchema():Schema(){}
    
    // constructor
    AuthSchema(int _id, bool _is_valid, char _username[], char _password[]): Schema(_id, _is_valid)
    {
        strcpy(username, _username);
        strcpy(password, _password);
    }

    // display method
    void display()
    {
        if(is_valid)
        {
            printf("username: %s\n", username);
            printf("password: %s\n", password);
        }
    }

    // setters 
    void set_username(char _username[])
    {
        strcpy(username, _username);
    }
    void set_password(char _password[])
    {
        strcpy(password, _password);
    }

    // getters
    char* get_username()
    {
        return username;
    }
    char* get_password()
    {
        return password;
    }
};

class ProductSchema: public Schema 
{
private:
    // attributes
    int price;
    int quantity;
    char item_name[32];
public:

    // doc says so
    ProductSchema():Schema(){}

    // constructor
    ProductSchema(int _id, bool _is_valid, int _price, int _quantity, char _item_name[]): Schema(_id, _is_valid)
    {
        price = _price;
        quantity = _quantity;
        strcpy(item_name, _item_name);
    }

    void display()
    {
        if(is_valid)
        {
            printf("--------------------------------------\n");
            printf("id: %d\n", id);
            printf("item_name: %s\n", item_name);
            printf("price: %d\n", price);
            printf("quantity: %d\n", quantity);
            printf("--------------------------------------\n");
        }
    }

    // setters
    void set_price(int _price)
    {
        price = _price;
    }
    void set_quantity(int _quantity)
    {
        quantity = _quantity;
    }
    void set_item_name(char _item_name[])
    {
        strcpy(item_name, _item_name);
    }

    // getters
    int get_price()
    {
        return price;
    }
    int get_quantity()
    {
        return quantity;
    }
    char* get_item_name()
    {
        return item_name;
    }
};

// 2. FILTER

class AuthSchema_Filter_Verifier: public Filter<AuthSchema>
{
private:
    char username[32];
    char password[32];
public:
    AuthSchema_Filter_Verifier(char _username[], char _password[])
    {
        strcpy(username, _username);
        strcpy(password, _password);
    }
    bool filter(AuthSchema *_rec)
    {
        if(strcmp(username, _rec->get_username()) == 0 && strcmp(password, _rec->get_password()) == 0)
            return true;
        return false;
    } 
};

class AuthSchema_Filter_FindUser: public Filter<AuthSchema>
{
private:
    char username[32];
public:
    AuthSchema_Filter_FindUser(char _username[])
    {
        strcpy(username, _username);
    }
    bool filter(AuthSchema *_rec)
    {
        return (strcmp(username, _rec->get_username()) == 0);
    }
};

template <class T>
class Filter_Allower: public Filter<T>
{
public:
    bool filter(T* _rec) override
    {
        return true;
    }
};

class ProductSchema_Filter_LessThan: public Filter<ProductSchema>
{
private:
    int threshold;
public:
    ProductSchema_Filter_LessThan(int _threshold)
    {
        threshold = _threshold;
    }
    bool filter(ProductSchema *_rec) override
    {
        return _rec->get_price() <= threshold;
    }
};

class ProductSchema_Filter_QuantityCheck: public Filter<ProductSchema>
{
private:
    int item_id;
    int quantity;
public:
    ProductSchema_Filter_QuantityCheck(int _item_id, int _quantity)
    {
        item_id = _item_id;
        quantity = _quantity;
    }

    bool filter(ProductSchema *_rec) override
    {
        if(_rec->get_id() == item_id && _rec->get_quantity() >= quantity)
            return true;
        return false;
    }
};

// 3. Appliers

class ProductSchema_Applier_Sender: public Applier<ProductSchema>
{
private:
    int fd;
public:
    ProductSchema_Applier_Sender(int _fd)
    {
        fd = _fd;
    }
    void apply(ProductSchema *_rec) override
    {
        int ok;
        read(fd, &ok, sizeof(int));
        write(fd, _rec, sizeof(ProductSchema));
    }
};


// 4. Modifier
class ProductSchema_Modifier_CartDeduction: public Modifier<ProductSchema>
{
private:
    int item_id;
    int quantity;
public:
    ProductSchema_Modifier_CartDeduction(int _item_id, int _quantity)
    {
        item_id = _item_id;
        quantity = _quantity;
    }

    void modify(ProductSchema* _in, ProductSchema* _out) override
    {
        *_out = *_in;
        _out->set_quantity(_in->get_quantity()-quantity);
    }
};

#endif // SOCKET_STORE_DP_HPP