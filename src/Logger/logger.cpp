//
// Created by Alex on 3/4/24.
//

#include <mutex>
#include <iostream>
#include "Logger/logger.hpp"

namespace Pronounce {

	Logger* Logger::pinstance_{nullptr};
	pros::Task Logger::task{[]() -> void {pros::Task::delay(50); Logger::update();}};
	bool Logger::installed{false};
	uint32_t Logger::currentIndex;
	std::string Logger::fileName;
	std::string Logger::buffer;

	void Logger::update() {

		if (pros::c::usd_is_installed() && !installed) {
			newFile();
			installed = true;
			if (buffer.size() > 1E6) {
				buffer = "";
			}
			return;
		} else if (installed && !pros::c::usd_is_installed()){
			installed = false;
			if (buffer.size() > 1E6) {
				buffer = "";
			}
			return;
		} else if (installed) {
			if (buffer.size() > 1E6) {
				buffer = "";
			}
		}

		FILE* logFile = fopen(fileName.c_str(), "a");

		if (logFile == nullptr) {
			if (buffer.size() > 1E6) {
				buffer = "";
			}
			return;
		}

		fputs(buffer.c_str(), logFile);
		fclose(logFile);

		buffer = "";
	}

	void Logger::log(const std::string &className, const std::string &logData) {
		Logger::writeLog(className + " " + std::to_string(pros::millis()) + ": " + logData + "\n");
	}

	void Logger::newFile() {
		FILE *indexFile = fopen(INDEX_FILE, "rb");

		uint32_t currentIndex;

		if (indexFile == nullptr) {
			currentIndex = 0;
			fclose(indexFile);
		} else {
			fread(&currentIndex, sizeof(uint32_t), 1, indexFile);
		}

		currentIndex++;

		Logger::currentIndex = currentIndex;

		indexFile = fopen(INDEX_FILE, "wb");

		fwrite(&currentIndex, sizeof(uint32_t), 1, indexFile);

		fclose(indexFile);

		fileName = "/usd/" + std::to_string(currentIndex) + ".log";

	}

	uint32_t Logger::getCurrentIndex() {
		return currentIndex;
	}

	const std::string &Logger::getFileName() {
		return fileName;
	}

	void Logger::writeLog(const std::string &write) {
		buffer.append(write);
		std::cout << write;
	}
} // Pronounce