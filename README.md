# MQTTSwitch
## DO NOT USE THIS LIB FOR THE MOMENT. WORK IS STILL IN PROGRESS. USE ORIGINAL REPOSITORY INSTEAD

[![Build Status](https://travis-ci.org/r89m/MQTTSwitch.svg?branch=master)](https://travis-ci.org/r89m/MQTTSwitch)
[![Coverage Status](https://coveralls.io/repos/github/r89m/MQTTSwitch/badge.svg?branch=master)](https://coveralls.io/github/r89m/MQTTSwitch?branch=master)

This library makes controlling and monitoring switches over MQTT easy.

You can use this library to easily control a digital pin, RGB light or any other kind of switch.

## Examples
The library comes with a number of example sketches. See File > Examples > MQTTSwitch within the Arduino IDE.

Documentation can be found in the [wiki](https://github.com/r89m/MQTTSwitch/wiki/API-Documentation).

## Testing

This code has been unit tested using virtualised Arduino hardware, ensuring that any reading and writing of digital pins has been verified to work.

Additionally, a virtual MQTT client was used to verify the behaviour of this library. The tests can be found in the [tests](tests) folder.

Code coverage is shown in the badge below the library name, and can be further inspected at the [Coveralls.io repo page](https://coveralls.io/github/r89m/MQTTSwitch?branch=master)

## How to Contribute

### Bug Reports
Bug reports can be reported using the [Issues List](https://github.com/r89m/MQTTSwitch/issues). Please start your title with `[Bug]` to help keep things organised.

### Feature Requests
Feature requests can be submitted using the [Issues List](https://github.com/r89m/MQTTSwitch/issues). Please start your title with `[Feature]` to help keep things organised.

## Licence
This code is released under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
