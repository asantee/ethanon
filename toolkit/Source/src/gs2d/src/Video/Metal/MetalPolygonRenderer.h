#ifndef GS2D_METAL_VIDEO_POLYGON_RENDERER_H_
#define GS2D_METAL_VIDEO_POLYGON_RENDERER_H_

#include "../../PolygonRenderer.h"

namespace gs2d {

class MetalPolygonRenderer : public PolygonRenderer
{
public:
	MetalPolygonRenderer(
		const std::vector<PolygonRenderer::Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		const PolygonRenderer::POLYGON_MODE mode);
	~MetalPolygonRenderer();

	void BeginRendering(const ShaderPtr& shader) override;
	void Render() override;
	void EndRendering() override;
};

typedef std::shared_ptr<MetalPolygonRenderer> MetalPolygonRendererPtr;

} // namespace gs2d

#endif
