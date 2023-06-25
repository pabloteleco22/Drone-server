/**
 * The MIT License (MIT)
 * Copyright (c) 2023 Pablo López Sedeño
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#pragma once

#include <string>
#include <mutex>

using std::string;
using std::mutex;

class ProRetCod {
	int code;
	string message;

	protected:
		/**
		 * @brief To create an instance with an error code and a particular message
		*/
		ProRetCod(int code, string message) : code{code}, message{message} {}

	public:
		ProRetCod(const ProRetCod &other) : code{other.code}, message{other.message} {}
		ProRetCod &operator=(const ProRetCod &other) {
			code = other.code;
			message = other.message;

			return *this;
		}
		bool operator==(const ProRetCod &other) const {
			return ((code == other.code) and (message == other.message));
		}
		bool operator!=(const ProRetCod &other) const {
			return ((code != other.code) or (message != other.message));
		}

		int get_code() const {
			return code;
		}
		string get_string() const {
			return message;
		}
};

struct OkCode : public ProRetCod {
	OkCode() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct BadArgument : public ProRetCod {
	BadArgument() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct ConnectionFailed : public ProRetCod {
	ConnectionFailed() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct NoSystemsFound : public ProRetCod {
	NoSystemsFound() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct TelemetryFailure : public ProRetCod {
	TelemetryFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct ActionFailure : public ProRetCod {
	ActionFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct OffboardFailure : public ProRetCod {
	OffboardFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct MissionFailure : public ProRetCod {
	MissionFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

struct UnknownFailure : public ProRetCod {
	UnknownFailure() : ProRetCod(code, message) {}

	static const int code;
	static const string message;
};

/** Check enough systems **/
class CheckEnoughSystems {
	protected:
	 	mutable mutex mut;
		const float expected_systems;
		float number_of_systems{0};

	public:
		/**
		 * @param expected_systems: maximum number of systems expected
		*/
		CheckEnoughSystems(float expected_systems) : expected_systems{expected_systems} {}
		virtual ~CheckEnoughSystems() {}

		/**
		 * @brief Indicates whether there are enough systems
		*/
		virtual bool exists_enough_systems() const = 0;

		/**
		 * @brief Generates a message in a string indicating the status of the test
		*/
		virtual string get_status() const = 0;

		/**
		 * @brief Appends a system to the systems counter
		*/
		virtual void append_system(float num=1);

		/**
		 * @brief Subtracts a system to the systems counter
		*/
		virtual void subtract_system(float num=1);

		/**
		 * @brief Gets the value of the systems counter
		*/
		virtual float get_number_of_systems() const;
};

struct PercentageCheck final : public CheckEnoughSystems {
	PercentageCheck(const float expected_systems, const float percentage_drones_required=100.0f);

	/**
	 * @brief Indicates whether there are enough systems
	*/
	bool exists_enough_systems() const override;

	/**
	 * @brief Generates a message in a string indicating the status of the test
	*/
	string get_status() const override;

	private:
		float percentage_required;
		float required_systems;
};