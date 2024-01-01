
#include "Relation.hpp"

template <class T>
Relation<T>::Relation(int _key, char _relation_name[])
{
    key = _key;
    rec_size = sizeof(T);
    strcpy(relation_name, _relation_name);
    fd = -1;

    // creating the directory
    mkdir("./relations", 0755);

    // setup the file
    char filename_1[64], filename_2[64];

    strcpy(filename_1, "./relations/");
    strcat(filename_1, relation_name);
    strcat(filename_1, "_db.mdb");

    strcpy(filename_2, "./relations/");
    strcat(filename_2, relation_name);
    strcat(filename_2, "_backup.mdb");

    int flags_1 = O_CREAT | O_RDWR;
    int flags_2 = O_CREAT | O_RDONLY;
    fd = open(filename_1, flags_1, 0644);
    int fd_backup = open(filename_2, flags_2, 0644);

    // verifying if the file discriptors have been created
    if(fd == -1 || fd_backup == -1)
    {
        fd = -1;
        printf("Relation -- Constructor Failure\n");
        return;
    }

    close(fd_backup);
}

template <class T>
int Relation<T>::connect()
{
    // semaphore controller setup
    semctrl = SemaphoreController(key);
    semctrl.reset();

    // setting up filenames
    char filename_1[64], filename_2[64];

    strcpy(filename_1, "./relations/");
    strcat(filename_1, relation_name);
    strcat(filename_1, "_db.mdb");

    strcpy(filename_2, "./relations/");
    strcat(filename_2, relation_name);
    strcat(filename_2, "_backup.mdb");

    char command[256];
    snprintf(command, sizeof(command), "cp %s %s", filename_2, filename_1);
    
    // executing command $ cp backup.mdb db.mdb
    int res = system(command);
    if(res != 0)
    {
        printf("Relation -- Backup\n");
        semctrl.unlock();
        return FAILURE;
    }
    
    semctrl.unlock();
    return SUCCESS;
}

template <class T>
int Relation<T>::disconnect()
{
    // checking if they are already closed
    if(fd == -1)
    {
        printf("Relation -- Disconnect Failure\n");
        return FAILURE;
    }

    // closing the file descriptors
    close(fd);
    fd = -1;
    // closing the semaphore
    semctrl.erase();
    return SUCCESS;
}

template <class T>
int Relation<T>::create_record(T* _rec)
{
    // concurrency control
    semctrl.lock();

    // creating the record

    // no file discriptor available
    if(fd == -1)
    {
        printf("Relation -- Create Record -- 1\n");
        semctrl.unlock();
        return FAILURE;
    }

    // catch lseek errors
    if(lseek(fd, 0, SEEK_END) == -1)
    {
        printf("Relation -- Create Record -- 2\n");
        semctrl.unlock();
        return FAILURE;
    }

    int wres = write(fd, _rec, rec_size);
    // catch write syscall errors
    if(wres != rec_size)
    {
        printf("Relation -- Create Record -- 3\n");
        semctrl.unlock();
        return FAILURE;
    }

    // concurrency control
    semctrl.unlock();
    return SUCCESS;
}

template <class T>
int Relation<T>::retrieve_record(Filter<T>* _filter, Collection<T> *_collection)
{
    // concurrency control
    semctrl.lock();

    // no file descriptor available
    if(fd == -1)
    {
        printf("Relation -- Retrieve Record -- 1\n");
        semctrl.unlock();
        return FAILURE;
    }

    // assert RecordLL* &_rec to be NULL
    _collection -> free();


    // catch lseek error
    if(lseek(fd, 0, SEEK_SET) == -1)
    {
        printf("Relation -- Retrieve Record -- 3\n");
        semctrl.unlock();
        return FAILURE;
    }

    // creating a buffer
    T buffer;
    while(1)
    {
        int rval = read(fd, &buffer, rec_size);
        if(rval == 0)
        {
            // reached the end of the file
            break;
        }
        else if(rval < 0)
        {
            printf("Relation -- Retrieve Record -- 4\n");
            semctrl.unlock();
            return FAILURE;
        }

        
        // filter out
        if(buffer.get_validity() && _filter->filter(&buffer))
        {
            // copying the record
            RecordLL<T> *new_rec_ptr = new RecordLL<T>();
            new_rec_ptr->record = new T(buffer);

            // adding the new record
            _collection->add(new_rec_ptr);
        }
    }
    
    // concurrency control
    semctrl.unlock();
    return SUCCESS;
}

template <class T>
int Relation<T>::update_record(Filter<T>* _filter, Modifier<T>* _modifier)
{
    // concurrency control
    semctrl.lock();

    // no file descriptor available
    if(fd == -1)
    {
        printf("Relation -- Update Record -- 1\n");
        semctrl.unlock();
        return FAILURE;
    }

    // catch lseek error
    if(lseek(fd, 0, SEEK_SET) == -1)
    {
        printf("Relation -- Update Record -- 3\n");
        semctrl.unlock();
        return FAILURE;
    }

    // creating a buffer
    T buffer;
    while(1)
    {
        int rval = read(fd, &buffer, rec_size);
        if(rval == 0)
        {
            // reached the end of the file
            break;
        }
        else if(rval < 0)
        {
            printf("Relation -- Update Record -- 4\n");
            semctrl.unlock();
            return FAILURE;
        }
        
        // filter out
        if(buffer.get_validity() && _filter->filter(&buffer))
        {
            // update the record using pass by reference
            T out_rec(buffer);
            _modifier->modify(&buffer, &out_rec);

            // go back by one record
            if(lseek(fd, -rec_size, SEEK_CUR) == -1)
            {
                printf("Relation -- Update Record -- 5\n");
                semctrl.unlock();
                return FAILURE;
            }

            // write the modified record
            int wres = write(fd, &out_rec, rec_size);
            // // free out_rec
            // delete out_rec;
            // catch write syscall errors
            if(wres != rec_size)
            {
                printf("Relation -- Update Record -- 6\n");
                semctrl.unlock();
                return FAILURE;
            }

        }
    }

    // concurrency control
    semctrl.unlock();
    return SUCCESS;
}

