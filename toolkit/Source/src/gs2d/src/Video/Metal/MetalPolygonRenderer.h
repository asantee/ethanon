#ifndef GS2D_METAL_VIDEO_POLYGON_RENDERER_H_
#define GS2D_METAL_VIDEO_POLYGON_RENDERER_H_

#include "../../PolygonRenderer.h"

#import <Metal/Metal.h>
#import <simd/simd.h>

namespace gs2d {

class MetalVideo;

class MetalPolygonRenderer : public PolygonRenderer
{
public:
	struct MetalVertex
	{
		simd_float4 position;
		simd_float4 texCoord;
		simd_float4 normal;
	};

	MetalPolygonRenderer(
		MetalVideo* metalVideo,
		const std::vector<PolygonRenderer::Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		const PolygonRenderer::POLYGON_MODE mode);
	~MetalPolygonRenderer();

	void BeginRendering(const ShaderPtr& shader) override;
	void Render() override;
	void EndRendering() override;
	
private:
	MTLPrimitiveType m_primitiveType;
	MetalVideo* m_metalVideo;
	id<MTLDevice> m_device;
	id<MTLBuffer> m_vertexBuffer;
	id<MTLBuffer> m_indexBuffer;
	std::vector<MetalVertex> m_vertices;
};

typedef std::shared_ptr<MetalPolygonRenderer> MetalPolygonRendererPtr;

} // namespace gs2d

#endif
