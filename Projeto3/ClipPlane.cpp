#include"ClipPlane.h"

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif

ClipPlanePtr ClipPlane::Make(const std::string& name)
{
    return ClipPlanePtr(new ClipPlane(name));
}

ClipPlanePtr ClipPlane::Make(const std::string& name, float a, float b, float c, float d)
{
    return ClipPlanePtr(new ClipPlane(name, a, b, c, d));
}

ClipPlane::ClipPlane(const std::string& name)
    : m_name(name)
{
}

ClipPlane::ClipPlane(const std::string& name, float a, float b, float c, float d)
    : m_name(name)
{
    AddPlane(a, b, c, d);
}

void ClipPlane::AddPlane(float a, float b, float c, float d)
{
    m_planes.push_back(glm::vec4(a, b, c, d));
}

ClipPlane::~ClipPlane()
{
}

void ClipPlane::Load(StatePtr st)
{
    ShaderPtr shd = st->GetShader();

    glm::mat4 mat = st->GetCurrentMatrix(); // model
    if (shd->GetLightingSpace() == "camera")
        mat = st->GetCamera()->GetViewMatrix() * mat;

    glm::mat4 mit = glm::transpose(glm::inverse(mat));

    std::vector<glm::vec4> planes;
    for (auto p : m_planes) {
        planes.push_back(mit * p);
    }

    st->GetShader()->SetUniform(m_name, planes);

    for (int i = 0; i < m_planes.size(); ++i)
        glEnable(GL_CLIP_DISTANCE0 + i);
}

void ClipPlane::Unload(StatePtr st)
{
    for (int i = 0; i < m_planes.size(); ++i)
        glDisable(GL_CLIP_DISTANCE0 + i);
}
