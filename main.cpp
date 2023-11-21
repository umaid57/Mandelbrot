#include "ComplexPlane.h"

using namespace std;


int main() {

	int screenWidth = VideoMode::getDesktopMode().width / 2;
	int screenHeight = VideoMode::getDesktopMode().height / 2;

	VideoMode vm(screenWidth, screenHeight);
	// Create and open a window for the game 
	RenderWindow window(vm, "Mandelbrot", Style::Default);

	ComplexPlane plane(screenWidth, screenHeight);

	sf::Font font;
	font.loadFromFile("Octosquares.ttf");
	sf::Text text;
	text.setFont(font);

	while (window.isOpen()) {

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					plane.zoomOut();
					plane.setCenter(Vector2i(event.mouseButton.x, event.mouseButton.y));
				}
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					plane.zoomIn();
					plane.setCenter(Vector2i(event.mouseButton.x, event.mouseButton.y));
				}
			}
			if (event.type == sf::Event::MouseMoved) {
				plane.setMouseLocation(Vector2i(event.mouseMove.x, event.mouseMove.y));
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		plane.updateRender();
		plane.loadText(text);

		window.clear();

		window.draw(plane);
		window.draw(text);

		window.display();
		
	
	}
	return 0;
}