cmake_minimum_required(VERSION 3.15)
project(eCall-MSD-Encoder-Decoder C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

# Find asn1c compiler
find_program(ASN1C_EXECUTABLE asn1c REQUIRED HINTS /usr/local/bin /opt/homebrew/bin)
message(STATUS "Found asn1c: ${ASN1C_EXECUTABLE}")

# ASN.1 schema compilation
set(ASN1_SCHEMA "${CMAKE_SOURCE_DIR}/asn1/15722.asn")
set(ASN1_GEN_DIR "${CMAKE_BINARY_DIR}/asn1_generated")

file(MAKE_DIRECTORY "${ASN1_GEN_DIR}")

# Generate C files from ASN.1 schema using asn1c
# List all required generated files explicitly
set(ASN1_GENERATED_C
    "${ASN1_GEN_DIR}/ECallMessage.c"
    "${ASN1_GEN_DIR}/MSDMessage.c"
    "${ASN1_GEN_DIR}/MSDStructure.c"
    "${ASN1_GEN_DIR}/ControlType.c"
    "${ASN1_GEN_DIR}/VehicleType.c"
    "${ASN1_GEN_DIR}/VIN.c"
    "${ASN1_GEN_DIR}/VehicleLocation.c"
    "${ASN1_GEN_DIR}/VehicleLocationDelta.c"
    "${ASN1_GEN_DIR}/VehiclePropulsionStorageType.c"
    "${ASN1_GEN_DIR}/AdditionalData.c"
    "${ASN1_GEN_DIR}/CurrentVersion.c"
    # ASN.1 support types
    "${ASN1_GEN_DIR}/BOOLEAN.c"
    "${ASN1_GEN_DIR}/INTEGER.c"
    "${ASN1_GEN_DIR}/INTEGER_oer.c"
    "${ASN1_GEN_DIR}/BIT_STRING.c"
    "${ASN1_GEN_DIR}/BIT_STRING_oer.c"
    "${ASN1_GEN_DIR}/OCTET_STRING.c"
    "${ASN1_GEN_DIR}/OCTET_STRING_oer.c"
    "${ASN1_GEN_DIR}/PrintableString.c"
    "${ASN1_GEN_DIR}/OBJECT_IDENTIFIER.c"
    "${ASN1_GEN_DIR}/NativeInteger.c"
    "${ASN1_GEN_DIR}/NativeInteger_oer.c"
    "${ASN1_GEN_DIR}/NativeEnumerated.c"
    "${ASN1_GEN_DIR}/NativeEnumerated_oer.c"
    "${ASN1_GEN_DIR}/OPEN_TYPE.c"
    "${ASN1_GEN_DIR}/OPEN_TYPE_oer.c"
    # ASN.1 codec support
    "${ASN1_GEN_DIR}/asn_application.c"
    "${ASN1_GEN_DIR}/asn_bit_data.c"
    "${ASN1_GEN_DIR}/asn_codecs_prim.c"
    "${ASN1_GEN_DIR}/asn_internal.c"
    "${ASN1_GEN_DIR}/asn_random_fill.c"
    "${ASN1_GEN_DIR}/constr_CHOICE.c"
    "${ASN1_GEN_DIR}/constr_CHOICE_oer.c"
    "${ASN1_GEN_DIR}/constr_SEQUENCE.c"
    "${ASN1_GEN_DIR}/constr_SEQUENCE_oer.c"
    "${ASN1_GEN_DIR}/constr_TYPE.c"
    "${ASN1_GEN_DIR}/per_support.c"
    "${ASN1_GEN_DIR}/per_decoder.c"
    "${ASN1_GEN_DIR}/per_encoder.c"
    "${ASN1_GEN_DIR}/per_opentype.c"
    "${ASN1_GEN_DIR}/ber_tlv_tag.c"
    "${ASN1_GEN_DIR}/ber_tlv_length.c"
    "${ASN1_GEN_DIR}/ber_decoder.c"
    "${ASN1_GEN_DIR}/der_encoder.c"
    "${ASN1_GEN_DIR}/constraints.c"
    "${ASN1_GEN_DIR}/oer_decoder.c"
    "${ASN1_GEN_DIR}/oer_encoder.c"
    "${ASN1_GEN_DIR}/oer_support.c"
    "${ASN1_GEN_DIR}/pdu_collection.c"
    "${ASN1_GEN_DIR}/xer_support.c"
    "${ASN1_GEN_DIR}/xer_decoder.c"
    "${ASN1_GEN_DIR}/xer_encoder.c"
)

# Create custom command to generate all ASN.1 code
add_custom_command(
    OUTPUT ${ASN1_GENERATED_C}
    COMMAND cd ${ASN1_GEN_DIR} && ${ASN1C_EXECUTABLE} -fcompound-names -pdu=all ${ASN1_SCHEMA}
    DEPENDS ${ASN1_SCHEMA}
    COMMENT "Generating C code from ASN.1 schema"
)

# Main executable
add_executable(ecall-msd-tool 
    src/main.cpp
    src/encoder.cpp
    src/decoder.cpp
    src/utils.cpp
    ${ASN1_GENERATED_C}
)

target_include_directories(ecall-msd-tool PRIVATE
    include
    ${ASN1_GEN_DIR}
)

# Test executable
add_executable(ecall-msd-test
    tests/test_main.cpp
    tests/test_encoder.cpp
    tests/test_decoder.cpp
    src/encoder.cpp
    src/decoder.cpp
    src/utils.cpp
    ${ASN1_GENERATED_C}
)

target_include_directories(ecall-msd-test PRIVATE
    include
    ${ASN1_GEN_DIR}
)

# Enable testing
enable_testing()
add_test(NAME eCall-Tests COMMAND ecall-msd-test)

# Installation
install(TARGETS ecall-msd-tool DESTINATION bin)
install(FILES asn1/15722.asn DESTINATION share/ecall-msd)
install(FILES README.md DESTINATION share/doc/ecall-msd)

message(STATUS "eCall MSD Encoder/Decoder project configured")
message(STATUS "ASN.1 Generated files: ${ASN1_GEN_DIR}")

