FROM ghcr.io/lobis/root-geant4-garfield:cpp17_root-v6-26-10_geant4-v11.1.0_garfield-e0a9f171

# Install some dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y \
    apt-utils binutils build-essential ca-certificates cmake curl \
    libpqxx-dev postgresql-server-dev-all && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/cache/apt/archives/* && \
    rm -rf /var/lib/apt/lists/*

# Install REST
ENV REST_PATH /usr/local/rest-for-physics

RUN git clone https://github.com/rest-for-physics/framework.git /tmp/framework \
    && cd /tmp/framework \
    && python3 pull-submodules.py --clean --force --dontask --latest:master \
    && mkdir build && cd build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=$REST_PATH -DREST_GARFIELD=ON -DREST_G4=ON -DRESTLIB_DETECTOR=ON -DRESTLIB_RAW=ON -DRESTLIB_TRACK=ON -DREST_WELCOME=OFF \
    && make -j$(nproc) install \
    && rm -rf /tmp/framework

ENV PATH $REST_PATH/bin:$PATH
ENV LD_LIBRARY_PATH $REST_PATH/lib:$LD_LIBRARY_PATH

RUN echo "source $REST_PATH/thisREST.sh" >> ~/.bashrc

# Copy source code
COPY ./* /source/

# Build
RUN cd /source && mkdir build && cd build && cmake .. && make -j$(nproc) && make install

ENTRYPOINT /usr/local/bin/restFileSystemIndexer

# docker build -t lobis/indexer .
# docker run -it lobis/indexer
