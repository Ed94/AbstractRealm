#pragma once


#include "GPUVK_Memory.hpp"
//#include "GPUVK_Renderer.hpp"
#include "GPUVK_Shaders.hpp"



namespace HAL::GPU::Vulkan
{
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

		ui32 GetMipmapLevels() const;

		bool HasStencilComponent() const;

		void TransitionLayout(EImageLayout _old, EImageLayout _new);

		operator Parent&();

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

		      Buffer     buffer      ;
		const Memory*    memory      ;
		      DeviceSize memoryOffset;
		      BufferView view        ;
	};

	class ImagePackage
	{
	public:
		Extent3D GetExtent() const;

		EFormat GetFormat() const;

		ui32 GetMipmapLevels() const;

		Image      image ;
		Memory     memory;
		//Memory*    memory;
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

	protected:

		Buffer     buffer      ;
		Memory     memory      ;
		DeviceSize memoryOffset;

		DynamicArray<ui32> indices;
	};

	class TextureImage : public ImagePackage
	{
	public:

		EResult Create(ptr<const uI8> _imageData, ui32 _width, ui32 _height);

		void Destroy();

	protected:

		void CreateImageView();

		void CreateSampler();

		void GenerateMipmaps();

		ImageView imageView;

		Sampler sampler;
	};


	template<typename Type, typename = void>
	struct IsVulkanVertex : std::false_type
	{};

	template<typename Type>
	/**
	@brief Will be defined with a true_type when Type has the function.
	*/
	struct IsVulkanVertex<Type, decltype(static_cast<void>(Type::GetAttributeDescriptions))> : IsClassType<Type>
	{};

	template <typename Type>
	/**
	@brief Returns true if IsVulkanVertex is false.
	*/
	constexpr Where<IsVulkanVertex<Type>::value, 
	bool> VulkanVertex() noexcept
	{
		return static_cast< ptr<void>>(Type::GetAttributeDescriptions) != nullptr ? true : false;
	}

	template<typename Type>
	constexpr Where<! IsVulkanVertex<Type>::value, 
	bool> VulkanVertex() noexcept
	{
		return false;
	}

	struct Vertex_WColor
	{
		using AttributeDescription = Pipeline::VertexInputState::AttributeDescription;
		using BindingDescription   = Pipeline::VertexInputState::BindingDescription;

		using VertexAttributes = StaticArray<AttributeDescription, 2>;
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

		/*struct CoordT
		{
			f32 X, Y;
		}

		TextureCoordinates;*/


		static constexpr VertexAttributes GetAttributeDescriptions()
		{
			VertexAttributes result{};

			// Position Attributes

			AttributeDescription& posAttrib = result.at(0);

			posAttrib.Binding  = 0;
			posAttrib.Location = 0;
			posAttrib.Format   = EFormat::R32_G32_B32_SFloat;
			//posAttrib.Format   = EFormat::R32_G32_SFloat;
			posAttrib.Offset   = (ui32) OffsetOf(Vertex_WColor::Position);

			// Color Attributes

			AttributeDescription& colorAttrib = result.at(1);

			colorAttrib.Binding  = 0;
			colorAttrib.Location = 1;
			colorAttrib.Format   = EFormat::R32_G32_B32_SFloat;
			colorAttrib.Offset   = (ui32) OffsetOf(Vertex_WColor::Color);

			// Texture Coordinate Attributes

			/*AttributeDescription& texCoordAttrib = result.at(2);

			texCoordAttrib.Binding  = 0;
			texCoordAttrib.Location = 2;
			texCoordAttrib.Format   = EFormat::R32_G32_SFloat;
			texCoordAttrib.Offset   = (ui32) OffsetOf(Vertex::TextureCoordinates);*/

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

		~ARenderable() {};

		virtual void RecordRender(const CommandBuffer& _commandBuffer) = NULL;

		virtual DynamicArray<AttributeDescription>& GetVertexAttributes() const = NULL;

		virtual DynamicArray<BindingDescription>& GetVertexBindings() const = NULL;

		virtual ptr< const AShader> GetShader() const = NULL;
	};

	

	template<typename VertexType>
	class TPrimitiveRenderable : public ARenderable
	{
	public:

		EnforceConstraint(VulkanVertex<VertexType>(), "VertexType must be of vulkan vertex type.");

		using Vertex = VertexType;

		void Create(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader);

		implem void RecordRender(const CommandBuffer& _commandBuffer);

		implem DynamicArray<AttributeDescription>& GetVertexAttributes() const;

		implem DynamicArray<BindingDescription>& GetVertexBindings() const;

		implem ptr< const AShader> GetShader() const;

	protected:

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

		void Create
		(
			const DynamicArray<VertexType>& _verticies, 
			const DynamicArray<ui32> _indicies,
			ptr<const uI8> _textureData,
			ui32 _width, ui32 _height,
			ptr<const AShader> _shader
		);

		implem void RecordRender(const CommandBuffer& _commandBuffer);

		implem DynamicArray<AttributeDescription>& GetVertexAttributes() const;

		implem DynamicArray<BindingDescription>& GetVertexBindings() const;

		implem ptr< const AShader> GetShader() const;

	protected:

		VertexBuffer vertBuffer;

		IndexBuffer indexBuffer;

		TextureImage textureImage;

		ptr<const AShader> shader;
	};

	//using ModelRenderable = TPrimitiveRenderable<Vertex_WTexture>;





	template<Meta::EGPU_Engage>
	class GPU_Resources_Maker;

	template<>
	class GPU_Resources_Maker<Meta::EGPU_Engage::Single>
	{
	public:
		template<typename VertexType>
		unbound Where< VulkanVertex<VertexType>(), ptr<ARenderable> > 
		Request_Renderable(const DynamicArray<VertexType>& _verticies, ptr<const AShader> _shader)
		{
			SPtr< TPrimitiveRenderable<VertexType>> newRenderable = MakeSPtr< TPrimitiveRenderable<VertexType>>();

			newRenderable->Create(_verticies, _shader);

			renderables.push_back(move(newRenderable));

			return renderables.back().get();
		}

		template<typename VertexType>
		unbound Where< VulkanVertex<VertexType>(), ptr<ARenderable> > 
		Request_Renderable
		(
			const DynamicArray<VertexType>& _verticies, 
			const DynamicArray<ui32> _indicies, 
			ptr<const uI8> _textureData,
			ui32 _width, ui32 _height,
			ptr<const AShader> _shader
		)
		{
			SPtr< TModelRenderable<VertexType>> newRenderable = MakeSPtr< TModelRenderable<VertexType>>();

			newRenderable->Create(_verticies, _indicies, _textureData, _width, _height, _shader);

			renderables.push_back(move(newRenderable));

			return renderables.back().get();
		}

		

	private:

		unbound DynamicArray< SPtr<ARenderable> > renderables;
	};

	using GPU_Resources = GPU_Resources_Maker<Meta::GPU_Engagement>;
}



// 	Template Implementation

#include "GPUVK_Resources.tpp"
