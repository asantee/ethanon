#include "ApplePlatform.h"

#include "../../Application.h"
#include "../FileLogger.h"
#include "../../PolygonRenderer.h"

#include <iostream>
#include <sstream>

namespace gs2d {

void ShowMessage(std::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	std::cout << stream.str() << std::endl;
}

PolygonRendererPtr PolygonRenderer::Create(
	const std::vector<PolygonRenderer::Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const PolygonRenderer::POLYGON_MODE mode)
{
	//return boost::shared_ptr<PolygonRenderer>(new GLPolygonRenderer(vertices, indices, mode));
	return PolygonRendererPtr();
}

} // namespace gs2d

namespace Platform {

std::string FileLogger::GetLogDirectory()
{
	return "";
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace Platform
