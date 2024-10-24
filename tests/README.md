# About e2e tests

The tests are written using the `pytest` framework, with the `pytest-order` plugin to manage the execution order of certain tests where dependencies exist.

## Prerequisites

Before running the tests, ensure you have the following installed:

Use pip
```sh
pip install -r requirements.txt
```

When pip install package is not directly accessable (we have a script to install via python3-xyz)
```sh
./install_with_python3-xyz.sh
```

## Running the Tests

To run all tests
```sh
pytest
```

To run tests in verbose mode
```sh
pytest -v
```

To specific python3 as intergrator (because packages are installed under python3 scope)
```sh
python3 -m pytest -v
```

## Further Information (pytest)

For more details on `pytest` and writing tests, refer to the [pytest documentation](https://docs.pytest.org/en/latest/).

Other reference (https://github.com/araujo88/MiniBoostRestAPI)
