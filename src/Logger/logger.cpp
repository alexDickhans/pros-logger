//
// Created by Alex on 3/4/24.
//

#include <mutex>
#include <iostream>
#include "Logger/logger.hpp"

namespace Pronounce {

	Logger* Logger::pinstance_{nullptr};
	pros::Mutex Logger::mutex_;

	Logger::Logger() {
		if (pros::c::usd_is_installed()) {
			newFile();
			installed = true;
		}

		task_ = std::make_shared<pros::Task>([=]() -> void {
			while (true) {
				pros::Task::delay(50);
				this->update();
			}
		});
	}

	Logger *Logger::getInstance() {
		std::lock_guard<pros::Mutex> lock(mutex_);
		if (pinstance_ == nullptr) {
			pinstance_ = new Logger();
		}
		return pinstance_;
	}

	void Logger::update() {
		uint32_t t = pros::micros();

		if (pros::c::usd_is_installed() && !installed) {
			newFile();
			installed = true;
		} else if (installed && !pros::c::usd_is_installed()){
			installed = false;
		}

		logFile = fopen(fileName.c_str(), "a");

		if (logFile == nullptr) {
			return;
		}

		fputs(buffer.c_str(), logFile);
		fclose(logFile);

		buffer = "";
	}

	void Logger::log(const std::string &className, const std::string &logData) {
		this->writeLog(className + " " + std::to_string(pros::millis()) + ": " + logData + "\n");
	}

	const std::string &Logger::getFileName() const {
		return fileName;
	}

	uint32_t Logger::getCurrentIndex() const {
		return currentIndex;
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

		this->currentIndex = currentIndex;

		indexFile = fopen(INDEX_FILE, "wb");

		fwrite(&currentIndex, sizeof(uint32_t), 1, indexFile);

		fclose(indexFile);

		fileName = "/usd/" + std::to_string(currentIndex) + ".log";

	}
} // Pronounce