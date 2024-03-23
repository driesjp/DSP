#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>

class Haar {
public:
    Haar(int scale) {
        this->scale = scale;
    }

    // Haar wavelet forward transform
    void Forward(std::vector<double>& data) {
        int N = data.size();
        std::vector<double> temp(N, 0.0);

        int h = N >> 1;
        for (int i = 0; i < h; ++i) {
            temp[i] = (data[2 * i] + data[2 * i + 1]) / 2.0;
            temp[i + h] = data[2 * i] - temp[i];
        }

        for (int i = 0; i < N; ++i) {
            data[i] = temp[i];
        }
    }

    // Haar wavelet inverse transform
    void Inverse(std::vector<double>& data) {
        int N = data.size();
        std::vector<double> temp(N, 0.0);

        int h = N >> 1;
        for (int i = 0; i < h; ++i) {
            temp[2 * i] = data[i] + data[i + h];
            temp[2 * i + 1] = data[i] - data[i + h];
        }

        for (int i = 0; i < N; ++i) {
            data[i] = temp[i];
        }
    }

private:
    int scale;
};

// Read audio data from a file
std::vector<double> ReadAudioData(const std::string& filePath) {
    std::ifstream audioFile(filePath, std::ios::binary);
    if (!audioFile.is_open()) {
        std::cerr << "Failed to open audio file: " << filePath << std::endl;
        return {};
    }

    audioFile.seekg(0, std::ios::end);
    std::streampos fileSize = audioFile.tellg();
    audioFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    audioFile.read(buffer.data(), fileSize);

    std::vector<double> audioData(fileSize / 2); // Assuming 16-bit PCM, 2 bytes per sample
    for (size_t i = 0; i < audioData.size(); ++i) {
        audioData[i] = static_cast<double>(reinterpret_cast<int16_t*>(&buffer[i * 2])[0]) / 32768.0; // Normalize to [-1, 1]. (int16_t)
    }

    return audioData;
}

// Apply wavelet transform
void Forward(std::vector<double>& data, Haar& wavelet) {
    wavelet.Forward(data);
}

// Apply inverse wavelet transform
void Inverse(std::vector<double>& data, Haar& wavelet) {
    wavelet.Inverse(data);
}

// Plot histogram of wavelet coefficients
void PlotHistogram(const std::vector<double>& data) {
    std::cout << "Histogram of Wavelet Coefficients:" << std::endl;

    // Number of bins to plot
    int numberOfBins = 100;

    // Create histogram bins
    std::vector<int> histogram(numberOfBins);
    double min = *std::min_element(data.begin(), data.end());
    double max = *std::max_element(data.begin(), data.end());

    // Populate histogram bins
    for (const auto& value : data) {
        int binIndex = static_cast<int>((value - min) / (max - min) * (numberOfBins - 1));
        histogram[binIndex]++;
    }

    // Display histogram
    for (int i = 0; i < numberOfBins; ++i) {
        std::cout << min + i * (max - min) / (numberOfBins - 1) << " - "
                  << min + (i + 1) * (max - min) / (numberOfBins - 1) << ": "
                  << histogram[i] << std::endl;
    }
}

int main() {
    // Audio file to perform calculations
    std::string audioFilePath = "d:\\samples\\acc_e3.wav";

    // Create a new Haar wavelet
    Haar wavelet(1);

    // Read audio data from the file
    std::vector<double> audioData = ReadAudioData(audioFilePath);

    // Apply the wavelet transform
    Forward(audioData, wavelet);

    // Visualize coefficients using a histogram
    PlotHistogram(audioData);

    // Apply the inverse wavelet transform
    Inverse(audioData, wavelet);

    return 0;
}