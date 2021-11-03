#ifndef PHM_APP_H
#define PHM_APP_H

#include <memory>
#include <vector>

#include "phm_window.h"
#include "phm_renderer.h"
#include "phm_object.h"


namespace phm
{
	class Application
	{

	public:
		static constexpr size_t WIDTH = 800;
		static constexpr size_t HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		PhmWindow window_{ WIDTH, HEIGHT, "Triangle" };
		PhmDevice device_{ window_ };
		PhmRenderer renderer_{ window_, device_ };

		std::vector<PhmObject> objects_; // TEMP

		void loadObjects(); // TEMP
	};
}



#endif /* PHM_APP_H */
