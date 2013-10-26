// GLSL Utility: A utility class for loading GLSL shaders
// Written by Varun Sampath and Patrick Cozzi, Copyright (c) 2012 University of Pennsylvania

#define GLEW_STATIC
#include <glslUtil/glslUtility.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>

using std::ios;

namespace glslUtility {

	// embedded passthrough shaders so that default passthrough shaders don't need to be loaded
	static std::string passthroughVS =
		"	attribute vec4 Position;"
		"	attribute vec2 Texcoords;"
		"	varying vec2 v_Texcoords;"
		"	"
		"	void main(void){"
		"		v_Texcoords = Texcoords;"
		"		gl_Position = Position;"
		"	}";
	static std::string passthroughFS =
		"	varying vec2 v_Texcoords;"
		"	"
		"	uniform sampler2D u_image;"
		"	"
		"	void main(void){"
		"		gl_FragColor = texture2D(u_image, v_Texcoords);"
		"	}";

	typedef struct {
		GLuint vertex;
		GLuint fragment;
	} shaders_t;

	char* loadFile(const char *fname, GLint &fSize)
	{
		// file read based on example in cplusplus.com tutorial
		std::ifstream file (fname, ios::in|ios::binary|ios::ate);
		if (file.is_open())
		{
			unsigned int size = (unsigned int)file.tellg();
			fSize = size;
			char *memblock = new char [size];
			file.seekg (0, ios::beg);
			file.read (memblock, size);
			file.close();
			//std::cout << "file " << fname << " loaded" << std::endl;
    		return memblock;
		}

		std::cout << "Unable to open file " << fname << std::endl;
		exit(EXIT_FAILURE);
	}

	// printShaderInfoLog
	// From OpenGL Shading Language 3rd Edition, p215-216
	// Display (hopefully) useful error messages if shader fails to compile
	void printShaderInfoLog(GLint shader)
	{
		int infoLogLen = 0;
		int charsWritten = 0;
		GLchar *infoLog;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

		if (infoLogLen > 1)
		{
			infoLog = new GLchar[infoLogLen];
			// error check for fail to allocate memory omitted
			glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
			//std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
			delete [] infoLog;
		}
	}

	void printLinkInfoLog(GLint prog) 
	{
		int infoLogLen = 0;
		int charsWritten = 0;
		GLchar *infoLog;

		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

		if (infoLogLen > 1)
		{
			infoLog = new GLchar[infoLogLen];
			// error check for fail to allocate memory omitted
			glGetProgramInfoLog(prog,infoLogLen, &charsWritten, infoLog);
			//std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
			delete [] infoLog;
		}
	}

	shaders_t loadDefaultShaders() {
		GLuint f, v;

		char *vs,*fs;

		v = glCreateShader(GL_VERTEX_SHADER);
		f = glCreateShader(GL_FRAGMENT_SHADER);	

		// load shaders & get length of each
		GLint vlen;
		GLint flen;

		vlen = (unsigned int)std::strlen(passthroughVS.c_str());
		flen = (unsigned int)std::strlen(passthroughFS.c_str());

		vs = new char[passthroughVS.length()+1];
		fs = new char[passthroughFS.length()+1];

		std::strcpy(vs, passthroughVS.c_str());
		std::strcpy(fs, passthroughFS.c_str());

		const char * vv = vs;
		const char * ff = fs;

		glShaderSource(v, 1, &vv,&vlen);
		glShaderSource(f, 1, &ff,&flen);

		GLint compiled;

		glCompileShader(v);
		glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			std::cout << "Vertex shader not compiled." << std::endl;
		} 
		printShaderInfoLog(v);

		glCompileShader(f);
		glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			std::cout << "Fragment shader not compiled." << std::endl;
		} 
		printShaderInfoLog(f);

		shaders_t out; out.vertex = v; out.fragment = f;

		delete [] vs; // dont forget to free allocated memory, or else really bad things start happening
		delete [] fs; // we allocated this in the loadFile function...

		return out;
	}

	shaders_t loadShaders(const char * vert_path, const char * frag_path) {
		GLuint f, v;

		char *vs,*fs;

		v = glCreateShader(GL_VERTEX_SHADER);
		f = glCreateShader(GL_FRAGMENT_SHADER);	

		// load shaders & get length of each
		GLint vlen;
		GLint flen;

		vs = loadFile(vert_path,vlen);
		fs = loadFile(frag_path,flen);

		const char * vv = vs;
		const char * ff = fs;

		glShaderSource(v, 1, &vv,&vlen);
		glShaderSource(f, 1, &ff,&flen);

		GLint compiled;

		glCompileShader(v);
		glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			std::cout << "Vertex shader not compiled." << std::endl;
		} 
		printShaderInfoLog(v);

		glCompileShader(f);
		glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			std::cout << "Fragment shader not compiled." << std::endl;
		} 
		printShaderInfoLog(f);

		shaders_t out; out.vertex = v; out.fragment = f;

		delete [] vs; // dont forget to free allocated memory, or else really bad things start happening
		delete [] fs; // we allocated this in the loadFile function...

		return out;
	}

	void attachAndLinkProgram( GLuint program, shaders_t shaders) {
		glAttachShader(program, shaders.vertex);
		glAttachShader(program, shaders.fragment);

		glLinkProgram(program);
		GLint linked;
		glGetProgramiv(program,GL_LINK_STATUS, &linked);
		if (!linked) 
		{
			std::cout << "Program did not link." << std::endl;
		}
		printLinkInfoLog(program);
	}

	GLuint createDefaultProgram(const char *attributeLocations[], GLuint numberOfLocations)
    {
	    glslUtility::shaders_t shaders = glslUtility::loadDefaultShaders();
	
	    GLuint program = glCreateProgram();

		for (GLuint i = 0; i < numberOfLocations; ++i)
		{
            glBindAttribLocation(program, i, attributeLocations[i]);
		}

	    glslUtility::attachAndLinkProgram(program, shaders);

        return program;
    }

    GLuint createProgram(const char *vertexShaderPath, const char *fragmentShaderPath, 
    					 const char *attributeLocations[], GLuint numberOfLocations)
    {
	    glslUtility::shaders_t shaders = glslUtility::loadShaders(vertexShaderPath, fragmentShaderPath);
	
	    GLuint program = glCreateProgram();

		for (GLuint i = 0; i < numberOfLocations; ++i)
		{
            glBindAttribLocation(program, i, attributeLocations[i]);
		}

	    glslUtility::attachAndLinkProgram(program, shaders);

        return program;
    }
}