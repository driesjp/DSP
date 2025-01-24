#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <stdexcept>

class Haar {
public:
    explicit Haar(int decompositionLevels) : levels(decompositionLevels) {
        if (decompositionLevels <= 0) {
            throw std::invalid_argument("Decomposition levels must be positive");
        }
    }

    void Forward(std::vector<double>& data) {
        validateDataSize(data.size());

        int N = data.size();
        for (int level = 0; level < levels; ++level) {
            int currentSize = N >> level;
            int halfSize = currentSize >> 1;

            std::vector<double> temp(currentSize);
            for (int i = 0; i < halfSize; ++i) {
                // Normalization using sqrt(2)
                temp[i] = (data[2 * i] + data[2 * i + 1]) / std::sqrt(2.0);
                temp[i + halfSize] = (data[2 * i] - data[2 * i + 1]) / std::sqrt(2.0);
            }

            std::copy(temp.begin(), temp.end(), data.begin());
        }
    }

    void Inverse(std::vector<double>& data) {
        validateDataSize(data.size());

        int N = data.size();
        for (int level = levels - 1; level >= 0; --level) {
            int currentSize = N >> level;
            int halfSize = currentSize >> 1;

            std::vector<double> temp(currentSize);
            for (int i = 0; i < halfSize; ++i) {
                // Normalization using sqrt(2)
                temp[2 * i] = (data[i] + data[i + halfSize]) / std::sqrt(2.0);
                temp[2 * i + 1] = (data[i] - data[i + halfSize]) / std::sqrt(2.0);
            }

            std::copy(temp.begin(), temp.end(), data.begin());
        }
    }

private:
    int levels;

    static void validateDataSize(size_t size) {
        if (size == 0 || (size & (size - 1)) != 0) {
            throw std::invalid_argument("Data size must be a power of 2");
        }
    }
};

void PlotHistogram(const std::vector<double>& data, int bins = 50) {
    if (data.empty()) return;

    auto [minVal, maxVal] = std::minmax_element(data.begin(), data.end());
    double range = *maxVal - *minVal;

    std::vector<int> histogram(bins, 0);
    for (const double& value : data) {
        int bin = static_cast<int>((value - *minVal) * (bins - 1) / range);
        histogram[bin]++;
    }

    int maxCount = *std::max_element(histogram.begin(), histogram.end());
    const int maxWidth = 50;

    std::cout << "\nWavelet Coefficients Histogram:\n";
    for (int i = 0; i < bins; ++i) {
        double binStart = *minVal + (range * i / bins);
        int barWidth = static_cast<int>(static_cast<double>(histogram[i]) * maxWidth / maxCount);

        printf("%6.2f: %s (%d)\n", binStart, std::string(barWidth, '*').c_str(), histogram[i]);
    }
}

std::vector<double> ReadAudioData(const std::string& filePath) {
    std::ifstream audioFile(filePath, std::ios::binary);
    if (!audioFile) {
        throw std::runtime_error("Failed to open audio file: " + filePath);
    }

    audioFile.seekg(0, std::ios::end);
    std::streampos fileSize = audioFile.tellg();
    audioFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    audioFile.read(buffer.data(), fileSize);

    std::vector<double> audioData(fileSize / 2);
    for (size_t i = 0; i < audioData.size(); ++i) {
        audioData[i] = static_cast<double>(reinterpret_cast<int16_t*>(&buffer[i * 2])[0]) / 32768.0;
    }
    return audioData;
}

void Forward(std::vector<double>& data, Haar& wavelet) {
    wavelet.Forward(data);
}

void Inverse(std::vector<double>& data, Haar& wavelet) {
    wavelet.Inverse(data);
}

int main() {
    try {
        std::string audioFilePath = "d:\\samples\\acc_e3.wav";

        // Multiple levels of decomposition
        Haar wavelet(3);

        std::vector<double> audioData = ReadAudioData(audioFilePath);

        // Pad to next power of 2
        size_t originalSize = audioData.size();
        size_t paddedSize = 1;
        while (paddedSize < originalSize) {
            paddedSize *= 2;
        }
        audioData.resize(paddedSize, 0.0);

        std::cout << "Original data size: " << originalSize
                  << ", Padded data size: " << paddedSize << std::endl;

        Forward(audioData, wavelet);

        PlotHistogram(audioData);

        Inverse(audioData, wavelet);

        // Truncate back to original size
        audioData.resize(originalSize);

        std::cout << "Inverse transform completed." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}