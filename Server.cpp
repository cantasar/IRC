#include "Server.hpp"
#include "Channel.hpp"
#include <sstream>

// IRC sunucusunu başlatan yapıcı fonksiyon
// port: Sunucunun dinleyeceği port numarası
// password: Sunucu şifresi
// Bu yapıcı fonksiyon, sunucuyu başlatır ve ana program döngüsünü çalıştırır
Server::Server(int port, std::string password) : port(port), password(password)
{
    startServer();
    max_fd = sockfd; // En yüksek dosya tanımlayıcısını saklar
    loopProgram();
}

// Kopyalama yapıcı fonksiyonu
// Bir Server nesnesini başka bir Server nesnesinden kopyalar
// Tüm sunucu durumunu ve bağlantıları korur
Server::Server(Server const &server)
{
    port = server.port;
    password = server.password;
    sockfd = server.sockfd;
    clients = server.clients;
    connected_clients = server.connected_clients;
    read_fds = server.read_fds;
    max_fd = server.max_fd;
}

// Atama operatörü
// Bir Server nesnesini başka bir Server nesnesine atar
// Kendine atamayı kontrol eder (this != &server)
Server &Server::operator=(Server const &server)
{
    if (this != &server)
    {
        port = server.port;
        password = server.password;
        sockfd = server.sockfd;
        clients = server.clients;
        connected_clients = server.connected_clients;
        read_fds = server.read_fds;
        max_fd = server.max_fd;
    }
    return *this;
}

// Yıkıcı fonksiyon
// Sunucu kapatıldığında kaynakları temizler
Server::~Server() {}

// Sunucuyu başlatma ve yapılandırma işlemleri
// Bu fonksiyon şunları yapar:
// 1. Sunucu adres yapısını ayarlar
// 2. TCP soketi oluşturur
// 3. Soket seçeneklerini yapılandırır
// 4. Soketi belirtilen porta bağlar
// 5. Bağlantı dinlemeye başlar
void Server::startServer()
{
    // Sunucu adres yapısını IPv4 için ayarlama
    server_addr.sin_family = AF_INET;  // IPv4 protokolü
    server_addr.sin_port = htons(port); // Port numarasını ağ byte sırasına çevirme
    server_addr.sin_addr.s_addr = INADDR_ANY; // Tüm ağ arayüzlerinden bağlantı kabul etme

    // TCP soketi oluşturma
    // AF_INET: IPv4 protokolü
    // SOCK_STREAM: TCP bağlantı tipi
    // 0: Varsayılan protokol
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        close(sockfd);
        std::cout << "Socket KO" << std::endl;
        exit(1);
    }

    // SO_REUSEADDR seçeneğini ayarlama
    // Bu seçenek, sunucu yeniden başlatıldığında aynı portu hemen kullanabilmeyi sağlar
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(sockfd);
        std::cout << "Setsockopt KO" << std::endl;
        exit(1);
    }

    // Soketi belirtilen adres ve porta bağlama
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sockfd);
        std::cout << "Bind KO" << std::endl;
        exit(1);
    }
    else
        std::cout << "Bind OK" << std::endl;

    // Bağlantı kuyruğunu başlatma
    // 3: Maksimum bekleyen bağlantı sayısı
    if (listen(sockfd, 3) < 0)
    {
        close(sockfd);
        std::cout << "Listen KO" << std::endl;
        exit(1);
    }
    else
        std::cout << "Listen OK" << std::endl;

    // select() için dosya tanımlayıcı kümesini hazırlama
    FD_ZERO(&read_fds); // Kümeyi sıfırlama
    FD_SET(sockfd, &read_fds); // Ana soketi kümeye ekleme
}

// Ana program döngüsü
// Bu sonsuz döngü şunları yapar:
// 1. Aktif soketleri select() ile izler
// 2. Yeni bağlantıları kabul eder
// 3. Mevcut bağlantılardan gelen mesajları işler
void Server::loopProgram()
{
    while (1)
    {
        // select() için aktif soketleri kopyalama
        fd_set active_fds = read_fds;
        // select() ile soketleri izleme
        // max_fd + 1: Kontrol edilecek en yüksek dosya tanımlayıcısı
        int activity = select(max_fd + 1, &active_fds, NULL, NULL, NULL);
        if (activity < 0)
        {
            std::cout << "Select KO" << std::endl;
            exit(1);
        }

        // Yeni bağlantı isteği varsa
        if (FD_ISSET(sockfd, &active_fds))
        {
            // Yeni bağlantıyı kabul etme
            int client_sockfd = accept(sockfd, NULL, NULL);
            if (client_sockfd < 0)
            {
                close(client_sockfd);
                std::cout << "Accept KO" << std::endl;
                exit(1);
            }
            else
            {
                std::cout << "Accept OK" << std::endl;
                // Yeni soketi izleme kümesine ekleme
                FD_SET(client_sockfd, &read_fds);
                connected_clients.push_back(client_sockfd);
                // En yüksek dosya tanımlayıcısını güncelleme
                if (client_sockfd > max_fd)
                    max_fd = client_sockfd;
            }
            // Yeni istemci nesnesi oluşturma
            Client new_client(client_sockfd);
            this->clients.push_back(new_client);
        }

        // Bağlı istemcilerden gelen mesajları işleme
        std::vector<char> buffer(1024, 0); // Mesaj tamponu
        for (size_t client_index = 0; client_index < connected_clients.size(); client_index++)
        {
            // İstemciden veri geldi mi kontrol etme
            if (FD_ISSET(connected_clients[client_index], &active_fds))
            {
                // Mesajı alma
                int bytes_received = recv(connected_clients[client_index], buffer.data(), buffer.size(), 0);

                // Durum bilgilerini yazdırma ve komutları işleme
                printServer();
                checkCommands(buffer);
                printAllInputs();
                executeCommand(client_index);
                printAllClients();
                logControl(client_index);

                // Kanal bilgilerini yazdırma
                for (size_t i = 0; i < channels.size(); ++i)
                {
                    channels[i].printChannel();
                }

                // Bağlantı durumunu kontrol etme
                if (bytes_received < 0)
                {
                    close(connected_clients[client_index]);
                    std::cout << "Recv KO" << std::endl;
                    exit(1);
                }
                else if (bytes_received == 0) // Bağlantı kapandı
                {
                    close(connected_clients[client_index]);
                    connected_clients.erase(connected_clients.begin() + client_index);
                    --client_index;
                    std::cout << "Client disconnected" << std::endl;
                    exit(1);
                }
                else // Mesaj başarıyla alındı
                {
                    logControl(client_index);
                    std::cout << "Received: " << buffer.data() << std::endl;
                    memset(buffer.data(), 0, buffer.size());
                }
            }
        }
    }
}

