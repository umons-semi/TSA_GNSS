# filepath: tools/gen_dds_rom.py
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUT = ROOT / "dds_lut_rom.h"
LUT_BITS = 10
LUT_SIZE = 1 << LUT_BITS

def generate_dds_lut():
    with OUT.open("w", encoding="utf-8") as f:
        f.write("#ifndef DDS_LUT_ROM_H\n#define DDS_LUT_ROM_H\n\n")
        f.write('#include "acq_serial.h"\n\n')
        f.write("static const osc_t DDS_SIN_LUT[DDS_LUT_SIZE] = {\n")
        for i in range(LUT_SIZE):
            v = math.sin(2.0 * math.pi * i / LUT_SIZE)
            f.write(f"    (osc_t){v:.18f},\n")
        f.write("};\n\n")
        f.write("static const osc_t DDS_COS_LUT[DDS_LUT_SIZE] = {\n")
        for i in range(LUT_SIZE):
            v = math.cos(2.0 * math.pi * i / LUT_SIZE)
            f.write(f"    (osc_t){v:.18f},\n")
        f.write("};\n\n#endif\n")

    print(f"Generated: {OUT}")

def test_dds_lut():
    sin_lut = [math.sin(2.0 * math.pi * i / LUT_SIZE) for i in range(LUT_SIZE)]

    for i in range(LUT_SIZE):
        angle = 2.0 * math.pi *i /LUT_SIZE
        adx = (angle * LUT_SIZE /(2.0 * math.pi))
        idx = int(adx+0.01)
        if i == idx:
            print(f"index {i = } {adx = }   {idx = }   lue {sin_lut[i]:.6f}  {math.sin(angle):.6f}")
            
        if sin_lut[idx] != math.sin(angle):
            print(f"Error at index {i = } {idx = }, LUT value {sin_lut[idx]:.6f} does not match expected {math.sin(angle):.6f}")
            return False
    return True


    const angle_t angle0 = (angle_t)(-((FREQUENCE_CENTRALE + FD_START) * TWO_PI_DIV_FS));
    

if __name__ == "__main__":
    # generate_dds_lut()
    if test_dds_lut():
        print("DDS LUT test passed.")
    else:
        print("DDS LUT test failed.")   
