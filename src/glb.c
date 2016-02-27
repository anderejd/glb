/**
 * THERE WILL BE NO REFACTORING OR PHILOSOFICAL CODING 
 * (NAME CONVENTION CHANGES ETC.) UNTIL THE GAME IS FINISHED.
 * SINGLE SOURCE FILE CHALLANGE! (except libs!)
 * STICK TO THE PLAN!
 * KEEP IT STIMPLE, SUPID!
 *
 * GLSL coding convention examples:
 *         i_vpos --> Input Vertex POSision
 *         o_fcol --> Outupt Fragment COLor
*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#define _CRT_SECURE_NO_WARNINGS 1
#pragma comment(lib, "opengl32")
#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2main")
#include <windows.h>
#include <SDL.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "glad/glad.h"


/** File system paths */
#define GLB_DATA_ROOT "data/"


/** Misc helper macros */
#define GLB_ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])


/** Program exit codes.  */
#define GLB_ERROR_LOG_INIT 1
#define GLB_ERROR_SDL 2
#define GLB_ERROR_LOAD_GL_FUNCS 3
#define GLB_ERROR_INSUFFICIENT_GL_VERSION 4


/**
 * Minimalist log util.
 * Concatenating fmt with "\n" forces fmt to be a string literal (a good thing).
 */
FILE *glb_log_file = NULL;
#define glb_log(fmt, ...) \
	fprintf (glb_log_file, fmt "\n", __VA_ARGS__); \
        printf  (fmt "\n", __VA_ARGS__)


static void
glb_log_open (void)
{
	glb_log_file = fopen ("log.txt", "w");
	if (NULL == glb_log_file)
	{
		printf ("Failed to open log file.\n");
		exit (GLB_ERROR_LOG_INIT);
	}
}


static void
glb_log_close (void)
{
	fclose (glb_log_file);
}


/**
 * 4x4 matrix of float
 *
 * basis vectors are layed out contiguously in memory
 * geometric operations assume premultiplication of column vectors
 * NOTE: glb_mat4 is an array type
 *
 * layout/indexing:
 *   _            _
 *  |  0  4  8 12  |
 *  |  1  5  9 13  |
 *  |  2  6 10 14  |
 *  |_ 3  7 11 15 _|
 *
 */
typedef float glb_mat4[16];


#define GLB_MAT4_IDENTITY \
{ \
	1.0, 0.0, 0.0, 0.0, \
	0.0, 1.0, 0.0, 0.0, \
	0.0, 0.0, 1.0, 0.0, \
	0.0, 0.0, 0.0, 1.0  \
}


/** copy source into destination */
void
glb_mat4_copy (const glb_mat4 source, glb_mat4 destination)
{
	memcpy (destination, source, sizeof (glb_mat4));
}


/** overwrites values in m making it an identity matrix */
void
glb_mat4_load_identity (glb_mat4 m)
{
	m[0] = 1.0f; m[4] = 0.0f; m[8]  = 0.0f; m[12] = 0.0f;
	m[1] = 0.0f; m[5] = 1.0f; m[9]  = 0.0f; m[13] = 0.0f;
	m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = 0.0f;
	m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
}


/** overwrites values in m making it a scale matrix */
void
glb_mat4_load_scale (float x, float y, float z, glb_mat4 m)
{
	m[0] = x;    m[4] = 0.0f; m[8]  = 0.0f; m[12] = 0.0f;
	m[1] = 0.0f; m[5] = y;    m[9]  = 0.0f; m[13] = 0.0f;
	m[2] = 0.0f; m[6] = 0.0f; m[10] = z;    m[14] = 0.0f;
	m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
}


/** overwrites values in m making it a translation matrix */
void
glb_mat4_load_translation (float x, float y, float z, glb_mat4 m)
{
	m[0] = 1.0f; m[4] = 0.0f; m[8]  = 0.0f; m[12] = x;
	m[1] = 0.0f; m[5] = 1.0f; m[9]  = 0.0f; m[13] = y;
	m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = z;
	m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
}


