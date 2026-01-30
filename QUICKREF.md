# Quick Reference Guide - eCall MSD Encoder/Decoder

## 🚀 Quick Start

### GUI Application
```bash
python3 ecall_msd_gui.py
```

### Verify Setup
```bash
python3 test_setup.py
```

### Analyze Hex Data
```bash
python3 analyze_msd.py "02 2D 01 02 ..."
```

---

## 📊 Common Tasks

### Task 1: Decode PSAP MSD Data
1. Run hex analyzer: `python3 analyze_msd.py "your_hex"`
2. Open GUI and go to **Decode** tab
3. Paste the hex (with or without first 2 bytes as suggested)
4. Click **Decode (UPER)**
5. View results

### Task 2: Create New MSD
1. Open GUI and go to **Encode** tab
2. Edit template with your data:
   ```python
   {
       'msdStructure': {
           'messageIdentifier': 1,
           'timestamp': 1706515200,
           'control': (b'\x00', 3),
           'vehicleIdentificationNumber': {
               'isowmi': 'ABC',
               'isovds': '123456',
               'isovisModelyear': 'T',
               'isovisSeqPlant': '0001234'
           },
           'vehicleType': (b'\x01', 7),
           'vehicleLocation': {'latitudeDelta': 100, 'longitudeDelta': -50},
           'vehicleDirection': 45
       }
   }
   ```
3. Click **Encode to Hex**
4. Copy hex output

### Task 3: Test System
1. Run: `python3 test_setup.py`
2. Should show: ✓ ALL TESTS PASSED

---

## 🔧 Troubleshooting Matrix

| Problem | First Byte | Solution |
|---------|-----------|----------|
| "expected X, got Y" | 0x02 | Remove first 2 bytes, try again |
| "expected X, got Y" | Other | Check schema matches PSAP format |
| Invalid hex format | - | Check hex uses only 0-9, A-F |
| Very short data | - | Verify complete MSD was provided |
| Decoding hangs | - | Check for corrupted wrapper bytes |

---

## 📝 File Reference

| File | Purpose | Command |
|------|---------|---------|
| `ecall_msd_gui.py` | Main application | `python3 ecall_msd_gui.py` |
| `analyze_msd.py` | Hex analyzer | `python3 analyze_msd.py "hex"` |
| `test_setup.py` | Verify setup | `python3 test_setup.py` |
| `ecall_samples.py` | Example data | `import ecall_samples` |
| `ecall_msd.asn` | ASN.1 schema | (Used internally) |

---

## 🎯 Field Encoding Quick Ref

### Message Identifier
```
Range: 0-255
Example: 1
```

### Timestamp
```
Format: Unix timestamp (seconds since epoch)
Example: 1706515200 (2024-01-29 16:00:00 UTC)
```

### Control Bits
```
Format: (binary_value, bit_length)
Format: (b'\x00', 3)  or  (b'\x01', 3)  or  (b'\x02', 3)

Values:
  0x00 = No flags
  0x01 = Automatic activation
  0x02 = Test call
  0x04 = Position trusted
```

### Vehicle Type
```
Format: (binary_value, 7)
Single bit: (b'\x01', 7) = Gasoline
Multiple:   (b'\x18', 7) = Electric + LPG

Bit mapping:
  0x01 = Gasoline
  0x02 = Diesel
  0x04 = CNG
  0x08 = LPG
  0x10 = Electric
  0x20 = Hydrogen
  0x40 = Other
```

### Location (Latitude/Longitude)
```
Format: Integer from -2048 to +2047
Unit: 1/10 of a second precision
Example: 100 = ~10 meters N/S or E/W
```

### Vehicle Direction
```
Format: Integer 0-359
Unit: Degrees
0 = North, 90 = East, 180 = South, 270 = West
```

---

## 📋 MSD Structure Overview

```
MSDMessage
├── msdStructure (REQUIRED)
│   ├── messageIdentifier
│   ├── timestamp
│   ├── control
│   ├── vehicleIdentificationNumber (VIN)
│   │   ├── isowmi (3 chars)
│   │   ├── isovds (6 chars)
│   │   ├── isovisModelyear (1 char)
│   │   └── isovisSeqPlant (7 chars)
│   ├── vehicleType
│   ├── vehicleLocation (REQUIRED)
│   │   ├── latitudeDelta
│   │   └── longitudeDelta
│   ├── vehicleDirection
│   ├── recentVehicleLocationN1 (optional)
│   ├── recentVehicleLocationN2 (optional)
│   ├── numberOfPassengers (optional)
│   ├── emergencyNumber (optional)
│   └── additionalData (optional)
└── optionalData (optional)
```

---

## 🐛 Debug Workflow

1. **Get raw hex**
   ```bash
   # From PSAP test record, clipboard, or file
   ```

2. **Analyze structure**
   ```bash
   python3 analyze_msd.py "02 2D 01 02 03 04 ..."
   ```

3. **Check for wrapper**
   - If first byte is 0x02, data includes length tag
   - Remove first 2 bytes for actual MSD

4. **Test decode**
   - Open GUI, Decode tab
   - Paste hex (analyzer output recommended)
   - Click Decode

5. **If fails**
   - Check error message
   - Review TROUBLESHOOTING.md
   - Verify data completeness
   - Contact PSAP provider if needed

---

## 📞 Getting Help

- **Installation issues**: Check `test_setup.py` output
- **Decoding failures**: Use `analyze_msd.py` and `TROUBLESHOOTING.md`
- **Schema mismatches**: Review `ecall_msd.asn` vs. EN 15722:2019
- **Encoding errors**: Check `ecall_samples.py` format
- **Library issues**: https://github.com/eerimoq/asn1tools

---

## ⚡ Performance Tips

- Analyze hex first with `analyze_msd.py`
- Remove wrapper bytes before decoding (saves processing)
- Use samples as template (faster than manual entry)
- Test on small data first, then scale

---

## 🔐 Security Notes

- Only decode trusted MSD data from known sources
- Validate input before processing
- Check file permissions on shared systems
- Keep asn1tools library updated

---

## 📚 Standards Reference

- **EN 15722:2019** - eCall MSD specification
- **ITU-T X.691** - UPER encoding rules
- **ISO 3779** - Vehicle ID numbers

---

**Last Updated**: January 29, 2026  
**Version**: 1.0.0
