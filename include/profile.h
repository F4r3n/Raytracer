#ifndef PROFILE_H
#define PROFILE_H
#include <chrono>
class Profile {

public:
    Profile() {
        start = std::chrono::system_clock::now();
        running= true;
    }
    Profile(const std::string& name)
    {
        fName = name;
        start = std::chrono::system_clock::now();
        running = true;
    }

    ~Profile()
    {
        stop();
    }

    void stop()
    {
        end = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        running = false;
    }
    unsigned long elapsed = 0;

private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    std::string fName;
    bool running = false;
};

#endif // PROFILE_H