/** multiplies matrix a with matrix b and stores the result in matrix out */
void
glb_mat4_multiply (const glb_mat4 a, const glb_mat4 b, glb_mat4 out)
{
	out[0]  = a[0] * b[0]  + a[4] * b[1]  + a[8]  * b[2]  + a[12] * b[3];
	out[1]  = a[1] * b[0]  + a[5] * b[1]  + a[9]  * b[2]  + a[13] * b[3];
	out[2]  = a[2] * b[0]  + a[6] * b[1]  + a[10] * b[2]  + a[14] * b[3];
	out[3]  = a[3] * b[0]  + a[7] * b[1]  + a[11] * b[2]  + a[15] * b[3];
	out[4]  = a[0] * b[4]  + a[4] * b[5]  + a[8]  * b[6]  + a[12] * b[7];
	out[5]  = a[1] * b[4]  + a[5] * b[5]  + a[9]  * b[6]  + a[13] * b[7];
	out[6]  = a[2] * b[4]  + a[6] * b[5]  + a[10] * b[6]  + a[14] * b[7];
	out[7]  = a[3] * b[4]  + a[7] * b[5]  + a[11] * b[6]  + a[15] * b[7];
	out[8]  = a[0] * b[8]  + a[4] * b[9]  + a[8]  * b[10] + a[12] * b[11];
	out[9]  = a[1] * b[8]  + a[5] * b[9]  + a[9]  * b[10] + a[13] * b[11];
	out[10] = a[2] * b[8]  + a[6] * b[9]  + a[10] * b[10] + a[14] * b[11];
	out[11] = a[3] * b[8]  + a[7] * b[9]  + a[11] * b[10] + a[15] * b[11];
	out[12] = a[0] * b[12] + a[4] * b[13] + a[8]  * b[14] + a[12] * b[15];
	out[13] = a[1] * b[12] + a[5] * b[13] + a[9]  * b[14] + a[13] * b[15];
	out[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
	out[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}


/** OpenGL error check and logging util. */
#define glb_check_gl_errors() \
        glb_check_gl_errors_impl(__FILE__, __func__, __LINE__)


static bool
glb_check_gl_errors_impl (const char *file, const char *func, int line)
{
	bool found = false;
	GLenum err;
	while (GL_NO_ERROR != (err = glGetError ()))
	{
		found = true;
		glb_log ("gl error: 0x%x, file: %s, fn: %s, line: %d",
		        err, file, func, line);
	}
	return found;
}


/** Wrapper for one Vertex Attribute Object and relates data */
struct glb_vao
{
	GLuint  id;
	GLuint  vbo[2];
	GLsizei vert_count;
};


/** Things we need to keep track of between frames. */
struct glb_state
{
	int           want_shutdown;
	GLuint        gl_program;
	bool          gl_vaos_need_update;
	struct glb_vao vao;
};


struct glb_state *
glb_state_new (void)
{
	struct glb_state *sta = malloc (sizeof (*sta));
	if (NULL == sta)
	{
		return NULL;
	}
	sta->want_shutdown = 0;
	sta->gl_program = 0;
	sta->gl_vaos_need_update = true;
	return sta;
}


void
glb_state_delete (struct glb_state *sta)
{
	free (sta);
}


static void
glb_sdl_kill (const char *msg)
{
	glb_log ("glb_sdl_kill message: %s sdl: %s", msg, SDL_GetError ());
	SDL_Quit ();
	exit (GLB_ERROR_SDL);
}


static void
glb_assert_sdl (void)
{
	const char *error = SDL_GetError ();
	if (*error != '\0')
	{
		glb_log ("SDL Error: %s", error);
		SDL_ClearError ();
		exit (GLB_ERROR_SDL);
	}
}


static const char *
glb_lookup_sdl_glattr_name (SDL_GLattr attr)
{
	const char *s = NULL;
	switch (attr)
	{
		case SDL_GL_CONTEXT_MAJOR_VERSION:
			s = "SDL_GL_CONTEXT_MAJOR_VERSION";
			break;
		case SDL_GL_CONTEXT_MINOR_VERSION:
			s = "SDL_GL_CONTEXT_MINOR_VERSION";
			break;
		case SDL_GL_CONTEXT_PROFILE_MASK:
			s = "SDL_GL_CONTEXT_PROFILE_MASK";
			break;
		default:
			s = "UNKNOWN ATTRIBUTE";
			break;
	}
	return s;
}


static int
glb_get_sdl_glattr (SDL_GLattr attr)
{
	int value = 0;
	const char *name = glb_lookup_sdl_glattr_name (attr);
	if (SDL_GL_GetAttribute (attr, &value) < 0)
	{
		glb_log ("SDL_GL_GetAttribute failed. name: %s, error: %s",
		        name,
		        SDL_GetError ());
		return -1;
	}
	return value;
}


static int
glb_gl_context_is_core (void)
{
	int value = 0;
	if (SDL_GL_GetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, &value) < 0)
	{
		glb_log ("SDL_GL_GetAttribute failed. name: %s, error: %s",
		        glb_lookup_sdl_glattr_name (SDL_GL_CONTEXT_PROFILE_MASK),
		        SDL_GetError ());
		return 0;
	}
	return 0 != (value & SDL_GL_CONTEXT_PROFILE_CORE);
}


static void
glb_log_gl_info (void)
{
	const unsigned char *vend  = glGetString (GL_VENDOR);
	const unsigned char *rend  = glGetString (GL_RENDERER);
	const unsigned char *ver   = glGetString (GL_VERSION);
	const unsigned char *slver = glGetString (GL_SHADING_LANGUAGE_VERSION);
	int major = glb_get_sdl_glattr (SDL_GL_CONTEXT_MAJOR_VERSION);
	int minor = glb_get_sdl_glattr (SDL_GL_CONTEXT_MINOR_VERSION);
	int core = glb_gl_context_is_core ();
	const char *line = "-----------------------------"
	                   "+-----------------------------";
	glb_log ("%s", line);
	glb_log ("GL_VENDOR                    | %s", vend);
	glb_log ("GL_RENDERER                  | %s", rend);
	glb_log ("GL_VERSION                   | %s", ver);
	glb_log ("GL_SHADING_LANGUAGE_VERSION  | %s", slver);
	glb_log ("SDL_GL_CONTEXT_MAJOR_VERSION | %d", major);
	glb_log ("SDL_GL_CONTEXT_MINOR_VERSION | %d", minor);
	glb_log ("SDL_GL_CONTEXT_PROFILE_CORE  | %d", core);
	glb_log ("%s", line);
}


struct glb_trimesh
{
	GLfloat *pos;
	GLfloat *col;
	int      pos_len;
	int      col_len;
};


static struct glb_vao
glb_gl_load_as_vao (struct glb_trimesh *tm)
{
	struct glb_vao vao;
	glGenVertexArrays (1, &vao.id);
	glBindVertexArray (vao.id);
	glGenBuffers (2, vao.vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vao.vbo[0]);
	glBufferData (GL_ARRAY_BUFFER,
	              tm->pos_len * sizeof (GLfloat),
	              tm->pos,
	              GL_STATIC_DRAW);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vao.vbo[1]);
	glBufferData (GL_ARRAY_BUFFER,
	              tm->col_len * sizeof (GLfloat),
	              tm->col,
	              GL_STATIC_DRAW);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (1);
	glBindVertexArray (0);
	vao.vert_count = tm->pos_len / 3;
	return vao;
}


