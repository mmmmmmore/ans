#ifndef DECODER_H
#define DECODER_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

struct DecodedMSD {
    uint8_t msd_version;
    uint8_t message_id;
    bool automatic_activation;
    bool test_call;
    bool position_trusted;
    std::string vehicle_type;
    std::string vin;
    bool gasoline_present;
    bool diesel_present;
    bool cng_present;
    bool lpg_present;
    bool electric_present;
    bool hydrogen_present;
    bool other_storage_present;
    uint32_t timestamp;
    int32_t latitude;
    int32_t longitude;
    uint16_t vehicle_direction;
    int16_t latitude_delta_n1;
    int16_t longitude_delta_n1;
    int16_t latitude_delta_n2;
    int16_t longitude_delta_n2;
    uint8_t num_occupants;
};

class Decoder {
public:
    Decoder();
    ~Decoder();
    
    /**
     * Decode UPER binary data (as hex string) to MSD structure
     * @param hex_data Hex-encoded UPER data (space-separated or continuous)
     * @return Decoded MSD message
     */
    DecodedMSD decodeMessage(const std::string& hex_data);
    
    /**
     * Decode from binary bytes
     */
    DecodedMSD decodeMessageBinary(const std::vector<uint8_t>& binary_data);
    
    /**
     * Get human-readable vehicle type name
     */
    static std::string getVehicleTypeName(int vehicle_type);
    
    /**
     * Get human-readable timestamp
     */
    static std::string formatTimestamp(uint32_t timestamp);
    
    /**
     * Convert vehicle direction to degrees
     */
    static float directionToDegrees(uint16_t direction);
    
private:
    std::vector<uint8_t> hex_to_binary(const std::string& hex_string);
};

#endif // DECODER_H
