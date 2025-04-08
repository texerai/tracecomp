#ifndef SPIKE_HANDLER_H
#define SPIKE_HANDLER_H

#include <string>
#include <fstream>

class SpikeHandler {
public:
    SpikeHandler(const std::string& command, const std::string& log_path);
    
    ~SpikeHandler();
    
    void run();

private:
    int child_pid;      // not always child
    int master_fd;      // file descriptorfor master
    std::ofstream log_file; // Log file stream

    void wait_for_prompt(const std::string& prompt);
    
    void write_to_child(const std::string& data);
    
    void interactive_loop();
};

#endif