#pragma once

#include "GPUVK_Memory.hpp"
#include "GPUVK_Shaders.hpp"
#include "GPUVK_Traits.hpp"


namespace HAL::GPU::Vulkan
{
	class Swapchain;

	// Classes

	class Buffer : public V3::Buffer
	{
	public:
		using Parent = V3::Buffer;

		Buffer() : Parent::Buffer()
		{}

		Buffer(const LogicalDevice& _device) : Parent::Buffer(_device)
		{}

		Buffer(const LogicalDevice& _device, const Memory::AllocationCallbacks& _allocator) :
			Parent::Buffer(_device, _allocator)
		{}

		EResult Create(const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

		ForceInline
		DeviceSize GetSize() const 
		{ 
			return info.Size; 
		}

	protected:

		CreateInfo info;
	};

	class Image : public V3::Image
	{
	public:

		using Parent = V3::Image;

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

		Extent3D GetExtent() const;

		EFormat GetFormat() const;

		u32 GetMipmapLevels() const;

		bool HasStencilComponent() const;

		void TransitionLayout(EImageLayout _old, EImageLayout _new);

		//operator Parent&();

	protected:

		CreateInfo info;
	};

	class ImageView : public V3::ImageView
	{
	public:

		using Parent = V3::ImageView;

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info);

		EResult Create(const LogicalDevice& _device, const CreateInfo& _info, const Memory::AllocationCallbacks& _allocator);

	protected:

		CreateInfo info;
	};

	class BufferPackage
	{
	public:

		BufferPackage();

		BufferPackage(const LogicalDevice& _device);

		BufferPackage(const LogicalDevice& _device, const Memory& _allocator);

		~BufferPackage();

		void Create(const Buffer::CreateInfo& _bufferInfo, Memory::PropertyFlags _memoryFlags);

		void Destroy();

		const Buffer&     GetBuffer();
		const Memory&     GetMemory();
		const DeviceSize& GetMemoryOffset();
		const BufferView& GetView();

		void WriteToGPU();

	protected:

		Buffer            buffer      ;
		ptr<const Memory> memory      ;
		DeviceSize        memoryOffset;
		BufferView        view        ;
	};

	class ImagePackage
	{
	public:
		Extent3D GetExtent() const;

		EFormat GetFormat() const;

		u32 GetMipmapLevels() const;

		Image      image ;
		Memory     memory;
		DeviceSize memoryOffset;
		ImageView  view  ;
	};

	struct VertexInputState : Pipeline::VertexInputState
	{
		DynamicArray<AttributeDescription> Attributes;
		BindingDescription                 Bind      ;
	};

	class VertexBuffer
	{
	public:

		EResult Create(ptr<const void> data, DeviceSize _dataSize, DeviceSize _stride);

		void Destroy();

		const Buffer& GetBuffer() const;

	protected:

		Buffer     buffer      ;
		Memory     memory      ;
		DeviceSize memoryOffset;
	};

	class IndexBuffer
	{
	public:

		EResult Create(ptr<const void> data, DeviceSize _dataSize, DeviceSize _stride);

		void Destroy();

		const Buffer& GetBuffer() const;

		ForceInline
		DeviceSize GetBufferSize() const
		{
			return buffer.GetSize();
		}

		u32 GetSize() const;

	protected:

		Buffer     buffer      ;
		Memory     memory      ;
		DeviceSize memoryOffset;

		u32 indices;
	};

	class TextureImage : public ImagePackage
	{
	public:

		EResult Create(ptr<const u8> _imageData, u32 _width, u32 _height);

		void Destroy();

		const ImageView& GetView() const;

		const Sampler& GetSampler() const;

	protected:

		void CreateImageView();

		void CreateSampler();

		void GenerateMipmaps();

		ImageView imageView;

		Sampler sampler;
	};

	class UniformBuffer
	{
	public:

		EResult Create(DeviceSize _bufferSize);

		void Destroy();

		void Write(ptr<const void> _data);

		const Buffer& GetBuffer() const;

		const Memory& GetMemory() const;

	protected:

		Buffer buffer;
		Memory memory;
	};

	struct Vertex_WColor
	{
		using AttributeDescription = Pipeline::VertexInputState::AttributeDescription;
		using BindingDescription   = Pipeline::VertexInputState::BindingDescription;

