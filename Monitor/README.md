# Monitor v1.0.0

Program to monitor the Er and U campaing!

## Build

The programs use `CMake` as its build system. To compile it:

```sh
cd build
cmake ..
cmake --build . --config Release --target all -- -j 10
cd ..
```

These commands create a `Monitor` executable on the [`bin`](bin) folder and keep all the build artifacts on the [`build`](build) folder.

## Run

To launch the program, execute the binary passing a (for now empty) configuration file:

```sh
bin/Monitor ""
```

## Results

The program creates a `Monitoring.root` file with graphs that are useful to check that the experiment is running correctly! 📈
