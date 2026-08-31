#pragma once

#include <string>

namespace solarium::rendering {

class Shader {
public:
    Shader(
        const std::string& vertexPath,
        const std::string& fragmentPath
    );

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void bind() const;
    void unbind() const;

    [[nodiscard]]
    unsigned int id() const noexcept;

    void setFloat(
        const std::string& name,
        float value
    ) const;

    void setVec3(
        const std::string& name,
        float x,
        float y,
        float z
    ) const;

    void setMat4(
        const std::string& name,
        const float* matrix
    ) const;

private:
    unsigned int programId_;

    static std::string readFile(
        const std::string& path
    );

    static unsigned int compileShader(
        unsigned int type,
        const std::string& source
    );

    static void checkCompileErrors(
        unsigned int shader,
        const char* type
    );
};

}