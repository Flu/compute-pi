## Compute PI with all your processors's might!

Now available on your computer: threaded application based on the number of your logical cores, calculates the infamous PI constant with the Monte Carlo Simulation.
All within one simple .c file. All you have to do (if you're on Linux) is go to the ```linux-build``` folder aaand:
```bash
$ ./compute-pi 5000
```
Takes only one argument, the number of calculations per thread. The larger the number, the happier you are because you are so so close to PI. In the case no argument was given, it chooses 10000 as the default. Cheers and have fun computing PI!
