FROM ubuntu:zesty

RUN apt-get update && apt-get install -y \
  gcc \
  make \
  cmake \
  python3-dev \
  build-essential \
  libssl-dev \
  zlib1g-dev \
  libbz2-dev \
  libreadline-dev \
  libsqlite3-dev \
  wget \
  curl \
  llvm \
  libncurses5-dev \
  libncursesw5-dev \
  xz-utils \
  tk-dev \
  git

ADD https://cmocka.org/files/1.1/cmocka-1.1.1.tar.xz cmocka-1.1.1.tar.xz
RUN cd cmocka-1.1.1.tar.xz/cmocka-1.1.1 \
  && mkdir build \
  && cd build \
  && cmake -DCMAKE_INSTALL_PREFIX=/usr .. \
  && make \
  && make install
RUN rm -rf cmocka-1.1.1.tar.xz

RUN useradd -ms /bin/bash test
RUN echo test:bingo12fish | chpasswd
ENV HOME /home/test
WORKDIR $HOME
USER test

RUN git clone https://github.com/pyenv/pyenv.git ~/.pyenv
RUN echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc
RUN echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
RUN echo 'eval "$(pyenv init -)"' >> ~/.bashrc

RUN ~/.pyenv/bin/pyenv install 3.6.2
RUN ~/.pyenv/bin/pyenv local 3.6.2
