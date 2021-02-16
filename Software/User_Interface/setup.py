from distutils.core import setup

setup(
    # Application name:
    name="CSPB_GUI",

    # Version number (initial):
    version="0.0.1",

    # Application author details:
    author="Gerard L. Muir",
    author_email="jerrymuir65@gmail.com",

    # Packages
    packages=["app"],

    # Include additional files into the package
    #include_package_data=True,

    # Details
    url="http://pypi.python.org/pypi/MyApplication_v010/",

    #
    # license="LICENSE.txt",
    description="Useful towel-related stuff.",

    # long_description=open("README.txt").read(),

    # Dependent packages (distributions)
    install_requires=[
        "cspb",
    ],
)