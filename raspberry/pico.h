#pragma once
#include <windows.h>
#include <string>
#include <iostream>

class PicoController {
private:
    HANDLE hSerial;
    std::string port;
    bool connected;

public:
    PicoController(const std::string& comPort = "your com port here") : port(comPort), connected(false) {}

    bool Connect() {
        hSerial = CreateFile(("\\\\.\\" + port).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cout << "[DEBUG] failed to open com port: " << port << std::endl;
            return false;
        }

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::cout << "[DEBUG] failed to get com state" << std::endl;
            CloseHandle(hSerial);
            return false;
        }

        dcbSerialParams.BaudRate = CBR_115200;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::cout << "[DEBUG] failed to set com state" << std::endl;
            CloseHandle(hSerial);
            return false;
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        if (!SetCommTimeouts(hSerial, &timeouts)) {
            std::cout << "[DEBUG] failed to set com timeouts" << std::endl;
            CloseHandle(hSerial);
            return false;
        }

        connected = true;
        std::cout << "[DEBUG] successfully connected to " << port << std::endl;
        return true;
    }

    void Disconnect() {
        if (connected) {
            CloseHandle(hSerial);
            connected = false;
            std::cout << "[DEBUG] disconnected from " << port << std::endl;
        }
    }

    bool SendMouseCommand(double total_time_ms, double movement_time_ms, int target_x, int target_y) {
        if (!connected) {
            std::cout << "[DEBUG] not connected, cannot send command" << std::endl;
            return false;
        }

        std::string command = "MOVE|" + std::to_string((int)total_time_ms) + "|" + std::to_string((int)movement_time_ms) + "|" + std::to_string(target_x) + "|" + std::to_string(target_y) + "\n";

        std::cout << "[DEBUG] sending command: " << command;

        DWORD bytesWritten;
        bool success = WriteFile(hSerial, command.c_str(), command.length(), &bytesWritten, NULL);

        std::cout << "[DEBUG] write result: " << (success ? "SUCCESS" : "FAILED") << ", bytes written: " << bytesWritten << "/" << command.length() << std::endl;

        if (success && bytesWritten == command.length()) {
            Sleep(100);
            char buffer[256] = { 0 };
            DWORD bytesRead;

            if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::cout << "[DEBUG] raspberry response: " << buffer;
            }
            else {
                std::cout << "[DEBUG] no response from raspberry" << std::endl;
            }
            return true;
        }

        return false;
    }

    ~PicoController() {
        Disconnect();
    }
};