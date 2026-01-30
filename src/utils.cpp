#include "utils.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <cmath>

std::string Utils::binaryToHex(const std::vector<uint8_t>& data) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::uppercase;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) oss << ' ';
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    
    return oss.str();
}

std::string Utils::binaryToHex(const uint8_t* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::uppercase;
    
    for (size_t i = 0; i < len; ++i) {
        if (i > 0) oss << ' ';
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    
    return oss.str();
}

std::vector<uint8_t> Utils::hexToBinary(const std::string& hex_string) {
    std::vector<uint8_t> binary;
    std::string cleaned;
    
    // Remove spaces and whitespace
    for (char c : hex_string) {
        if (!std::isspace(c)) {
            cleaned += std::tolower(c);
        }
    }
    
    // Check for valid hex characters
    for (char c : cleaned) {
        if ((c < '0' || c > '9') && (c < 'a' || c > 'f')) {
            throw std::runtime_error("Invalid hex character: " + std::string(1, c));
        }
    }
    
    // Convert pairs to bytes
    for (size_t i = 0; i < cleaned.length(); i += 2) {
        if (i + 1 >= cleaned.length()) {
            throw std::runtime_error("Hex string has odd number of characters");
        }
        
        std::string byte_str = cleaned.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
        binary.push_back(byte);
    }
    
    return binary;
}

void Utils::printHex(const std::vector<uint8_t>& data, const std::string& label) {
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    std::cout << binaryToHex(data) << std::endl;
}

bool Utils::validateVIN(const std::string& vin) {
    if (vin.length() != 17) {
        return false;
    }
    
    for (char c : vin) {
        if ((c < '0' || c > '9') && (c < 'A' || c > 'Z')) {
            return false;
        }
        // VIN cannot contain I, O, or Q
        if (c == 'I' || c == 'O' || c == 'Q') {
            return false;
        }
    }
    
    return true;
}

bool Utils::validateCoordinates(int32_t latitude, int32_t longitude) {
    // Latitude range: -2147483648 to 2147483647 (valid: -324000000 to 324000000)
    // Longitude range: -2147483648 to 2147483647 (valid: -648000000 to 648000000)
    
    if (latitude < -324000000 || latitude > 324000000) {
        return false;
    }
    
    if (longitude < -648000000 || longitude > 648000000) {
        return false;
    }
    
    return true;
}

std::string Utils::formatSize(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    double size = static_cast<double>(bytes);
    int unit = 0;
    
    while (size >= 1024.0 && unit < 3) {
        size /= 1024.0;
        unit++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}
