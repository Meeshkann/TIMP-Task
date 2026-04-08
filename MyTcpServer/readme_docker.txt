# сборка образа
docker build -t timp-echo-server .

# запуск контейнера
docker run -d -p 54678:54678 --name my-server timp-echo-server

# остановка
docker stop my-server

# удаление контейнера
docker rm my-server