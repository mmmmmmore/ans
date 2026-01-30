#ifndef ENCODER_H
#define ENCODER_H

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

// Forward declarations (generated from ASN.1 via asn1c)
struct ECallMessage_s;

class Encoder {
public:
    Encoder();
    ~Encoder();
    
    /**
     * Encode an eCall MSD message to UPER binary format
     * @param msd_version MSD format version (should be 3)
     * @param message_id Message identifier (0-255)
     * @param automatic_activation Whether automatically activated
     * @param test_call Whether this is a test call
     * @param position_trusted Whether position can be trusted
     * @param vehicle_type Vehicle type enum value
     * @param vin Vehicle Identification Number (17 chars)
     * @param timestamp Unix timestamp of incident
     * @param latitude Latitude in milliarcseconds (WGS84)
     * @param longitude Longitude in milliarcseconds (WGS84)
     * @param vehicle_direction Direction of travel (0-179 or 255)
     * @param num_occupants Number of occupants (optional, 0-255 or 255 for unknown)
     * @return Encoded binary data as hex string
     */
    std::string encodeMessage(
        uint8_t msd_version,
        uint8_t message_id,
        bool automatic_activation,
        bool test_call,
        bool position_trusted,
        int vehicle_type,
        const std::string& vin,
        uint32_t timestamp,
        int32_t latitude,
        int32_t longitude,
        int vehicle_direction,
        uint8_t num_occupants = 255
    );
    
    /**
     * Encode with propulsion/storage information
     */
    std::string encodeMessageFull(
        uint8_t msd_version,
        uint8_t message_id,
        bool automatic_activation,
        bool test_call,
        bool position_trusted,
        int vehicle_type,
        const std::string& vin,
        bool gasoline,
        bool diesel,
        bool cng,
        bool lpg,
        bool electric,
        bool hydrogen,
        bool other_storage,
        uint32_t timestamp,
        int32_t latitude,
        int32_t longitude,
        int vehicle_direction,
        int lat_delta_n1,
        int lon_delta_n1,
        int lat_delta_n2,
        int lon_delta_n2,
        uint8_t num_occupants = 255
    );
    
private:
    std::vector<uint8_t> binary_data;
};

#endif // ENCODER_H
