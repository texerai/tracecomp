#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <util.h>
#include <utmp.h>

class SpikeHandler {
    int child_pid;
    int master_fd;
    bool running;
    
public:
    SpikeHandler(const std::string& command) 
        : child_pid(-1), master_fd(-1), running(true) {
        
        int slave_fd;
        char name[256];
        
        if (openpty(&master_fd, &slave_fd, name, nullptr, nullptr) == -1) {
            throw std::runtime_error("Failed to create PTY");
        }
        
        child_pid = fork();
        if (child_pid == -1) {
            close(master_fd);
            close(slave_fd);
            throw std::runtime_error("Failed to fork process");
        }
        
        if (child_pid == 0) {
            close(master_fd);
            setsid();
            if (login_tty(slave_fd) == -1) _exit(127);
            execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
            _exit(127);
        }
        close(slave_fd);
    }

    void run_until_done() {
        std::string buffer;
        bool flip = false;
        
        // Initial prompt wait
        wait_for_prompt("(spike) ", buffer);
        std::cout << buffer;
        buffer.clear();

        while (running) {
            // Alternate between reg dump and step
            write_to_child(flip ? "\n" : "reg 0\n");
            flip = !flip;

            // Read until next prompt
            if (!wait_for_prompt("(spike) ", buffer, 2000)) {
                std::cerr << "Timeout or EOF detected" << std::endl;
                break;
            }
            
            std::cout << buffer;
            
            // Check termination conditions
            if (buffer.find("tohost") != std::string::npos || 
                buffer.find("core   0: ") == std::string::npos) {  // Spike exited
                running = false;
            }
            buffer.clear();
        }
    }

    ~SpikeHandler() {
        if (master_fd != -1) close(master_fd);
        if (child_pid != -1) waitpid(child_pid, nullptr, 0);
    }

private:
    bool wait_for_prompt(const std::string& prompt, std::string& buffer, int timeout_ms=5000) {
        struct pollfd pfd = {master_fd, POLLIN, 0};
        char buf[256];
        ssize_t count;

        while (true) {
            int ready = poll(&pfd, 1, timeout_ms);
            if (ready == -1) throw std::runtime_error("Poll error");
            if (ready == 0) return false;  // Timeout
            
            count = read(master_fd, buf, sizeof(buf));
            if (count <= 0) return false;  // EOF
            
            buffer.append(buf, count);
            
            if (buffer.find(prompt) != std::string::npos) {
                // Remove prompt from buffer
                size_t pos = buffer.find(prompt);
                buffer.erase(pos, prompt.length());
                return true;
            }
        }
    }

    void write_to_child(const std::string& data) {
        if (write(master_fd, data.c_str(), data.size()) != data.size()) {
            throw std::runtime_error("Write to child failed");
        }
    }
};

int main() {
    try {
        // Important: Use '-d' for debug mode and include 'pk'
        SpikeHandler spike("spike --log-commits /Users/sayat/Documents/GitHub/tracecomp/test/tests/bin/riscv-tests/rv64ui-p-add &> spike_output.log");
        spike.run_until_done();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}