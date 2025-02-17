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
- KICK [#channel] [nickname] :[reason]
  - eger komutu yazan kisi operator ise kisiyi kanaldan atar
  - kanalin varligini kullanicinin bulunup bulunmadigini kontrol eder
- MODE [#channel] [+/-][o/k/i/t/l] [parameter]
  - Channel operatorleri tarafindan kullanilabilir
  - +o [nickname] ile kullaniciya operator yetkisi verilir
  - -o [nickname] ile kullanicinin operator yetkisi alinir
  - 
  - +k [key] ile channel a sifre konulur
  - -k ile channel in sifresi kaldirilir
  - 
  - +i ile channel invite-only olur
  - -i ile channel invite-only olmaktan cikar
  - 
  - +t ile sadece operatorler topic degistirebilir
  - -t ile tum kullanicilar topic degistirebilir
  - 
  - +l [limit] ile channel a kullanici limiti konulur
  - -l ile channel in kullanici limiti kaldirilir
  - 
  - kullanicinin channel da olmasi ve operator olmasi gerekir
  - tum channel uyelerine mode degisikligi mesaji gider
- INVITE [nickname] [#channel]
  - Channel operatorleri tarafindan kullanilabilir
  - Invite edilen kullanicinin server da olmasi gerekir
  - Invite eden kisinin channel da olmasi ve operator olmasi gerekir
  - Invite edilen kullanici channel da degilse invite edilir
  - Invite edilen kullanici channel a katilabilir
  - Invite edilen kullanici channel a katilmak icin JOIN komutunu kullanmalidir


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

<hr>

## IRC Server Implementation Details

### 1. Server Başlatma Süreci
1. Server constructor'ı çağrılır (port ve password parametreleri ile)
2. `startServer()` fonksiyonu çalıştırılır
3. Socket oluşturulur ve yapılandırılır
4. Server ana döngüsü başlatılır (`loopProgram()`)

### 2. Socket Oluşturma ve Yapılandırma
1. **Socket Oluşturma**
   - `socket(AF_INET, SOCK_STREAM, 0)` çağrısı yapılır
   - AF_INET: IPv4 protokolünü kullanacağımızı belirtir
   - SOCK_STREAM: TCP bağlantı tipini belirtir
   - 0: Protocol number. When set to `0`, the default protocol (TCP for `SOCK_STREAM`) is automatically chosen.

2. **Socket Yapılandırma**
   - `setsockopt()` ile SO_REUSEADDR ayarlanır
   - Bu ayar, sunucu yeniden başlatıldığında aynı portu hemen kullanabilmeyi sağlar
   - `bind()` ile socket belirli bir port ve IP adresine bağlanır
   - `listen()` ile bağlantı dinlemeye başlanır

### 3. fd_set ve Select Mekanizması
1. **fd_set Kullanım Amacı**
   - Birden fazla socket'i aynı anda dinleyebilmek için kullanılır
   - Bloklamayan I/O işlemleri yapabilmeyi sağlar
   - Hem server socket'ini hem de client socket'lerini aynı anda yönetebilmeyi sağlar

2. **Select Mekanizması**
   - `select()` fonksiyonu ile hangi socket'lerde aktivite olduğu kontrol edilir
   - Yeni bağlantı istekleri ve mevcut bağlantılardan gelen veriler tespit edilir
   - Timeout mekanizması ile sürekli CPU kullanımı engellenir

### 4. Client Bağlantı Süreci
1. **Bağlantı Kabul Etme**
   - Client bağlantı isteği gönderir
   - Server `accept()` ile bağlantıyı kabul eder
   - Yeni bir socket oluşturulur (client ile iletişim için)
   - Client bilgileri (IP, port) alınır

2. **Client Doğrulama**
   - Client PASS komutu ile şifre gönderir
   - USER komutu ile kullanıcı bilgilerini gönderir
   - NICK komutu ile nickname belirlenir
   - Tüm bu bilgiler doğrulandığında client "registered" durumuna geçer

### 5. Client-Server İletişimi
1. **Veri Alışverişi**
   - `recv()` fonksiyonu ile client'tan veri alınır
   - Alınan veri buffer'a yazılır
   - Buffer'daki veri parse edilir ve komutlara ayrılır
   - Komutlar işlenir ve gerekli cevaplar `send()` ile gönderilir

2. **Komut İşleme**
   - Her komut için özel handler fonksiyonları bulunur (PRIVMSG, JOIN, etc.)
   - Komutlar sırayla işlenir
   - Gerekli kontroller yapılır (yetki, parametre sayısı, etc.)
   - Sonuç client'a bildirilir

### 6. Netcat (nc) Kullanımı
1. **Netcat Nedir?**
   - TCP/IP bağlantıları için kullanılan bir ağ aracıdır
   - "İsviçre çakısı" olarak bilinir
   - Basit TCP/IP bağlantıları kurmak ve test etmek için kullanılır

2. **IRC ile Kullanımı**
   - `nc hostname port` komutu ile sunucuya bağlanılır
   - Raw TCP bağlantısı sağlar
   - IRC komutları doğrudan gönderilebilir
   - Test ve debug için idealdir

### 7. Veri Akışı
1. **Client -> Server**
   - Client mesaj gönderir
   - Veri TCP soketi üzerinden iletilir
   - Server veriyi buffer'a alır
   - Buffer parse edilir
   - Komut işlenir

2. **Server -> Client**
   - Server cevap hazırlar
   - Cevap formatlanır (IRC protokolüne uygun şekilde)
   - TCP soketi üzerinden client'a gönderilir
   - Client cevabı alır ve işler

### 8. Kanal Yönetimi
1. **Kanal Oluşturma**
   - İlk JOIN komutu ile kanal oluşturulur
   - İlk kullanıcı operator olur
   - Kanal ayarları belirlenir (mode)

2. **Kanal İşlemleri**
   - Kullanıcı ekleme/çıkarma
   - Mode değişiklikleri
   - Topic yönetimi
   - Mesaj iletimi
   - Operator yönetimi

### 9. Güvenlik Mekanizmaları
1. **Bağlantı Güvenliği**
   - Password kontrolü
   - Kullanıcı doğrulama
   - Operator yetkilendirme

2. **Kanal Güvenliği**
   - Mode kontrolleri (+i, +k, etc.)
   - Operator yetki kontrolleri
   - Invite-only kontrolleri
   - Ban mekanizması

Bu dokümantasyon, IRC sunucusunun temel çalışma prensiplerini ve teknik detaylarını açıklamaktadır. Her bir bileşen ve mekanizma, sunucunun güvenli ve verimli çalışması için önemli rol oynamaktadır.

<hr>
<hr>
<hr>
<hr>

## Detailed Technical Implementation of IRC Server

### 1. Server Initialization Process
1. **Constructor Call**
   - Port ve password parametreleri alınır
   - `startServer()` fonksiyonu çağrılır
   - İlk socket (master socket) oluşturulur
   - `max_fd` değişkeni master socket ile başlatılır

2. **Socket Creation & Configuration**
   - `socket(AF_INET, SOCK_STREAM, 0)` ile master socket oluşturulur
     - AF_INET: IPv4 protokolü
     - SOCK_STREAM: TCP bağlantı tipi (güvenilir, sıralı, çift yönlü)
     - 0: Sistem varsayılan protokolü
   - Socket file descriptor (fd) döndürülür
   - Negatif değer dönerse hata oluşmuştur

3. **Socket Options Configuration**
   - `setsockopt()` ile SO_REUSEADDR ayarlanır
     ```cpp
     int opt = 1;
     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
     ```
   - Bu ayar, sunucu kapanıp tekrar açıldığında "Address already in use" hatasını engeller
   - TIME_WAIT durumundaki portların tekrar kullanılabilmesini sağlar

4. **Server Address Structure**
   ```cpp
   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(port);
   server_addr.sin_addr.s_addr = INADDR_ANY;
   ```
   - sin_family: IPv4 protokolü
   - sin_port: Port numarası (network byte order'a çevrilir)
   - sin_addr: Tüm network interface'lerden bağlantı kabul edilir

5. **Binding & Listening**
   - `bind()`: Socket'i belirtilen adres ve porta bağlar
   - `listen()`: Socket'i pasif dinleme moduna alır
     - Bağlantı kuyruğu boyutu belirlenir (backlog)
     - Gelen bağlantılar bu kuyrukta bekletilir

### 2. File Descriptor Sets (fd_set)
1. **fd_set Yapısı**
   - Bit array olarak implemente edilmiştir
   - Her bit bir file descriptor'ı temsil eder
   - Maximum FD_SETSIZE kadar (genelde 1024) fd tutabilir

2. **fd_set Operations**
   ```cpp
   FD_ZERO(&read_fds);     // Tüm bitleri sıfırlar
   FD_SET(sockfd, &read_fds);   // Belirtilen fd'yi set eder
   FD_CLR(fd, &read_fds);   // fd'yi set'ten çıkarır
   FD_ISSET(fd, &read_fds); // fd'nin set'te olup olmadığını kontrol eder
   ```

### 3. Main Event Loop (Event-Driven Architecture)
1. **Loop Initialization**
   ```cpp
   while (1) {
       fd_set active_fds = read_fds;  // Her döngüde read_fds kopyalanır
   }
   ```
   - read_fds: Master fd set (tüm aktif bağlantıları tutar)
   - active_fds: select() için kullanılan geçici kopya

2. **Select System Call**
   ```cpp
   select(max_fd + 1, &active_fds, NULL, NULL, NULL);
   ```
   - Parametreler:
     1. max_fd + 1: Kontrol edilecek maximum fd değeri
     2. &active_fds: Okuma için kontrol edilecek fd'ler
     3. NULL: Yazma için kontrol edilecek fd'ler (kullanılmıyor)
     4. NULL: Exception için kontrol edilecek fd'ler (kullanılmıyor)
     5. NULL: Timeout değeri (NULL = sonsuz bekleme)
   - Bloklu çalışır: Herhangi bir fd'de aktivite olana kadar bekler
   - Döndüğünde active_fds'de sadece aktif fd'ler kalır

### 4. Client Connection Process
1. **New Connection Detection**
   ```cpp
   if (FD_ISSET(sockfd, &active_fds)) {
       // Yeni bağlantı isteği var
   }
   ```
   - Master socket'te aktivite = Yeni bağlantı isteği

2. **Connection Acceptance**
   ```cpp
   int client_sockfd = accept(sockfd, NULL, NULL);
   ```
   - Yeni bir socket oluşturulur (client_sockfd)
   - Bu socket sadece bu client ile iletişim için kullanılır
   - Master socket dinlemeye devam eder

3. **Client Socket Management**
   - Yeni socket fd_set'e eklenir:
     ```cpp
     FD_SET(client_sockfd, &read_fds);
     ```
   - max_fd güncellenir (gerekirse):
     ```cpp
     if (client_sockfd > max_fd)
         max_fd = client_sockfd;
     ```
   - Client listesine eklenir:
     ```cpp
     Client new_client(client_sockfd);
     clients.push_back(new_client);
     ```

### 5. Data Reception & Processing
1. **Data Check**
   ```cpp
   if (FD_ISSET(client_fd, &active_fds)) {
       // Client'tan veri gelmiş
   }
   ```

2. **Data Reception**
   ```cpp
   std::vector<char> buffer(1024);
   int bytes_received = recv(client_fd, buffer.data(), buffer.size(), 0);
   ```
   - buffer: Gelen veriyi tutacak alan
   - bytes_received:
     - > 0: Alınan byte sayısı
     - = 0: Bağlantı kapanmış
     - < 0: Hata oluşmuş

3. **Connection Status Check**
   ```cpp
   if (bytes_received <= 0) {
       close(client_fd);
       FD_CLR(client_fd, &read_fds);
       // Client listesinden çıkar
   }
   ```

### 6. Buffer Management
1. **Buffer Structure**
   - Fixed-size vector kullanılır (1024 bytes)
   - Her recv() çağrısında sıfırlanır
   - Null-terminated string olarak işlenir

2. **Data Parsing**
   - Buffer string'e çevrilir
   - Kontrol karakterleri temizlenir (\r\n)
   - Komut ve parametreler ayrıştırılır

### 7. Error Handling
1. **System Call Errors**
   - Her system call sonrası hata kontrolü
   - errno değeri kontrol edilir
   - Uygun hata mesajı loglanır

2. **Resource Management**
   - Kapatılan bağlantıların temizlenmesi
   - Buffer'ların temizlenmesi
   - fd'lerin kapatılması

Bu teknik dokümantasyon, IRC sunucusunun low-level çalışma mekanizmalarını detaylı olarak açıklamaktadır. Network programlama ve sistem programlama konseptlerinin pratik bir uygulaması olarak sunulmuştur.


