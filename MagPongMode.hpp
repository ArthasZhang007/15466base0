#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include <time.h>
namespace
{
	float getrand(float l, float r)
	{
		float t = (float)rand() / RAND_MAX;
		return l + (r - l) * t;
	};
}; // namespace
/*
 * MagPongMode is a game mode that implements a single-player game of MagPong.
 */

struct MagPongMode : Mode
{
	MagPongMode();
	virtual ~MagPongMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	glm::vec2 court_radius = glm::vec2(7.0f, 5.0f);
	glm::vec2 paddle_radius = glm::vec2(0.2f, 1.0f);
	glm::vec2 ball_radius = glm::vec2(0.2f, 0.2f);
	glm::vec2 ai_radius = glm::vec2(0.2f, 5.0f);

	glm::vec2 left_paddle = glm::vec2(-court_radius.x + 0.5f, 0.0f);
	glm::vec2 right_paddle = glm::vec2(court_radius.x - 0.5f, 0.0f);

	size_t num_balls = 5;
	char paddle_mode = 'N';
	struct boken
	{
		boken(glm::vec2 a, glm::vec2 b, std::deque<glm::vec3> lis) : loc(a), v(b), ball_trail(lis){};
		glm::vec2 loc;
		glm::vec2 v;
		std::deque<glm::vec3> ball_trail;
	};
	std::vector<boken> balls;
	float ball_init_speed = 1.0f;

	//glm::vec2 ball = glm::vec2(0.0f, 0.0f);
	//glm::vec2 ball_velocity = glm::vec2(-1.0f, 0.0f);

	uint32_t left_score = 0;
	uint32_t right_score = 0;

	float ai_offset = 0.0f;
	float ai_offset_update = 0.0f;

	//----- pretty gradient trails -----

	float trail_length = 1.3f;
	std::deque<glm::vec3> ball_trail; //stores (x,y,age), oldest elements first

	//----- opengl assets / helpers ------

	//draw functions will work on vectors of vertices, defined as follows:
	struct Vertex
	{
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) : Position(Position_), Color(Color_), TexCoord(TexCoord_) {}
		glm::vec3 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 4 * 3 + 1 * 4 + 4 * 2, "MagPongMode::Vertex should be packed");

	//Shader program that draws transformed, vertices tinted with vertex colors:
	ColorTextureProgram color_texture_program;

	//Buffer used to hold vertex data during drawing:
	GLuint vertex_buffer = 0;

	//Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
	GLuint vertex_buffer_for_color_texture_program = 0;

	//Solid white texture:
	GLuint white_tex = 0;

	//matrix that maps from clip coordinates to court-space coordinates:
	glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
	// computed in draw() as the inverse of OBJECT_TO_CLIP
	// (stored here so that the mouse handling code can use it to position the paddle)
};
