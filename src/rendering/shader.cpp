#include "solarium/rendering/shader.hpp"

#include <glad/gl.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace solarium::rendering {

std::string Shader::readFile(
    const std::string& path
) {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error(
            "Failed to open shader: " + path
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

unsigned int Shader::compileShader(
    unsigned int type,
    const std::string& source
) {
    const unsigned int shader =
        glCreateShader(type);

    const char* sourceCode =
        source.c_str();

    glShaderSource(
        shader,
        1,
        &sourceCode,
        nullptr
    );

    glCompileShader(shader);

    checkCompileErrors(
        shader,
        type == GL_VERTEX_SHADER
            ? "VERTEX"
            : "FRAGMENT"
    );

    return shader;
}

void Shader::checkCompileErrors(
    unsigned int shader,
    const char* type
) {
    int success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success) {

        char infoLog[1024];

        glGetShaderInfoLog(
            shader,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        throw std::runtime_error(
            std::string(type) +
            " shader compilation failed:\n" +
            infoLog
        );
    }
}

Shader::Shader(
    const std::string& vertexPath,
    const std::string& fragmentPath
)
    : programId_(0) {

    const std::string vertexSource =
        readFile(vertexPath);

    const std::string fragmentSource =
        readFile(fragmentPath);

    const unsigned int vertexShader =
        compileShader(
            GL_VERTEX_SHADER,
            vertexSource
        );

    const unsigned int fragmentShader =
        compileShader(
            GL_FRAGMENT_SHADER,
            fragmentSource
        );

    programId_ =
        glCreateProgram();

    glAttachShader(
        programId_,
        vertexShader
    );

    glAttachShader(
        programId_,
        fragmentShader
    );

    glLinkProgram(programId_);

    int success = 0;

    glGetProgramiv(
        programId_,
        GL_LINK_STATUS,
        &success
    );

    if (!success) {

        char infoLog[1024];

        glGetProgramInfoLog(
            programId_,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        throw std::runtime_error(
            std::string(
                "Shader program linking failed:\n"
            ) + infoLog
        );
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {

    if (programId_ != 0) {
        glDeleteProgram(programId_);
    }
}

Shader::Shader(Shader&& other) noexcept
    : programId_(other.programId_) {

    other.programId_ = 0;
}

Shader& Shader::operator=(
    Shader&& other
) noexcept {

    if (this != &other) {

        if (programId_ != 0) {
            glDeleteProgram(programId_);
        }

        programId_ = other.programId_;
        other.programId_ = 0;
    }

    return *this;
}

void Shader::bind() const {
    glUseProgram(programId_);
}

void Shader::unbind() const {
    glUseProgram(0);
}

unsigned int Shader::id() const noexcept {
    return programId_;
}

void Shader::setFloat(
    const std::string& name,
    float value
) const {

    const int location =
        glGetUniformLocation(
            programId_,
            name.c_str()
        );

    glUniform1f(location, value);
}

void Shader::setVec3(
    const std::string& name,
    float x,
    float y,
    float z
) const {

    const int location =
        glGetUniformLocation(
            programId_,
            name.c_str()
        );

    glUniform3f(
        location,
        x,
        y,
        z
    );
}

void Shader::setMat4(
    const std::string& name,
    const float* matrix
) const {

    const int location =
        glGetUniformLocation(
            programId_,
            name.c_str()
        );

    glUniformMatrix4fv(
        location,
        1,
        GL_FALSE,
        matrix
    );
}

}