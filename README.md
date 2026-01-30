# eCall MSD Encoder/Decoder

A Python GUI application for encoding and decoding ASN.1 eCall MSD (Minimum Set of Data) messages according to EN 15722:2019 standard.

## Status: ✓ FULLY IMPLEMENTED AND TESTED

All components working correctly:
- ✓ ASN.1 schema validated and compiled
- ✓ GUI with three functional tabs (Decode, Encode, Debug)
- ✓ UPER codec for efficient binary encoding
- ✓ 6 complete sample datasets with verified encoding/decoding
- ✓ Support for PSAP test record data
- ✓ All tests passing

## Features

- **Decode Tab**: Input hex-encoded UPER MSD data and see detailed decoded values
- **Encode Tab**: Create MSD messages from Python dictionary format and get hex output
- **Analyze Hex**: Debug tool to analyze hex input structure and identify encoding issues
- **Debug Tab**: Troubleshooting guide and helpful information
- **User-friendly GUI**: Built with Tkinter for easy interaction

## Installation

1. Ensure Python 3.7+ is installed
2. Install required packages:
   ```bash
   pip install asn1tools
   ```

## Quick Start

Run the application:
```bash
cd /Users/maochun/CLionProjects/test/ans
source .venv/bin/activate  # If using venv
python3 ecall_msd_gui.py
```

## Usage

### Decoding MSD Data from PSAP Test Records

1. Go to the **"Decode (Hex/UPER → Data)"** tab
2. Enter hex-encoded MSD data (space-separated or continuous, e.g., `02 2D 01 02 ...` or `022D0102...`)
3. Click **"Analyze Hex"** button to inspect the structure first
4. Click **"Decode (UPER)"** button to decode
5. View the detailed decoded values for each element

### If Decoding Fails

**Common Issue**: `expected X, got Y`

**Solution Steps**:
1. Click the **"Analyze Hex"** button
2. If it shows first byte is `0x02` (DER/BER length tag), this means:
   - The data has a wrapper with length indicator
   - Try removing the first 2 bytes and decode again
3. The "Analyze Hex" output will show you the data without the first 2 bytes

### Encoding MSD Data

1. Go to the **"Encode (Data → Hex/UPER)"** tab
2. Enter MSD data in Python dictionary format (see template below)
3. Click **"Load Template"** for a starting example
4. Click **"Encode to Hex"** to generate hex output
5. Copy the hex result for testing or transmission

## Data Structure

### Control Field (SEQUENCE)
```python
'control': {
    'automaticActivation': BOOLEAN,    # Automatically triggered
    'testCall': BOOLEAN,                # Test message flag
    'positionCanBeTrusted': BOOLEAN,   # GPS accuracy flag
    'vehicleType': 'passengerVehicleClassM1'  # Vehicle class enum
}
```

### Vehicle Type Values
```python
'passengerVehicleClassM1'          # Passenger cars
'passengerVehicleClassM2'          # Small buses
'passengerVehicleClassM3'          # Large buses
'lightCommercialVehicleClassM1'    # Light commercial
'heavyDutyVehicleClassN2'          # Medium duty truck
'heavyDutyVehicleClassN3'          # Heavy duty truck
'motorcycleClassL1e'               # Motorcycle <125cc
'motorcycleClassL2e'               # Motorcycle 125-250cc
'motorcycleClassL3e'               # Motorcycle >250cc
```

### Propulsion Storage (SEQUENCE with BOOLEAN fields)
```python
'vehiclePropulsionStorageType': {
    'gasolineTankPresent': BOOLEAN,
    'dieselTankPresent': BOOLEAN,
    'compressedNaturalGas': BOOLEAN,
    'liquidPropaneGas': BOOLEAN,
    'hydrogenStorage': BOOLEAN,
    'otherStorage': BOOLEAN
}
```

## Sample Data

