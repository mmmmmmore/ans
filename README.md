# eCall MSD Encoder/Decoder

A Python GUI application for encoding and decoding ASN.1 eCall MSD (Minimum Set of Data) messages according to EN 15722 standard.

## Features

- **Decode Tab**: Input hex-encoded MSD data and see detailed decoded values
- **Encode Tab**: Create MSD messages from Python dictionary format and get hex output
- **User-friendly GUI**: Built with Tkinter for easy interaction
- **Example Templates**: Load example data to get started quickly

## Installation

1. Ensure Python 3.7+ is installed
2. Install required package:
   ```bash
   pip install asn1tools
   ```

## Usage

Run the application:
```bash
python ecall_msd_gui.py
```

### Decoding MSD Data

1. Go to the "Decode" tab
2. Enter hex-encoded MSD data (with or without spaces)
3. Click "Decode" button
4. View the detailed decoded values for each element

### Encoding MSD Data

1. Go to the "Encode" tab
2. Enter MSD data in Python dictionary format
3. Click "Encode" button
4. View the hex-encoded output

## Files

- `ecall_msd_gui.py` - Main GUI application
- `ecall_msd.asn` - ASN.1 schema definition for eCall MSD

## eCall MSD Structure

The MSD message contains:
- Message Identifier
- Timestamp
- Control bits (automatic activation, test call, position trust, etc.)
- Vehicle Identification Number (VIN)
- Vehicle Type (propulsion/storage)
- Vehicle Location (latitude/longitude deltas)
- Vehicle Direction
- Number of Passengers
- Additional optional data

## Example

See the application's "Load Template" and "Load Example" buttons for sample data.
