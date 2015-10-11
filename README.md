# pstate-controller
P-State auto controller that cuts down the power usage of a Intel CPU on Linux.

pstate-controller is able to automaticlly adjest P-State setting based on the current CPU usage without harming the overall performace (like some programs do).

## Requirements
- Intel P-State driver (avliable on Linux Kernel >= 3.9)

## Build

```shell
git clone https://github.com/marty1885/pstate-controller
cd pstate-controller
cmake .
make
```

### Build requirements
- A C++11 competiable compiler
- CMake

## Usage
NOTE : You must be root (or can write to pstate files) to run it.

```shell
sudo bin/pstate-controlld
```

to run it in background

```shell
sudo bin/pstate-controlld &
```

## License
GUN GPLv3 or later. See the LICENSE file in the repo.

## NOTE
This application is under early development. It DOES NOT support config files and don't have propper deamon support now.
