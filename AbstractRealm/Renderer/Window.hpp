#pragma once




#include "OSAL_Windowing.hpp"





namespace Renderer
{
	class Window
	{
		 Window();
		~Window();


	protected:


		void On_SizeChanged();
		

		OSAL::FrameBufferDimensions dimensions;
	};
}
