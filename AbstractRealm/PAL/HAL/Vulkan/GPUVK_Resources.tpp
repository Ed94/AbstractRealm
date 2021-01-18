// Parent Header
// GPUVK_Resources.hpp




namespace HAL::GPU::Vulkan
{
#pragma region PrimitiveRenderable

	template<typename VertexType>
	void TPrimitiveRenderable<VertexType>::Create(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader)
	{
		vertBuffer.Create(_verticies.data(), _verticies.size(), 0);

		shader = _shader;
	}

	template<typename VertexType>
	void TPrimitiveRenderable<VertexType>::RecordRender(const CommandBuffer& _commandBuffer)
	{
		unbound DynamicArray<Buffer::Handle> handles;

		if (handles.empty())
		{
			Buffer::Handle handle = vertBuffer.GetBuffer();

			handles.push_back(handle);
		}

		_commandBuffer.BindVertexBuffers(0, 1, handles.data());

		_commandBuffer.Draw(0, 3, 0, 1);
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::AttributeDescription>&
	TPrimitiveRenderable<VertexType>::GetVertexAttributes() const
	{
		unbound DynamicArray<AttributeDescription> attributeDescriptions;

		if (attributeDescriptions.size() == 0)
		{
			for (auto& attribute : VertexType::GetAttributeDescriptions())
			{
				attributeDescriptions.push_back(attribute);
			}
		}

		return attributeDescriptions;
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::BindingDescription>&
	TPrimitiveRenderable<VertexType>::GetVertexBindings() const
	{
		unbound DynamicArray<BindingDescription> bindingDescriptions;

		if (bindingDescriptions.size() == 0)
		{
			for (auto& binding : VertexType::GetBindingDescription())
			{
				bindingDescriptions.push_back(binding);
			}
		}

		return bindingDescriptions;
	}

	template<typename VertexType>
	ptr< const AShader> TPrimitiveRenderable<VertexType>::GetShader() const
	{
		return shader;
	}

#pragma endregion PrimitiveRenderable	

#pragma region ModelRenderable

	template<typename VertexType>
	void TModelRenderable<VertexType>::Create
	(
		const DynamicArray<VertexType>& _verticies, 
		const DynamicArray<ui32> _indicies, 
		ptr<const uI8> _textureData,
		ui32 _width, ui32 _height,
		ptr<const AShader> _shader
	)
	{
		vertBuffer.Create(_verticies.data(), _verticies.size(), 0);

		indexBuffer.Create(_indicies.data(), _indicies.size(), 0);

		textureImage.Create(_textureData, _width, _height);

		shader = _shader;
	}

	template<typename VertexType>
	void TModelRenderable<VertexType>::RecordRender(const CommandBuffer& _commandBuffer)
	{
		unbound DynamicArray<Buffer::Handle> handles;

		if (handles.empty())
		{
			Buffer::Handle handle = vertBuffer.GetBuffer();

			handles.push_back(handle);
		}

		//_commandBuffer.BindVertexBuffers(0, 1, handles.data());

		//_commandBuffer.Draw(0, 3, 0, 1)

		// use index drawing...

	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::AttributeDescription>&
	TModelRenderable<VertexType>::GetVertexAttributes() const
	{
		unbound DynamicArray<AttributeDescription> attributeDescriptions;

		if (attributeDescriptions.size() == 0)
		{
			for (auto& attribute : VertexType::GetAttributeDescriptions())
			{
				attributeDescriptions.push_back(attribute);
			}
		}

		return attributeDescriptions;
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::BindingDescription>&
	TModelRenderable<VertexType>::GetVertexBindings() const
	{
		unbound DynamicArray<BindingDescription> bindingDescriptions;

		if (bindingDescriptions.size() == 0)
		{
			for (auto& binding : VertexType::GetBindingDescription())
			{
				bindingDescriptions.push_back(binding);
			}
		}

		return bindingDescriptions;
	}

	template<typename VertexType>
	ptr< const AShader> TModelRenderable<VertexType>::GetShader() const
	{
		return shader;
	}

#pragma endregion ModelRenderable

#pragma region GPU_Resources

	//template<typename VertexType>
	////Where< VulkanVertex<VertexType>(), ptr<ARenderable> > 
	//ptr<ARenderable>
	//GPU_Resources_Maker_Single::Request_Renderable<VertexType>(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader)
	//{
	//	// Hardcoding to primtive renderable for now...
	//	// Also not checking if renderable already exists.

	//	SPtr< PrimitiveRenderable> newRenderable = MakeSPtr<PrimitiveRenderable>();

	//	newRenderable->Create(_verticies, _shader);

	//	renderables.push_back(move(newRenderable));

	//	return renderables.back();
	//}

#pragma endregion GPU_Resources
}