Six complete sample MSD datasets are included in `ecall_samples.py`:
```python
from ecall_samples import minimal_msd, complete_msd, emergency_msd
from ecall_samples import lpg_vehicle_msd, electric_vehicle_msd, hybrid_vehicle_msd
```

All samples have been verified with encode/decode testing (round-trip verification).

## Testing

Run the test suite:
```bash
# Basic setup validation
python3 test_setup.py

# Test all sample data
python3 test_samples.py
```

All tests should show ✓ PASS

## File Structure
```
/Users/maochun/CLionProjects/test/ans/
├── ecall_msd.asn              # ASN.1 schema (EN 15722:2019)
├── ecall_msd_gui.py           # Main GUI application
├── ecall_samples.py           # 6 example MSD datasets
├── test_setup.py              # Basic validation tests
├── test_samples.py            # Sample data encoding tests
├── analyze_msd.py             # Hex analysis tool (for PSAP data)
├── README.md                  # This file
└── .venv/                      # Python virtual environment
```

## PSAP Test Data Support

The system is ready to handle PSAP (Public Safety Answering Point) test records:

1. **Wrapper Bytes**: Some PSAP data includes DER/BER length wrapper bytes
   - First byte: 0x02 (length tag)
   - Second byte: Length value
   - Solution: Use "Analyze Hex" to identify and remove wrapper bytes

2. **Hex Input Formats Supported**:
   - Space-separated: `02 2D 01 02 03 ...`
   - Continuous: `022D010203...`
   - Both handled automatically by the GUI

## Encoding Details

- **Codec**: UPER (Unaligned Packed Encoding Rules)
- **Minimal MSD Size**: 32 bytes (space-efficient binary format)
- **Maximum MSD Size**: ~100+ bytes (with all optional fields)
- **Format**: Binary (hex display in GUI)

## Encoding Examples

```python
import asn1tools
import binascii
from ecall_samples import minimal_msd

compiler = asn1tools.compile_files('ecall_msd.asn', codec='uper')
encoded = compiler.encode('MSDMessage', minimal_msd)
hex_data = binascii.hexlify(encoded).decode().upper()
print(f"Encoded MSD: {hex_data}")
# Output: 00090991EBC586C9A45850664C60C1C306E82CB6EB6009F127...
```

## Decoding Examples

```python
import asn1tools
import binascii

compiler = asn1tools.compile_files('ecall_msd.asn', codec='uper')
hex_data = "00090991EBC586C9A45850664C60C1C306E82CB6EB6009F127..."
encoded = binascii.unhexlify(hex_data)
decoded = compiler.decode('MSDMessage', encoded)

print(f"Vehicle Type: {decoded['msdStructure']['control']['vehicleType']}")
print(f"Position: {decoded['msdStructure']['vehicleLocation']['positionLatitude']}")
```

## Troubleshooting

### GUI doesn't start
- Verify Tkinter is installed: `python3 -c "import tkinter"`
- On macOS, may require: `brew install python-tk`

### Decoding fails with "expected X, got Y"
- Use the "Analyze Hex" tool to inspect data structure
- Check if first byte is 0x02 (DER wrapper byte)
- Try removing first 2 bytes if wrapper detected

### Encoding produces unexpected output
- Verify all required fields are present
- Check enum values match exactly (case-sensitive)
- Verify numeric ranges are within valid bounds

## References

- EN 15722:2019 - Intelligent transport systems - eCalls
- ASN.1 Basic Encoding Rules (BER)
- UPER (Unaligned Packed Encoding Rules) specification

## License

This implementation is based on the EN 15722:2019 standard for eCall MSD data.

3. Click **"Encode to Hex"** button
4. View the hex-encoded output

## Files

- `ecall_msd_gui.py` - Main GUI application
- `ecall_msd.asn` - ASN.1 schema definition for eCall MSD (EN 15722:2019)
- `analyze_msd.py` - Command-line hex analyzer for debugging

## Troubleshooting PSAP MSD Data

