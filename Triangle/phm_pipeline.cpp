
#include "phm_pipeline.h"

#include <fstream>
#include <stdexcept>


namespace phm
{
	PhmPipeline::PhmPipeline(const std::string& vertFilePath, const std::string& fragFilePath)
	{
        createGraphicsPipeline(vertFilePath, fragFilePath);
	}

    void PhmPipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath)
    {
        // Read in the vertex and fragment shader code.
        std::vector<char> vertCode = readFile(vertFilePath);
        std::vector<char> fragCode = readFile(fragFilePath);

    }

	std::vector<char> PhmPipeline::readFile(const std::string& filename)
	{
        // Read in binary and start at the end of the file
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        // Try to open the file
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }
         
        // Alocate a buffer of the right size
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        // Seek back to the beginning and read all of the bytes at once
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        // Close the file
        file.close();

        // Return the buffer.
        return buffer;
	}
}