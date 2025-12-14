#include <iostream>
#include <map>

int main() {
    // Create a map with key-value pairs of int and string
    std::map<int, std::string> myMap;

    // Insert some elements into the map
    myMap.insert(std::make_pair(1, "apple"));
    myMap.insert(std::make_pair(2, "banana"));
    myMap.insert(std::make_pair(3, "cherry"));

    // Access elements by key using the subscript operator []
    std::cout << "myMap[1] = " << myMap[1] << std::endl;
    std::cout << "myMap[2] = " << myMap[2] << std::endl;
    std::cout << "myMap[3] = " << myMap[3] << std::endl;

    // Access elements by key using the at() method
    std::cout << "myMap.at(1) = " << myMap.at(1) << std::endl;
    std::cout << "myMap.at(2) = " << myMap.at(2) << std::endl;
    std::cout << "myMap.at(3) = " << myMap.at(3) << std::endl;

    return 0;
}

