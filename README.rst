Lunar Lander EX
===============

Este é nosso projeto de Computação Gráfica. Um remake do jogo Lunar Lander.


Compilando
----------

Para compilar no Ubuntu 12.04, são necessários os seguintes pacotes:

::

    sudo apt-get install build-essential cmake libgl1-mesa-dev freeglut3-dev libjpeg-dev libxi-dev


É necessário também utilizar a versão 2.2.1 da Box2D. Como ela não está nos
repositórios do Ubuntu, faça:

::

    cd path/to/lunar-lander-ex
    wget http://box2d.googlecode.com/files/Box2D_v2.2.1.zip
    unzip Box2D_v2.2.1.zip
    cd Box2D_v2.2.1
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX="../../box2d" ..
    make && make install


Então é só compilar o projeto em si.

::

    cd path/to/lunar-lander-ex
    make

E rodar

::

    ./lunar_lander
