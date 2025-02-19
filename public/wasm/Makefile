TARGET = build/Path
SRC = Path.cpp
EMFLAGS = -s USE_WEBGL2=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 \
          -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS="['_dijkstra', '_malloc', '_free']" \
          -s "EXPORTED_RUNTIME_METHODS=['cwrap']" -O2

all: $(TARGET).js

$(TARGET).js: $(SRC)
	emcc $(SRC) -o $(TARGET).js $(EMFLAGS)

clean:
	rm -f $(TARGET).js $(TARGET).wasm
