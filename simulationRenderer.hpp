#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <sstream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "random.hpp"
#include "utils.hpp"
#include "map.hpp"
#include "simulation.hpp"

class SimulationRenderer : public sf::Drawable
{
	Simulation& sim;
	int worldRes;

	static inline bool glewIsInit = false;

	sf::Shader shader;
	GLuint vertexbuffer;
	GLuint VertexArrayID;

public:
	SimulationRenderer(Simulation& sim, int worldRes) : sim(sim), worldRes(worldRes)
	{
		shader.loadFromFile("./vertex.glsl", "./geometry.glsl", "./fragment.glsl");

		//window.setActive(true);

		if (!glewIsInit)
		{
			glewIsInit = true;
			glewInit();
		}


		glGenVertexArrays(1, &VertexArrayID);

		glBindVertexArray(VertexArrayID);
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexbuffer);
		// The following commands will talk about our 'vertexbuffer' buffer
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// Give our vertices to OpenGL.
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * sim.particles.size(), reinterpret_cast<char*>(sim.particles.data()), GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		target.pushGLStates();

		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * sim.particles.size(), reinterpret_cast<char*>(sim.particles.data()), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_INT,           // type
			GL_FALSE,
			sizeof(Particle),                  // stride
			(void*)offsetof(Particle, pos)            // array buffer offset
		);
		glVertexAttribIPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			1,                  // size
			GL_INT,           // type
			sizeof(Particle),                  // stride
			(void*)offsetof(Particle, index)            // array buffer offset
		);
		glVertexAttribIPointer(
			2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			1,                  // size
			GL_INT,           // type
			sizeof(Particle),                  // stride
			(void*)offsetof(Particle, type)            // array buffer offset
		);

		const auto view = target.getView();

		const auto shaderId = shader.getNativeHandle();
		glUseProgram(shaderId);
		glProgramUniform2f(shaderId, glGetUniformLocation(shaderId, "resolution"), view.getSize().x / worldRes, view.getSize().y / worldRes);
		glProgramUniform4fv(shaderId, glGetUniformLocation(shaderId, "colors"), cellColors.size(), (float*)cellColors.data());
		//glProgramUniform2f(shaderId, glGetUniformLocation(shaderId, "size"), view.getSize().x / map.getWidth(), view.getSize().y / map.getHeight());


		glDrawArrays(GL_POINTS, 0, sim.particles.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);

		target.popGLStates();
		target.resetGLStates();
	}
};