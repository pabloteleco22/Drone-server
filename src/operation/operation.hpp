/** Copyright (C) 2023  Pablo López Sedeño
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <string>
#include <barrier>
#include <mutex>
#include <functional>

#include "../errorcontrol/error_control.hpp"

using std::string;
using std::mutex;
using std::function;
using std::barrier;

class OperationTools {
	OkCode ok_code;
	ProRetCod operation_code{ok_code};
	string operation_name;
	mutable mutex mut;
	bool critical{false};

	public:
		OperationTools();

		/**
		 * @brief Return the status of the operation
		*/
		ProRetCod get_status_code() const;

		/**
		 * @brief Indicates if a critical error has occurred
		*/
		bool is_critical() const;

		/**
		 * @brief Returns the name of the operation
		*/
		string get_name() const;

		/**
		 * @brief To establish whether an error has occurred and whether it has been critical
		*/
		void set_failure(const ProRetCod error_code, bool critical=false);

		/**
		 * @brief Change the operation name
		*/
		void set_name(const string operation_name);
};

class Operation {
    OperationTools *operation_tools;
	barrier<function<void()>> *sync_point;

    public:
        Operation(OperationTools &operation_tools, barrier<function<void()>> *sync_point);

		/**
		 * @brief Starts a new operation
		 * @tparam T: it is the type that accepts the operation callback
		 * @param operation_name: the name of the operation
		 * @param operation_action: the callback of the operation
		 * @param operation_args: the operation callback arguments
		*/
		template<typename T>
		ProRetCod new_operation(string operation_name,
							function<ProRetCod(OperationTools &, T *)> operation_action,
							T *operation_args) {

		operation_tools->set_name(operation_name);

		ProRetCod ret{operation_action(*operation_tools, operation_args)};

		OkCode ok_code;

		if (ret == ok_code)
			sync_point->arrive_and_wait();
		else
			sync_point->arrive_and_drop();

		return ret;
	}
};