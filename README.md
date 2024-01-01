# SocketStore

SocketStore is a C++ project that simulates an e-commerce website using socket programming and multi-threading. This project aims to provide a basic understanding of implementing a simple e-commerce system with a client-server architecture. It is a command-line interface-based program.

The project is designed with a client-server model, allowing multiple clients to interact with the server concurrently. We utilize multi-threading to handle multiple client connections simultaneously, ensuring efficient communication.

**_NOTE:_** I utilize **`miniDB`**, a simple database service that I created ([click here](https://github.com/SilverSurge/miniDB)) for managing the data for products and users.  It also maintains concurrency control (implemented in miniDB).

# Usage
Anyone with a basic understanding of sockets, and multi-threading can understand the code. I have also provided a template for socket programming to give you a head start for your socket programming endeavors. To run the program write the following commands in the command line.

Initialize the database and add some dummy records using the following commands. Run the executable only once.
```
$ g++ initialize.cpp -o initialize
$ ./initialize
```
Run the SocketStore program using the following commands.

```
$ g++ client.cpp -o client
$ g++ server.cpp -pthread -o server
$ ./server
$ ./client
```

# Core Functionalities
the core functionalities of SocketStore are a follows.
```
0. Register
1. Login
2. Logout
3. Exit
4. View All Products
5. View All Products Cheaper than
6. Add to Cart
7. View Cart
8. Checkout [Everything is Free]
9. Print Menu
```

