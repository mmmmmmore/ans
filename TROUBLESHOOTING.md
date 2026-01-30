# eCall MSD Decoding Troubleshooting Guide

## Problem: "expected X, got Y" Error

This error occurs when the ASN.1 schema doesn't match the actual data structure. Here's how to diagnose and fix it:

### Step 1: Analyze Your Hex Data

Use the provided hex analyzer:

```bash
python3 analyze_msd.py "02 2D 01 02 03 04 ..."
```

This will show:
- Byte-by-byte breakdown
- If first byte is `0x02` (DER/BER length wrapper)
- Suggested fixes

### Step 2: Check for Length Wrapper

eCall MSD from PSAP test records often includes a **DER/BER wrapper**:

```
02 2D [actual MSD data...]
│  │
│  └─ Length: 0x2D (45 bytes)
└──── Tag: 0x02 (length indicator)
```

**Solution**: Remove the first 2 bytes and decode

### Step 3: Try Decoding

1. **In GUI**:
   - Go to "Decode (Hex/UPER → Data)" tab
   - Click "Analyze Hex" button
   - If first byte is `0x02`, copy the suggested hex (without first 2 bytes)
   - Paste and click "Decode (UPER)"

2. **In Terminal**:
   ```bash
   # If your hex is: 02 2D 01 02 03 04 ...
   # Extract data part (skip first 2 bytes):
   # New hex: 01 02 03 04 ...
   ```

## Common Scenarios

### Scenario 1: Raw UPER Encoded Data
- **Indicator**: First byte is NOT 0x02, data looks random
- **Action**: Decode directly in GUI
- **Expected**: Shows message fields if schema matches

### Scenario 2: DER/BER Wrapped UPER Data
- **Indicator**: First byte is 0x02, followed by length value
- **Example**: `02 2D 01 02 03 04 ...`
- **Action**: 
  1. Click "Analyze Hex" 
  2. Copy suggested data (skip first 2 bytes)
  3. Decode the extracted data

### Scenario 3: Multiple Wrapper Bytes
- **Indicator**: Multiple 0x02 bytes at start
- **Action**: Analyze carefully, may need to skip multiple wrapper levels

## Field Errors

### "expected BIT STRING"
- VehicleType or Control field has wrong format
- In Python dict, should be: `(b'\x00', 8)` where 8 is bit length

### "expected SEQUENCE"  
- Vehicle Identification Number has wrong structure
- Should have exactly 4 fields: isowmi, isovds, isovisModelyear, isovisSeqPlant

### "expected INTEGER"
- messageIdentifier, timestamp, or location field has wrong type
- Should be numeric values

## Debug Mode

Enable detailed debugging by modifying the GUI:

1. Open `ecall_msd_gui.py`
2. Uncomment debug print statements (if any exist)
3. Run with Python directly:
   ```bash
   python3 ecall_msd_gui.py 2>&1 | tee debug.log
   ```

## Testing Your PSAP Data

1. **Get your PSAP MSD hex**: Request from test provider
2. **Analyze it**:
   ```bash
   python3 analyze_msd.py "your_hex_data_here"
   ```
3. **Check output**:
   - Note first byte
   - Note suggested modifications
4. **Test in GUI**:
   - Copy hex from analyzer output
   - Paste in Decode tab
   - Try decoding

## If Decoding Still Fails

### Possible Causes:

1. **Schema mismatch**: ASN.1 schema doesn't match actual PSAP MSD standard
   - Solution: Obtain correct schema from PSAP provider
   - Update `ecall_msd.asn` file

2. **Multiple encoding layers**: Data wrapped multiple times
   - Solution: Gradually remove wrapper bytes
   - Use hex editor to visualize structure

3. **Different UPER rules**: Aligned vs Unaligned PER
   - Solution: Check UPER settings in asn1tools
   - Current setting: `codec='uper'` (unaligned)

4. **Incomplete data**: Only part of MSD provided
   - Solution: Get complete message from PSAP provider

### Next Steps:

1. Save hex output from `analyze_msd.py` to file
2. Extract data portion (skip known wrappers)
3. Compare with working MSD samples
4. Adjust schema if structure doesn't match
5. Contact PSAP provider for schema documentation

## Advanced: Custom ASN.1 Schema

If none of the above works, you may need a custom schema:

1. Get actual MSD structure from PSAP provider
2. Create new `ecall_msd_custom.asn` based on EN 15722 spec
3. Update GUI to load custom schema:
   ```python
   self.compiler = asn1tools.compile_files('ecall_msd_custom.asn', codec='uper')
   ```

## Support Information

- **EN 15722:2019**: eCall MSD standard document
- **ITU-T X.691**: UPER encoding specification
- **asn1tools**: https://github.com/eerimoq/asn1tools

## Files Reference

- `ecall_msd_gui.py` - Main application
- `ecall_msd.asn` - Current ASN.1 schema
- `analyze_msd.py` - Hex analyzer tool
- `test_setup.py` - Setup verification script
