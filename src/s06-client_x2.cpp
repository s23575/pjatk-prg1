#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/select.h>
#include <unistd.h>

#include <iostream>
#include <string>

auto main() -> int
{
    auto sock = socket(AF_INET, SOCK_STREAM, 0);

    auto const ip = std::string{"127.0.0.1"};

    auto const port = uint16_t{8080};

    sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htobe16(port);
    inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr);

    const int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    auto server_sock =
        connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (server_sock == -1) {
        perror("connect(2)");
        return 1;
    }

    auto buf = std::string{};
    std::cout << "Wiadomość do wysłania: ";
    std::getline(std::cin, buf);

    auto n = write(sock, &buf[0], buf.size());
    if (n == -1) {
        perror("write(2)");
    }
    /*
        auto wielkosc = int{1024};
        auto resp     = std::string{};
        resp.resize(wielkosc);

        auto o = read(server_sock, &resp[0], resp.size());
        if (o == -1) {
            perror("read(2)");
        }

        std::cout << "Wiadomość odebrana: " << resp << "\n";
    */
    shutdown(sock, SHUT_RDWR);

    close(sock);

    return 0;
}
