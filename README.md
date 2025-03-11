# IRC Server Projesi

Bu proje, Internet Relay Chat (IRC) protokolünü temel alan bir sunucu uygulamasıdır. Modern C++ kullanılarak geliştirilmiş olup, POSIX soket programlama API'lerini kullanmaktadır.

## İçindekiler

1. [Proje Yapısı](#proje-yapısı)
2. [Kurulum](#kurulum)
3. [Kullanım](#kullanım)
4. [Teknik Detaylar](#teknik-detaylar)
5. [Soket Programlama](#soket-programlama)
6. [IRC Komutları](#irc-komutları)

## Proje Yapısı

Proje, modüler bir yapıda tasarlanmış olup aşağıdaki ana bileşenlerden oluşmaktadır:

```
.
├── Server.cpp            # Ana sunucu sınıfı implementasyonu
├── ServerConnection.cpp  # Bağlantı yönetimi işlemleri
├── ServerCommand.cpp     # Komut işleme mekanizmaları
├── Client.cpp           # İstemci yönetimi
├── Channel.cpp          # Kanal yönetimi
└── Commands/            # IRC komutları implementasyonları
```

## Kurulum

Projeyi derlemek için aşağıdaki komutları kullanın:

```bash
make        # Projeyi derler
make clean  # Obje dosyalarını temizler
make fclean # Tüm derlenmiş dosyaları temizler
make re     # Temiz bir şekilde yeniden derler
```

## Kullanım

Sunucuyu başlatmak için:

```bash
./ircserv <port> <password>
```

- `port`: Sunucunun dinleyeceği port numarası (1024-65535 arası)
- `password`: Sunucu şifresi

## Teknik Detaylar

### Soket Programlama

IRC sunucumuz, POSIX soket programlama API'lerini kullanarak çok istemcili bir ağ uygulaması olarak çalışır. Programın çalışma mantığı şu şekildedir:

1. **Başlangıç Aşaması**
   - Sunucu başlatıldığında, öncelikle bir TCP soketi oluşturulur.
   - Bu soket, belirtilen port üzerinden gelen bağlantıları dinlemek üzere yapılandırılır.
   - Soket, tekrar kullanılabilir (reusable) olarak ayarlanır ki sunucu yeniden başlatıldığında "address already in use" hatası alınmasın.

2. **Dinleme Aşaması**
   - Soket, belirtilen porta bağlanır (bind edilir).
   - Pasif dinleme moduna geçilir ve bağlantı istekleri kabul edilmeye başlanır.
   - Bu aşamada sunucu, yeni bağlantıları kabul etmeye hazırdır.

3. **Çoklu İstemci Yönetimi**
   - Sunucu, tek bir thread üzerinde birden fazla istemciyi yönetebilmek için `select()` mekanizmasını kullanır.
   - Her bir soket için okuma, yazma ve hata durumları eşzamanlı olarak izlenir.
   - Bu sayede sunucu, bloklama olmadan tüm istemcilere hizmet verebilir.

4. **Bağlantı Yönetimi**
   - Yeni bir istemci bağlandığında:
     * Bağlantı kabul edilir ve yeni bir soket oluşturulur
     * İstemcinin IP adresi alınır ve kaydedilir
     * İstemci bilgileri veri yapılarına eklenir
   - Mevcut istemcilerden veri geldiğinde:
     * Veri okunur ve işlenir
     * IRC protokolüne göre komutlar yorumlanır
     * Gerekli yanıtlar oluşturulur ve gönderilir
   - Bir istemci bağlantıyı kestiğinde:
     * Soket kapatılır
     * İstemci bilgileri temizlenir
     * İlgili kanallardan çıkarılır

5. **Veri İşleme**
   - İstemcilerden gelen veriler tamponlarda (buffer) toplanır
   - Tampondaki veriler tam komutlar halinde işlenir
   - Her komut için uygun işleyici (handler) çağrılır
   - Yanıtlar oluşturulur ve ilgili istemcilere iletilir

6. **Hata Yönetimi**
   - Bağlantı kopmaları tespit edilir ve temizlenir
   - Soket hataları kontrol edilir ve yönetilir
   - Buffer taşmaları engellenir
   - Geçersiz komutlar reddedilir

Bu yapı sayesinde sunucu:
- Çok sayıda istemciye eşzamanlı hizmet verebilir
- Kaynakları verimli kullanır
- Güvenilir ve kararlı çalışır
- Ölçeklenebilir bir yapı sunar

#### Soket Programlama Akışı

1. **Soket Oluşturma ve Yapılandırma**
```cpp
// Soket oluşturma
sockfd = socket(AF_INET, SOCK_STREAM, 0);

// Soket yapılandırması
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;           // IPv4 protokolü
server_addr.sin_port = htons(port);         // Port numarası (network byte order)
server_addr.sin_addr.s_addr = INADDR_ANY;   // Tüm interface'leri dinle

// SO_REUSEADDR ve SO_REUSEPORT seçeneklerini etkinleştirme
int opt = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
```

2. **Soketin Bağlanması ve Dinleme**
```cpp
// Soketi belirtilen adres ve porta bağlama
bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

// Soketi pasif dinleme moduna alma (max 3 bekleyen bağlantı)
listen(sockfd, 3);
```

3. **I/O Multiplexing ile Bağlantı Yönetimi**
```cpp
// fd_set yapısını hazırlama
fd_set read_fds;
FD_ZERO(&read_fds);                // Kümeyi temizle
FD_SET(sockfd, &read_fds);        // Ana soketi dinleme kümesine ekle

while (true) {
    fd_set active_fds = read_fds;  // Orijinal kümeyi koru
    
    // Soketleri izle (blocking)
    select(max_fd + 1, &active_fds, NULL, NULL, NULL);
    
    // Yeni bağlantı kontrolü
    if (FD_ISSET(sockfd, &active_fds)) {
        handleNewConnection();
    }
    
    // Mevcut bağlantıları kontrol et
    for (client_index = 0; client_index < connected_clients.size(); client_index++) {
        if (FD_ISSET(connected_clients[client_index], &active_fds)) {
            handleClientData(client_index, active_fds);
        }
    }
}
```

4. **Yeni Bağlantı Kabulü**
```cpp
void handleNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Yeni bağlantıyı kabul et
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    
    // IP adresini string formatına dönüştür
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    
    // Yeni soketi izleme kümesine ekle
    FD_SET(client_sockfd, &read_fds);
    connected_clients.push_back(client_sockfd);
    
    // Client nesnesini oluştur ve yapılandır
    Client new_client(client_sockfd);
    new_client.setIp_address(ip_str);
    clients.push_back(new_client);
}
```

5. **Veri Alışverişi**
```cpp
void handleClientData(size_t client_index, fd_set& active_fds) {
    std::vector<char> buffer(1024, 0);
    
    // Veri al
    int bytes_received = recv(connected_clients[client_index], &buffer[0], buffer.size(), 0);
    
    if (bytes_received <= 0) {
        disconnectClient(client_index);
        return;
    }
    
    // Alınan veriyi işle
    std::string received_data(&buffer[0], bytes_received);
    clients[client_index].appendToCommandBuffer(received_data);
    processClientBuffer(client_index);
}

// Veri gönderme (Client sınıfı içinde)
void Client::message(const std::string &message) const {
    send(c_sockfd, message.c_str(), message.size(), 0);
}
```

6. **Bağlantı Sonlandırma**
```cpp
void disconnectClient(size_t client_index) {
    int client_fd = connected_clients[client_index];
    
    // Soketi kapat
    close(client_fd);
    
    // İzleme kümesinden çıkar
    FD_CLR(client_fd, &read_fds);
    
    // Client verilerini temizle
    connected_clients.erase(connected_clients.begin() + client_index);
    clients.erase(clients.begin() + client_index);
}
```

Bu akış, POSIX soket API'lerini kullanarak çok istemcili bir IRC sunucusu implementasyonunu gerçekleştirir. `select()` fonksiyonu ile I/O multiplexing sağlanarak, tek bir thread üzerinde birden fazla bağlantı eşzamanlı olarak yönetilir.

#### 1. socket()

```cpp
int socket(int domain, int type, int protocol);
```

- **Açıklama**: Yeni bir soket oluşturur
- **Parametreler**:
  - `domain`: Haberleşme domain'i (AF_INET: IPv4)
  - `type`: Soket tipi (SOCK_STREAM: TCP)
  - `protocol`: Protokol tipi (0: varsayılan)
- **Dönüş Değeri**: Başarılı olursa soket dosya tanımlayıcısı, hata durumunda -1
- **Kullanım Örneği**:
```cpp
sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

#### 2. setsockopt()

```cpp
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

- **Açıklama**: Soket seçeneklerini ayarlar
- **Parametreler**:
  - `sockfd`: Soket dosya tanımlayıcısı
  - `level`: Protokol seviyesi (SOL_SOCKET)
  - `optname`: Seçenek adı (SO_REUSEADDR)
  - `optval`: Seçenek değeri
  - `optlen`: optval'in boyutu
- **Önemli**: SO_REUSEADDR seçeneği, sunucunun yeniden başlatılması durumunda "Address already in use" hatasını önler

#### 3. bind()

```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- **Açıklama**: Soketi belirli bir adres ve porta bağlar
- **Parametreler**:
  - `sockfd`: Soket dosya tanımlayıcısı
  - `addr`: Bağlanılacak adres yapısı
  - `addrlen`: Adres yapısının boyutu
- **Adres Yapısı**:
```cpp
struct sockaddr_in {
    sa_family_t sin_family;     // AF_INET
    in_port_t sin_port;         // Port numarası (network byte order)
    struct in_addr sin_addr;    // IPv4 adresi
};
```

#### 4. listen()

```cpp
int listen(int sockfd, int backlog);
```

- **Açıklama**: Soketi pasif dinleme moduna alır
- **Parametreler**:
  - `sockfd`: Soket dosya tanımlayıcısı
  - `backlog`: Bekleyen bağlantı kuyruğunun maksimum uzunluğu
- **Not**: Backlog değeri sistem tarafından sınırlandırılabilir (Linux'ta genellikle 128)

#### 5. accept()

```cpp
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- **Açıklama**: Gelen bağlantı isteğini kabul eder
- **Parametreler**:
  - `sockfd`: Dinleyen soket
  - `addr`: İstemci adres bilgisi (NULL olabilir)
  - `addrlen`: addr yapısının boyutu
- **Dönüş**: Yeni bağlantı için soket tanımlayıcısı

#### 6. recv()

```cpp
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

- **Açıklama**: Soketten veri okur
- **Parametreler**:
  - `sockfd`: Soket tanımlayıcısı
  - `buf`: Veri tamponu
  - `len`: Tampon boyutu
  - `flags`: Alım seçenekleri (genellikle 0)
- **Dönüş**: Okunan byte sayısı, 0 bağlantı kapandığında, -1 hata durumunda

#### 7. select()

```cpp
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

- **Açıklama**: I/O multiplexing için kullanılır, birden fazla soketi eşzamanlı olarak izler
- **Parametreler**:
  - `nfds`: İzlenecek en yüksek dosya tanımlayıcısı + 1
  - `readfds`: Okuma için izlenecek soketler kümesi
  - `writefds`: Yazma için izlenecek soketler kümesi
  - `exceptfds`: İstisnai durumlar için izlenecek soketler
  - `timeout`: Zaman aşımı değeri


### IP İlişkili Fonksiyonlar

#### 1. inet_ntop()

```cpp
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

- **Açıklama**: IP adresini okunabilir string formatına dönüştürür
- **Parametreler**:
  - `af`: Adres ailesi (AF_INET: IPv4)
  - `src`: Kaynak IP adresi
  - `dst`: Hedef string buffer
  - `size`: Buffer boyutu
- **Dönüş**: Başarılı olursa dst pointer'ı, hata durumunda NULL
- **Kullanım Örneği**:
```cpp
char ip_str[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
```

#### 2. inet_pton()

```cpp
int inet_pton(int af, const char *src, void *dst);
```

- **Açıklama**: String formatındaki IP adresini binary formata dönüştürür
- **Parametreler**:
  - `af`: Adres ailesi (AF_INET: IPv4)
  - `src`: Kaynak IP string'i
  - `dst`: Hedef binary buffer
- **Dönüş**: Başarılı olursa 1, hatalı format 0, hata durumunda -1
- **Kullanım Örneği**:
```cpp
struct sockaddr_in addr;
inet_pton(AF_INET, "192.168.1.1", &(addr.sin_addr));
```

#### 3. htons() / ntohs()

```cpp
uint16_t htons(uint16_t hostshort);  // Host to Network Short
uint16_t ntohs(uint16_t netshort);   // Network to Host Short
```

- **Açıklama**: Port numaralarını host ve network byte order arasında dönüştürür
- **Parametreler**:
  - `hostshort`: Host byte order'daki değer
  - `netshort`: Network byte order'daki değer
- **Kullanım Örneği**:
```cpp
server_addr.sin_port = htons(port);        // Port numarasını network byte order'a çevirir
int client_port = ntohs(addr.sin_port);    // Network byte order'dan host byte order'a çevirir
```

#### 4. INADDR_ANY

```cpp
server_addr.sin_addr.s_addr = INADDR_ANY;
```

- **Açıklama**: Sunucunun tüm network interface'lerinden gelen bağlantıları dinlemesini sağlar
- **Özellikler**:
  - Tüm yerel IP adreslerini dinler
  - 0.0.0.0 IP adresine karşılık gelir
  - Sunucu uygulamalarında yaygın olarak kullanılır



### fd_set ve İlgili Makrolar

```cpp
typedef struct {
    unsigned long fds_bits[FD_SETSIZE/NFDBITS];
} fd_set;
```

- **Açıklama**: fd_set, dosya tanımlayıcılarını bir bit maskesi olarak tutan bir veri yapısıdır
- **Önemli Makrolar**:

1. **FD_ZERO**
```cpp
void FD_ZERO(fd_set *set);
```
- fd_set yapısını sıfırlar
- Kullanım örneği:
```cpp
fd_set read_fds;
FD_ZERO(&read_fds);  // read_fds kümesini temizler
```

2. **FD_SET**
```cpp
void FD_SET(int fd, fd_set *set);
```
- Belirtilen dosya tanımlayıcısını kümeye ekler
- Kullanım örneği:
```cpp
FD_SET(sockfd, &read_fds);  // sockfd'yi izlenecek soketler kümesine ekler
```

3. **FD_CLR**
```cpp
void FD_CLR(int fd, fd_set *set);
```
- Belirtilen dosya tanımlayıcısını kümeden çıkarır
- Kullanım örneği:
```cpp
FD_CLR(client_fd, &read_fds);  // client_fd'yi kümeden çıkarır
```

4. **FD_ISSET**
```cpp
int FD_ISSET(int fd, fd_set *set);
```
- Belirtilen dosya tanımlayıcısının kümede olup olmadığını kontrol eder
- Dönüş değeri: Dosya tanımlayıcısı kümede varsa true (0'dan farklı), yoksa false (0)
- Kullanım örneği:
```cpp
if (FD_ISSET(client_fd, &read_fds)) {
    // client_fd üzerinde okuma için hazır veri var
}
```

#### fd_set Kullanım Örneği

```cpp
fd_set read_fds;
FD_ZERO(&read_fds);                // Kümeyi temizle
FD_SET(server_fd, &read_fds);      // Ana sunucu soketini ekle

// Bağlı istemcileri ekle
for (int client_fd : connected_clients) {
    FD_SET(client_fd, &read_fds);
}

// select() ile soketleri izle
fd_set temp_fds = read_fds;        // Orijinal kümeyi koru
select(max_fd + 1, &temp_fds, NULL, NULL, NULL);

// Hazır olan soketleri kontrol et
if (FD_ISSET(server_fd, &temp_fds)) {
    // Yeni bağlantı isteği var
    accept_new_connection();
}

// İstemci soketlerini kontrol et
for (int client_fd : connected_clients) {
    if (FD_ISSET(client_fd, &temp_fds)) {
        // İstemciden veri gelmiş
        handle_client_data(client_fd);
    }
}
```

## IRC Komutları

### Temel Komutlar

| Komut | Sözdizimi | Parametreler | Açıklama | Gereksinimler |
|-------|-----------|--------------|-----------|----------------|
| USER | `USER <nick> <mode> <unused> :<username>` | • nick: Takma ad<br>• mode, unused: Kullanılmıyor<br>• username: Kullanıcı adı | İlk bağlantı için kullanıcı kaydı | • İlk bağlantıda zorunlu<br>• Benzersiz nickname |
| PASS | `PASS <password>` | • password: Sunucu şifresi | Sunucu şifresi ile giriş | • İlk bağlantıda zorunlu |
| NICK | `NICK <nickname>` | • nickname: Yeni takma ad | Kullanıcı takma adını değiştirir | • Benzersiz olmalı |

### Kanal Komutları

| Komut | Sözdizimi | Parametreler | Açıklama | Gereksinimler |
|-------|-----------|--------------|-----------|----------------|
| JOIN | `JOIN <#channel>` | • #channel: Kanal adı | Kanala katılım | • Login olmuş olmak |
| PRIVMSG | `PRIVMSG <hedef> [:mesaj]` | • hedef: Kanal/kullanıcı<br>• mesaj: İletilecek mesaj | Mesaj gönderimi | • Kanala üye olmak<br>• : ile boşluklu mesaj |
| TOPIC | `TOPIC <#channel> <topic>` | • #channel: Kanal<br>• topic: Yeni konu | Kanal konusunu değiştirir | • Operator olmak |
| KICK | `KICK <#channel> <nick> [:reason]` | • #channel: Kanal<br>• nick: Hedef kullanıcı<br>• reason: Sebep | Kullanıcıyı kanaldan atar | • Operator olmak |

### Yönetim Komutları

| Komut | Sözdizimi | Parametreler | Açıklama | Özellikler |
|-------|-----------|--------------|-----------|------------|
| MODE | `MODE <#channel> <+/-><flags> [param]` | • flags: o/k/i/t/l<br>• param: Gerekli parametre | Kanal ayarlarını değiştirir | **Mod Tipleri**:<br>• o: Operator<br>• k: Şifre<br>• i: Invite-only<br>• t: Topic koruması<br>• l: Kullanıcı limiti |
| INVITE | `INVITE <nick> <#channel>` | • nick: Kullanıcı<br>• #channel: Kanal | Kullanıcıyı kanala davet eder | • Operator olmak gerekli |
| HELP | `HELP [komut]` | • komut: Komut adı | Komut yardımı gösterir | • Herkes kullanabilir |

### Hata Kodları

| Kod | Hata | Açıklama |
|-----|------|-----------|
| 401 | ERR_NOSUCHNICK | Belirtilen nickname bulunamadı |
| 403 | ERR_NOSUCHCHANNEL | Belirtilen kanal bulunamadı |
| 431 | ERR_NONICKNAMEGIVEN | Nickname belirtilmedi |
| 432 | ERR_ERRONEUSNICKNAME | Geçersiz nickname |
| 433 | ERR_NICKNAMEINUSE | Nickname kullanımda |
| 461 | ERR_NEEDMOREPARAMS | Eksik parametre |
| 464 | ERR_PASSWDMISMATCH | Yanlış şifre |
| 482 | ERR_CHANOPRIVSNEEDED | Operator yetkisi gerekli |


## Güvenlik Önlemleri

1. Şifre doğrulama
2. Kanal operatör yetkileri
3. Bağlantı sınırlamaları
4. Buffer overflow koruması

## Performans Optimizasyonları

1. Select() ile etkin I/O multiplexing
2. Verimli bellek yönetimi
3. Komut işleme optimizasyonları
4. Bağlantı havuzu yönetimi

