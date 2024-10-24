import pytest
import requests
import subprocess
import time

BASE_URL = 'http://localhost:1999'
PERSON_API = '/api/person'

@pytest.fixture(scope="session", autouse=True)
def start_and_stop_program():
    # Start the program (e.g., a server)
    process = subprocess.Popen(['../build/WebApi'])
    time.sleep(1)  # Wait for the server to start

    yield  # This is where the testing happens

    # Stop the program
    process.terminate()
    process.wait()

@pytest.mark.order(1)
def test_post_person():
    url = f"{BASE_URL}{PERSON_API}"
    payload = {'name': 'John Doe', 'age': 30}
    headers = {'Content-Type': 'application/json'}
    response = requests.post(url, json=payload, headers=headers)
    assert response.status_code == 201

@pytest.mark.order(2)
def test_get_persons():
    url = f"{BASE_URL}{PERSON_API}"
    response = requests.get(url)
    assert response.status_code == 200

@pytest.mark.order(3)
def test_get_person_by_id():
    url = f"{BASE_URL}{PERSON_API}/0"
    response = requests.get(url)
    assert response.status_code == 200

@pytest.mark.order(4)
def test_get_person_by_id_not_found():
    url = f"{BASE_URL}{PERSON_API}/42"
    response = requests.get(url)
    assert response.status_code == 404

@pytest.mark.order(5)
def test_delete_person_by_id():
    url = f"{BASE_URL}{PERSON_API}/0"
    response = requests.delete(url)
    assert response.status_code == 204

@pytest.mark.order(6)
def test_not_found():
    url = f"{BASE_URL}/42"
    response = requests.get(url)
    assert response.status_code == 404