// Kullanıcı kimlik doğrulama ve giriş kontrolü
// Bu fonksiyon, istemcinin tüm gerekli bilgileri sağlayıp sağlamadığını kontrol eder
// (şifre, kullanıcı adı, takma ad) ve giriş durumunu günceller
void Server::logControl(size_t client_index)
{
    if (clients[client_index].getPassword() != "" &&
        clients[client_index].getUsername() != "" &&
        clients[client_index].getNickname() != "" &&
        clients[client_index].getConnected() == false)
    {
        clients[client_index].setConnected(true);
        std::cout << "Client connected" << std::endl;
        // Hoş geldin mesajı gönderme
        clients[client_index].message(":" + clients[client_index].getIp_address() + " 001 " + clients[client_index].getNickname() + " :Welcome to the Internet Relay Network " + clients[client_index].getNickname() + "!" + clients[client_index].getUsername() + "@" + clients[client_index].getIp_address() + "\r\n");
    }
}

// Gelen komutları ayrıştırma
// Bu fonksiyon, istemciden gelen ham veriyi işler ve komutları ayırır
// Özel olarak gerçek isim (realname) alanını işler
void Server::checkCommands(std::vector<char> &buffer)
{
    // Buffer'ı string'e çevirme
    std::string buffer_string(buffer.data(), buffer.size());

    // Sondaki kontrol karakterlerini temizleme
    while (!buffer_string.empty() && (buffer_string.back() == '\r' || buffer_string.back() == '\n' || buffer_string.back() == '\0'))
    {
        buffer_string.pop_back();
    }

    std::istringstream iss(buffer_string);
    std::string token;
    bool realname_started = false;
    std::string realname;

    // Komutları ve parametreleri ayrıştırma
    while (iss >> token) 
    {
        if (realname_started)
            realname += (realname.empty() ? "" : " ") + token;
        else if (token[0] == ':') // Gerçek isim başlangıcı
        {
            realname_started = true;
            realname = token.substr(1);
        }
        else
            this->input.push_back(token);
    }

    // Gerçek ismi komut listesine ekleme
    if (!realname.empty())
        this->input.push_back(realname);
}

// Komutları işleme ve yürütme
// Bu fonksiyon, ayrıştırılmış komutları alır ve uygun işlevleri çağırır
void Server::executeCommand(size_t c_index)
{
    std::vector<std::string> commands;
    std::vector<fpoint> functions;

    // Desteklenen IRC komutları
    commands.push_back("NICK");    // Takma ad değiştirme
    commands.push_back("USER");    // Kullanıcı bilgilerini ayarlama
    commands.push_back("PASS");    // Şifre doğrulama
    commands.push_back("PRIVMSG"); // Özel mesaj gönderme
    commands.push_back("JOIN");    // Kanala katılma
    commands.push_back("TOPIC");   // Kanal konusunu değiştirme
    commands.push_back("KICK");    // Kullanıcıyı kanaldan atma
    commands.push_back("MODE");    // Kanal/kullanıcı modlarını değiştirme
    commands.push_back("INVITE");  // Kanala davet etme
    commands.push_back("HELP");    // Yardım bilgisi

    // Komutlara karşılık gelen işlevler
    functions.push_back(&Server::nick);
    functions.push_back(&Server::user);
    functions.push_back(&Server::pass);
    functions.push_back(&Server::privmsg);
    functions.push_back(&Server::join);
    functions.push_back(&Server::topic);
    functions.push_back(&Server::kick);
    functions.push_back(&Server::mode);
    functions.push_back(&Server::invite);
    functions.push_back(&Server::help);

    // Komutları eşleştirme ve çalıştırma
    for (size_t i = 0; i < commands.size(); i++)
    {
        for(size_t j = 0; j < this->input.size(); j++)
        {
            if (this->input[j] == commands[i])
                (this->*functions[i])(c_index);
        }
    }

    // İşlenmiş komutları temizleme
    this->input.clear();
}
