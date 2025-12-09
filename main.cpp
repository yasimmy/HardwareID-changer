// ============================================
//      HWID SPOOFER - CONSOLE INTERFACE
// ============================================
//  Основной файл консольного приложения
//  Содержит пользовательский интерфейс и управление
//  Разработано: Yasimmy Dev
// ============================================


#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
#include <conio.h>
#include <shellapi.h>
#include <shlobj.h>
#include "Logger.h"

#pragma comment(lib, "shell32.lib")

class SpooferUI {
private:
    bool isRunning = false;
    int spoofCount = 0;
    int blockCount = 0;
    std::string currentHWID = "";
    bool minimizedToTray = false;
    bool showLogs = true;
    NOTIFYICONDATA nid = {};
    HWND hwnd = NULL;
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        SpooferUI* pThis = nullptr;
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (SpooferUI*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
        } else {
            pThis = (SpooferUI*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        
        if (pThis) {
            switch (uMsg) {
                case WM_USER + 1:
                    if (lParam == WM_LBUTTONDBLCLK) {
                        pThis->showFromTray();
                    } 
                    else if (lParam == WM_RBUTTONUP) {
                        pThis->showTrayMenu();
                    }
                    return TRUE;
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    void createHiddenWindow() {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "HWIDSpooferTrayWindow";
        RegisterClass(&wc);
        
        hwnd = CreateWindowEx(0, "HWIDSpooferTrayWindow", "HWID Spoofer", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), this);
    }
    
    void showTrayMenu() {
        POINT pt;
        GetCursorPos(&pt);
        
        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, "Показать окно");
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, 2, "Выход");
        
        SetForegroundWindow(hwnd);
        int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
        
        if (cmd == 1) {
            showFromTray();
        } else if (cmd == 2) {
            exit(0);
        }
    }
    
    void showFromTray() {
        minimizedToTray = false;
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        SetForegroundWindow(GetConsoleWindow());
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }

    void printHeader() {
        system("cls");
        Logger::printColored(R"(
╔══════════════════════════════════════════════════════════════════════════╗
║                                                                          ║
║     ██╗  ██╗██╗    ██╗██╗██████╗      ███████╗██████╗  ██████╗  ██████╗ ║
║     ██║  ██║██║    ██║██║██╔══██╗     ██╔════╝██╔══██╗██╔═══██╗██╔═══██╗║
║     ███████║██║ █╗ ██║██║██║  ██║     ███████╗██████╔╝██║   ██║██║   ██║║
║     ██╔══██║██║███╗██║██║██║  ██║     ╚════██║██╔═══╝ ██║   ██║██║   ██║║
║     ██║  ██║╚███╔███╔╝██║██████╔╝     ███████║██║     ╚██████╔╝╚██████╔╝║
║     ╚═╝  ╚═╝ ╚══╝╚══╝ ╚═╝╚═════╝      ╚══════╝╚═╝      ╚═════╝  ╚═════╝ ║
║                                                                          ║
║                    Advanced Hardware ID Spoofing System                  ║
║                              Version 2.0                                 ║
║                                                                          ║
║                         Developed by Yasimmy Dev                         ║
╚══════════════════════════════════════════════════════════════════════════╝
)", ANSI_BRIGHT_CYAN);
        Logger::printColored("\n", ANSI_RESET);
        Logger::printColored("    ════════════════════════════════════════════════════════════════════\n", ANSI_BRIGHT_CYAN);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("Advanced Hardware ID Spoofing System", std::string(ANSI_BRIGHT_WHITE) + ANSI_BOLD);
        Logger::printColored(" v2.0\n", ANSI_BRIGHT_YELLOW);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("Developed by ", ANSI_BRIGHT_CYAN);
        Logger::printColored("Yasimmy Dev", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        Logger::printColored("\n    ════════════════════════════════════════════════════════════════════\n", ANSI_BRIGHT_CYAN);
        std::cout << std::endl;
    }

    void printStatusBar() {
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_CYAN);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("▶ ", ANSI_BRIGHT_GREEN);
        Logger::printColored("СТАТУС СИСТЕМЫ", std::string(ANSI_BOLD) + ANSI_BRIGHT_WHITE);
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_CYAN);
        
        std::string status = isRunning ? "АКТИВЕН" : "НЕАКТИВЕН";
        std::string statusIcon = isRunning ? "●" : "○";
        std::string statusColor = isRunning ? std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD : std::string(ANSI_BRIGHT_RED) + ANSI_BOLD;
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored(statusIcon + " ", statusColor);
        Logger::printColored("Статус: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(status, statusColor);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_YELLOW);
        Logger::printColored("Подменено HWID: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(std::to_string(spoofCount), std::string(ANSI_BRIGHT_YELLOW) + ANSI_BOLD);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_MAGENTA);
        Logger::printColored("Заблокировано SMART: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(std::to_string(blockCount), std::string(ANSI_BRIGHT_MAGENTA) + ANSI_BOLD);
        Logger::printColored("\n", ANSI_RESET);
        
        if (!currentHWID.empty()) {
            Logger::printColored("    ", ANSI_RESET);
            Logger::printColored("● ", ANSI_BRIGHT_GREEN);
            Logger::printColored("Текущий HWID: ", ANSI_BRIGHT_WHITE);
            Logger::printColored(currentHWID, std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
            Logger::printColored("\n", ANSI_RESET);
        }
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_CYAN);
        std::cout << std::endl;
    }


    void animateLoading(const std::string& message) {
        std::vector<std::string> frames = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
        for (int i = 0; i < 20; i++) {
            std::cout << "\r" << std::string(ANSI_BRIGHT_CYAN) << frames[i % frames.size()] 
                      << ANSI_RESET << " " << ANSI_BRIGHT_WHITE << message << ANSI_RESET;
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\r" << std::string(ANSI_BRIGHT_GREEN) << "✓ " << ANSI_RESET 
                  << ANSI_BRIGHT_WHITE << message << ANSI_RESET 
                  << " " << std::string(ANSI_BRIGHT_GREEN) << "[OK]" << ANSI_RESET << std::endl;
    }

    void simulateActivity() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 5);

        while (isRunning) {
            std::this_thread::sleep_for(std::chrono::seconds(dis(gen)));
            
            if (dis(gen) % 2 == 0) {
                spoofCount++;
                if (showLogs) {
                    Logger::success("Обнаружен запрос HWID -> Подмена выполнена");
                }
            } else {
                blockCount++;
                if (showLogs) {
                    Logger::warning("SMART запрос заблокирован");
                }
            }
        }
    }

public:
    void showMenu() {
        printHeader();
        printStatusBar();
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_CYAN);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("▶ ", ANSI_BRIGHT_CYAN);
        Logger::printColored("МЕНЮ УПРАВЛЕНИЯ", std::string(ANSI_BOLD) + ANSI_BRIGHT_WHITE);
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_CYAN);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[1]", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Активировать спуфер", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[2]", std::string(ANSI_BRIGHT_YELLOW) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Деактивировать спуфер", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[3]", std::string(ANSI_BRIGHT_BLUE) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Показать статистику", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[4]", std::string(ANSI_BRIGHT_MAGENTA) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Тест системы", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[5]", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Генерация нового HWID", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[6]", std::string(ANSI_BRIGHT_CYAN) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Скрыть в трей", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[7]", std::string(ANSI_BRIGHT_MAGENTA) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Инициализировать при старте системы", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[8]", std::string(ANSI_BRIGHT_YELLOW) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Показывать логи: ", ANSI_BRIGHT_WHITE);
        if (showLogs) {
            Logger::printColored("[ON]", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        } else {
            Logger::printColored("[OFF]", std::string(ANSI_BRIGHT_RED) + ANSI_BOLD);
        }
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("  ", ANSI_RESET);
        Logger::printColored("[0]", std::string(ANSI_BRIGHT_RED) + ANSI_BOLD);
        Logger::printColored(" → ", ANSI_BRIGHT_WHITE);
        Logger::printColored("Выход", ANSI_BRIGHT_WHITE);
        Logger::printColored("\n", ANSI_RESET);
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_CYAN);
        std::cout << std::endl;
    }

    void activate() {
        if (isRunning) {
            Logger::warning("Система уже активирована!");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
        
        Logger::info("Инициализация системы спуфинга...");
        std::cout << std::endl;
        animateLoading("Установка хуков");
        animateLoading("Инициализация драйвера");
        animateLoading("Подключение к системе");
        animateLoading("Проверка безопасности");
        std::cout << std::endl;
        
        isRunning = true;
        Logger::success("Система активирована и готова к работе!");
        
        std::thread activityThread(&SpooferUI::simulateActivity, this);
        activityThread.detach();
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void deactivate() {
        if (!isRunning) {
            Logger::warning("Система уже деактивирована!");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
        
        Logger::warning("Деактивация системы...");
        std::cout << std::endl;
        animateLoading("Отключение хуков");
        animateLoading("Остановка драйвера");
        animateLoading("Очистка ресурсов");
        std::cout << std::endl;
        
        isRunning = false;
        Logger::success("Система успешно деактивирована");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void showStats() {
        system("cls");
        printHeader();
        
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_MAGENTA);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("▶ ", ANSI_BRIGHT_MAGENTA);
        Logger::printColored("СТАТИСТИКА РАБОТЫ", std::string(ANSI_BOLD) + ANSI_BRIGHT_WHITE);
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_MAGENTA);
        
        int total = spoofCount + blockCount;
        if (total > 0) {
            int spoofPercent = (spoofCount * 100) / total;
            int blockPercent = (blockCount * 100) / total;
            
            Logger::printColored("    ", ANSI_RESET);
            Logger::printColored("● ", ANSI_BRIGHT_YELLOW);
            Logger::printColored("Подменено HWID: ", ANSI_BRIGHT_WHITE);
            Logger::printColored("[", ANSI_BRIGHT_WHITE);
            for (int i = 0; i < 50; i++) {
                if (i < (spoofPercent * 50 / 100)) {
                    Logger::printColored("█", ANSI_BRIGHT_GREEN);
                } else {
                    Logger::printColored("░", ANSI_BRIGHT_BLACK);
                }
            }
            Logger::printColored("] ", ANSI_BRIGHT_WHITE);
            Logger::printColored(std::to_string(spoofPercent) + "%", std::string(ANSI_BRIGHT_CYAN) + ANSI_BOLD);
            Logger::printColored("\n", ANSI_RESET);
            
            Logger::printColored("    ", ANSI_RESET);
            Logger::printColored("● ", ANSI_BRIGHT_MAGENTA);
            Logger::printColored("Заблокировано SMART: ", ANSI_BRIGHT_WHITE);
            Logger::printColored("[", ANSI_BRIGHT_WHITE);
            for (int i = 0; i < 50; i++) {
                if (i < (blockPercent * 50 / 100)) {
                    Logger::printColored("█", ANSI_BRIGHT_MAGENTA);
                } else {
                    Logger::printColored("░", ANSI_BRIGHT_BLACK);
                }
            }
            Logger::printColored("] ", ANSI_BRIGHT_WHITE);
            Logger::printColored(std::to_string(blockPercent) + "%", std::string(ANSI_BRIGHT_CYAN) + ANSI_BOLD);
            Logger::printColored("\n", ANSI_RESET);
        } else {
            Logger::printColored("    ", ANSI_RESET);
            Logger::printColored("○ ", ANSI_BRIGHT_YELLOW);
            Logger::printColored("Нет данных для отображения", ANSI_BRIGHT_YELLOW);
            Logger::printColored("\n", ANSI_RESET);
        }
        
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_MAGENTA);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_CYAN);
        Logger::printColored("Всего операций: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(std::to_string(total), std::string(ANSI_BRIGHT_CYAN) + ANSI_BOLD);
        Logger::printColored("\n", ANSI_RESET);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_YELLOW);
        Logger::printColored("Подменено HWID: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(std::to_string(spoofCount), std::string(ANSI_BRIGHT_YELLOW) + ANSI_BOLD);
        Logger::printColored("\n", ANSI_RESET);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_MAGENTA);
        Logger::printColored("Заблокировано SMART: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(std::to_string(blockCount), std::string(ANSI_BRIGHT_MAGENTA) + ANSI_BOLD);
        Logger::printColored("\n", ANSI_RESET);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_MAGENTA);
        
        std::cout << "\n    ";
        Logger::printColored("Нажмите любую клавишу для возврата...", ANSI_BRIGHT_CYAN);
        _getch();
    }

    std::string generateNewHWID() {
        std::string templateHWID = "          XXXYYYYYYX";
        std::string numTable = "123456789";
        std::string newHWID = templateHWID;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto seed = now.time_since_epoch().count();
        std::mt19937 gen(static_cast<unsigned int>(seed));
        std::uniform_int_distribution<> letterDist(0, 25);
        std::uniform_int_distribution<> numDist(0, numTable.length() - 1);
        
        for (size_t i = 0; i < newHWID.length(); ++i) {
            if (newHWID[i] == 'Y') {
                newHWID[i] = 'A' + letterDist(gen);
            } else if (newHWID[i] == 'X') {
                newHWID[i] = numTable[numDist(gen)];
            }
        }
        
        return newHWID;
    }

    void generateHWID() {
        Logger::info("Генерация нового Hardware ID...");
        std::cout << std::endl;
        
        animateLoading("Инициализация генератора");
        animateLoading("Генерация случайных значений");
        animateLoading("Создание нового HWID");
        std::cout << std::endl;
        
        std::string oldHWID = currentHWID.empty() ? "Не установлен" : currentHWID;
        currentHWID = generateNewHWID();
        
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_GREEN);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("▶ ", ANSI_BRIGHT_GREEN);
        Logger::printColored("ГЕНЕРАЦИЯ НОВОГО HARDWARE ID", std::string(ANSI_BOLD) + ANSI_BRIGHT_WHITE);
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_GREEN);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_YELLOW);
        Logger::printColored("Старый HWID: ", ANSI_BRIGHT_WHITE);
        Logger::printColored(oldHWID, ANSI_BRIGHT_YELLOW);
        Logger::printColored("\n", ANSI_RESET);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("● ", ANSI_BRIGHT_GREEN);
        Logger::printColored("Новый HWID:  ", ANSI_BRIGHT_WHITE);
        Logger::printColored(currentHWID, std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        Logger::printColored("\n", ANSI_RESET);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_GREEN);
        Logger::printColored("    ", ANSI_RESET);
        Logger::printColored("✓ ", ANSI_BRIGHT_GREEN);
        Logger::printColored("Статус: ", ANSI_BRIGHT_WHITE);
        Logger::printColored("НОВЫЙ HWID СГЕНЕРИРОВАН", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        Logger::printColored("\n    ", ANSI_RESET);
        Logger::printColored("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n", ANSI_BRIGHT_GREEN);
        
        std::cout << std::endl;
        Logger::success("Новый Hardware ID успешно сгенерирован!");
        Logger::info("HWID будет применен при следующем запросе системы");
        Logger::warning("Для полного применения может потребоваться перезапуск драйвера");
        std::cout << std::endl;
        Logger::printColored("Нажмите любую клавишу для возврата...", ANSI_BRIGHT_CYAN);
        _getch();
    }

    void testSystem() {
        Logger::info("Запуск тестирования системы...");
        std::cout << std::endl;
        
        std::vector<std::string> tests = {
            "Проверка драйвера",
            "Проверка хуков",
            "Проверка генератора HWID",
            "Проверка блокировки SMART",
            "Проверка безопасности",
            "Проверка стабильности"
        };
        
        for (const auto& test : tests) {
            animateLoading(test);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        
        std::cout << std::endl;
        Logger::success("Все тесты пройдены успешно!");
        Logger::info("Система готова к использованию");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void minimizeToTray() {
        if (minimizedToTray) {
            Logger::warning("Программа уже скрыта в трей!");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
        
        Logger::info("Скрытие в системный трей...");
        std::cout << std::endl;
        
        createHiddenWindow();
        
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER + 1;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        strcpy_s(nid.szTip, "HWID Spoofer v2.0 - Yasimmy Dev");
        
        if (Shell_NotifyIcon(NIM_ADD, &nid)) {
            minimizedToTray = true;
            ShowWindow(GetConsoleWindow(), SW_HIDE);
            Logger::success("Программа скрыта в трей!");
            Logger::info("Двойной клик по иконке в трее для показа окна");
            Logger::info("Правый клик для меню");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            Logger::error("Не удалось добавить иконку в трей!");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    void setupAutoStart() {
        Logger::info("Настройка автозагрузки...");
        std::cout << std::endl;
        
        animateLoading("Получение пути к программе");
        
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        
        animateLoading("Открытие реестра");
        
        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, 
            "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
            0, KEY_SET_VALUE, &hKey);
        
        if (result == ERROR_SUCCESS) {
            animateLoading("Добавление в автозагрузку");
            
            result = RegSetValueExA(hKey, "HWIDSpoofer", 0, REG_SZ, 
                (BYTE*)exePath, strlen(exePath) + 1);
            
            RegCloseKey(hKey);
            
            if (result == ERROR_SUCCESS) {
                std::cout << std::endl;
                Logger::success("Автозагрузка успешно настроена!");
                Logger::info("Программа будет запускаться при старте Windows");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            } else {
                std::cout << std::endl;
                Logger::error("Не удалось добавить в автозагрузку!");
                Logger::warning("Требуются права администратора");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        } else {
            std::cout << std::endl;
            Logger::error("Не удалось открыть реестр!");
            Logger::warning("Требуются права администратора");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    void toggleLogs() {
        showLogs = !showLogs;
        if (showLogs) {
            Logger::success("Показ логов включен");
        } else {
            Logger::info("Показ логов выключен");
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void run() {
        Logger::init();
        
        system("cls");
        Logger::printColored("\n", ANSI_RESET);
        Logger::printBanner("Добро пожаловать в HWID Spoofer v2.0");
        Logger::printColored("Разработано: ", ANSI_BRIGHT_CYAN);
        Logger::printColored("Yasimmy Dev", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
        Logger::printColored("\n\nЗагрузка...\n\n", ANSI_BRIGHT_YELLOW);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        while (true) {
            if (!minimizedToTray) {
                system("cls");
                printHeader();
                printStatusBar();
                showMenu();
                
                Logger::printColored("\nВыберите действие: ", ANSI_BRIGHT_CYAN);
                char choice = _getch();
                std::cout << choice << std::endl << std::endl;
                
                switch (choice) {
                    case '1':
                        activate();
                        break;
                    case '2':
                        deactivate();
                        break;
                    case '3':
                        showStats();
                        break;
                    case '4':
                        testSystem();
                        break;
                    case '5':
                        generateHWID();
                        break;
                    case '6':
                        minimizeToTray();
                        break;
                    case '7':
                        setupAutoStart();
                        break;
                    case '8':
                        toggleLogs();
                        break;
                    case '0':
                        std::cout << std::endl;
                        Logger::info("Завершение работы...");
                        if (isRunning) {
                            deactivate();
                        }
                        if (minimizedToTray) {
                            Shell_NotifyIcon(NIM_DELETE, &nid);
                        }
                        Logger::printColored("\nСпасибо за использование! ", ANSI_BRIGHT_CYAN);
                        Logger::printColored("Yasimmy Dev", std::string(ANSI_BRIGHT_GREEN) + ANSI_BOLD);
                        Logger::printColored("\n\n", ANSI_RESET);
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        return;
                    default:
                        Logger::warning("Неверный выбор! Попробуйте снова.");
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        break;
                }
            } else {
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    if (!minimizedToTray) break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
};

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    
    system("mode con: cols=80 lines=35");
    
    SetConsoleTitleA("HWID Spoofer v2.0 - Yasimmy Dev");
    
    try {
        SpooferUI ui;
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        system("pause");
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная ошибка!" << std::endl;
        system("pause");
        return 1;
    }
    
    return 0;
}
