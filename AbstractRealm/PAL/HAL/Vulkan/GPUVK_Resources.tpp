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
	void TPrimitiveRenderable<VertexType>::Destroy()
	{
		vertBuffer.Destroy();
	}

	template<typename VertexType>
	void TPrimitiveRenderable<VertexType>::RecordRender(u32 _index, const CommandBuffer& _commandBuffer, const PipelineLayout& _pipelineLayout)
	{
		
	}

	template<typename VertexType>
	void TPrimitiveRenderable<VertexType>::RecordRender(const CommandBuffer& _commandBuffer)
	{
		_commandBuffer.BindVertexBuffers(0, 1, vertBuffer.GetBuffer().operator const Buffer::Handle*());

		_commandBuffer.Draw(0, 3, 0, 1);
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::AttributeDescription>& 
	TPrimitiveRenderable<VertexType>::GetVertexAttributes() const
	{
		static DynamicArray<AttributeDescription> _AttributeDescriptions;

		if (_AttributeDescriptions.size() == 0)
		{
			for (auto& attribute : VertexType::GetAttributeDescriptions())
			{
				_AttributeDescriptions.push_back(attribute);
			}
		}

		return _AttributeDescriptions;
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::BindingDescription>&
	TPrimitiveRenderable<VertexType>::GetVertexBindings() const
	{
		static DynamicArray<BindingDescription> _BindingDescriptions;

		if (_BindingDescriptions.size() == 0)
		{
			for (auto& binding : VertexType::GetBindingDescription())
			{
				_BindingDescriptions.push_back(binding);
			}
		}

		return _BindingDescriptions;
	}

	template<typename VertexType>
	ptr<const AShader> TPrimitiveRenderable<VertexType>::GetShader() const
	{
		return shader;
	}

	template<typename VertexType>
	ptr<const DescriptorSetLayout> TPrimitiveRenderable<VertexType>::GetDescriptorsLayout() const
	{
		return nullptr;
	}

#pragma endregion PrimitiveRenderable	

#pragma region ModelRenderable

	// Public

	template<typename VertexType>
	void TModelRenderable<VertexType>::Create
	(
		const DynamicArray<VertexType>& _verticies, 
		const DynamicArray<u32>&        _indicies, 
		      ptr<const u8>             _textureData,
		      u32                       _width,
		      u32                       _height,
		      ptr<const AShader>        _shader
	)
	{
		vertBuffer.Create(_verticies.data(), _verticies.size(), 0);

		indexBuffer.Create(_indicies.data(), _indicies.size(), 0);

		textureImage.Create(_textureData, _width, _height);

		shader = _shader;

		CreateDescriptorsLayout();

		uniformData.resize(shader->GetUniformSize());
	}

	template<typename VertexType>
	void TModelRenderable<VertexType>::Destroy()
	{
		vertBuffer.Destroy();
		indexBuffer.Destroy();
		textureImage.Destroy();

		for (auto& uniformBuffer : uniformBuffers)
		{
			uniformBuffer.Destroy();
		}

		descriptorsLayout.Destroy();
	}

	template<typename VertexType>
	void TModelRenderable<VertexType>::RecordRender(u32 _index, const CommandBuffer& _commandBuffer, const PipelineLayout& _pipelineLayout)
	{
		DeviceSize offsets = 0;

		_commandBuffer.BindVertexBuffers(0, 1, vertBuffer.GetBuffer().operator const Buffer::Handle*(), &offsets);

		_commandBuffer.BindIndexBuffer(indexBuffer.GetBuffer(), 0, EIndexType::uInt32);

		uniformBuffers[_index].Write(uniformData.data());

		_commandBuffer.BindDescriptorSets
		(
			EPipelineBindPoint::Graphics,
			_pipelineLayout,
			0,
			1,
			descriptors[_index]
		);

		_commandBuffer.DrawIndexed
		(
			SCast<u32>(indexBuffer.GetSize()),
			1,
			0,
			0,
			0
		);
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::AttributeDescription>&
	TModelRenderable<VertexType>::GetVertexAttributes() const
	{
		static DynamicArray<AttributeDescription> _AttributeDescriptions;

		if (_AttributeDescriptions.size() == 0)
		{
			for (auto& attribute : VertexType::GetAttributeDescriptions())
			{
				_AttributeDescriptions.push_back(attribute);
			}
		}

		return _AttributeDescriptions;
	}

	template<typename VertexType>
	DynamicArray< Pipeline::VertexInputState::BindingDescription>&
	TModelRenderable<VertexType>::GetVertexBindings() const
	{
		static DynamicArray<BindingDescription> _BindingDescriptions;

		if (_BindingDescriptions.size() == 0)
		{
			for (auto& binding : VertexType::GetBindingDescription())
			{
				_BindingDescriptions.push_back(binding);
			}
		}

		return _BindingDescriptions;
	}

	template<typename VertexType>
	ptr< const AShader> TModelRenderable<VertexType>::GetShader() const
	{
		return shader;
	}

	template<typename VertexType>
	ptr<const DescriptorSetLayout> TModelRenderable<VertexType>::GetDescriptorsLayout() const
	{
		return getPtr(descriptorsLayout);
	}

	template<typename VertexType>
	void TModelRenderable<VertexType>::CreateDescriptorSets(u32 _count, const DescriptorPool& _descriptorPool)
	{
		uniformBuffers.resize(_count);

		for (auto& uniformBuffer : uniformBuffers)
		{
			uniformBuffer.Create(shader->GetUniformSize());
		}

		DynamicArray<DescriptorSetLayout::Handle> layouts (_count, descriptorsLayout);

		DescriptorPool::AllocateInfo allocInfo;

		allocInfo.DescriptorPool     = _descriptorPool;
		allocInfo.DescriptorSetCount = _count;
		allocInfo.SetLayouts         = layouts.data();

		if (_descriptorPool.Allocate(allocInfo, descriptors) != EResult::Success)
		{
			Exception::Fatal::Throw("Failed to allocate descriptor sets.");
		}

		for (uDM index = 0; index < _count; index++)
		{
			DescriptorSet::BufferInfo bufferInfo;

			auto& buffer = uniformBuffers[index].GetBuffer();

			bufferInfo.Buffer = buffer;
			bufferInfo.Offset = 0      ;
			bufferInfo.Range  = buffer.GetSize();


			DescriptorSet::ImageInfo imageInfo{};

			imageInfo.ImageLayout = EImageLayout::Shader_ReadonlyOptimal;
			imageInfo.ImageView   = textureImage.GetView();
			imageInfo.Sampler     = textureImage.GetSampler();


			StaticArray<DescriptorSet::Write, 2> descriptorWrites;

			descriptorWrites[0].DstSet          = descriptors[index];
			descriptorWrites[0].DstBinding      = 0                    ;
			descriptorWrites[0].DstArrayElement = 0                    ;

			descriptorWrites[0].DescriptorType  = EDescriptorType::UniformBuffer;
			descriptorWrites[0].DescriptorCount = 1                             ;

			descriptorWrites[0].BufferInfo      = getPtr(bufferInfo);
			descriptorWrites[0].ImageInfo       = nullptr    ; // Optional
			descriptorWrites[0].TexelBufferView = nullptr    ; // Optional

			descriptorWrites[1].DstSet          = descriptors[index]          ;
			descriptorWrites[1].DstBinding      = 1                              ;
			descriptorWrites[1].DstArrayElement = 0                              ;

			descriptorWrites[1].DescriptorType  = EDescriptorType::CombinedImageSampler;
			descriptorWrites[1].DescriptorCount = 1                                    ;

			descriptorWrites[1].BufferInfo      = nullptr   ;
			descriptorWrites[1].ImageInfo       = getPtr(imageInfo); // Optional
			descriptorWrites[1].TexelBufferView = nullptr   ; // Optional

			descriptors[index].Update(SCast<u32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);	
		}
	}

	template<typename VertexType>
	void TModelRenderable<VertexType>::UpdateUniforms(ptr<const void> _data, DeviceSize _size)
	{
		ptr<const byte> dataBytes = RCast<const byte>(_data);

		ptr<const byte> endAddress = dataBytes + _size;

		uniformData.assign(dataBytes, endAddress);
	}

	// Private

	template<typename VertexType>
	void TModelRenderable<VertexType>::CreateDescriptorsLayout()
	{
		DescriptorSetLayout::Binding uboLayoutBinding;

		uboLayoutBinding.BindingID = 0;
		uboLayoutBinding.Type      = EDescriptorType::UniformBuffer;
		uboLayoutBinding.Count     = 1;

		uboLayoutBinding.StageFlags = EShaderStageFlag::Vertex;

		uboLayoutBinding.ImmutableSamplers = nullptr;

		DescriptorSetLayout::Binding samplerLayoutBinding;

		samplerLayoutBinding.BindingID = 1;
		samplerLayoutBinding.Count     = 1;
		samplerLayoutBinding.Type      = EDescriptorType::CombinedImageSampler;

		samplerLayoutBinding.ImmutableSamplers = nullptr;

		samplerLayoutBinding.StageFlags.Set(EShaderStageFlag::Fragment);

		StaticArray<DescriptorSetLayout::Binding, 2> bindings = 
		{ uboLayoutBinding, samplerLayoutBinding };

		DescriptorSetLayout::CreateInfo layoutInfo;

		layoutInfo.BindingCount = SCast<ui32>(bindings.size());
		layoutInfo.Bindings     = bindings.data();

		if (descriptorsLayout.Create(Comms::GetEngagedDevice(), layoutInfo) != EResult::Success)
			Exception::Fatal::Throw("Failed to create descriptor set layout.");
	}

#pragma endregion ModelRenderable

#pragma region Resources

	//template<typename VertexType>
	////Where< VulkanVertex<VertexType>(), ptr<ARenderable> > 
	//ptr<ARenderable>
	//GPU_Resources_Maker_Single::Request_Renderable<VertexType>(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader)
	//{
	//	// Hardcoding to primtive renderable for now...
	//	// Also not checking if renderable already exists.

	//	SPtr< PrimitiveRenderable> newRenderable = MakeShared<PrimitiveRenderable>();

	//	newRenderable->Create(_verticies, _shader);

	//	renderables.push_back(move(newRenderable));

	//	return renderables.back();
	//}

#pragma endregion Resources
}