		using VertexAttributes = StaticArray<AttributeDescription, 3>;
		using VertexBindings   = StaticArray<BindingDescription, 1>;

		struct PositionT
		{
			f32 X, Y, Z;
			//f32 X, Y;
		} 

		Position;

		struct ColorT
		{
			f32 R,G,B;
		}

		Color;

		struct CoordT
		{
			f32 X, Y;
		}

		TextureCoordinates;


		static constexpr VertexAttributes GetAttributeDescriptions()
		{
			VertexAttributes result{};

			// Position Attributes

			AttributeDescription& posAttrib = result.at(0);

			posAttrib.Binding  = 0;
			posAttrib.Location = 0;
			posAttrib.Format   = EFormat::R32_G32_B32_SFloat;
			//posAttrib.Format   = EFormat::R32_G32_SFloat;
			posAttrib.Offset   = (u32) OffsetOf(Vertex_WColor::Position);

			// Color Attributes

			AttributeDescription& colorAttrib = result.at(1);

			colorAttrib.Binding  = 0;
			colorAttrib.Location = 1;
			colorAttrib.Format   = EFormat::R32_G32_B32_SFloat;
			colorAttrib.Offset   = (u32) OffsetOf(Vertex_WColor::Color);

			// Texture Coordinate Attributes

			AttributeDescription& texCoordAttrib = result.at(2);

			texCoordAttrib.Binding  = 0;
			texCoordAttrib.Location = 2;
			texCoordAttrib.Format   = EFormat::R32_G32_SFloat;
			texCoordAttrib.Offset   = (u32) OffsetOf(Vertex_WColor::TextureCoordinates);

			return result;
		}

		static constexpr VertexBindings GetBindingDescription()
		{
			VertexBindings result {};

			result[0].Binding   = 0;
			result[0].Stride    = sizeof(Vertex_WColor);
			result[0].InputRate = EVertexInputRate::Vertex;

			return result;
		}
	};

	class ARenderable
	{
	public:

		using AttributeDescription = Pipeline::VertexInputState::AttributeDescription;
		using BindingDescription   = Pipeline::VertexInputState::BindingDescription;

		virtual ~ARenderable() {};

		virtual void Destroy() = NULL;

		virtual void RecordRender(u32 _index, const CommandBuffer& _commandBuffer, const PipelineLayout& _pipelineLayout) = NULL;

		virtual DynamicArray<AttributeDescription>& GetVertexAttributes() const = NULL;

		virtual DynamicArray<BindingDescription>& GetVertexBindings() const = NULL;

		virtual ptr<const AShader> GetShader() const = NULL;

		virtual ptr<const DescriptorSetLayout> GetDescriptorsLayout() const = NULL;

		virtual void CreateDescriptorSets(u32 _count, const DescriptorPool& _descriptorPool) = NULL;

		//virtual void CreateUniforms

		virtual void UpdateUniforms(ptr<const void> _data, DeviceSize _size) = NULL;
	};

	template<typename VertexType>
	class TPrimitiveRenderable : public ARenderable
	{
	public:

		EnforceConstraint(VulkanVertex<VertexType>(), "VertexType must be of vulkan vertex type.");

		~TPrimitiveRenderable() 
		{
			Destroy();
		}

		using Vertex = VertexType;

		void Create(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader);

		void RecordRender(const CommandBuffer& _commandBuffer);

#pragma region ARenderable
		virtual void Destroy() override;

		virtual void RecordRender(u32 _index, const CommandBuffer& _commandBuffer, const PipelineLayout& _pipelineLayout) override;

		virtual DynamicArray<AttributeDescription>& GetVertexAttributes() const override;

		virtual DynamicArray<BindingDescription>& GetVertexBindings() const override;

		virtual ptr<const AShader> GetShader() const override;

		virtual ptr<const DescriptorSetLayout> GetDescriptorsLayout() const override;
#pragma endregion ARenderable

	protected:

		DynamicArray<Buffer::Handle> handles;

		DynamicArray<AttributeDescription> attributeDescriptions;

		DynamicArray<BindingDescription> bindingDescriptions;

		VertexBuffer vertBuffer;

		ptr<const AShader> shader;
	};

	using PrimitiveRenderable = TPrimitiveRenderable<Vertex_WColor>;

