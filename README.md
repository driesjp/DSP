# HaarWavelet.cpp

- Specifies the path to an audio file.
- Creates an instance of the Haar class for wavelet transformations.
- Reads audio data from the specified file.
- Applies the forward Haar wavelet transform to the audio data.
- Plots a histogram of the wavelet coefficients to visualize their distribution.
- Applies the inverse Haar wavelet transform to convert the data back to its original form.

- It does not automatically compensate for the sample rate and bit depth yet, it assumes 44.1khz, 16-bit.