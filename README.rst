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


Compilando em Windows
---------------------

Se você por, por qualquer motivo que seja, quiser compilar o Lunar Lander em
Windows, não se desespere. É possível. Pelo menos nós conseguimos uma vez.

Naquela época, utilizamos as seguintes versões das coisas:

    - MinGW-Get
    - libjpeg-8d
    - freeglut-2.8.0
    - cmake-2.8.8
    - Box2D 2.2.1

Primeiramente, instale o MinGW-Get[1]. Fique atento, e na tela de seleção de
componentes do wizard de instalação, certifique-se de marcar o compilador C++ e
o "MSYS Basic System".

Acesse, então, o shell do MinGW. Ele provavelmente estará no menu iniciar. É um
Msão grande e azul.

Para facilitar nossa vida, vamos tentar realizar o maior número de
procedimentos dentro do shell. Como vamos precisar baixar e extrair alguns
arquivos, instale o msys-wget e o msys-unzip[2]. No shell do MinGW, digite

::
    
    mingw-get install msys-wget msys-unzip


Agora baixe, compile e instale a libjpeg.

::

    cd ~
    wget http://www.ijg.org/files/jpegsrc.v8d.tar.gz
    tar xzf jpegsrc.v8d.tar.gz
    cd jpeg-8d
    ./configure
    make && make install


Feito isto, baixe, compile e instale a freeglut.

::
    
    cd ~
    wget http://prdownloads.sourceforge.net/freeglut/freeglut-2.8.0.tar.gz
    tar xzf freeglut-2.8.0.tar.gz
    cd freeglut-2.8.0
    ./configure
    make && make install


Agora baixe, compile e instale o CMake. Para facilitar algumas coisas e evitar
problemas futuros, compilaremos e instalaremos o CMake dentro do ambiente MSYS.
Mas há quem diga que há maneiras alternativas de instalá-lo...

::

    cd ~
    wget http://www.cmake.org/files/v2.8/cmake-2.8.8.tar.gz
    tar xzf cmake-2.8.8.tar.gz
    cd cmake-2.8.8
    ./configure --prefix=/usr/local
    make && make install


Em seguida, baixe o lunar-lander-ex [3][4].

::
    
    cd ~
    wget https://github.com/ravishi/lunar-lander-ex/zipball/master -O lunar-lander-ex.zip #[3]
    unzip lunar-lander-ex.zip
    cd ravishi-lunar-lander-ex-82af820 #[4]


Dentro da pasta do lunar-lander-ex, baixe e compile o Box2D.

::
    
    wget http://box2d.googlecode.com/files/Box2D_v2.2.http://9gag.com/gag/4525397?#_=_1.zip
    unzip Box2D_v2.2.1.zip
    cd Box2D_v2.2.1
    cmake -G "MSYS Makefiles" .
    make


E finalmente, compile o lunar-lander-ex.


::

    cd ..
    make -f Makefile.msys


Pronto! Agora é só executar e pousar sua nave!


::
    
    ./lunar_lander.exe


Se você for curioso, acabará descobrindo que o jogo só pode ser executado a
partir do shell do MinGW. Isto é porque o jogo depende de algumas DLLs que não
estão instaladas no Windows, mas estão instaladas no ambiente do MinGW.

Se você quiser executar o jogo fora do shell do MinGW, pode copiar as DLLs
necessárias para o mesmo diretório onde está o executável. São elas:

    - libgcc_s_dw2-1.dll
    - libglut-0.dll
    - libjpeg-8.dll
    - libstdc++-6.dll

Se você seguiu os passos de instalação cima, elas provavelmente estarão
nos diretórios MinGW/bin ou MinGW/msys/1.0/local/bin.



[1] http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/
[2] Este passo é opcional. Se você souber do que se trata esse lance de wget
e unzip, provavelmente perceberá que pode baixar os pacotes, extraí-los e
movê-los para o lugar certo sem essa dupla de programas.
[3] Atenção! Neste ponto, o wget encontrou problemas com o certificado do
github. Se isso acontecer, basta adicionar a flag --no-check-certificates ao
comando executado que tudo dará certo.
[4] O hash no final do nome da pasta descomprimida poderá variar. Fique atento.
