#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#define ASSERT(x) if(!x) __debugbreak();
#define GLCall(x)\
	GLClearErrors();\
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))


static void GLClearErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* functin, const char* file, int line)
{
	bool bClearFromErrors = true;
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error]: (" << error << ")\n" << functin << ": " << file << ": " << line << std::endl;
		bClearFromErrors = false;
	}
	return bClearFromErrors;
}

struct ShaderProgramSource
{
	std::string VertexSource, FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
	std::ifstream stream(filePath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType shaderType = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				shaderType = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				shaderType = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)shaderType] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompilerShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int shader = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(shader, 1, &src, nullptr));
	GLCall(glCompileShader(shader));

	int result;
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));

		char* msg = (char*)malloc(sizeof(char) * length);

		GLCall(glGetShaderInfoLog(shader, length, &length, msg));
		std::cout << "Error in " << ((type == GL_VERTEX_SHADER) ? "Vertex" : "Fragmenet") << "\n";
		std::cout << msg << std::endl;
		GLCall(glDeleteShader(shader));
		return 0;

	}

	return shader;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vshader = CompilerShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fshader = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vshader));
	GLCall(glAttachShader(program, fshader));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vshader));
	GLCall(glDeleteShader(fshader));

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);


	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error!\n";
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "Status: OpenGL Version %s\n", glGetString(GL_VERSION));

	//float positions[6] = {
	//	-0.5, -0.5,
	//	 0.0,  0.5,
	//	 0.5, -0.5
	//};

	float positions[] = {
		-0.5, -0.5,		0.0,0.0,1.0, //0 - Pos (2) / RGB (3)
		 0.5, -0.5,		1.0,0.0,0.0, //1 - Pos (2) / RGB (3)
		 0.5,  0.5,		0.0,1.0,0.0, //2 - Pos (2) / RGB (3)
		-0.5,  0.5,		1.0,1.0,1.0  //3 - Pos (2) / RGB (3)
	};

	unsigned int indicies[] = {
		0, 1, 2,
		0, 2, 3
	};


	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	unsigned int vbo;
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 5, positions, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(2 * sizeof(float))));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indicies, GL_STATIC_DRAW));

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);

	glUseProgram(program);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	int uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, 1, 1, 0, 1);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	GLCall(glDeleteProgram(program));

	glfwTerminate();
	return 0;
}
