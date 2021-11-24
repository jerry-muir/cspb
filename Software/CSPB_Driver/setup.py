import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="cspb",
    version="1.0.0",
    author="Gerard L. Muir",
    author_email="jerrymuir65@gmail.com",
    description="CSPB driver package",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/jerry-muir/cspb",
    packages=['cspb'],
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Operating System :: POSIX :: Linux",
    ],
    install_requires=['smbus'],
    python_requires=">=2.7",
    keywords = 'cluster system power cspb driver raspberry pi',
)
