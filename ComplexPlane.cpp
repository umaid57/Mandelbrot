#include "ComplexPlane.h"

using namespace std;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight) {
	
	m_pixel_size = { pixelWidth, pixelHeight };
	m_aspectRatio = static_cast<float>(pixelHeight) / pixelWidth;
	m_plane_center = { 0,0 };
	m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
	m_zoomCount = 0;
	m_State = State::CALCULATING;
	m_vArray.setPrimitiveType(Points);
	m_vArray.resize(pixelWidth * pixelHeight);

}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const {

	target.draw(m_vArray);

}

void ComplexPlane::multiRender(int startX, int stopX) {
	
	for (int i = 0; i < m_pixel_size.y; i++) {
		for (int j = startX; j < stopX; j++) {
			
			m_vArray[j + i * m_pixel_size.x].position = { (float)j,(float)i };

			Vector2i pixel(m_vArray[j + i * m_pixel_size.x].position.x,
				m_vArray[j + i * m_pixel_size.x].position.y);

			Vector2f coord = mapPixelToCoords(pixel);
			int iterations = countIterations(coord);

			Uint8 r, g, b;
			iterationsToRGB(iterations, r, g, b);
			m_vArray[j + i * m_pixel_size.x].color = { r, g, b };

		}
	}
}

void ComplexPlane::updateRender() {
	const auto processor_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	if (m_State == State::CALCULATING) {
		for (auto i = 0; i < processor_count; i++) {
			int startX = i * (m_pixel_size.x / processor_count);
			int stopX = (i + 1) * (m_pixel_size.x / processor_count);

			threads.push_back(std::thread(&ComplexPlane::multiRender, this, startX, stopX));

		}
		for (auto& thread : threads) {
			thread.join();
		}

		m_State = State::DISPLAYING;
	
	}

}

void ComplexPlane::zoomIn() {
	
	m_zoomCount++;
	float zoomX = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float zoomY = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size = { zoomX, zoomY };
	m_State = State::CALCULATING;

}

void ComplexPlane::zoomOut() {
	
	m_zoomCount--;
	float zoomX = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
	float zoomY = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
	m_plane_size = { zoomX, zoomY };
	m_State = State::CALCULATING;

}

void ComplexPlane::setCenter(Vector2i mousePixel) {
	
	m_plane_center = mapPixelToCoords(mousePixel);
	m_State = State::CALCULATING;

}

void ComplexPlane::setMouseLocation(Vector2i mousePixel) {

	m_mouseLocation = mapPixelToCoords(mousePixel);

}

void ComplexPlane::loadText(Text& text) {
	
	ostringstream info;
	info << "Mandlebrot Set" <<  endl;
	info << "Center: (" << m_plane_center.x << ", "  << m_plane_size.y << ")" << endl;
	info << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")" << endl;
	info << "Left-click to Zoom in" << endl;
	info << "Right-click to Zoom out" << endl;

	text.setCharacterSize(20);

	text.setFillColor(sf::Color::White);

	text.setString(info.str());

	text.setPosition(0, 0);

}

int ComplexPlane::countIterations(Vector2f coord) {

	int iterations = 0;

	float re = coord.x;
	float im = coord.y;
	complex<double> c(re, im);
	complex<double> z(0, 0);
	
	for (int i = 0; i <= MAX_ITER; i++) {
		z = z * z + c;
		if (abs(z) > 2.0) {
			return iterations;
		}
		else {
			iterations++;
		}
	}
	
	return iterations;

}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b) {
	Uint8 baseColor = static_cast<Uint8>(255 * count / MAX_ITER);
	if (count == MAX_ITER) {
		r = 0;
		g = 0;
		b = 0;
	}

	else if (count < MAX_ITER / 5) {
		r = 21;
		g = 182;
		b = 221;
	}
	else if (count < 2 * MAX_ITER / 5) {
		r = 0;
		g = baseColor;
		b = baseColor;
	}
	else if (count < 3 * MAX_ITER / 5) {
		r = 0;
		g = baseColor;
		b = 0;
	}
	else if (count < 4 * MAX_ITER / 5) {
		r = baseColor;
		g = baseColor;
		b = 0;
	}
	else {
		r = baseColor;
		g = 0;
		b = 0;
	}
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel) {

	// formula to map a value n from range [a,b] into range [c,d] is: 
	//((n - a) / (b - a))* (d - c) + c

	float x = ((static_cast<float>(mousePixel.x) - 0) / (m_pixel_size.x - 0)) 
									* m_plane_size.x + m_plane_center.x - m_plane_size.x / 2.0;

	float y = ((static_cast<float>(mousePixel.y) - m_pixel_size.y) / (0 - m_pixel_size.y)) 
									* m_plane_size.y + m_plane_center.y - m_plane_size.y / 2.0;

	return Vector2f(x, y);

}
