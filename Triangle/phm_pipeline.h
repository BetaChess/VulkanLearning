#ifndef PHM_PIPELINE_H
#define PHM_PIPELINE_H

#include <string>
#include <vector>

namespace phm
{
	class PhmPipeline
	{
	public:
		// Constructor(s)
		PhmPipeline(const std::string& vertFilePath, const std::string& fragFilePath);


	private:
		static std::vector<char> readFile(const std::string& filename);


		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);

	};

}


#endif /* PHM_PIPELINE_H */
