#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

std::string ComputeFileHash(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return "";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(static_cast<size_t>(size));
    if (!file.read(buffer.data(), size)) {
        return "";
    }

    std::hash<std::string_view> hasher;
    return std::to_string(hasher(std::string_view(buffer.data(), buffer.size())));
}

void EmbedResourcesRecursive(std::ofstream& output, const std::string& currentDir, const std::string& relativePath) {
    for (const auto& entry : fs::directory_iterator(currentDir)) {
        std::string fileName = entry.path().filename().string();
        std::string safeFileName = std::regex_replace(fileName, std::regex("[^a-zA-Z0-9_]"), "_");

        if (fs::is_directory(entry)) {
            output << "namespace " << safeFileName << "{";
            EmbedResourcesRecursive(output, entry.path().string(), relativePath + "/" + fileName);
            output << "}";
        } else {
            output << "static const uint8_t " << safeFileName << "[]={";

            std::ifstream file(entry.path(), std::ios::binary);
            std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            for (char c : fileContents) {
                output << "0x" << std::hex << static_cast<int>(static_cast<unsigned char>(c)) << ",";
            }

            output << "};";
        }
    }
}

void EmbedResources(const std::string& outputFile, const std::string& resourceDir, const std::string& namespaceName) {
    fs::path outputPath(outputFile);
    std::string outputFileName = outputPath.filename().string();
    std::string hashFile = (outputPath.parent_path() / outputPath.stem()).string() + ".hashes";

    std::string headerGuard = std::regex_replace(outputFileName, std::regex("[^a-zA-Z0-9_]"), "_");
    std::transform(headerGuard.begin(), headerGuard.end(), headerGuard.begin(), ::toupper);

    // Get the list of files and their hashes in the resource directory
    std::unordered_map<std::string, std::string> resourceHashes;
    for (const auto& entry : fs::recursive_directory_iterator(resourceDir)) {
        if (!fs::is_directory(entry)) {
            std::string relativeFilePath = fs::relative(entry, resourceDir).string();
            std::string fileHash = ComputeFileHash(entry.path().string());
            resourceHashes[relativeFilePath] = fileHash;
        }
    }

    // Load the last saved hashes from the hash file
    std::unordered_map<std::string, std::string> lastResourceHashes;
    if (fs::exists(hashFile)) {
        std::ifstream hashInput(hashFile);
        std::string line;
        while (std::getline(hashInput, line)) {
            size_t delim = line.find(';');
            if (delim != std::string::npos) {
                lastResourceHashes[line.substr(0, delim)] = line.substr(delim + 1);
            }
        }
    }

    // Check if any resource file has changed since the last generation
    if (resourceHashes != lastResourceHashes || !fs::exists(outputPath)) {
        std::ofstream output(outputFile);
        output << "/* * * * * * * * * * * * * * * * * * *\n"
                 " * Auto-generated file. Do not edit. *\n"
                 " * * * * * * * * * * * * * * * * * * */\n\n"
                 "#ifndef " << headerGuard << "\n"
                 "#define " << headerGuard << "\n"
                 "#include <cstdint>\n"
                 "namespace " << namespaceName << "{";

        EmbedResourcesRecursive(output, resourceDir, "");

        output << "}\n"
                 "#endif\n";

        // Save the current hashes to the hash file
        std::ofstream hashOutput(hashFile);
        for (const auto& [filePath, hash] : resourceHashes) {
            hashOutput << filePath << ";" << hash << "\n";
        }

        std::cout << "Updated resources" << std::endl;
    } else {
        std::cout << "Resources are up-to-date" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <output_file> <resource_directory> <namespace_name>" << std::endl;
        return 1;
    }

    EmbedResources(argv[1], argv[2], argv[3]);
    return 0;
}