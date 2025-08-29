from setuptools import setup
import versioningit


if __name__ == "__main__":
    # Minimal setuptools setup to allow calling `load_setup_py_data`
    # from conda meta.yaml
    setup(version=get_version())
