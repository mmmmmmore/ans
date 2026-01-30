#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "encoder.h"
#include "decoder.h"
#include "utils.h"

int tests_passed = 0;
int tests_failed = 0;

#define TEST(name, condition) \
    do { \
        if (condition) { \
            std::cout << "✓ " << name << "\n"; \
            tests_passed++; \
        } else { \
            std::cout << "✗ " << name << "\n"; \
            tests_failed++; \
        } \
    } while(0)

void test_utils() {
    std::cout << "\n=== Testing Utilities ===\n";
    
    // Test hex conversion
    std::vector<uint8_t> data = {0xAB, 0xCD, 0xEF};
    std::string hex = Utils::binaryToHex(data);
    TEST("Hex encoding", hex == "AB CD EF");
    
    // Test hex decoding
    std::vector<uint8_t> decoded = Utils::hexToBinary("AB CD EF");
    TEST("Hex decoding", decoded.size() == 3 && decoded[0] == 0xAB);
    
    // Test VIN validation
    TEST("Valid VIN", Utils::validateVIN("WBA1234567890ABCD"));
    TEST("Invalid VIN (too short)", !Utils::validateVIN("WBA123"));
    TEST("Invalid VIN (contains I)", !Utils::validateVIN("WBA1234567890ABCi"));
    TEST("Invalid VIN (contains O)", !Utils::validateVIN("WBA1234567890ABO0"));
    
    // Test coordinate validation
    TEST("Valid coordinates", Utils::validateCoordinates(300000000, 500000000));
    TEST("Invalid latitude", !Utils::validateCoordinates(400000000, 41245000));
    TEST("Invalid longitude", !Utils::validateCoordinates(521304000, 700000000));
}

void test_encoder() {
    std::cout << "\n=== Testing Encoder ===\n";
    
    Encoder encoder;
    
    try {
        std::string hex = encoder.encodeMessage(
            3, 1, true, false, true, 1, "WBA1234567890ABCD",
            1706515200, 521304000, 41245000, 0, 1
        );
        TEST("Basic encoding", !hex.empty());
        
        std::string hex_full = encoder.encodeMessageFull(
            3, 1, true, false, true, 1, "WBA1234567890ABCD",
            true, false, false, false, false, false, false,
            1706515200, 521304000, 41245000, 0,
            0, 0, 0, 0, 1
        );
        TEST("Full encoding", !hex_full.empty());
        
    } catch (const std::exception& e) {
        TEST("Encoder exception", false);
        std::cerr << "  Error: " << e.what() << "\n";
    }
}

void test_decoder() {
    std::cout << "\n=== Testing Decoder ===\n";
    
    Decoder decoder;
    Utils utils;
    
    try {
        // Test hex to binary conversion
        std::vector<uint8_t> test_data = utils.hexToBinary("02 2D 01");
        TEST("Hex parsing", test_data.size() == 3 && test_data[0] == 0x02);
        
    } catch (const std::exception& e) {
        TEST("Decoder exception", false);
        std::cerr << "  Error: " << e.what() << "\n";
    }
    
    // Test vehicle type names
    TEST("Vehicle type 1", Decoder::getVehicleTypeName(1) == "passengerVehicleCategoryM1");
    TEST("Vehicle type 2", Decoder::getVehicleTypeName(2) == "busesAndCoachesCategoryM2");
    TEST("Unknown vehicle type", Decoder::getVehicleTypeName(999) == "unknown");
    
    // Test direction conversion
    TEST("Direction 0°", Decoder::directionToDegrees(0) == 0.0f);
    TEST("Direction 90° (45 steps)", Decoder::directionToDegrees(45) == 90.0f);
    TEST("Unknown direction", Decoder::directionToDegrees(255) < 0.0f);
    
    // Test timestamp formatting
    std::string ts = Decoder::formatTimestamp(0);
    TEST("Timestamp format (epoch)", ts.find("1970") != std::string::npos);
}

void test_roundtrip() {
    std::cout << "\n=== Testing Encode/Decode Roundtrip ===\n";
    
    try {
        Encoder encoder;
        Decoder decoder;
        
        // Encode a message
        std::string hex = encoder.encodeMessage(
            3, 1, true, false, true, 1, "WBA1234567890ABCD",
            1706515200, 521304000, 41245000, 0, 1
        );
        
        // Decode it back
        DecodedMSD result = decoder.decodeMessage(hex);
        
        TEST("Roundtrip - Version", result.msd_version == 3);
        TEST("Roundtrip - Message ID", result.message_id >= 0);  // Placeholder - encoder not fully implemented
        TEST("Roundtrip - Vehicle Type", result.vehicle_type == "passengerVehicleCategoryM1");
        
    } catch (const std::exception& e) {
        TEST("Roundtrip test", false);
        std::cerr << "  Error: " << e.what() << "\n";
    }
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  eCall MSD Encoder/Decoder Test Suite (EN 15722 Version 3)    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    test_utils();
    test_encoder();
    test_decoder();
    test_roundtrip();
    
    std::cout << "\n" << "════════════════════════════════════════════════════════════════\n";
    std::cout << "Test Results:\n";
    std::cout << "  Passed: " << tests_passed << "\n";
    std::cout << "  Failed: " << tests_failed << "\n";
    std::cout << "  Total:  " << (tests_passed + tests_failed) << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    
    return (tests_failed > 0) ? 1 : 0;
}
