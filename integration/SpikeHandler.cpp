#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <util.h>  // For openpty
#include <utmp.h> // For login_tty
#include "SpikeHandler.h"
class SpikeHandler {
    int child_pid;
    int master_fd;
    std::ofstream log_file;
    
public:
    SpikeHandler(const std::string& command, const std::string& log_path) 
        : child_pid(-1), master_fd(-1), log_file(log_path) {
        
        int slave_fd;
        char name[256];
        
        // Create PTY pair
        if (openpty(&master_fd, &slave_fd, name, nullptr, nullptr) == -1) {
            throw std::runtime_error("Failed to create PTY");
        }
        
        child_pid = fork();
        if (child_pid == -1) {
            close(master_fd);
            close(slave_fd);
            throw std::runtime_error("Failed to fork process");
        }
        
        if (child_pid == 0) { // Child process
            close(master_fd);
            
            // Create new session and set up PTY
            setsid();
            if (login_tty(slave_fd) == -1) {
                _exit(127);
            }
            
            // Execute command
            execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
            _exit(127); // Only reached if exec fails
        }
        
        close(slave_fd);
        fcntl(master_fd, F_SETFL, O_NONBLOCK);
    }

    // ... rest of the class remains the same as previous version ...
    // (run(), ~SpikeHandler(), wait_for_prompt(), etc.)
void run() {
        // Wait for debug prompt
        wait_for_prompt("(spike) ");
        
        // Send run command
        write_to_child("r\n");
        
        // Interactive mode
        interactive_loop();
    }

    ~SpikeHandler() {
        if (master_fd != -1) close(master_fd);
        if (child_pid != -1) waitpid(child_pid, nullptr, 0);
    }

private:
    void wait_for_prompt(const std::string& prompt) {
        std::string buffer;
        struct pollfd pfd = {master_fd, POLLIN, 0};
        
        while (true) {
            int ready = poll(&pfd, 1, 5000); // 5 second timeout
            if (ready == -1) throw std::runtime_error("Poll error");
            if (ready == 0) throw std::runtime_error("Prompt timeout");
            
            char ch;
            while (read(master_fd, &ch, 1) > 0) {
                buffer += ch;
                log_file << ch;
                
                if (buffer.find(prompt) != std::string::npos) {
                    return;
                }
            }
        }
    }

    void write_to_child(const std::string& data) {
        write(master_fd, data.c_str(), data.size());
    }

    void interactive_loop() {
        struct pollfd pfds[2] = {
            {master_fd, POLLIN, 0},
            {STDIN_FILENO, POLLIN, 0}
        };

        while (true) {
            int ready = poll(pfds, 2, -1);
            if (ready == -1) break;

            // Child output
            if (pfds[0].revents & POLLIN) {
                char buffer[4096];
                ssize_t count = read(master_fd, buffer, sizeof(buffer));
                if (count <= 0) break;
                
                log_file.write(buffer, count);
                std::cout.write(buffer, count);
            }

            // User input
            if (pfds[1].revents & POLLIN) {
                char buffer[4096];
                ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (count <= 0) break;
                
                write(master_fd, buffer, count);
            }
        }
    }

};

