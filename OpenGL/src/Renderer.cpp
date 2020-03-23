#include "Renderer.h"

#include <iostream>

void GLClearErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* functin, const char* file, int line)
{
	bool bClearFromErrors = true;
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error]: (" << error << ")\n" << functin << ": " << file << ": " << line << std::endl;
		bClearFromErrors = false;
	}
	return bClearFromErrors;
}
