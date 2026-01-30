#include "encoder.h"
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cstring>

// Forward declarations from generated ASN.1 code
extern "C" {
    #include "ECallMessage.h"
    #include "MSDStructure.h"
    #include "MSDMessage.h"
    #include "ControlType.h"
    #include "VIN.h"
    #include "VehicleLocation.h"
    #include "VehicleLocationDelta.h"
    #include "VehiclePropulsionStorageType.h"
    #include "VehicleType.h"
    #include "asn_application.h"
    #include "per_encoder.h"
}

Encoder::Encoder() {
}

Encoder::~Encoder() {
}

std::string Encoder::encodeMessage(
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
    uint8_t num_occupants) {
    
    // Return encoding with default propulsion values
    return encodeMessageFull(
        msd_version, message_id, automatic_activation, test_call,
        position_trusted, vehicle_type, vin,
        true, false, false, false, false, false, false,  // Only gasoline
        timestamp, latitude, longitude, vehicle_direction,
        0, 0, 0, 0,  // No location deltas
        num_occupants
    );
}

std::string Encoder::encodeMessageFull(
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
    uint8_t num_occupants) {
    
    MSDMessage_t *msdMsg = nullptr;
    ECallMessage_t *message = nullptr;
    
    try {
        // Allocate MSDMessage
        msdMsg = (MSDMessage_t *)calloc(1, sizeof(MSDMessage_t));
        if (!msdMsg) throw std::runtime_error("MSDMessage allocation failed");
        msdMsg->optionalAdditionalData = nullptr;
        MSDStructure_t *msdStruct = &msdMsg->msdStructure;
        
        // Set message identifier
        msdStruct->messageIdentifier = message_id;
        
        // Set control type
        msdStruct->control.automaticActivation = automatic_activation ? 1 : 0;
        msdStruct->control.testCall = test_call ? 1 : 0;
        msdStruct->control.positionCanBeTrusted = position_trusted ? 1 : 0;
        msdStruct->control.vehicleType = vehicle_type;
        
        // Set VIN (split into components: 3 + 6 + 1 + 7 = 17 chars)
        if (vin.length() != 17) {
            throw std::runtime_error("VIN must be exactly 17 characters");
        }
        OCTET_STRING_fromBuf(&msdStruct->vehicleIdentificationNumber.isowmi, 
                             vin.substr(0, 3).c_str(), 3);
        OCTET_STRING_fromBuf(&msdStruct->vehicleIdentificationNumber.isovds, 
                             vin.substr(3, 6).c_str(), 6);
        OCTET_STRING_fromBuf(&msdStruct->vehicleIdentificationNumber.isovisModelyear, 
                             vin.substr(9, 1).c_str(), 1);
        OCTET_STRING_fromBuf(&msdStruct->vehicleIdentificationNumber.isovisSeqPlant, 
                             vin.substr(10, 7).c_str(), 7);
        
        // Set vehicle propulsion storage type (defaults to FALSE)
        msdStruct->vehiclePropulsionStorageType.gasolineTankPresent = gasoline ? 1 : 0;
        msdStruct->vehiclePropulsionStorageType.dieselTankPresent = diesel ? 1 : 0;
        msdStruct->vehiclePropulsionStorageType.compressedNaturalGas = cng ? 1 : 0;
        msdStruct->vehiclePropulsionStorageType.liquidPropaneGas = lpg ? 1 : 0;
        msdStruct->vehiclePropulsionStorageType.electricEnergyStorage = electric ? 1 : 0;
        msdStruct->vehiclePropulsionStorageType.hydrogenStorage = hydrogen ? 1 : 0;
        msdStruct->vehiclePropulsionStorageType.otherStorage = other_storage ? 1 : 0;
        
        // Set timestamp
        msdStruct->timestamp = timestamp;
        
        // Set vehicle location
        msdStruct->vehicleLocation.positionLatitude = latitude;
        msdStruct->vehicleLocation.positionLongitude = longitude;
        
        // Set vehicle direction
        msdStruct->vehicleDirection = vehicle_direction;
        
        // Set recent location deltas
        msdStruct->recentVehicleLocationN1.latitudeDelta = lat_delta_n1;
        msdStruct->recentVehicleLocationN1.longitudeDelta = lon_delta_n1;
        msdStruct->recentVehicleLocationN2.latitudeDelta = lat_delta_n2;
        msdStruct->recentVehicleLocationN2.longitudeDelta = lon_delta_n2;
        
        // Set number of occupants (optional)
        if (num_occupants != 255) {
            msdStruct->numberOfOccupants = (long *)calloc(1, sizeof(long));
            *msdStruct->numberOfOccupants = num_occupants;
        }
        
        // Validate the structure before encoding
        char err_buf[256];
        size_t err_len;
        int check_result = asn_check_constraints(
            &asn_DEF_MSDMessage,
            msdMsg,
            err_buf,
            &err_len
        );
        
        if (check_result != 0) {
            throw std::runtime_error(std::string("MSDMessage constraint validation failed: ") + err_buf);
        }
        
        // Encode MSDMessage to a newly allocated UPER buffer
        void *msd_buffer = nullptr;
        ssize_t msd_bytes = uper_encode_to_new_buffer(
            &asn_DEF_MSDMessage,
            nullptr,
            msdMsg,
            &msd_buffer
        );
        
        if (msd_bytes < 0 || !msd_buffer) {
            throw std::runtime_error("MSDMessage UPER encoding failed");
        }
        
        // Create ECallMessage
        message = (ECallMessage_t *)calloc(1, sizeof(ECallMessage_t));
        if (!message) throw std::runtime_error("ECallMessage allocation failed");
        
        message->msdVersion = msd_version;
        OCTET_STRING_fromBuf(&message->msd, (const char *)msd_buffer, (size_t)msd_bytes);
        
        // Encode ECallMessage to final buffer
        uint8_t buffer[512];
        asn_enc_rval_t enc_result = uper_encode_to_buffer(
            &asn_DEF_ECallMessage,
            nullptr,
            message,
            buffer,
            sizeof(buffer)
        );
        
        if (enc_result.encoded < 0) {
            throw std::runtime_error("ECallMessage UPER encoding failed");
        }
        
        // Convert binary to hex string with spaces
        std::ostringstream hex_stream;
        hex_stream << std::hex << std::setfill('0') << std::uppercase;
        
        size_t encoded_bytes = (enc_result.encoded + 7) / 8;
        for (size_t i = 0; i < encoded_bytes; i++) {
            if (i > 0) hex_stream << ' ';
            hex_stream << std::setw(2) << static_cast<int>(buffer[i]);
        }
        
        // Cleanup
        if (msd_buffer) {
            free(msd_buffer);
        }
        ASN_STRUCT_FREE(asn_DEF_MSDMessage, msdMsg);
        ASN_STRUCT_FREE(asn_DEF_ECallMessage, message);
        
        return hex_stream.str();
        
    } catch (const std::exception& e) {
        // Cleanup on error
        if (msdMsg) ASN_STRUCT_FREE(asn_DEF_MSDMessage, msdMsg);
        if (message) ASN_STRUCT_FREE(asn_DEF_ECallMessage, message);
        throw std::runtime_error(std::string("Encoding failed: ") + e.what());
    }
}
