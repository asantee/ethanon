#include "GLES2PolygonRenderer.h"
#include "GLES2Shader.h"

namespace gs2d {

PolygonRendererPtr PolygonRenderer::Create(
	const std::vector<Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const POLYGON_MODE mode)
{
	return PolygonRendererPtr(new GLES2PolygonRenderer(vertices, indices, mode));
}

GLES2PolygonRenderer::GLES2PolygonRenderer(
	const std::vector<PolygonRenderer::Vertex>& vertices,
	const std::vector<uint32_t>& indices,
	const PolygonRenderer::POLYGON_MODE mode) :
	m_vertexCount(static_cast<GLsizei>(indices.size()))
{
	switch (mode)
	{
		case PolygonRenderer::TRIANGLE_STRIP:
			m_glMode = GL_TRIANGLE_STRIP;
		break;

		case PolygonRenderer::TRIANGLE_FAN:
			m_glMode = GL_TRIANGLE_FAN;
		break;

		case PolygonRenderer::TRIANGLE_LIST:
		default:
			m_glMode = GL_TRIANGLES;
		break;
	}

	// make vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PolygonRenderer::Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// make index buffer
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	std::vector<GLushort> shortIndices;
	for (unsigned int t = 0; t < indices.size(); ++t) { shortIndices.push_back(static_cast<GLushort>(indices[t])); }
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * shortIndices.size(), &shortIndices[0], GL_STATIC_DRAW);

	/*const unsigned int size = sizeof(PolygonRenderer::Vertex);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size, (void*)(sizeof(math::Vector3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, size, (void*)(sizeof(math::Vector3) + sizeof(math::Vector2)));
	glEnableVertexAttribArray(2);*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLES2PolygonRenderer::~GLES2PolygonRenderer()
{
	glDeleteBuffers(1, &m_indexBuffer);
	glDeleteBuffers(1, &m_vertexBuffer);
}

void GLES2PolygonRenderer::BeginRendering(const ShaderPtr& shader)
{
	shader->SetShader();
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	GLES2Shader* gles2Shader = static_cast<GLES2Shader*>(shader.get());
	const GLuint program = gles2Shader->GetProgram();

	const GLint vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(PolygonRenderer::Vertex), (void*)0);

	const GLint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(PolygonRenderer::Vertex), (void*)(sizeof(math::Vector3)));

	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vTexCoord);

	const GLint vNormal = glGetAttribLocation(program, "vNormal");
	if (vNormal != -1)
	{
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(PolygonRenderer::Vertex), (void*)(sizeof(math::Vector3) + sizeof(math::Vector2)));
		glEnableVertexAttribArray(vNormal);
	}
}

void GLES2PolygonRenderer::Render()
{
	glDrawElements(m_glMode, m_vertexCount, GL_UNSIGNED_SHORT, (void*)0);
}

void GLES2PolygonRenderer::EndRendering()
{
}

} // namespace gs2d
