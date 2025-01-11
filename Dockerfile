FROM alpine:3.21

# Install required packages
RUN apk add --no-cache \
	git \
	gcc \
	g++ \
	cmake \
	make \
	python3 \
	python3-dev \
	npm \
	nodejs \
	dune \
	ocaml \
	bash \
	cargo \
	rust
	
SHELL ["/bin/bash", "-c"]

RUN git clone https://github.com/ErwanTLG/sinfourmis2025.git sinfourmis
RUN cd sinfourmis \
	&& mkdir build \
	&& cd build \
	&& cmake .. \
	&& make -j15 \
	&& cp sinfourmis ../

WORKDIR /sinfourmis
