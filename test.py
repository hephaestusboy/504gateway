import requests
import webbrowser
import certifi
import ssl
from time import sleep
context =ssl._create_unverified_context()
def fetch_with_retries(url, max_retries=100, delay=5):
    """
    Fetches a URL with retry logic for 504 Gateway Timeout errors.

    Args:
        url (str): The URL to fetch.
        max_retries (int): The maximum number of retries before giving up.
        delay (int): The delay between retries in seconds.

    Returns:
        str: The URL to open in the browser if successful.
    """
    attempt = 0
    while attempt < max_retries:
        try:
            response = requests.get(url)
            # Check for 504 status code
            if response.status_code == 504:
                raise requests.exceptions.RequestException("Received 504 Gateway Timeout")
            # Return the URL if successful
            return url
        except requests.exceptions.RequestException as e:
            print(f"Attempt {attempt + 1} failed: {e}")
            attempt += 1
            if attempt < max_retries:
                print(f"Retrying in {delay} seconds...")
                sleep(delay)
            else:
                print("Max retries reached. Exiting.")
                raise

if __name__ == "__main__":
    url_to_fetch = 'http://app.ktu.edu.in/eu/anon/refferError.htm'  # Replace with the URL you want to fetch
    try:
        url_to_open = fetch_with_retries(url_to_fetch)
        print(f"Success! Opening {url_to_open} in the web browser.")
        webbrowser.open(url_to_open)
    except requests.exceptions.RequestException as e:
        print(f"Failed to fetch URL after several retries: {e}")
