#include "Logger.hpp"


#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>

using namespace std;


Logger::LogEntry::LogEntry(const vector<double>& _values,
	const pair<double, double>& _bounds, unsigned int _height)
	: values(_values)
	,	bounds(_bounds)
	,	height(_height) {
}

Logger::Logger(const vector<string>& _variables,
	const vector<pair<double, double>>& _bounds, unsigned int _maxDatapoints,
	unsigned int _width, unsigned int _heightPerVar,
	const std::string& _windowName)
	: maxDatapoints{_maxDatapoints}
	,	window(sf::VideoMode(_width,
		(_heightPerVar + PADDING) *  _variables.size() + PADDING), _windowName)
	, width{_width}
	,	heightPerVar{_heightPerVar}
	,	threadRun{true}
	,	updateThread(bind(&Logger::updateRoutine, this)) {

	if(!font.loadFromFile("font.ttf")) {
		cout << "[Error] Logger::Logger: Unable to load font file" << endl;
	}

	window.setActive(false);

	if(_variables.size() != _bounds.size())
		throw runtime_error("Logger::Logger: Variables and bounds vectors have "
			"different dimensions");
	
	//Construct the values map
	for(unsigned int i = 0; i < _variables.size(); ++i) {
		values.insert({_variables[i], {{}, _bounds[i], i}});
	}
}

Logger::~Logger() {
	threadRun = false;

	//Wait for thread
	updateThread.join();
}

void Logger::update(const std::string& _name, double _value) {
	unique_lock<mutex> valuesLock(valuesMutex);

	auto itr = values.find(_name);

	if(itr == values.end()) {
		throw runtime_error("Logger::update: Invalid variable name \""
			+ _name + "\"");
	}

	itr->second.values.push_back(_value);

	if(itr->second.values.size() > maxDatapoints)
		itr->second.values.erase(itr->second.values.begin());
}

void Logger::updateWindow() {
	//Need to update window events from main thread
	//Needed because of bug #989

	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed)
			window.close();
	}
}

void Logger::updateRoutine() {

	sf::sleep(sf::milliseconds(100));

	while(threadRun && window.isOpen()) {

		window.clear(sf::Color(32, 32, 32));

		{
			unique_lock<mutex> valuesLock(valuesMutex);
			for(auto& log : values) {

				drawEntry(window, log.first, log.second);
			}
		}

		window.display();

		sf::sleep(sf::milliseconds(10));
	}

	if(window.isOpen())
		window.close();
}

void Logger::drawEntry(sf::RenderWindow& window, const string& _name,
	const Logger::LogEntry& _log) {
	static const sf::Color COLORS[] =
		{sf::Color::Red, sf::Color::Green, sf::Color::Blue};
	static const unsigned int COLOR_COUNT = 3;
	
	unsigned int y = _log.height * (heightPerVar + PADDING) + PADDING;

	sf::RectangleShape box(sf::Vector2f(width - 2*PADDING, heightPerVar));
	box.setPosition(PADDING, y);
	box.setFillColor(sf::Color::Black);
	box.setOutlineColor(sf::Color(192, 192, 192));

	window.draw(box);

	if(_log.values.size() > 1) {
		//Construct the line
		vector<sf::Vertex> points;
		double lowerBound = _log.bounds.first, upperBound = _log.bounds.second;

		for(unsigned int i = 0; i < _log.values.size(); ++i) {
			double val = _log.values[i];
			sf::Color c;
			if(val < lowerBound || val > upperBound)
				c = sf::Color(112, 112, 112);
			else
				c = COLORS[_log.height % COLOR_COUNT];

			val = max(lowerBound, min(val, upperBound));

			points.emplace_back(
				sf::Vector2f(PADDING + i*(width - 2*PADDING)/(maxDatapoints),
					y + (1. - ((val - lowerBound)/(upperBound - lowerBound)))*heightPerVar),
				c);
		}

		//Grid lines
		for(unsigned int i = 0; i <= 4; ++i) {
			unsigned int height = y + i*heightPerVar/4;
			auto color = sf::Color(112, 112, 112);
			sf::Vertex line[2] = {
				{sf::Vector2f(PADDING, height), color},
				{sf::Vector2f(width - PADDING, height), color}
			};

			window.draw(line, 2, sf::Lines);
		}

		window.draw(&(points[0]), _log.values.size(), sf::LineStrip);
	}

	sf::Text name;
	name.setFont(font);
	name.setString(_name);
	name.setCharacterSize(PADDING/2);
	name.setFillColor(COLORS[_log.height % COLOR_COUNT]);

	unsigned int textWidth = name.getLocalBounds().width;

	name.setPosition((width -textWidth)/2, y - 3*PADDING/4);

	window.draw(name);

}
