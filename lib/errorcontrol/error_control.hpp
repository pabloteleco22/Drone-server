#pragma once

#include <string>
#include <mutex>

using std::string;
using std::mutex;

class ProRetCod {
	int code;
	string message;

	protected:
		ProRetCod(int code, string message) : code{code}, message{message} {}

	public:
		ProRetCod(const ProRetCod &other) : code{other.code}, message{other.message} {}
		ProRetCod &operator=(const ProRetCod &other) {
			code = other.code;
			message = other.message;

			return *this;
		}
		bool operator==(const ProRetCod &other) {
			return code == other.code;
		}
		bool operator!=(const ProRetCod &other) {
			return code != other.code;
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
		CheckEnoughSystems(float expected_systems) : expected_systems{expected_systems} {}
		virtual ~CheckEnoughSystems() {}
		virtual bool exists_enough_systems() const = 0;
		virtual string get_status() const = 0;
		virtual void append_system(float num=1);
		virtual void subtract_system(float num=1);
		virtual float get_number_of_systems() const;
};

struct PercentageCheck final : public CheckEnoughSystems {
	PercentageCheck(const float expected_systems, const float percentage_drones_required=100.0f);

	bool exists_enough_systems() const override;
	string get_status() const override;

	private:
		float percentage_required;
		float required_systems;
};