// Crystal Growth Simulation with OpenGL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <random>

// Simulation settings
const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_PARTICLES = 100;
const float GRID_SIZE = 0.1f;
float simulationSpeed = 1.0f;
bool paused = false;

// OpenGL objects
GLuint particleBuffer, gridBuffer, gridCellBuffer;
GLuint computeShaderProgram, renderShaderProgram;
GLuint vao;

// Shader Sources
const char* computeShaderSource = R"glsl(
#version 430
layout(local_size_x = 256) in;

struct Particle {
    vec3 position;
    vec3 velocity;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

uniform float deltaTime;

void main() {
    uint id = gl_GlobalInvocationID.x;
    particles[id].position += particles[id].velocity * deltaTime;
}
)glsl";

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 modelViewProjection;
void main() {
    gl_Position = modelViewProjection * vec4(position, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)glsl";

// Function to check shader compilation
void checkShaderCompilation(GLuint shader, const char* shaderType) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error compiling " << shaderType << " shader:\n" << infoLog << std::endl;
    }
}

// OpenGL Debug Callback
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
}

// Initialize Compute Shader
void initComputeShader() {
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, nullptr);
    glCompileShader(computeShader);
    checkShaderCompilation(computeShader, "Compute");

    computeShaderProgram = glCreateProgram();
    glAttachShader(computeShaderProgram, computeShader);
    glLinkProgram(computeShaderProgram);

    GLint success;
    glGetProgramiv(computeShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(computeShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error linking compute shader program:\n" << infoLog << std::endl;
    }

    glDeleteShader(computeShader);
}

// Initialize Render Shader
void initRenderShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader, "Vertex");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader, "Fragment");

    renderShaderProgram = glCreateProgram();
    glAttachShader(renderShaderProgram, vertexShader);
    glAttachShader(renderShaderProgram, fragmentShader);
    glLinkProgram(renderShaderProgram);

    GLint success;
    glGetProgramiv(renderShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(renderShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error linking render shader program:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Initialize Particles
void initParticles() {
    std::vector<glm::vec3> initialPositions(MAX_PARTICLES, glm::vec3(0.0f));
    std::vector<glm::vec3> initialVelocities(MAX_PARTICLES, glm::vec3(0.01f, 0.0f, 0.0f));

    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(glm::vec3) * 2, nullptr, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, MAX_PARTICLES * sizeof(glm::vec3), initialPositions.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(glm::vec3), MAX_PARTICLES * sizeof(glm::vec3), initialVelocities.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

    // Debugging Particle Positions
    for (int i = 0; i < std::min(10, MAX_PARTICLES); ++i) {
        std::cout << "Particle " << i << " initialized at position: "
                  << initialPositions[i].x << ", "
                  << initialPositions[i].y << ", "
                  << initialPositions[i].z << std::endl;
    }
}

// Initialize OpenGL
void initOpenGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);

    initComputeShader();
    initRenderShader();
    initParticles();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

// Run Compute Shader
void runComputeShader(float deltaTime) {
    glUseProgram(computeShaderProgram);
    glUniform1f(glGetUniformLocation(computeShaderProgram, "deltaTime"), deltaTime);
    glDispatchCompute(MAX_PARTICLES / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// Render Particles
void renderParticles(const glm::mat4& mvp) {
    glUseProgram(renderShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "modelViewProjection"), 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
}

// Display Function
void display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 1.0f, 100.0f);
    glm::mat4 mvp = projection * view;

    renderParticles(mvp);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Main Function
int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Crystal Growth Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugCallback, nullptr);

    initOpenGL();

    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (!paused) {
            runComputeShader(deltaTime * simulationSpeed);
        }

        display(window);
    }

    glfwTerminate();
    return 0;
}
