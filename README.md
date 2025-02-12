## To-Do List

#### her kullanici ilk baglandiginda USER ve PASS komutu kullanarak login olmali

- USER [nickname] [mode] [unused] :[username]
  - nickname uniq olmaki buyuzden daha once alinmadigini kontrol eder
  - mode ve unused degerlerini kullnmadik
  - username iki noktadan sonra ise birden fazla kelime olabilir
- PASS [password]
  - server baslatilirken belirlenen sifre ile giris yapar
  - kullanci daha once giris yapmis mi kontrol eder

#### Eger kullanici login olabilmisse bu komutlari kullanbilir

- JOIN [#channel]
  - eger channel yoksa olusturuyor
  - channel varsa ve client daha once eklenmediyse ekliyor
- PRIVMSG [#channel] [mesaj]
  - eger kullanici channelda ekli degilse mesaj gondermez
- PRIVMSG [nickname] [mesaj]
  - nickname ile eslesen kullanici yoksa hata mesaji doner
- PRIVMSG [#channel] :[birden fazla bosluklu mesaj]
  - iki nokta eklenirse bosluklu mesajlar gonderilir
- NICK [nickname]
  - clientin nicknameini degistirebilmesi saglar
  - login olmadan kullanilabir
  - nicknamein daha once alinip alinmadigini kontrol eder
- TOPIC [#channel] [topic]
  - Channel in topic mesajini set eder
  - userin channel a katilmis olmasi gerekli
  - userin operator olmasi gerekli
  - topic degistiginde tum channel uyelerine mesaj gider


- HELP veya help
  - client a serverda rehber olacak komutlarin nasil kullanildigini vs. aciklayacak
  - kvirc kucuk harfle gonderiyor servera 

---
<br>

# IRC  - Server Setup and Connection
![img](https://github.com/user-attachments/assets/e9b69897-c5b7-44c7-a986-1480c4925ffd)

This project develops an IRC application using C++. The following sections provide detailed information on setting up the server, making connections, and using sockets.

## Server Address Configuration

To run the server, the necessary configuration is made using the `sockaddr_in` structure, which represents IPv4 addresses and contains the address information required for creating a socket or listening socket.

### sockaddr_in Structure

```cpp
struct sockaddr_in {
    short            sin_family;   // Address family (AF_INET/AF_INET6)
    unsigned short   sin_port;     // Port number (converted using htons())
    struct in_addr   sin_addr;     // IP address (e.g., INADDR_ANY)
    char             sin_zero[8];  // Padding for alignment
};
```

### Structure Members:

- **sin_family**: Specifies the address family (use `AF_INET` for IPv4).
- **sin_port**: Sets the port number to listen on. The port number is converted to network byte order using `htons()`.
- **sin_addr**: Represents the IP address. If set to `INADDR_ANY`, it listens on all available IP addresses.
- **sin_zero**: Padding added to align the structure to the appropriate size.

### Creating a Socket

The `socket()` function is used to create a socket in your server application:

```cpp
int sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) {
    std::cerr << "Socket creation failed" << std::endl;
    return -1;
}
```

#### `socket()` Function:

- **AF_INET**: Specifies the IPv4 address family.
- **SOCK_STREAM**: Indicates the TCP protocol, providing reliable, connection-oriented data transmission.
- **0**: Protocol number. When set to `0`, the default protocol (TCP for `SOCK_STREAM`) is automatically chosen.

If the socket creation fails, the `socket()` function will return `-1`.

### Configuring the Server Address

To configure the server's IP address and port, use the following code:

```cpp
struct sockaddr_in serv_addr;
serv_addr.sin_family = AF_INET;                          // IPv4 protocol
serv_addr.sin_port = htons(8080);                         // Convert the port number to network byte order
serv_addr.sin_addr.s_addr = inet_addr("192.168.1.1");     // Specified IP address
```

### Why Use `htons()`?

The `htons()` function is used to convert the port number from the host byte order (little-endian) to the network byte order (big-endian). This conversion is necessary because networks use big-endian format for data transmission.

### Listening on All IP Addresses

If you want the server to accept connections from any available IP address, you can set `sin_addr.s_addr` to `INADDR_ANY`:

```cpp
serv_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all connected IP addresses
```

This allows the server to accept connections from any IP address.

---

## Key Functions and Concepts

### `setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))`
- **SO_REUSEADDR**: This option allows the socket to reuse a port that is already in use, which is useful when the server is restarted or if there is an error that causes the socket to close unexpectedly.
- **sockfd**: The socket descriptor obtained after calling `socket()`.
- **SOL_SOCKET**: Specifies that the option is set at the socket level.
- **SO_REUSEADDR**: The option to allow port reuse.
- **&opt**: A pointer to the option value (1 in this case, meaning the option is enabled).
- **sizeof(opt)**: Specifies the size of the `opt` variable.

### `bind(server_fd, (struct sockaddr *)&address, sizeof(address))`
- The `bind()` function binds a socket to a specific IP address and port.
- `server_fd`: The socket file descriptor.
- `address`: A structure holding the server's IP and port.
- `addrlen`: The length of the address structure.
- Example address setup:
  ```cpp
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  ```

### `listen(server_fd, 3)`
- The `listen()` function prepares the server socket to accept incoming connections. 
- The second argument (`3`) specifies the maximum number of connections that can be queued before the server starts accepting them.

### `accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)`
- The `accept()` function accepts an incoming connection from a client.
- Returns a new socket file descriptor (`new_socket`) that is used for communication with the client.
- `address`: The client's IP and port information.
- `addrlen`: The length of the address structure, which is updated upon return.

### Client-Server Communication Loop
```cpp
while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
    if (bytes_read <= 0) break;
    std::cout << "Message from client: " << buffer;
    send(new_socket, buffer, strlen(buffer), 0);
}
```
- **read(new_socket, buffer, BUFFER_SIZE)**: Reads data sent by the client into the `buffer`. If no data is received or the client disconnects, the loop ends.
- **send(new_socket, buffer, strlen(buffer), 0)**: Echoes the received message back to the client.

### `fd_set` and `select()`
- **fd_set**: A data structure used to store a collection of file descriptors (sockets) that can be monitored for readiness (e.g., readability, writability, or errors).
- **FD_ZERO()**: Clears all file descriptors in the `fd_set`.
- **FD_SET()**: Adds a file descriptor to the `fd_set`.
- **FD_CLR()**: Removes a file descriptor from the `fd_set`.
- **FD_ISSET()**: Checks if a file descriptor is ready for reading, writing, or has an error.

#### Example of using `select()`:
```cpp
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(sockfd, &readfds);

struct timeval timeout;
timeout.tv_sec = 5;
timeout.tv_usec = 0;

int activity = select(MAX_FDS, &readfds, NULL, NULL, &timeout);
if (activity == -1) {
    std::cerr << "select() error" << std::endl;
} else if (activity == 0) {
    std::cout << "Timeout occurred, no data to read" << std::endl;
} else {
    if (FD_ISSET(sockfd, &readfds)) {
        std::cout << "Data is available to read on sockfd" << std::endl;
    }
}
```

### Accepting New Connections
```cpp
if (FD_ISSET(sockfd, &tmpfds)) {
    new_socket = accept(sockfd, NULL, NULL);
    if (new_socket < 0)
        perr("accept error failed", sockfd);
    else {
        std::cout << "Connection accepted" << std::endl;
        FD_SET(new_socket, &readfds);
        if (new_socket > max_sd)
            max_sd = new_socket;
        connected_clients.push_back(new_socket);
    }
}
```
- **FD_SET(new_socket, &readfds)**: Adds the new socket to the set for monitoring.
- **max_sd**: Keeps track of the highest file descriptor, which is used by `select()` to monitor multiple file descriptors.
- **connected_clients.push_back(new_socket)**: Adds the new client socket to the list of connected clients.

### Receiving Data with `recv()`
```cpp
int valread = recv(connected_clients[i], buffer, sizeof(buffer), 0);
```
- **recv()**: Receives data from a socket.
- Returns the number of bytes read, `0` if the client closes the connection, or a negative value if there is an error.
  - **sockfd**: The socket file descriptor.
  - **buf**: The buffer to store the received data.
  - **len**: The maximum number of bytes to read.
  - **flags**: Usually set to `0` for normal data reception.

