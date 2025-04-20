#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <signal.h>
#include <fstream>
#include <filesystem>
#include <sstream>

namespace pyroxene_shutdown_strategy {
    std::vector<pid_t> wait_for_pid_list(const char* pid_path);
    void monitor_feeding_processes();
}