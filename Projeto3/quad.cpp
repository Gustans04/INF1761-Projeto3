#include "quad.h"
#include "error.h"

#ifdef _WIN32
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif

QuadPtr Quad::Make()
{
    return QuadPtr(new Quad());
}

Quad::Quad()
{
    // Quad no plano XY, normal +Z.
    // Tamanho 1x1, centrado.
    float coords[] = {
        -0.5f, -0.5f, 0.0f,   // v0 (inferior esquerdo)
         0.5f, -0.5f, 0.0f,   // v1 (inferior direito)
         0.5f,  0.5f, 0.0f,   // v2 (superior direito)
        -0.5f,  0.5f, 0.0f    // v3 (superior esquerdo)
    };

    // Normal → virado para o +Z
    float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };

    // Tangente → direção +X
    float tangents[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    // UV layout padrão
    float texcoords[] = {
        0.0f, 0.0f,  // v0
        1.0f, 0.0f,  // v1
        1.0f, 1.0f,  // v2
        0.0f, 1.0f   // v3
    };

    // Triângulos em ordem anti-horária, vistos do +Z
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // ---- VAO ----
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint id[4];
    glGenBuffers(4, id);

    // posições
    glBindBuffer(GL_ARRAY_BUFFER, id[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // normais
    glBindBuffer(GL_ARRAY_BUFFER, id[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // tangentes
    glBindBuffer(GL_ARRAY_BUFFER, id[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // UV
    glBindBuffer(GL_ARRAY_BUFFER, id[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    // índices
    GLuint index;
    glGenBuffers(1, &index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

Quad::~Quad()
{
}

void Quad::Draw(StatePtr)
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
