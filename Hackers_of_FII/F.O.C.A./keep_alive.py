#!/usr/bin/env python3

from flask import Flask
from threading import Thread
from waitress import serve

app = Flask('')


@app.route('/')
def home():
    return 'Alive.'


def run():
    serve(app, host='0.0.0.0', port=8080)


G_EXECUTION_THREAD = Thread(target=run)


def keep_alive():
    G_EXECUTION_THREAD.start()
