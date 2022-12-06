
sudo apt-get update

sudo apt remove libsdl2-dev libsdl2-image-dev -y
sudo apt install libsdl2-dev libsdl2-image-dev -y

sudo mkdir -p /usr/lib/x86_64-linux-gnu/cmake/SDL2
sudo cp -r ./sdl2/cmake/* /usr/lib/x86_64-linux-gnu/cmake/SDL2/

sudo mkdir -p /usr/lib/x86_64-linux-gnu/cmake/sdl2_image
sudo cp -r ./sdl2_image/cmake/* /usr/lib/x86_64-linux-gnu/cmake/sdl2_image/
