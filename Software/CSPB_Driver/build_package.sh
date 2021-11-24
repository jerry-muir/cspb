#!/bin/bash
rm -rv build
rm -rv cspb.egg-info
rm -rv dist
python3 setup.py sdist bdist_wheel