template <class T>
int Relation<T>::delete_record(Filter<T>* _filter)
{
    // concurrency control
    semctrl.lock();

    // no file descriptor available
    if(fd == -1)
    {
        printf("Relation -- Delete Record -- 1\n");
        semctrl.unlock();
        return FAILURE;
    }

    // catch lseek error
    if(lseek(fd, 0, SEEK_SET) == -1)
    {
        printf("Relation -- Delete Record -- 3\n");
        semctrl.unlock();
        return FAILURE;
    }

    // creating a buffer
    T buffer;
    while(1)
    {
        int rval = read(fd, &buffer, rec_size);
        if(rval == 0)
        {
            // reached the end of the file
            break;
        }
        else if(rval < 0)
        {
            printf("Relation -- Delete Record -- 4\n");
            semctrl.unlock();
            return FAILURE;
        }
        
        // filter out
        if(buffer.get_validity() && _filter->filter(&buffer))
        {
            // turn the validity to false
            buffer.set_validity(false);

            // go back by one record
            if(lseek(fd, -rec_size, SEEK_CUR) == -1)
            {
                printf("Relation -- Delete Record -- 5\n");
                semctrl.unlock();
                return FAILURE;
            }

            // write the modified record
            int wres = write(fd, &buffer, rec_size);
            // catch write syscall errors
            if(wres != rec_size)
            {
                printf("Relation -- Delete Record -- 6\n");
                semctrl.unlock();
                return FAILURE;
            }

        }
    }

    // concurrency control
    semctrl.unlock();
    return SUCCESS;
}

template <class T>
int Relation<T>::thrash()
{
    // concurrency control
    semctrl.lock();

    // no file descriptor available
    if(fd == -1)
    {
        printf("Relation -- Thrash -- 1\n");
        semctrl.unlock();
        return FAILURE;
    }


    // catch lseek error
    if(lseek(fd, 0, SEEK_SET) == -1)
    {
        printf("Relation -- Thrash -- 2\n");
        semctrl.unlock();
        return FAILURE;
    }

    // setup thrash file
    char thrash_filename[64];

    strcpy(thrash_filename, "./relations/");
    strcat(thrash_filename, relation_name);
    strcat(thrash_filename, "_thrash.mdb");

    

    int flags_1 = O_CREAT | O_RDWR;
    int fd_thrash = open(thrash_filename, flags_1, 0644);
    if(fd_thrash == -1)
    {
        printf("Relation -- Thrash -- 3\n");
        semctrl.unlock();
        return FAILURE;
    }

    // creating a buffer
    char buffer[rec_size];
    while(1)
    {
        int rval = read(fd, buffer, rec_size);
        if(rval == 0)
        {
            // reached the end of the file
            break;
        }
        else if(rval < 0)
        {
            printf("Relation -- Thrash -- 4\n");
            semctrl.unlock();
            return FAILURE;
        }

        
        // filter out
        if(((Schema*)buffer)->get_validity())
        {
            // write to thrash file
            int wres = write(fd_thrash, buffer, rec_size);
            if(wres != rec_size)
            {
                printf("Relation -- Thrash -- 5\n");
                semctrl.unlock();
                return FAILURE;
            }
        }
    }

    // create the fd file again
    close(fd);
    close(fd_thrash);

    char filename_1[64];

    strcpy(filename_1, "./relations/");
    strcat(filename_1, relation_name);
    strcat(filename_1, "_db.mdb");

    if(remove(filename_1) != 0)
    {
        printf("%s\n", filename_1);
        printf("Relation -- Thrash -- 6\n");
        semctrl.unlock();
        return FAILURE;
    } 

    if(rename(thrash_filename, filename_1) != 0)
    {
        printf("Relation -- Thrash -- 7\n");
        semctrl.unlock();
        return FAILURE;
    }

    fd = open(filename_1, O_RDWR, 0644);
    if(fd == -1)
    {
        printf("Relation -- Thrash -- 8\n");
        semctrl.unlock();
        return FAILURE;
    }

    // concurrency control
    semctrl.unlock();
    return SUCCESS;
}

template <class T>
int Relation<T>::commit()
{
    // concurrency control
    semctrl.lock();

    // setting up filenames
    char filename_1[64], filename_2[64];

    strcpy(filename_1, "./relations/");
    strcat(filename_1, relation_name);
    strcat(filename_1, "_db.mdb");

    strcpy(filename_2, "./relations/");
    strcat(filename_2, relation_name);
    strcat(filename_2, "_backup.mdb");

    char command[256];
    snprintf(command, sizeof(command), "cp %s %s", filename_1, filename_2);
    
    int res = system(command);
    if(res != 0)
    {
        printf("Relation -- Backup\n");
        semctrl.unlock();
        return FAILURE;
    }

    semctrl.unlock();
    return SUCCESS;
}