from fpdf import FPDF

class PDF(FPDF):
    def header(self):
        pass  # No header needed as per the instruction

    def add_product(self, sku, name, qty, price, total):
        self.set_font("Arial", size=8)
        self.cell(30, 6, sku, ln=0)
        self.cell(80, 6, name, ln=0,)
        self.cell(20, 6, f"Qty: {qty}", ln=0)
        self.cell(30, 6, f"BDT {price}", ln=0)
        self.cell(30, 6, f"BDT {total}", ln=1)

pdf = PDF()
pdf.add_page()

# Product list
products = [
    ("1162", "15G Solid Rosin Welding Soldering Flux Paste High Purity", 1, 10, 10),
    ("2027", "2 Pin Pitch 5.0mm Straight Pin Screw PCB Terminal Block Connector Blue", 2, 8, 16),
    ("1041", "5V LED Traffic Light Module", 1, 65, 65),
    ("1397", "Arduino Mega 2560 CH340", 1, 1345, 1345),
    ("1713", "BAKU BK-150 SOLDERING PASTE", 1, 60, 60),
    ("3072", "Desoldering Braid Solder Remover Wick 2mm", 1, 90, 90),
    ("1658", "Double Sided FR-4 PCB Prototype Board 4x6cm", 2, 50, 100),
    ("2044", "ESP32-CAM-MB MICRO USB Download Module for ESP32 CAM Development Board", 1, 220, 220)
]

for product in products:
    pdf.add_product(*product)

# Save PDF
pdf.output("receipt.pdf")
print("Receipt generated as receipt.pdf")
