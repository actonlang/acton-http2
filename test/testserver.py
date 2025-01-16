from flask import Flask

app = Flask(__name__)

@app.route('/testget', methods = ['GET'])
def testget():
    return 'You have reached the dummy GET endpoint'

@app.route('/testpost', methods = ['POST'])
def testpost():
    return f'You have reached the dummy POST endpoint, submitting {request.data}'
