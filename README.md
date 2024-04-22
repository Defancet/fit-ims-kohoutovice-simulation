# IMS Kohoutovice Aquapark Simulation

## Project Overview

This project simulates the visitor dynamics at the Kohoutovice Aquapark. It models visitor interactions within various
park facilities such as pools, saunas, and water slides to analyze queue times and capacity utilization.

## Authors

- Maksim Kalutski (xkalut00)
- Sergei Rasstrigin (xrasst00)

## Requirements

- C++ compiler supporting C++23
- SIMLIB simulation library
- CMake build system
- Linux Operating System (Ubuntu 22.04 recommended)

## Installation

1. Ensure you have the C++ compiler and CMake installed on your system.
2. Clone the repository:

    ```bash
    git clone [repository-url]
    ```

3. Navigate to the project directory and create a build directory:

    ```bash
    cd [project-directory]
    mkdir build && cd build
    ```

4. Run CMake and make to compile the project:

    ```bash
    cmake ..
    make
    ```

## Usage

To run the simulation, execute the binary with the day type parameter:

```bash
./kohoutovice -d [day_type]
```

Where day_type can be:

- `weekday` for simulating a regular weekday
- `weekend` for simulating a weekend day
- `holiday` for simulating a public holiday

## Output

The simulation outputs are stored in `kohoutovice.dat`, including statistics on facility usage, queue lengths, and wait
times.
