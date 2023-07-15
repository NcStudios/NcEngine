#include "BinaryBuffer.h"

#include <fstream>

namespace nc::serialize
{
auto ReadBinaryFile(const std::filesystem::path& filePath) -> std::vector<uint8_t>
{
    if (!std::filesystem::exists(filePath))
    {
        throw NcError(fmt::format("File doesn't exist '{}'.", filePath.string()));
    }

    const auto fileSize = std::filesystem::file_size(filePath);
    auto out = std::vector<uint8_t>{};
    out.reserve(fileSize);

    auto file = std::ifstream{filePath, std::ios::binary};
    if (!file.is_open())
    {
        throw NcError(fmt::format("Could not open file '{}'.", filePath.string()));
    }

    out.insert(out.begin(),
               std::istream_iterator<char>{file},
               std::istream_iterator<char>{});
    return out;
}

void WriteBinaryFile(const std::filesystem::path& filePath, std::span<const uint8_t> bytes)
{
    auto file = std::ofstream{filePath, std::ios::binary};
    if (!file.is_open())
    {
        throw NcError(fmt::format("Could not open file '{}'.", filePath.string()));
    }

    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}
} // namespace nc::serialize