static void
glb_gl_update_vaos (struct glb_state *sta)
{
	if (!sta->gl_vaos_need_update)
	{
		return;
	}
	GLfloat pos[] = 
	{
		  0.0f,  0.5f, 0.0f,
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f
	};
	GLfloat col[] = 
	{
		 1.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 1.0f
	};
	struct glb_trimesh mesh;
	mesh.pos = &pos[0];
	mesh.col = &col[0];
	mesh.pos_len = GLB_ARRAY_LEN (pos);
	mesh.col_len = GLB_ARRAY_LEN (col);
	sta->vao = glb_gl_load_as_vao (&mesh);
	sta->gl_vaos_need_update = false;
}


static void
glb_gl_render_frame (const struct glb_state *sta, int frame)
{
	float half_pi = 0.5 * M_PI;
	float r = 0.5 * (1 + sin (0.004 * frame - half_pi));
	float g = 0.5 * (1 + sin (0.01  * frame - half_pi));
	float b = 0.5 * (1 + sin (0.005 * frame - half_pi));
	glViewport (0, 0, 512, 512);
	glClearColor (r, g, b, 0.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray (sta->vao.id);
	glUseProgram (sta->gl_program);
	glDrawArrays (GL_TRIANGLES, 0, sta->vao.vert_count);
	glBindVertexArray (0);
}


static void
glb_log_sdl_keyboard_event (const char *type, const SDL_KeyboardEvent *ev)
{
	glb_log ("%s, scan: %s, key: %s",
	        type,
	        SDL_GetScancodeName (ev->keysym.scancode),
	        SDL_GetKeyName (ev->keysym.sym));
}


static void
glb_handle_keydown (const SDL_KeyboardEvent *ev, struct glb_state *st)
{
	glb_log_sdl_keyboard_event ("SDL_KEYDOWN", ev);
	switch (ev->keysym.sym)
	{
		case SDLK_ESCAPE:
		case SDLK_q:
			st->want_shutdown = 1;
			break;
		default:
			break;
	}
}


/**
 * @return Shader handle or 0 on failure.
 */
static GLuint
glb_load_shader (GLenum type, const char *source_code)
{
	GLuint shader;
	GLint compiled;
	shader = glCreateShader (type);
	if (shader == 0)
	{
		return 0;
	}
	glShaderSource (shader, 1, &source_code, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled)
	{
		return shader;
	}
	glb_log ("%s", "Failed to compile shader.");
	GLint info_len = 0;
	glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 1)
	{
		char *info_log = malloc (sizeof (char) * info_len);
		glGetShaderInfoLog (shader, info_len, NULL, info_log);
		glb_log ("%s", info_log);
		free (info_log);
	}
	glDeleteShader (shader);
	return 0;
}


