#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>


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
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompilerShader(unsigned int type, const std::string& source)
{
	unsigned int shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		char* msg = (char*)malloc(sizeof(char) * length);

		glGetShaderInfoLog(shader, length, &length, msg);
		std::cout << "Error!\n";
		std::cout << msg << std::endl;

		glDeleteShader(shader);
		return 0;

	}

	return shader;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vshader = CompilerShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fshader = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vshader);
	glDeleteShader(fshader);

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
		-0.5, -0.5, //0
		 0.5, -0.5, //1	 
		 0.5,  0.5, //2
		-0.5,  0.5 //3
	};

	unsigned int indicies[] = {
		0, 1, 2,
		0, 2, 3
	};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 10, positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indicies, GL_STATIC_DRAW);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);

	glUseProgram(program);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}
