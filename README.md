# Alicia Server
Server, yippie!

## Requirements

- CMake
- A C++20 compiler
- Boost
- libpq
- Other dependencies included as submodules

## Building

1. Clone the repo

2. ```git submodule update --init --recursive```

3. ```mkdir build; cd build```

4. ```cmake ..```

5. ```cmake --build .```

## Running

After building, ```./alicia-server```