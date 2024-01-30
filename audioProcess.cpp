#include <iostream>
#include <string>
#include <Windows.h>
#include <BluetoothAPIs.h>

#pragma comment(lib, "Bthprops.lib")

int main() {
    // Specify the address of your Arduino's Bluetooth module
    const wchar_t* arduinoAddress = L"XX:XX:XX:XX:XX:XX";  // Replace with the actual address

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // Create a Bluetooth socket
    SOCKET bluetoothSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (bluetoothSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create Bluetooth socket\n";
        WSACleanup();
        return 1;
    }

    // Set the Bluetooth address and port
    SOCKADDR_BTH socketAddress;
    ZeroMemory(&socketAddress, sizeof(socketAddress));
    socketAddress.addressFamily = AF_BTH;
    socketAddress.btAddr = *reinterpret_cast<ULONGLONG*>(arduinoAddress);
    socketAddress.serviceClassId = RFCOMM_PROTOCOL_UUID;
    socketAddress.port = 1;  // 1 is the channel used by most Bluetooth modules

    // Connect to the Arduino
    if (connect(bluetoothSocket, reinterpret_cast<SOCKADDR*>(&socketAddress), sizeof(socketAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to Arduino\n";
        closesocket(bluetoothSocket);
        WSACleanup();
        return 1;
    }

    // Send data to Arduino
    const char* sendData = "Hello, Arduino!";
    send(bluetoothSocket, sendData, strlen(sendData), 0);

    // Receive data from Arduino
    char recvData[1024];
    int bytesReceived = recv(bluetoothSocket, recvData, sizeof(recvData), 0);
    recvData[bytesReceived] = '\0';
    std::cout << "Received data: " << recvData << "\n";

    // Close the connection
    closesocket(bluetoothSocket);
    WSACleanup();

    return 0;
}