
```shell
$ git clone -b 0.5.6 https://github.com/kendryte/kendryte-standalone-sdk.git
$ git clone --recursive https://github.com/tuupola/k210_effects.git
$ cd k210_effects/build
$ cmake .. -DSDK=../kendryte-standalone-sdk -DTOOLCHAIN=/opt/riscv/bin
$ make VERBOSE=1
$ kflash -B dan -t firmware.bin
```