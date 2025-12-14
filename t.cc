#include <iostream>
#include <sstream>

class catcher {
public:
    catcher& operator<<(const std::string& str) {
        stream_ << str;
        return *this;
    }

    template<typename T>
    catcher& operator<<(const T& val) {
        stream_ << val;
        return *this;
    }

    void print() const {
        std::cout << stream_.str() << std::endl;
    }

private:
    std::stringstream stream_;
};

int main() {
    catcher x;
    x << "hello" << std::hex << 1234 << "world" << 99;
    x.print();
    return 0;
}
