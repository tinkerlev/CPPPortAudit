#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

void initializeSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
#endif
}

void cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void closeSocket(SOCKET sockfd) {
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
}

bool isValidIPAddress(const std::string& ipAddress) {
    sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0; // Check for valid IP address
}

void scanPorts(const std::string& ipAddress, int startPort, int endPort) {
    std::ofstream outFile("scan_results.txt");

    if (startPort > endPort) {
        std::cerr << "Invalid port range. Start port must be less than or equal to end port." << std::endl;
        return;
    }

    for (int port = startPort; port <= endPort; ++port) {
#ifdef _WIN32
        SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
#else
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif
        if (sockfd < 0) {
#ifdef _WIN32
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            outFile << "Error creating socket: " << WSAGetLastError() << std::endl;
#else
            std::cerr << "Error creating socket" << std::endl;
            outFile << "Error creating socket" << std::endl;
#endif
            continue;
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr);

        int result = connect(sockfd, (sockaddr*)&address, sizeof(address));
        if (result == 0) {
            std::cout << "Port " << port << " is open." << std::endl;
            outFile << "Port " << port << " is open." << std::endl;
        }
        else {
#ifdef _WIN32
            std::cerr << "Port " << port << " is closed. Error: " << WSAGetLastError() << std::endl;
            outFile << "Port " << port << " is closed. Error: " << WSAGetLastError() << std::endl;
#else
            std::cerr << "Port " << port << " is closed." << std::endl;
            outFile << "Port " << port << " is closed." << std::endl;
#endif
        }

        closeSocket(sockfd);
    }

    outFile.close();
}

int main() {
    std::string ipAddress;
    int startPort, endPort;

    initializeSockets();

    while (true) {
        std::cout << "Enter IP address: ";
        std::cin >> ipAddress;
        if (isValidIPAddress(ipAddress)) {
            break;
        }
        else {
            std::cerr << "Invalid IP address. Please try again." << std::endl;
        }
    }

    std::cout << "Enter start port: ";
    std::cin >> startPort;
    std::cout << "Enter end port: ";
    std::cin >> endPort;

    scanPorts(ipAddress, startPort, endPort);

    cleanupSockets();

    std::cout << "Press any key to close the console..." << std::endl;
    std::cin.get(); // מחכה לקלט מהמשתמש
    std::cin.get(); // מחכה לקלט מהמשתמש

    return 0;
}
