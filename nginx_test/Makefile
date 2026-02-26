all:
	docker-compose up --build -d

down:
	docker-compose down

clean:
	docker-compose down -v

fclean: clean
	docker system prune -af