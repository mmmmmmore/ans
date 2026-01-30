# Decoder Status Report

## ✅ Successfully Implemented & Working

### Encode/Decode Cycle
- **Encoder**: Fully functional, creates UPER-encoded eCall MSD messages
- **Decoder**: Fully functional for messages created by the encoder
- **Round-trip**: Messages encoded can be successfully decoded

### ControlType SEQUENCE Support
Both encoder and decoder now fully support the ControlType SEQUENCE with all four fields:
1. `automaticActivation` (BOOLEAN)
2. `testCall` (BOOLEAN)
3. `positionCanBeTrusted` (BOOLEAN)
4. `vehicleType` (ENUM 1-23)

### Test Example
```bash
# Encode with ControlType flags
./ecall-msd-tool encode \
  --message-id 1 \
  --auto-activation \
  --test-call \
  --position-trusted \
  --vehicle-type 1 \
  --vin LGWEC24ECALL08071 \
  --timestamp 802 \
  --latitude 2147483647 \
  --longitude 2147483647 \
  --direction 255 \
  --occupants 3

# Decode the output - works perfectly!
./ecall-msd-tool decode "03 24 10 1A 02 88 3A 71 81..."
```

## ⚠️ Known Limitation

The decoder **cannot decode** the hex data from the T-E-C device log:
```
022D5C0401441D38C08438C2945002001C17F0800000645FFFFFFFFFFFFFFFFFF00401004000604100208A434D7180
```

### Possible Reasons
1. **Different ASN.1 Schema**: Device may use a different version of EN 15722
2. **Different Encoding**: Device may not use standard UPER encoding
3. **Proprietary Format**: Device may use custom encoding rules
4. **Missing Constraints**: Device encoder may have different constraints configured

### Decoder Strategies Attempted
The decoder tries multiple decode approaches in order:
1. **Raw UPER MSDMessage** - Direct UPER decode
2. **UPER ECallMessage** - Standard format (our encoder)
3. **BER ECallMessage** - Alternative BER encoding
4. **All failed** - Device data doesn't match any standard format

## Recommendation

To debug the T-E-C device data format:

1. **Request Device Documentation**: Ask T-E-C for:
   - ASN.1 schema used
   - Encoding rules (UPER/BER/DER/XER/OER)
   - Any non-standard constraints or encoding options

2. **Compare Encodings**: Generate test messages with both:
   - Our C/C++ encoder
   - T-E-C device encoder
   - Use asn1c with different options to decode

3. **Alternative Approach**: If device data is essential, consider:
   - Using Python's `asn1tools` library with exact schema
   - Contacting T-E-C for decoder library/source
   - Reverse-engineering the format from multiple samples

## Files Modified

- `src/decoder.cpp` - Enhanced decoder with multiple strategies
- `src/main.cpp` - Added ControlType fields to output
- `include/decoder.h` - Added helper function declarations
- `.gitignore` - Added comprehensive ignore rules

## Build Status

```
✓ Clean compilation
✓ Zero errors/warnings
✓ All tests compile successfully
✓ Both encoder and decoder functional
```
