#ifndef PHM_APP_H
#define PHM_APP_H

#include "phm_window.h"
#include "phm_pipeline.h"

namespace phm
{
	class Application
	{

	public:
		static constexpr size_t WIDTH = 800;
		static constexpr size_t HEIGHT = 600;

		void run();

	private:
		PhmWindow m_window{ WIDTH, HEIGHT, "Triangle" };
		PhmDevice m_device{ m_window };
		PhmPipeline m_pipeline{
			m_device,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			PhmPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
		};


	};
}



#endif /* PHM_APP_H */
