import numpy as np
import matplotlib.pyplot as plt
from scipy.io.wavfile import write

frequences = [100,750,1300,1850,2200,2750,3300,3850,4400,5500]
fs = 2 * max(frequences) + 100

x = np.arange(fs)
signal=0
amplitudes=[1000,5500,2000,3600,9880,1000,8880,6000,1100,7800]

j = 0
for i in range(len (frequences)):
    y1 = np.sin(2 * np.pi * frequences[i] * (x/fs))
    signal = signal+3000*y1
    j = j + 1

with open("Core/Inc/input_signal.h", "w") as f:

    f.write(f"#include <stdint.h>\n")

    f.write("#ifndef SIGNAL_DATA_H\n")
    f.write("#define SIGNAL_DATA_H\n\n")
    f.write("static const uint16_t signal_data[] = {")

    for value in signal.astype(np.int16):
        f.write(f"{value}, ")
    
    f.write("};\n\n")

    f.write(f"#define SIGNAL_LENGTH {len(signal)}\n")
    f.write(f"#define SAMPLE_RATE {fs}\n\n")
    f.write("#endif // SIGNAL_DATA_H\n")

print("Signal data written to signal_data.h")

t = np.linspace(0., 1., fs)
write("generated.wav", fs, signal.astype(np.int16))

total_ts_sec = len(signal)/fs
print("The total time series length = {} sec (N points = {}) ".format(total_ts_sec, len(signal)))
plt.figure(figsize=(20,3))
plt.plot(signal)
plt.xticks(np.arange(0,len(signal),fs),
        np.arange(0,len(signal)/fs,1))
plt.ylabel("Amplitude")
plt.xlabel("Time (second)")
plt.show()

