#include "Sim.h"

Sim::Sim(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initial fps value
	fps = 0.0f;

	// Set up font
	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Font can't load" << std::endl;
	}

	isServer = false;
	isClient = false;

	// Choice text
	std::cout << "Press '1' for Server , '2' for Client." << std::endl;
}

Sim::~Sim()
{
}

void Sim::update(float dt)
{
	// Calculate FPS
	fps = 1.0f / dt;

	if (!isClient && !isServer)
	{
		if (input->isKeyDown(sf::Keyboard::Num1))
		{
			input->setKeyUp(sf::Keyboard::Num1);
			isServer = true;
			// Set up server
			server = new Server(window, input);
		}
	}

	if (!isServer && !isClient)
	{
		if (input->isKeyDown(sf::Keyboard::Num2))
		{
			input->setKeyUp(sf::Keyboard::Num2);
			isClient = true;
			// Set up client
			client = new Client(window, input);
		}
	}

	if (isServer)
	{
		// Update server
		server->update(dt);
	}

	if (isClient)
	{
		// Update client
		client->update(dt);
	}

	// Update all the text variables
	updateText();
}

void Sim::render()
{
	beginDraw();

	if (isServer)
	{
		// Render server stuff
		server->render(window);
	}

	if (isClient)
	{
		// Render client stuff
		client->render(window);
	}

	// Draw FPS text
	window->draw(fpsText);

	endDraw();
}

void Sim::reset()
{
}

void Sim::updateText()
{
	// FPS text
	fpsText.setFont(font);
	fpsText.setCharacterSize(12);
	fpsText.setString("FPS: " + std::to_string(fps));
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition(window->getSize().x - 162, 0);	
}

void Sim::beginDraw()
{
	if (isServer)
	{
		window->clear(sf::Color::Black);
	}
	if (isClient)
	{
		// Initialise random number generator
		//srand(time(NULL));

		//// Calculate random float from 0 - 255 for colour values of boids
		//float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
		//float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
		//float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
		//sf::Color col(r, g, b, 255.0f);
		//window->clear(col);
		window->clear(sf::Color(0, 0, 125, 255));
	}
	else
	{
		window->clear(sf::Color::Black);
	}
}

void Sim::endDraw()
{
	window->display();
}
