#include "decoder.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <map>

// Forward declarations from generated ASN.1 code
extern "C" {
    #include "ECallMessage.h"
    #include "MSDMessage.h"
    #include "MSDStructure.h"
    #include "asn_application.h"
    #include "per_decoder.h"
    #include "ber_decoder.h"
}

Decoder::Decoder() {
}

Decoder::~Decoder() {
}

std::vector<uint8_t> Decoder::hex_to_binary(const std::string& hex_string) {
    std::vector<uint8_t> binary;
    std::string cleaned;
    
    // Remove spaces and convert to lowercase
    for (char c : hex_string) {
        if (c != ' ' && c != '\n' && c != '\r') {
            cleaned += std::tolower(c);
        }
    }
    
    // Convert hex pairs to bytes
    for (size_t i = 0; i < cleaned.length(); i += 2) {
        if (i + 1 >= cleaned.length()) {
            throw std::runtime_error("Invalid hex string length");
        }
        
        std::string byte_str = cleaned.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
        binary.push_back(byte);
    }
    
    return binary;
}

DecodedMSD Decoder::decodeMessage(const std::string& hex_data) {
    std::vector<uint8_t> binary = hex_to_binary(hex_data);
    return decodeMessageBinary(binary);
}

DecodedMSD Decoder::decodeMessageBinary(const std::vector<uint8_t>& binary_data) {
    DecodedMSD result = {};
    
    try {
        MSDMessage_t *message = nullptr;
        asn_dec_rval_t rval = {RC_FAIL, 0};
        
        // Step 0: First try direct UPER decode as raw MSDMessage (some devices use this)
        rval = uper_decode(
            nullptr,
            &asn_DEF_MSDMessage,
            (void**)&message,
            binary_data.data(),
            binary_data.size(),
            0, 0
        );
        
        if (rval.code == RC_OK && message) {
            return extractDecodedData(message);
        }
        
        // Step 1: Try to decode as ECallMessage using UPER (our encoder format)
        ECallMessage_t *ecall_msg = nullptr;
        rval = uper_decode(
            nullptr,
            &asn_DEF_ECallMessage,
            (void**)&ecall_msg,
            binary_data.data(),
            binary_data.size(),
            0, 0
        );
        
        if (rval.code == RC_OK && ecall_msg && ecall_msg->msd.buf && ecall_msg->msd.size > 0) {
            // Extract MSD content (which is UPER-encoded MSDMessage)
            rval = uper_decode(
                nullptr,
                &asn_DEF_MSDMessage,
                (void**)&message,
                ecall_msg->msd.buf,
                ecall_msg->msd.size,
                0, 0
            );
            ASN_STRUCT_FREE(asn_DEF_ECallMessage, ecall_msg);
            
            if (rval.code == RC_OK && message) {
                return extractDecodedData(message);
            }
        }
        
        // Step 2: If ECallMessage decode failed, try BER-encoded ECallMessage
        ecall_msg = nullptr;
        rval = ber_decode(
            nullptr,
            &asn_DEF_ECallMessage,
            (void**)&ecall_msg,
            binary_data.data(),
            binary_data.size()
        );
        
        if (rval.code == RC_OK && ecall_msg && ecall_msg->msd.buf && ecall_msg->msd.size > 0) {
            // Try to decode MSD as UPER-encoded MSDMessage
            rval = uper_decode(
                nullptr,
                &asn_DEF_MSDMessage,
                (void**)&message,
                ecall_msg->msd.buf,
                ecall_msg->msd.size,
                0, 0
            );
            ASN_STRUCT_FREE(asn_DEF_ECallMessage, ecall_msg);
            
            if (rval.code == RC_OK && message) {
                return extractDecodedData(message);
            }
        }
        
        throw std::runtime_error("All decode strategies failed: raw UPER MSDMessage, UPER ECallMessage, BER ECallMessage");
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Decoding failed: ") + e.what());
    }
}

