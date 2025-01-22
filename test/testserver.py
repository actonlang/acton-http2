from flask import Flask, request

app = Flask(__name__)

@app.route('/testget', methods = ['GET'])
def testget():
    return 'You have reached the dummy GET endpoint\n'

@app.route('/testpost', methods = ['POST'])
def testpost():
    return f'You have reached the dummy POST endpoint, submitting {request.data}\n'
