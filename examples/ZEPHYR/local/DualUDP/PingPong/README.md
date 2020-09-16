# Zephyr PingPong example
This setup consists of a Transmitter and a Receiver. The Transmitter will regularly send a small packet to the Receiver, and the
Receiver will reply back every time.


This example is tested with the [nucleo_f429zi](https://docs.zephyrproject.org/latest/boards/arm/nucleo_f429zi/doc/index.html) board.
You need a command-line Zephyr development environment. Follow [this](https://docs.zephyrproject.org/latest/boards/arm/nucleo_f429zi/doc/index.html)
to install it.

# Build the example
To build the *Receiver* or the *Transmitter* type the following commands in the shell:

```
#> cd Transmitter
#> west update
#> source zephyr/zephyr-env.sh
#> west build -b nucleo_f429zi application
#> west flash
```

