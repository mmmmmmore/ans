#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>

class Utils {
public:
    /**
     * Convert binary data to hex string (space-separated)
     */
    static std::string binaryToHex(const std::vector<uint8_t>& data);
    static std::string binaryToHex(const uint8_t* data, size_t len);
    
    /**
     * Convert hex string to binary data
     */
    static std::vector<uint8_t> hexToBinary(const std::string& hex_string);
    
    /**
     * Print binary data as hex
     */
    static void printHex(const std::vector<uint8_t>& data, const std::string& label = "");
    
    /**
     * Parse VIN string and validate
     */
    static bool validateVIN(const std::string& vin);
    
    /**
     * Validate latitude/longitude ranges
     */
    static bool validateCoordinates(int32_t latitude, int32_t longitude);
    
    /**
     * Format bytes as readable size
     */
    static std::string formatSize(size_t bytes);
};

#endif // UTILS_H