The eCall MSD data from PSAP test records sometimes includes wrapper bytes. Follow these steps:

### Step 1: Analyze the hex data
```bash
python3 analyze_msd.py "02 2D 01 02 03 04 ..."
```

This will show:
- Byte structure
- If first byte is `0x02` (length tag)
- Suggestion to remove first 2 bytes

### Step 2: Use the GUI
- Paste the analyzed hex into the Decode tab
- If first analysis shows 0x02, try the suggested hex without first 2 bytes
- Click "Decode (UPER)"

### Common Patterns

| First Byte | Meaning | Solution |
|-----------|---------|----------|
| `0x02` | DER/BER length tag wrapper | Remove first 2 bytes, try decoding again |
| Other | Direct UPER encoded data | Decode directly, check schema match |

## eCall MSD Structure

The MSD message contains:
- **Message Identifier** - Unique ID for each message
- **Timestamp** - Time of event (seconds since epoch)
- **Control Bits** - Automatic activation, test call flag, position trust
- **Vehicle Identification Number (VIN)** - 17-character identifier per ISO 3779
  - ISO World Manufacturer Identifier (3 chars)
  - ISO Vehicle Descriptor Section (6 chars)
  - Model year (1 char)
  - Sequential plant code (7 chars)
- **Vehicle Type** - Propulsion/storage type (gasoline, diesel, electric, LPG, CNG, etc.)
- **Vehicle Location** - Latitude/longitude deltas (1/10 second precision)
- **Vehicle Direction** - 0-359 degrees
- **Number of Passengers** - Optional
- **Additional Data** - Optional extended data

## Example Data

Template for Python dictionary encoding:
```python
{
    'msdStructure': {
        'messageIdentifier': 1,
        'timestamp': 1706515200,
        'control': (b'\x00', 8),
        'vehicleIdentificationNumber': {
            'isowmi': 'ABC',
            'isovds': '123456',
            'isovisModelyear': 'T',
            'isovisSeqPlant': '0001234'
        },
        'vehicleType': (b'\x01', 7),
        'vehicleLocation': {
            'latitudeDelta': 100,
            'longitudeDelta': -50
        },
        'vehicleDirection': 45,
        'numberOfPassengers': 2
    }
}
```

## ASN.1 Encoding Methods

This tool uses **UPER** (Unaligned Packed Encoding Rules) for encoding/decoding:
- Compact binary format used by eCall standard
- Optimized for communication efficiency
- Requires proper ASN.1 schema definition

## References

- EN 15722:2019 - eCall minimum set of data (MSD)
- ITUV-T Recommendation X.691 - UPER encoding
- ISO 3779 - Vehicle Identification Number (VIN)

## Debugging Commands

```bash
# Analyze hex data (interactive)
python3 analyze_msd.py

# Analyze hex data (with data as argument)
python3 analyze_msd.py "02 2D 01 02 ..."

# Check Python syntax
python3 -m py_compile ecall_msd_gui.py
```

## Support

For PSAP test record issues:
1. Use `analyze_msd.py` to inspect the hex structure
2. Check if first byte is 0x02 (wrapper)
3. Try removing wrapper bytes (first 2 bytes) and decode
4. Verify schema matches your MSD format
5. Compare with known-good MSD samples



02 2D 5C 04 01 44 1D 38 C0 84 38 C2 94 50 02 00 1C 17 F0 80 00 00 53 1F FF FF FF FF FF FF FF FF F0 04 01 00 40 00 60 41 00 20 8A 43 4D 71 80

0x02 0x2D 0x5C 0x04 0x01 0x44 0x1D 0x38 0xC0 0x84 0x38 0xC2 0x94 0x50 0x02 0x00 0x1C 0x17 0xF0 0x80 0x00 0x00 0x53 0x1F 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xF0 0x04 0x01 0x00 0x40 0x00 0x60 0x41 0x00 0x20 0x8A 0x43 0x4D 0x71 0x80