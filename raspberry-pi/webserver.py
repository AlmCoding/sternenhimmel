#!/usr/bin/env python

# Test script
# http://localhost:5000/api?number=12

import multiprocessing
import RPi.GPIO as GPIO
import smbus
import time

from flask import Flask
from flask import jsonify
from flask import request

import gpioexpander as expander
import show2output

running_process = None


app = Flask(__name__)


@app.route('/', methods=['GET','PUT','POST'])
def root_node():
    return jsonify( test_example='10.0.0.1:80/test',
                    show_example='10.0.0.1:80/show?number=3',
                    output_example='10.0.0.1:80/output?number=3,4',
                    timing_example='10.0.0.1:80/timing?offset=0.5&pulse=0.9&pause=0.5&count=4&stay=16.0')


@app.route('/output', methods=['GET','PUT','POST'])
def output_node():
    # get number parameter
    number = request.args.get('number', '')
    number = [int(num) for num in number.split(',')]

    def task(number):
        exp = expander.GPIOexpander()
        exp.animate_outputs(number)
    
    global running_process
    if running_process:
        running_process.terminate()
    running_process = multiprocessing.Process(target=task, args=(number,))
    running_process.start()
    
    return jsonify(result='Ok.')


@app.route('/show', methods=['GET','PUT','POST'])
def show_node():
    # get number parameter
    number = request.args.get('number', '')
    number = [int(num) for num in number.split(',')]
    
    if number[0] in show2output.mapping.keys():
        number = show2output.mapping[number[0]]
    else:
        number = [number[0]]

    def task(number):
        exp = expander.GPIOexpander()
        exp.animate_outputs(number)
    
    global running_process
    if running_process:
        running_process.terminate()
    running_process = multiprocessing.Process(target=task, args=(number,))
    running_process.start()
    
    return jsonify(result='Ok.')


@app.route('/timing', methods=['GET','PUT','POST'])
def timing_node():
    # Set timing parameter in gpioexpander file
    expander.OFFSET_TIME = float(request.args.get('offset', expander.OFFSET_TIME))
    expander.PULSE_TIME = float(request.args.get('pulse', expander.PULSE_TIME))
    expander.PAUSE_TIME = float(request.args.get('pause', expander.PAUSE_TIME))
    expander.PULSE_COUNT = int(request.args.get('count', expander.PULSE_COUNT))
    expander.STAY_TIME = float(request.args.get('stay', expander.STAY_TIME))
    return jsonify(result='Ok.')


@app.route('/test', methods=['GET','PUT','POST'])
def test_node():
    def task():
        exp = expander.GPIOexpander()
        exp.test_expander_mapping()
    
    global running_process
    if running_process:
        running_process.terminate()
    running_process = multiprocessing.Process(target=task)
    running_process.start()

    return jsonify(result='Ok.')


@app.route('/shutdown', methods=['GET','PUT','POST'])
def shutdown_node():
    request.environ.get('werkzeug.server.shutdown')()
    return jsonify(result='Server shutting down...')


if __name__ == '__main__':
    # run flask server app
    app.run(debug=False, host="0.0.0.0", port=80)
