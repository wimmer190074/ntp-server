#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>

#define SERVER "91.227.237.208"
#define PORT 27000
#define BUFLEN 48

int main() {
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_aton(SERVER, &serverAddr.sin_addr) == 0) {
        std::cerr << "Invalid address: " << SERVER << "\n";
        close(clientSocket);
        return 2;
    }

    // Create and send NTP request packet
    char ntpRequest[BUFLEN] = {0x1B};  // NTP header, version 4, mode 3 (client)
    if (sendto(clientSocket, ntpRequest, sizeof(ntpRequest), 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "sendto() failed\n";
        close(clientSocket);
        return 3;
    }

    // Receive and parse NTP response packet
    char ntpResponse[BUFLEN] = {};
    socklen_t serverAddrSize = sizeof(serverAddr);
    int receivedBytes = recvfrom(clientSocket, ntpResponse, BUFLEN, 0, (sockaddr*)&serverAddr, &serverAddrSize);
    if (receivedBytes == -1) {
        std::cerr << "recvfrom() failed\n";
        close(clientSocket);
        return 4;
    }

    uint32_t secondsSince1900 = ntohl(*(uint32_t*)&ntpResponse[40]);
    time_t serverTime = secondsSince1900 - 2208988800;

    std::cout << "Server time: " << std::ctime(&serverTime);

    close(clientSocket);
    return 0;
}