	template<typename VertexType>
	class TModelRenderable : public ARenderable
	{
	public:

		EnforceConstraint(VulkanVertex<VertexType>(), "VertexType must be of vulkan vertex type.");

		using Vertex = VertexType;

		~TModelRenderable()
		{
			Destroy();
		}

		void Create
		(
			const DynamicArray<VertexType>& _verticies, 
			const DynamicArray<u32>&        _indicies,
			      ptr<const u8>             _textureData,
			      u32                       _width, 
			      u32                       _height,
			      ptr<const AShader>        _shader
		);

#pragma region ARenderable
		virtual void Destroy() override;

		void RecordRender(u32 _index, const CommandBuffer& _commandBuffer, const PipelineLayout& _pipelineLayout) override;

		DynamicArray<AttributeDescription>& GetVertexAttributes() const override;

		DynamicArray<BindingDescription>& GetVertexBindings() const override;

		ptr<const AShader> GetShader() const override;

		ptr<const DescriptorSetLayout> GetDescriptorsLayout() const override;

		void CreateDescriptorSets(u32 _count, const DescriptorPool& _descriptorPool) override;

		//void CreateUniforms()

		void UpdateUniforms(ptr<const void> _data, DeviceSize _size) override;
#pragma endregion ARenderable

	protected:

		DynamicArray<Buffer::Handle> handles;

		// Attributes

		DynamicArray<AttributeDescription> attributeDescriptions;

		DynamicArray<BindingDescription> bindingDescriptions;

		void CreateDescriptorsLayout();

		VertexBuffer vertBuffer;

		IndexBuffer indexBuffer;

		DynamicArray<byte> uniformData;

		DynamicArray<UniformBuffer> uniformBuffers;

		TextureImage textureImage;

		DynamicArray<DescriptorSet> descriptors;
												
		//ptr<const DescriptorSetLayout> descriptorsLayout;

		DescriptorSetLayout descriptorsLayout;

		ptr<const AShader> shader;
	};

	//using ModelRenderable = TPrimitiveRenderable<Vertex_WTexture>;


	template<Meta::EGPU_Engage>
	class Resources_Maker;

	template<>
	class Resources_Maker<Meta::EGPU_Engage::Single>
	{
	public:

		template<typename VertexType>
		static Where< VulkanVertex<VertexType>(), ptr<ARenderable> >
		Request_Renderable(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader)
		{
			SPtr< TPrimitiveRenderable<VertexType>> newRenderable = Make_SPtr< TPrimitiveRenderable<VertexType>>();

			newRenderable->Create(_verticies, _shader);

			renderables.push_back(move(newRenderable));

			return renderables.back().get();
		}

		template<typename VertexType>
		static Where< VulkanVertex<VertexType>(), ptr<ARenderable> >
		Request_Renderable
		(
			const DynamicArray<VertexType>& _verticies, 
			const DynamicArray<u32>&        _indicies, 
			      ptr<const u8>             _textureData,
			      u32                       _width, 
			      u32                       _height,
			      ptr<const AShader>        _shader
		)
		{
			SPtr< TModelRenderable<VertexType>> newRenderable = Make_SPtr< TModelRenderable<VertexType>>();

			newRenderable->Create(_verticies, _indicies, _textureData, _width, _height, _shader);

			renderables.push_back(move(newRenderable));

			return renderables.back().get();
		}

		static 
		void Clear();

		//// Just doing a raw one for now...
		//static ptr<DescriptorPool> Request_DescriptorPool()
		//{
		//	descriptorPools.push_back(DescriptorPool());

		//	return getPtr(descriptorPools.back());
		//}

		//static ptr<DescriptorSetLayout> Request_DescriptorSetLayout()
		//{

		//}

		//static ptr<DescriptorSet> Request_DescriptorSet()
		//{

		//}



	private:

		//static DescriptorPool descriptorPool;

	/*	static DynamicArray<DescriptorPool> descriptorPools;

		static DynamicArray<DescriptorSetLayout> descriptorSetLayouts;

		static DynamicArray<DescriptorSet> descriptorSets;*/

		static DynamicArray< SPtr<ARenderable> > renderables;
	};

	using Resources = Resources_Maker<Meta::GPU_Engagement>;
}


// 	Template Implementation

#include "GPUVK_Resources.tpp"