/**
 * Read a file.
 * Caller must free the returned buffer.
 */
char * 
glb_read_file (const char *filepath)
{
	FILE *fptr;
	long length;
	char *buf;
	fptr = fopen (filepath, "rb");
	if (!fptr)
	{
		glb_log ("Could not read file: %s", filepath);
		return NULL;
	}
	fseek (fptr, 0, SEEK_END);
	length = ftell (fptr);
	buf = (char*) malloc (length + 1);
	fseek (fptr, 0, SEEK_SET);
	fread (buf, length, 1, fptr);
	fclose (fptr);
	buf[length] = 0;
	return buf;
}


/**
 * Read GLSL file, compile and return shader handle.
 *
 * @return Shader handle or 0 on failure.
 */
static GLuint
glb_load_glsl_file (GLenum type, const char *filepath)
{
	GLchar *source_code = glb_read_file (filepath);
	if (!source_code)
	{
		return 0;
	}
	GLuint shader = glb_load_shader (type, source_code);
	free (source_code);
	return shader;
}


static bool
glb_gl_init_shaders (struct glb_state *sta)
{
	glb_log ("%s", "Loading shaders...");
	const char *glslv_path = GLB_DATA_ROOT "shader.glslv";
	const char *glslf_path = GLB_DATA_ROOT "shader.glslf";
	GLuint vshader = glb_load_glsl_file (GL_VERTEX_SHADER, glslv_path);
	GLuint fshader = glb_load_glsl_file (GL_FRAGMENT_SHADER, glslf_path);
	if (glb_check_gl_errors () || 0 == vshader || 0 == fshader)
	{
		return false;
	}
	GLuint program = glCreateProgram ();
	if (0 == program)
	{
		return false;
	}
	glAttachShader (program, vshader);
	glAttachShader (program, fshader);
	glBindAttribLocation (program, 0, "i_vpos");
	glBindAttribLocation (program, 1, "i_vcol");
	glLinkProgram (program);
	GLint linked = 0;
	glGetProgramiv (program, GL_LINK_STATUS, &linked);
	if (linked)
	{
		sta->gl_program = program;
		glb_log ("%s", "Loading shaders...OK!");
		return true;
	}
	GLint info_len = 0;
	glGetProgramiv (program, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 1)
	{
		char *info_log = malloc (sizeof (char) * info_len);
		glGetProgramInfoLog (program, info_len, NULL, info_log);
		glb_log ("%s", info_log);
		free (info_log);
	}
	glDeleteProgram (program);
	return false;
}


