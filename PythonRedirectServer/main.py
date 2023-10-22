from socket import *
from flask import Flask, Response


app = Flask(__name__)


@app.route("/<num>")
def number(num: int):
    s = socket(AF_INET, SOCK_DGRAM)
    s.sendto(int(num).to_bytes(4, "little"), ("127.0.0.1", 5566))
    # print(f"Got number {num}")
    return "<h1>ok</h1>"


@app.route("/")
def test():
    print("root")
    return "<h1>a</h1>"

@app.route("/log/<text>")
def log(text):
    print("logged:", text)
    return "<h1>l</h1>"


if __name__ == '__main__':
    app.run(debug=True, port=4455)

