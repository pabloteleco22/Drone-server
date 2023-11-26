help:
	@echo Commands:
	@echo - docker-config-build: Make the build directory.
	@echo - docker-build: Build the project.
	@echo - docker-run: Run the application.
	@echo "      Must pass a ports variable with the ports to listen."
	@echo "      <make docker-run ports=\"14540 14541\">"

docker-config-build:
	docker exec -ti mavsdk bash -c "cd /server && cmake -Bbuild -H."

docker-build:
	docker exec -ti mavsdk bash -c "cd /server && cmake --build build -j4"

docker-run:
	docker exec -ti mavsdk bash -c "cd /server && build/drone_server $(ports)"

docker-run-tests:
	docker exec -ti mavsdk bash -c "cd /server && ./run_tests.bash"

docker-run-drone-server-n-times:
	docker exec -ti mavsdk bash -c "cd /server && ./run_drone_server_n_times.bash $(n)"