static void
glb_handle_keyup (const SDL_KeyboardEvent *ev)
{
	(void) ev;
}


static void
glb_handle_sdl_events (struct glb_state *st)
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_KEYDOWN:
				glb_handle_keydown (&ev.key, st);
				break;
			case SDL_KEYUP:
				glb_handle_keyup (&ev.key);
				break;
			default:
				break;
		}
	}
}


static void
glb_main_run (SDL_Window *wnd, struct glb_state *sta)
{
	int frame = 0;
	for (;;)
	{
		glb_handle_sdl_events (sta);
		if (1 == sta->want_shutdown)
		{
			break;
		}
		glb_gl_update_vaos (sta);
		glb_gl_render_frame (sta, frame);
		SDL_GL_SwapWindow (wnd);
		frame++;
	}
}


static void
glb_sdl_shutdown (SDL_Window *wnd, SDL_GLContext ctx)
{
	SDL_GL_DeleteContext (ctx);
	SDL_DestroyWindow (wnd);
	SDL_Quit ();
}


static GLADvoidfn
glb_gl_getprocaddress(const char *name)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	return (GLADvoidfn) SDL_GL_GetProcAddress(name);
#pragma GCC diagnostic pop
}


static void
glb_sdl_init (SDL_Window **wnd_out, SDL_GLContext *ctx_out)
{
	SDL_Window    *wnd = NULL;
	SDL_GLContext  ctx = NULL;
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		glb_sdl_kill ("Unable to initialize SDL");
	}
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK,
	                     SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
	wnd = SDL_CreateWindow ("glb",
	                        SDL_WINDOWPOS_CENTERED,
	                        SDL_WINDOWPOS_CENTERED,
	                        512,
	                        512,
	                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (NULL == wnd)
	{
		glb_sdl_kill ("Unable to create window");
	}
	glb_assert_sdl ();
	glb_log ("%s", "Created window.");
	ctx = SDL_GL_CreateContext (wnd);
	if (NULL == ctx)
	{
		glb_sdl_kill ("Could not create gl context.");
	}
	glb_assert_sdl ();
	glb_log ("%s", "Created context.");
	glb_log ("%s", "gladLoadGLLoader...");
	if (!gladLoadGLLoader(glb_gl_getprocaddress))
	{
		glb_log ("%s", "gladLoaderGLLoader failed.");
		glb_sdl_shutdown (wnd, ctx);
		exit (GLB_ERROR_LOAD_GL_FUNCS);
	}
	if (glb_check_gl_errors ())
	{
		glb_sdl_shutdown (wnd, ctx);
		exit (GLB_ERROR_LOAD_GL_FUNCS);
	}
	glb_log ("%s", "gladLoadGLLoader...OK!");
	glb_log_gl_info ();
	if (!GLAD_GL_VERSION_3_2)
	{
		glb_log ("%s", "Failed to find OpenGL 3.2 or above.");
		glb_sdl_shutdown (wnd, ctx);
		exit (GLB_ERROR_INSUFFICIENT_GL_VERSION);
	}
	SDL_GL_SetSwapInterval (1);
	*wnd_out = wnd;
	*ctx_out = ctx;
}


int
main (int argc, char **argv)
{
	(void) argc;
	(void) argv;
	SDL_Window *wnd = NULL;
	SDL_GLContext ctx = NULL;
	glb_log_open ();
	glb_sdl_init (&wnd, &ctx);
	struct glb_state *sta = glb_state_new ();
	if (glb_gl_init_shaders (sta))
	{
		glb_main_run (wnd, sta);
	}
	glb_state_delete (sta);
	sta = NULL;
	glb_sdl_shutdown (wnd, ctx);
	wnd = NULL;
	ctx = NULL;
	glb_log_close ();
	return 0;
}


