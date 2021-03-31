#include "common.h"
#include "Image.h"
#include "game.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

const unsigned int WINDOW_WIDTH = 1820, WINDOW_HEIGHT = 1024;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Game MyGame(WINDOW_WIDTH, WINDOW_HEIGHT);

void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  switch (key)
  {
  case GLFW_KEY_ESCAPE:
    if (action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
  default:
    if (action == GLFW_PRESS)
      MyGame.keys[key] = true;
    else if (action == GLFW_RELEASE)
      MyGame.keys[key] = false;
      if (MyGame.deltaIsQ > 1.0f && key == GLFW_KEY_Q) {
          MyGame.IsKeyQ = true;
      }
  }
}
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    MyGame.captureMouse = !MyGame.captureMouse;

  if (MyGame.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    MyGame.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (MyGame.firstMouse)
  {
    MyGame.lastX = float(xpos);
    MyGame.lastY = float(ypos);
    MyGame.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - MyGame.lastX;
  GLfloat yoffset = MyGame.lastY - float(ypos);

  MyGame.lastX = float(xpos);
  MyGame.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }

  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

  return 0;
}

int main(int argc, char** argv)
{
  if(!glfwInit())
    return -1;

//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
  if (window == nullptr)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window); 

  glfwSetKeyCallback        (window, OnKeyboardPressed);  
  glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
  glfwSetScrollCallback     (window, OnMouseScroll);

  if(initGL() != 0) 
    return -1;
  
  //Reset any OpenGL errors which could be present for some reason
  GLenum gl_error = glGetError();
  while (gl_error != GL_NO_ERROR)
    gl_error = glGetError();

  Image img("resources/tex.png");
  Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

  MyGame.Init();

  //game loop
  while (!glfwWindowShouldClose(window))
    {
        // Вычисление дельты времени
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // Обрабатываем пользовательский ввод с клавиатуры
        MyGame.ProcessInput(deltaTime);

        // Обновляем состояние игры
        //MyGame.Update(deltaTime);

        // Рендер
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        MyGame.Update(deltaTime, window);
        MyGame.Render();

        glfwSwapBuffers(window);
    }

    // Освобождение всех ресурсов, загруженных с использованием менеджера ресурсов

    glfwTerminate();
    return 0;
}
