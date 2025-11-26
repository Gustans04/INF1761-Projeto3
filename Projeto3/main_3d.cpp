
#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "material.h"
#include "transform.h"
#include "error.h"
#include "shader.h"
#include "sphere.h"
#include "texture.h"
#include "quad.h"
#include "light.h"
#include "mesh.h"
#include "cube.h"
#include "ClipPlane.h"

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {0.0f, 0.0f, 2.0f};

static ScenePtr scene;
static ScenePtr reflector;
static Camera3DPtr camera;        // Main camera
static ArcballPtr arcball;        // Active arcball

static void initialize (void)
{
  glClearColor(1.0f,1.0f,1.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);  // cull back faces

  // create cameras
  camera = Camera3D::Make(viewer_pos[0], viewer_pos[1], viewer_pos[2]);
  arcball = camera->CreateArcball();

  MaterialPtr white = Material::Make(1.0f,1.0f,1.0f);
  white->SetSpecular(0.0f,0.0f,0.0f); // remove pontos de brilho nos astros

  MaterialPtr reflection = Material::Make(1.0f,1.0f,1.0f, 0.5f);

  LightPtr sunLight = Light::Make(0.0f, 5.0f, 2.0f, 1.0f, "world");

  // Define a different shader for texture mapping
  // An alternative would be to use only this shader with a "white" texture for untextured objects
  ShaderPtr shd_tex = Shader::Make(sunLight,"world");
  shd_tex->AttachVertexShader("./shaders/ilum_vert/vertex_texture.glsl");
  shd_tex->AttachFragmentShader("./shaders/ilum_vert/fragment_texture.glsl");
  shd_tex->Link();

  // Create a shader with geometry shader for special effects
  ShaderPtr shd_geom = Shader::Make(sunLight,"world");
  shd_geom->AttachVertexShader("./shaders/ilum_vert/vertex_geometry.glsl");
  shd_geom->AttachGeometryShader("./shaders/ilum_vert/geometry_texture.glsl");
  shd_geom->AttachFragmentShader("./shaders/ilum_vert/fragment_geometry.glsl");
  shd_geom->Link();

  // Create a shader with bump mapping
  ShaderPtr shd_bump = Shader::Make(sunLight,"world");
  shd_bump->AttachVertexShader("./shaders/ilum_vert/vertex_bump.glsl");
  shd_bump->AttachFragmentShader("./shaders/ilum_vert/fragment_bump.glsl");
  shd_bump->Link();

  // Create a shader for planar reflection
  ShaderPtr shd_reflect = Shader::Make(sunLight,"world");
  shd_reflect->AttachVertexShader("./shaders/ilum_vert/vertex_reflect.glsl");
  shd_reflect->AttachFragmentShader("./shaders/ilum_vert/fragment_reflect.glsl");
  shd_reflect->Link();

  //Skull setup
  auto skullSpriteTex = Texture::Make("decal", "models/Skull.jpg");
  auto skullTrf = Transform::Make();
  skullTrf->Translate(0.5f, 1.45f, 0.0f);
  skullTrf->Scale(0.01f, 0.01f, 0.01f);
  skullTrf->Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
  auto skull = Node::Make(shd_tex, skullTrf, { skullSpriteTex, white }, { Mesh::Make("models/skull.obj") }); //General and Sprite Node

  //Mercury Setup
  auto mercurySpriteTex = Texture::Make("decal", "images/mercurymap.jpg");
  auto mercurySpriteTrf = Transform::Make();
  mercurySpriteTrf->Translate(-0.5f, 1.65f, 0.0f);
  mercurySpriteTrf->Scale(0.2f, 0.2f, 0.2f);

  auto mercurySprite = Node::Make(shd_geom, mercurySpriteTrf, { mercurySpriteTex, white }, { Sphere::Make(32,32) }); //Mercury Sprite Node
  auto mercury = Node::Make({ mercurySprite }); //General Mercury Node

  //Table setup
  auto tableSpriteTex = Texture::Make("decal", "models/table.png");
  auto tableNormalTex = Texture::Make("normalMap", "models/table_nrm.jpg");
  auto tableTrf = Transform::Make();
  tableTrf->Translate(0.0f, -0.5f, 0.0f);
  tableTrf->Scale(0.5f, 0.5f, 0.5f);
  auto table = Node::Make(shd_tex, tableTrf, { tableSpriteTex,tableNormalTex, white }, { Mesh::Make("models/table.obj") }, { skull, mercury }); //General and Sprite Node

  auto platformSpriteTrf = Transform::Make();
  auto platformSprite = Node::Make(shd_reflect, platformSpriteTrf, { reflection }, { Quad::Make() }); //Platform Node
  auto platform = Node::Make({ platformSprite }); //General Platform Node

  // Reflection trf
  auto reflectTrf = Transform::Make();
  reflectTrf->Translate(0.0f, 0.0f, -1.1f);
  reflectTrf->Scale(5.0f, 5.0f, 0.1f);  

  // build scene
  auto root = Node::Make({ table });
  scene = Scene::Make(root);
  reflector = Scene::Make(Node::Make(shd_reflect, reflectTrf, { reflection }, { platform }));

}

static void display (GLFWwindow* win)
{ 
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
    Error::Check("before render");

    // ’’ desenha ’’ refletor no stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NEVER, 1, 0xFFFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    reflector->Render(camera);

    // desenha cena refletida
    glStencilFunc(GL_EQUAL, 1, 0xFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    NodePtr root = scene->GetRoot();
    TransformPtr trf = Transform::Make();
    trf->LoadIdentity();
    trf->Translate(0.0f, 0.0f, -2.2f);
    trf->Scale(1.0f, 1.0f, -1.0f);
    root->SetTransform(trf);
    glFrontFace(GL_CW); // invert front face incidence
    scene->Render(camera);
    glFrontFace(GL_CCW); // restore front face incidence
    root->SetTransform(nullptr);
    glDisable(GL_STENCIL_TEST);

    
    // Render with the active camera
    scene->Render(camera);

    // desenha plataforma refletora
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    reflector->Render(camera);
    glDisable(GL_BLEND);
    
    Error::Check("after render");
}

static void error (int code, const char* msg)
{
    printf("GLFW error %d: %s\n", code, msg);
    glfwTerminate();
    exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void cursorpos (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->AccumulateMouseMotion(int(x),int(y));
}
static void cursorinit (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->InitMouseMotion(int(x),int(y));
  glfwSetCursorPosCallback(win, cursorpos);     // cursor position callback
}
static void mousebutton (GLFWwindow* win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    glfwSetCursorPosCallback(win, cursorinit);     // cursor position callback
  }
  else // GLFW_RELEASE 
    glfwSetCursorPosCallback(win, nullptr);      // callback disabled
}

static void update (float dt)
{
  scene->Update(dt);
}

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);       // required for mac os
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);  // option for mac os
#endif

    glfwSetErrorCallback(error);

    GLFWwindow* win = glfwCreateWindow(1024, 600, "Window title", nullptr, nullptr);
    assert(win);
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback
    glfwSetMouseButtonCallback(win, mousebutton); // mouse button callback

    glfwMakeContextCurrent(win);
#ifdef __glad_h_
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD OpenGL context\n");
        exit(1);
    }
#endif
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t-t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

