# cspb

The cspb package contains a python driver class for communications with the Cluster System Power Board (cspb) hardware.

## Introduction

The cspb package contains a wrapper driver around the smbus package. It enables easy communication with the cluster system power board hardware via an i2c bus.

Currently supported methods are:

- set_power

- shutdown

- signal_shutdown

- read_register

- write_register

- set_register_number

- send_command

## Dependencies

This driver depends on the [smbus](https://pypi.org/project/smbus/) package.

## CSPB code examples

### Example 1: Display the current power state

```
from cspb.CSPB import CSPB

i2c_bus_number = 1
i2c_address = 21
cspb = CSPB(i2c_bus_number, i2c_address)
power_state = cspb.read_register(cspb.PWR_STTS_REGSTR_ADDR)
print(power_state)
```

### Example 2: Request shutdown of all power slots

```
from cspb.CSPB import CSPB

i2c_bus_number = 1
i2c_address = 21
cspb = CSPB(i2c_bus_number, i2c_address)
cspb.shutdown(0xff)
```

## Installation Instructions

cspb is a pure Python and requires no compilation. Install as follows:

```
pip install cspb
```
