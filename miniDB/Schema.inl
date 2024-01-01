#include "Schema.hpp"

Schema::Schema(int _id, bool _is_valid)
{
    id = _id;
    is_valid = _is_valid;
}

void Schema::set_validity(bool _is_valid)
{
    is_valid = _is_valid;
}

bool Schema::get_validity()
{
    return is_valid;
}

void Schema::display()
{
    printf("Schema Defaut Display\n");
}

int Schema::get_id()
{
    return id;
}