#pragma once

#include <vector>
#include <utility>
#include <map>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>

#include <SFML/Graphics.hpp>

class Logger
{
public:
	Logger(const std::vector<std::string>&,
		const std::vector<std::pair<double, double>>&,
		unsigned int, unsigned int, unsigned int, const std::string&);
	~Logger();

	void update(const std::string&, double);

	void updateWindow();
private:
	const unsigned int PADDING = 20;

	struct LogEntry {
		LogEntry(const std::vector<double>&, const std::pair<double, double>&,
			unsigned int);
		std::vector<double> values;
		std::pair<double, double> bounds;
		unsigned int height;
	};

	void updateRoutine();
	void drawEntry(sf::RenderWindow&, const std::string&, const LogEntry&);

	std::map<std::string, LogEntry> values;
	unsigned int maxDatapoints;
	std::mutex valuesMutex;

	sf::RenderWindow window;
	sf::Font font;
	unsigned int width, heightPerVar;

	std::atomic<bool> threadRun;
	std::thread updateThread;
};
