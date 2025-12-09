#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

#define ANSI_RESET   "\033[0m"
#define ANSI_BLACK   "\033[30m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"
#define ANSI_BRIGHT_BLACK   "\033[90m"
#define ANSI_BRIGHT_RED     "\033[91m"
#define ANSI_BRIGHT_GREEN   "\033[92m"
#define ANSI_BRIGHT_YELLOW  "\033[93m"
#define ANSI_BRIGHT_BLUE    "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN    "\033[96m"
#define ANSI_BRIGHT_WHITE   "\033[97m"

#define ANSI_BOLD    "\033[1m"
#define ANSI_DIM     "\033[2m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_BLINK   "\033[5m"

class Logger {
private:
    static bool enableVTMode() {
        static bool enabled = false;
        if (!enabled) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut != INVALID_HANDLE_VALUE) {
                DWORD dwMode = 0;
                if (GetConsoleMode(hOut, &dwMode)) {
                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    SetConsoleMode(hOut, dwMode);
                    enabled = true;
                }
            }
        }
        return enabled;
    }

    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

public:
    static void init() {
        enableVTMode();
    }

    static void log(const std::string& level, const std::string& color, const std::string& message) {
        enableVTMode();
        std::cout << color << "[" << getCurrentTime() << "] " 
                  << ANSI_BOLD << "[" << level << "]" << ANSI_RESET << " " 
                  << color << message << ANSI_RESET << std::endl;
    }

    static void success(const std::string& message) {
        log("SUCCESS", ANSI_BRIGHT_GREEN, message);
    }

    static void info(const std::string& message) {
        log("INFO", ANSI_BRIGHT_CYAN, message);
    }

    static void warning(const std::string& message) {
        log("WARNING", ANSI_BRIGHT_YELLOW, message);
    }

    static void error(const std::string& message) {
        log("ERROR", ANSI_BRIGHT_RED, message);
    }

    static void debug(const std::string& message) {
        log("DEBUG", ANSI_BRIGHT_MAGENTA, message);
    }

    static void printColored(const std::string& message, const std::string& color) {
        enableVTMode();
        std::cout << color << message << ANSI_RESET;
    }

    static void printBanner(const std::string& text) {
        enableVTMode();
        std::cout << ANSI_BRIGHT_CYAN << ANSI_BOLD << text << ANSI_RESET << std::endl;
    }
};
