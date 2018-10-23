//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.cxx ---
//
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <string>
#include <Windows.h>
#include <io.h>

//#define USE_GL3W
#ifndef USE_GL3W

#define GLEW_STATIC
#include <GL/glew.h>
#else
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#endif

#include "LoadShaders.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


Utils::Utils()
{
	std::string path;
	char buf[255];
	GetModuleFileName(NULL,buf,255);
	path = buf;
	path = path.substr(0,path.find_last_of("\\/")+1);
	m_strAppPath = path;

	std::string mediaPath = path + "Media";
	if(_access(mediaPath.c_str(),0)!=0) // not find Media
	{
		std::string newPath = path.substr(0,path.find_last_of("\\/"));
		newPath =  newPath.substr(0,newPath.find_last_of("\\/")+1);
		mediaPath = newPath  + "Media";
		if(_access(mediaPath.c_str(),0) == 0)
		{
			path = newPath;
		}
	}
		
	m_strMediaPath = path;//+ "Media/";
}

Utils* Utils::instance()
{
	static Utils s_instance;
	return &s_instance;
}

//----------------------------------------------------------------------------

static const GLchar*
ReadShader( const char* filename )
{
#ifdef WIN32
	FILE* infile;
	fopen_s( &infile, filename, "rb" );
#else
    FILE* infile = fopen( filename, "rb" );
#endif // WIN32

    if ( !infile ) {
#ifdef _DEBUG
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    GLchar* source = new GLchar[len+1];

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return const_cast<const GLchar*>(source);
}

//----------------------------------------------------------------------------


GLuint
LoadShaders( ShaderInfo* shaders )
{
    if ( shaders == NULL ) { return 0; }

    GLuint program = glCreateProgram();
	std::string path = Utils::instance()->getMediaPath();
    ShaderInfo* entry = shaders;
    while ( entry->type != GL_NONE ) {
        GLuint shader = glCreateShader( entry->type );

        entry->shader = shader;

		std::string fileName = path  + entry->filename;
		const GLchar* source = ReadShader( fileName.c_str() );
        if ( source == NULL ) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
                glDeleteShader( entry->shader );
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource( shader, 1, &source, NULL );
        delete [] source;

        glCompileShader( shader );

        GLint compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( shader, len, &len, log );
			std::string type = "vertex_shader";
			if(entry->type == GL_VERTEX_SHADER)
				type = "vertex_shader";
			else if( entry->type == GL_FRAGMENT_SHADER)
				type = "fragment_shader";
			else if( entry->type == GL_GEOMETRY_SHADER)
				type = "geometry_shader";
			else if( entry->type == GL_TESS_CONTROL_SHADER)
				type = "tess_control shader";
			else if( entry->type == GL_TESS_EVALUATION_SHADER)
				type = "tess_evaluation shader";
			else
				type = "unknown type";
            std::cerr <<"type:" << type << ":Shader compilation failed: " << log << std::endl;
            delete [] log;
#endif /* DEBUG */

            return 0;
        }

        glAttachShader( program, shader );
        
        ++entry;
    }

#ifdef GL_VERSION_4_1
    if ( GLEW_VERSION_4_1 ) {
        // glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
    }
#endif /* GL_VERSION_4_1 */
    
    glLinkProgram( program );

    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( program, len, &len, log );
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete [] log;
#endif /* DEBUG */

        for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
            glDeleteShader( entry->shader );
            entry->shader = 0;
        }
        
        return 0;
    }

    return program;
}

//----------------------------------------------------------------------------
// ��ʾprogam������Ϣ
void ShowShaderInfo(GLuint program)
{
	std::cout << "--------------show program info:-----------------" << std::endl;
	GLint outputs = 0;
	glGetProgramInterfaceiv(program,GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES,&outputs);
	static const GLenum props[] = {GL_TYPE,GL_LOCATION};
	GLint params[2];
	GLchar name[64];
	const char* type_name;

	if(outputs > 0)
		std::cout << "***Input:***" << std::endl;

	for(int i = 0; i != outputs; ++i)
	{
		//  glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar *name);
		glGetProgramResourceName(program,GL_PROGRAM_INPUT,i,sizeof(name),NULL, name);
		
		glGetProgramResourceiv(program,GL_PROGRAM_INPUT, i, 2, props, 2, NULL,params);
		type_name = name;

		std::cout << "(" << type_name << ")" << "location:" << params[1] << std::endl;
	}

	glGetProgramInterfaceiv(program,GL_PROGRAM_OUTPUT,GL_ACTIVE_RESOURCES,&outputs);
	if(outputs >0)
		std::cout <<"***Output:***" << std::endl;

	for(int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(program,GL_PROGRAM_OUTPUT,i,sizeof(name),NULL,name);
		glGetProgramResourceiv(program,GL_PROGRAM_OUTPUT,i,2,props,2,NULL,params);
		type_name = name;
		std::cout << "(" << type_name << ")" << "location:" << params[1] << std::endl;
	}

	glGetProgramInterfaceiv(program,GL_UNIFORM_BLOCK,GL_ACTIVE_RESOURCES,&outputs);
	if(outputs > 0)
		std::cout <<"***Uniform Block:***" << std::endl;

	for(int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(program,GL_UNIFORM_BLOCK,i,sizeof(name),NULL,name);
		glGetProgramResourceiv(program,GL_UNIFORM_BLOCK,i,2,props,2,NULL,params);
		type_name = name;
		std::cout << "(" << type_name << ")" << "uniform:" << params[1] << std::endl;
	}

	glGetProgramInterfaceiv(program, GL_UNIFORM,  GL_ACTIVE_RESOURCES, &outputs);
	if (outputs > 0)
		std::cout << "***Uniform***" << std::endl;
	if (outputs > 100)
		return ;
	for (int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(program, GL_UNIFORM, i, sizeof(name), NULL, name);
		glGetProgramResourceiv( program, GL_UNIFORM, i, 2, props, 2, NULL, params);

		type_name = name;
		//std::cout << "Index " << i << std::endl;
		std::cout  <<  "(" <<  type_name  << ")" << " locatoin: " << params[1] << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
}

//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus


