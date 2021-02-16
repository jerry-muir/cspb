# cspb-tools

The Cluster System Power Board (cspb) tools package contains example python software for communicating with the cluster system power board hardware.

## Introduction

The cspb-tools package contains python code examples that make use of the cspb driver package to provide communications to the cluster system power board hardware via an i2c serial bus. The software was developed with a focus on the Raspberry Pi single board computer.

This package contains the following programs:

- cspb_gui:

     A basic graphical user interface for communicating with and programming 
the cspb hardware.

## Dependencies

This package depends on the [cspb](https://pypi.org/project/cspb/) package.

## Code Examples

Running the GUI application:

```
run_cspb_gui
```

## Installation Instructions

The cspb_tools package is pure Python and requires no compilation. Install as follows:

```
pip install cspb_tools
```
