#!/usr/bin/env python3
"""
eCall MSD Encoder/Decoder GUI Application
Provides a graphical interface for encoding and decoding ASN.1 eCall MSD messages
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import binascii
import os
import sys

try:
    import asn1tools
except ImportError:
    print("asn1tools library not found. Please install it using: pip install asn1tools")
    sys.exit(1)


class ECallMSDApp:
    def __init__(self, root):
        self.root = root
        self.root.title("eCall MSD Encoder/Decoder")
        self.root.geometry("900x700")
        
        # Load ASN.1 schema
        try:
            schema_path = os.path.join(os.path.dirname(__file__), 'ecall_msd.asn')
            self.compiler = asn1tools.compile_files(schema_path, codec='uper')
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load ASN.1 schema: {str(e)}")
            sys.exit(1)
        
        self.create_widgets()
        
    def create_widgets(self):
        # Create notebook for tabs
        notebook = ttk.Notebook(self.root)
        notebook.pack(fill='both', expand=True, padx=10, pady=10)
        
        # Decode tab
        decode_frame = ttk.Frame(notebook)
        notebook.add(decode_frame, text="Decode")
        self.create_decode_tab(decode_frame)
        
        # Encode tab
        encode_frame = ttk.Frame(notebook)
        notebook.add(encode_frame, text="Encode")
        self.create_encode_tab(encode_frame)
        
    def create_decode_tab(self, parent):
        # Input frame
        input_frame = ttk.LabelFrame(parent, text="Encoded MSD Input (Hex)", padding=10)
        input_frame.pack(fill='both', expand=False, padx=10, pady=10)
        
        self.decode_input = scrolledtext.ScrolledText(input_frame, height=5, wrap=tk.WORD)
        self.decode_input.pack(fill='both', expand=True)
        self.decode_input.insert('1.0', "Enter hex-encoded MSD data here...")
        
        # Button frame
        button_frame = ttk.Frame(parent)
        button_frame.pack(fill='x', padx=10, pady=5)
        
        ttk.Button(button_frame, text="Decode", command=self.decode_msd).pack(side='left', padx=5)
        ttk.Button(button_frame, text="Clear", command=self.clear_decode).pack(side='left', padx=5)
        ttk.Button(button_frame, text="Load Example", command=self.load_example_decode).pack(side='left', padx=5)
        
        # Output frame
        output_frame = ttk.LabelFrame(parent, text="Decoded MSD Details", padding=10)
        output_frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        self.decode_output = scrolledtext.ScrolledText(output_frame, height=20, wrap=tk.WORD, font=('Courier', 10))
        self.decode_output.pack(fill='both', expand=True)
        
    def create_encode_tab(self, parent):
        # Input frame
        input_frame = ttk.LabelFrame(parent, text="MSD Data (Python Dict Format)", padding=10)
        input_frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        self.encode_input = scrolledtext.ScrolledText(input_frame, height=15, wrap=tk.WORD, font=('Courier', 10))
        self.encode_input.pack(fill='both', expand=True)
        self.encode_input.insert('1.0', "Enter MSD data as Python dictionary...")
        
        # Button frame
        button_frame = ttk.Frame(parent)
        button_frame.pack(fill='x', padx=10, pady=5)
        
        ttk.Button(button_frame, text="Encode", command=self.encode_msd).pack(side='left', padx=5)
        ttk.Button(button_frame, text="Clear", command=self.clear_encode).pack(side='left', padx=5)
        ttk.Button(button_frame, text="Load Template", command=self.load_template).pack(side='left', padx=5)
        
        # Output frame
        output_frame = ttk.LabelFrame(parent, text="Encoded MSD (Hex)", padding=10)
        output_frame.pack(fill='both', expand=False, padx=10, pady=10)
        
        self.encode_output = scrolledtext.ScrolledText(output_frame, height=8, wrap=tk.WORD, font=('Courier', 10))
        self.encode_output.pack(fill='both', expand=True)
        
    def decode_msd(self):
        try:
            # Get input and clean it
            hex_input = self.decode_input.get('1.0', tk.END).strip()
            hex_input = hex_input.replace(' ', '').replace('\n', '').replace('\r', '')
            
            # Convert hex to bytes
            byte_data = binascii.unhexlify(hex_input)
            
            # Decode using ASN.1 compiler
            decoded_data = self.compiler.decode('MSDMessage', byte_data)
            
            # Format output
            output = self.format_decoded_data(decoded_data)
            
            # Display result
            self.decode_output.delete('1.0', tk.END)
            self.decode_output.insert('1.0', output)
            
        except binascii.Error as e:
            messagebox.showerror("Error", f"Invalid hex input: {str(e)}")
        except Exception as e:
            messagebox.showerror("Error", f"Decoding failed: {str(e)}")
    
    def format_decoded_data(self, data, indent=0):
        """Format decoded data for readable display"""
        output = []
        indent_str = "  " * indent
        
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, dict):
                    output.append(f"{indent_str}{key}:")
                    output.append(self.format_decoded_data(value, indent + 1))
                elif isinstance(value, (list, tuple)):
                    output.append(f"{indent_str}{key}: [")
                    for item in value:
                        if isinstance(item, dict):
                            output.append(self.format_decoded_data(item, indent + 2))
                        else:
                            output.append(f"{indent_str}  {item}")
                    output.append(f"{indent_str}]")
                elif isinstance(value, bytes):
                    output.append(f"{indent_str}{key}: {binascii.hexlify(value).decode()}")
                else:
                    # Add description for specific fields
                    desc = self.get_field_description(key, value)
                    output.append(f"{indent_str}{key}: {value}{desc}")
        else:
            return str(data)
            
        return "\n".join(output)
    
    def get_field_description(self, field_name, value):
        """Get human-readable description for field values"""
        descriptions = {
            'messageIdentifier': ' (Message ID)',
            'timestamp': f' ({self.format_timestamp(value)} UTC)',
            'vehicleDirection': f' ({value * 2} degrees)',
            'numberOfPassengers': ' (passenger count)',
        }
        return descriptions.get(field_name, '')
    
    def format_timestamp(self, timestamp):
        """Convert timestamp to readable format"""
        import datetime
        try:
            # Assuming timestamp is seconds since epoch
            dt = datetime.datetime.fromtimestamp(timestamp)
            return dt.strftime('%Y-%m-%d %H:%M:%S')
        except:
            return 'Invalid timestamp'
    
    def encode_msd(self):
        try:
            # Get input
            input_text = self.encode_input.get('1.0', tk.END).strip()
            
            # Parse Python dict
            data = eval(input_text)
            
            # Encode using ASN.1 compiler
            encoded_bytes = self.compiler.encode('MSDMessage', data)
            
            # Convert to hex
            hex_output = binascii.hexlify(encoded_bytes).decode().upper()
            
            # Format hex output (add spaces every 2 characters)
            formatted_hex = ' '.join([hex_output[i:i+2] for i in range(0, len(hex_output), 2)])
            
            # Display result
            self.encode_output.delete('1.0', tk.END)
            self.encode_output.insert('1.0', f"Hex: {formatted_hex}\n\n")
            self.encode_output.insert(tk.END, f"Length: {len(encoded_bytes)} bytes\n\n")
            self.encode_output.insert(tk.END, f"Base64: {binascii.b2a_base64(encoded_bytes).decode()}")
            
        except SyntaxError as e:
            messagebox.showerror("Error", f"Invalid Python dict format: {str(e)}")
        except Exception as e:
            messagebox.showerror("Error", f"Encoding failed: {str(e)}")
    
    def clear_decode(self):
        self.decode_input.delete('1.0', tk.END)
        self.decode_output.delete('1.0', tk.END)
    
    def clear_encode(self):
        self.encode_input.delete('1.0', tk.END)
        self.encode_output.delete('1.0', tk.END)
    
    def load_example_decode(self):
        # Example hex-encoded MSD
        example = "01 23 45 67 89 AB CD EF"
        self.decode_input.delete('1.0', tk.END)
        self.decode_input.insert('1.0', example)
    
    def load_template(self):
        template = """{
    'msdStructure': {
        'messageIdentifier': 1,
        'timestamp': 1706515200,
        'control': (b'\\x00', 8),
        'vehicleIdentificationNumber': {
            'isowmi': 'ABC',
            'isovds': '123456',
            'isovisModelyear': 'T',
            'isovisSeqPlant': '0001234'
        },
        'vehicleType': (b'\\x01', 7),
        'vehicleLocation': {
            'latitudeDelta': 100,
            'longitudeDelta': -50
        },
        'vehicleDirection': 45,
        'numberOfPassengers': 2
    }
}"""
        self.encode_input.delete('1.0', tk.END)
        self.encode_input.insert('1.0', template)


def main():
    root = tk.Tk()
    app = ECallMSDApp(root)
    root.mainloop()


if __name__ == '__main__':
    main()
