#pragma once




namespace Renderer
{


	// A context for rendering. May have a window or target binded to it.
	class Context
	{
	public:

		Context();
		~Context();


		void Bind_Window();

		void Bind_Target();

		void Add_Scene();


	protected:

		// Binding/s (Not sure if I want to allow multiple bindings per context yet...)

	};
}
