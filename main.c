#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#define WIDTH 800
#define HEIGHT 600
#define RAND_NUM 32767
#define SCROLL_MUL 5

extern float monteCarlo(int rand_count, float rands[][2], float square[3],
	float circle[3], int width, int height, unsigned char* buffer);

float square[3] = { 400, 450, 50 };
float circle[3] = { 400, 400, 50 };
float* rands;

unsigned char* pixels;

double mouse[2] = {0};
double pressing_square = 0;
double pressing_circle = 0;

int within_square()
{
	return  fabs(square[0] - mouse[0]) < square[2] &&
			fabs(square[1] - mouse[1]) < square[2];
}

int within_circle()
{
	float dx = circle[0] - mouse[0];
	float dy = circle[1] - mouse[1];

	return dx * dx + dy * dy < circle[2] * circle[2];
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(button != 0) return;

	if(action == GLFW_PRESS)
	{
		pressing_square = within_square();
		pressing_circle = within_circle();
	}
	else if(action == GLFW_RELEASE)
	{
		pressing_square = 0;
		pressing_circle = 0;
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	ypos = HEIGHT - ypos;

	double x_diff = xpos - mouse[0];
	double y_diff = ypos - mouse[1];

	mouse[0] = xpos;
	mouse[1] = ypos;

	if(pressing_square)
	{
		square[0] += x_diff;
		square[1] += y_diff;
	}

	if(pressing_circle)
	{
		circle[0] += x_diff;
		circle[1] += y_diff;
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(pressing_square || within_square())
	{
		square[2] += yoffset * SCROLL_MUL;
	}

	if(pressing_circle || within_circle())
	{
		circle[2] += yoffset * SCROLL_MUL;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glFlush();
}

int main()
{
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(
		WIDTH, HEIGHT, "MonteCarlo", NULL, NULL);
	if(!window)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glfwSetWindowSizeLimits(window, WIDTH, HEIGHT, WIDTH, HEIGHT);

	glfwMakeContextCurrent(window);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	rands = malloc(sizeof(float) * 2 * RAND_NUM);
	if(!rands)
	{
		fprintf(stderr, "Failed to allocate rands memory.\n");
		goto goto_glfw;
	}

	for(int i = 0; i < RAND_NUM; ++i) {
		rands[i * 2 + 0] = ((double) rand() / RAND_MAX) * (WIDTH - 1);
		rands[i * 2 + 1] = ((double) rand() / RAND_MAX) * (HEIGHT - 1);
	}

	pixels = malloc(WIDTH * HEIGHT * 4);
	if(!pixels)
	{
		fprintf(stderr, "Failed to allocate pixel memory.\n");
		goto goto_rands;
	}

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		float count = monteCarlo(RAND_NUM, (float (*)[2]) rands,
			square, circle, WIDTH, HEIGHT, pixels);

		printf("%.02f%%\n", count);

		display();

		glfwSwapBuffers(window);
	}

	goto_pixels:
	free(pixels);

	goto_rands:
	free(rands);

	goto_glfw:
	glfwTerminate();

	return 0;
}