DecodedMSD Decoder::extractDecodedData(MSDMessage_t *message) {
    DecodedMSD result = {};
    
    if (!message) {
        throw std::runtime_error("Invalid message pointer");
    }
    
    MSDStructure_t *msd = &message->msdStructure;
        
        // Extract message identifier
        result.msd_version = 3;
        result.message_id = msd->messageIdentifier;
        
        // Extract control bits
        ControlType_t *control = &msd->control;
        result.automatic_activation = control->automaticActivation;
        result.test_call = control->testCall;
        result.position_trusted = control->positionCanBeTrusted;
        
        // Extract vehicle type
        int vehicle_type = control->vehicleType;
        result.vehicle_type = getVehicleTypeName(vehicle_type);
        
        // Extract VIN
        VIN_t *vin = &msd->vehicleIdentificationNumber;
        char vin_buffer[18] = {0};
        
        // PrintableString_t is actually OCTET_STRING which has buf pointer and size
        // Copy each VIN component, handling OCTET_STRING structure
        int offset = 0;
        if (vin->isowmi.buf && vin->isowmi.size) {
            memcpy(vin_buffer + offset, vin->isowmi.buf, vin->isowmi.size);
            offset += vin->isowmi.size;
        }
        if (vin->isovds.buf && vin->isovds.size) {
            memcpy(vin_buffer + offset, vin->isovds.buf, vin->isovds.size);
            offset += vin->isovds.size;
        }
        if (vin->isovisModelyear.buf && vin->isovisModelyear.size) {
            memcpy(vin_buffer + offset, vin->isovisModelyear.buf, vin->isovisModelyear.size);
            offset += vin->isovisModelyear.size;
        }
        if (vin->isovisSeqPlant.buf && vin->isovisSeqPlant.size) {
            memcpy(vin_buffer + offset, vin->isovisSeqPlant.buf, vin->isovisSeqPlant.size);
            offset += vin->isovisSeqPlant.size;
        }
        vin_buffer[offset] = '\0';
        result.vin = std::string(vin_buffer);
        
        // Extract timestamp
        result.timestamp = msd->timestamp;
        
        // Extract vehicle location
        VehicleLocation_t *loc = &msd->vehicleLocation;
        result.latitude = loc->positionLatitude;
        result.longitude = loc->positionLongitude;
        
        // Extract recent vehicle locations (deltas)
        VehicleLocationDelta_t *delta1 = &msd->recentVehicleLocationN1;
        if (delta1->latitudeDelta != 0 || delta1->longitudeDelta != 0) {
            result.latitude_delta_n1 = delta1->latitudeDelta;
            result.longitude_delta_n1 = delta1->longitudeDelta;
        }
        
        VehicleLocationDelta_t *delta2 = &msd->recentVehicleLocationN2;
        if (delta2->latitudeDelta != 0 || delta2->longitudeDelta != 0) {
            result.latitude_delta_n2 = delta2->latitudeDelta;
            result.longitude_delta_n2 = delta2->longitudeDelta;
        }
        
        // Extract vehicle direction
        result.vehicle_direction = msd->vehicleDirection;
        
        // Extract number of occupants
        if (msd->numberOfOccupants) {
            result.num_occupants = (uint8_t)*msd->numberOfOccupants;
        }
        
        // Extract propulsion storage types
        VehiclePropulsionStorageType_t *propulsion = &msd->vehiclePropulsionStorageType;
        result.gasoline_present = propulsion->gasolineTankPresent;
        result.diesel_present = propulsion->dieselTankPresent;
        result.cng_present = propulsion->compressedNaturalGas;
        result.lpg_present = propulsion->liquidPropaneGas;
        result.electric_present = propulsion->electricEnergyStorage;
        result.hydrogen_present = propulsion->hydrogenStorage;
        result.other_storage_present = propulsion->otherStorage;
        
        // Clean up
        ASN_STRUCT_FREE(asn_DEF_MSDMessage, message);
        
        return result;
}

std::string Decoder::getVehicleTypeName(int vehicle_type) {
    static const std::map<int, std::string> types = {
        {1, "passengerVehicleCategoryM1"},
        {2, "busesAndCoachesCategoryM2"},
        {3, "busesAndCoachesCategoryM3"},
        {4, "lightCommercialVehiclesN1"},
        {5, "heavyDutyVehiclesCategoryN2"},
        {6, "heavyDutyVehiclesCategoryN3"},
        {7, "motorcyclesCategoryL1e"},
        {8, "motorcyclesCategoryL2e"},
        {9, "motorcyclesCategoryL3e"},
        {10, "motorcyclesCategoryL4e"},
        {11, "motorcyclesCategoryL5e"},
        {12, "motorcyclesCategoryL6e"},
        {13, "motorcyclesCategoryL7e"},
        {14, "trailersCategoryO"},
        {15, "agriVehiclesCategoryR"},
        {16, "agriVehiclesCategoryS"},
        {17, "agriVehiclesCategoryT"},
        {18, "offRoadVehiclesCategoryG"},
        {19, "specialPurposeMotorCaravanCategorySA"},
        {20, "specialPurposeArmouredVehicleCategorySB"},
        {21, "specialPurposeAmbulanceCategorySC"},
        {22, "specialPurposeHearseCategorySD"},
        {23, "otherVehicleCategory"}
    };
    
    auto it = types.find(vehicle_type);
    return (it != types.end()) ? it->second : "unknown";
}

std::string Decoder::formatTimestamp(uint32_t timestamp) {
    time_t t = static_cast<time_t>(timestamp);
    struct tm* timeinfo = gmtime(&t);
    
    char buffer[26];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", timeinfo);
    return std::string(buffer);
}

float Decoder::directionToDegrees(uint16_t direction) {
    if (direction == 255) {
        return -1.0f;  // Invalid/unknown
    }
    return direction * 2.0f;  // 2-degree steps
}
