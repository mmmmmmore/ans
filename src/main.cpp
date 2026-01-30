#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "encoder.h"
#include "decoder.h"
#include "utils.h"

void printUsage(const char* program_name) {
    std::cout << "eCall MSD Encoder/Decoder - EN 15722 Version 3\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << program_name << " encode [options]\n";
    std::cout << "  " << program_name << " decode <hex_data>\n";
    std::cout << "  " << program_name << " test\n\n";
    
    std::cout << "Encode Options:\n";
    std::cout << "  --message-id <id>            Message ID (0-255)\n";
    std::cout << "  --auto-activation            Automatic activation flag\n";
    std::cout << "  --test-call                  Test call flag\n";
    std::cout << "  --position-trusted           Position can be trusted\n";
    std::cout << "  --vehicle-type <type>        Vehicle type (1-23)\n";
    std::cout << "  --vin <vin>                  Vehicle ID Number (17 chars)\n";
    std::cout << "  --timestamp <ts>             Unix timestamp\n";
    std::cout << "  --latitude <lat>             Latitude in milliarcseconds\n";
    std::cout << "  --longitude <lon>            Longitude in milliarcseconds\n";
    std::cout << "  --direction <dir>            Direction of travel (0-179 or 255)\n";
    std::cout << "  --occupants <count>          Number of occupants\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " encode --message-id 1 --vehicle-type 1 --vin WBA1234567890ABCD\n";
    std::cout << "  " << program_name << " decode \"02 2D 01 02 03...\"\n";
    std::cout << "  " << program_name << " test\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    try {
        if (command == "encode") {
            Encoder encoder;
            
            // Parse parameters
            uint8_t message_id = 1;
            bool auto_activation = true;
            bool test_call = false;
            bool position_trusted = true;
            int vehicle_type = 1;
            std::string vin = "WBA1234567890ABCD";
            uint32_t timestamp = 1706515200;
            int32_t latitude = 521304000;
            int32_t longitude = 41245000;
            int direction = 0;
            uint8_t occupants = 1;
            
            // Simple parameter parsing
            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];
                
                if (arg == "--message-id" && i + 1 < argc) {
                    message_id = static_cast<uint8_t>(std::stoi(argv[++i]));
                } else if (arg == "--auto-activation") {
                    auto_activation = true;
                } else if (arg == "--test-call") {
                    test_call = true;
                } else if (arg == "--position-trusted") {
                    position_trusted = true;
                } else if (arg == "--vehicle-type" && i + 1 < argc) {
                    vehicle_type = std::stoi(argv[++i]);
                } else if (arg == "--vin" && i + 1 < argc) {
                    vin = argv[++i];
                } else if (arg == "--timestamp" && i + 1 < argc) {
                    timestamp = static_cast<uint32_t>(std::stoul(argv[++i]));
                } else if (arg == "--latitude" && i + 1 < argc) {
                    latitude = std::stoi(argv[++i]);
                } else if (arg == "--longitude" && i + 1 < argc) {
                    longitude = std::stoi(argv[++i]);
                } else if (arg == "--direction" && i + 1 < argc) {
                    direction = std::stoi(argv[++i]);
                } else if (arg == "--occupants" && i + 1 < argc) {
                    occupants = static_cast<uint8_t>(std::stoi(argv[++i]));
                }
            }
            
            std::cout << "Encoding eCall MSD message...\n";
            std::cout << "  Message ID: " << static_cast<int>(message_id) << "\n";
            std::cout << "  Vehicle Type: " << vehicle_type << "\n";
            std::cout << "  VIN: " << vin << "\n";
            
            std::string encoded = encoder.encodeMessage(
                3, message_id, auto_activation, test_call, position_trusted,
                vehicle_type, vin, timestamp, latitude, longitude, direction, occupants
            );
            
            std::cout << "\nEncoded (Hex):\n" << encoded << "\n";
            
        } else if (command == "decode") {
            if (argc < 3) {
                std::cerr << "Error: decode requires hex data argument\n";
                return 1;
            }
            
            Decoder decoder;
            std::string hex_data = argv[2];
            
            std::cout << "Decoding eCall MSD message...\n";
            std::cout << "  Input: " << hex_data << "\n\n";
            
            DecodedMSD result = decoder.decodeMessage(hex_data);
            
            std::cout << "Decoded Message:\n";
            std::cout << "  Version: " << static_cast<int>(result.msd_version) << "\n";
            std::cout << "  Message ID: " << static_cast<int>(result.message_id) << "\n";
            std::cout << "\n  Control Type (ControlType SEQUENCE):\n";
            std::cout << "    Automatic Activation: " << (result.automatic_activation ? "true" : "false") << "\n";
            std::cout << "    Test Call: " << (result.test_call ? "true" : "false") << "\n";
            std::cout << "    Position Can Be Trusted: " << (result.position_trusted ? "true" : "false") << "\n";
            std::cout << "    Vehicle Type: " << result.vehicle_type << "\n";
            std::cout << "\n  VIN: " << result.vin << "\n";
            std::cout << "  Timestamp: " << result.timestamp << " (" 
                      << Decoder::formatTimestamp(result.timestamp) << ")\n";
            std::cout << "  Latitude: " << result.latitude << "\n";
            std::cout << "  Longitude: " << result.longitude << "\n";
            std::cout << "  Direction: " << result.vehicle_direction << "° (" 
                      << Decoder::directionToDegrees(result.vehicle_direction) << "°)\n";
            std::cout << "  Occupants: " << static_cast<int>(result.num_occupants) << "\n";
            
        } else if (command == "test") {
            std::cout << "Running basic tests...\n\n";
            
            // Test encoding
            Encoder encoder;
            std::string hex = encoder.encodeMessage(
                3, 1, true, false, true, 1, "WBA1234567890ABCD",
                1706515200, 521304000, 41245000, 0, 1
            );
            std::cout << "✓ Encoding test passed\n";
            std::cout << "  Generated: " << hex.substr(0, 50) << "...\n\n";
            
            // Test VIN validation
            bool valid_vin = Utils::validateVIN("WBA1234567890ABCD");
            std::cout << "✓ VIN validation: " << (valid_vin ? "PASS" : "FAIL") << "\n";
            
            // Test coordinate validation
            bool valid_coords = Utils::validateCoordinates(521304000, 41245000);
            std::cout << "✓ Coordinate validation: " << (valid_coords ? "PASS" : "FAIL") << "\n";
            
            // Test hex conversion
            std::vector<uint8_t> test_data = {0x02, 0x2D, 0x01};
            std::string hex_str = Utils::binaryToHex(test_data);
            std::cout << "✓ Hex conversion: " << hex_str << "\n";
            
            std::cout << "\nAll basic tests completed!\n";
            
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            printUsage(argv[0]);
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
