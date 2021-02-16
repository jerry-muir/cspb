# Copyright 2020 Gerard L. Muir 
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and or sell
#  copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE.

##################
#
# This script is used by the pip setup tools to create a sdist
# and wheel install package that can be uploaded to the pypi
# package repository.
#
##################
import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="cspb_tools",
    version="1.0.1",
    author="Gerard L. Muir",
    author_email="jerrymuir65@gmail.com",
    description="CSPB User Interface and Tools package",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/jerry-muir/cspb",
    packages=['cspb_tools'],
    package_data={"":['cspb_gui.glade']},
    include_package_Data=True,
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Operating System :: POSIX :: Linux",
    ],
    install_requires=['smbus','cspb'],
    python_requires=">=2.7",
    entry_points={"console_scripts": ["run_cspb_gui = cspb_tools.run_cspb_gui:run_gui"]},
    keywords = 'cluster system power cspb raspberry pi gui',
)
