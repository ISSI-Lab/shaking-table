#!/bin/sh

#stop all the running docker containers and then remove all of them
docker stop $(docker ps -a -q)
docker rm $(docker ps -a -q)
#remove the image called "python" and "flask"
docker image rm $(docker images 'python' -a -q | head -n 1)
docker image rm $(docker images 'flask' -a -q | head -n 1)

#after clean up, build the image called "flask"
docker build -t flask .
#run the container and expose the port 5000
docker run -d -v"$PWD"/app/static:/app/static -v"$PWD"/app/templates:/app/templates -v"$PWD"/app/log:/app/log -v"$PWD"/app/data:/app/data -p 5000:5000 flask
docker container ls
