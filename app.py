from flask import Flask, request, jsonify
import json
import pymongo
from pymongo import MongoClient
from cryptography.fernet import Fernet
import hashlib
from datetime import datetime
from flask_cors import CORS

projectId=""
cluster = MongoClient("mongodb+srv://srushtinandal29:lovedance2910@cluster0.x2dsham.mongodb.net/") 
db = cluster.EmergingTechnologies
collection1 = db.EnvironmentData

app = Flask(__name__)
CORS(app)

@app.route('/', methods=['GET'])
def ping():
    return "Hello world"

@app.route('/enterData', methods=['POST'])
def enter_data():
    data = json.loads(request.data)
    collection1.insert_one({"temperature":str(data["temperature"]), "pressure":str(data['pressure']), "motorStatus":str(data['motorStatus']), "time":datetime.utcnow()})
    response = jsonify({"enteredData" : "true"})
    # response.headers.add('Access-Control-Allow-Origin', '*')
    # response.headers.add('Access-Control-Allow-Credentials', '*')
    return response

@app.route('/sendData', methods=['GET'])
def send_data():
    latest_record = collection1.find_one(sort=[("time", -1)])
    # latest_record = list(collection1.find())
    print(latest_record)
    response_data={ 
        "temperature":latest_record['temperature'],
        "pressure":latest_record['pressure'],
        "motorStatus":latest_record['motorStatus'],

    }
    response=jsonify(response_data)
    # response.headers.add('Access-Control-Allow-Origin', '*')
    # response.headers.add('Access-Control-Allow-Credentials', '*')
    return response

if __name__ == "__main__":
    print("Hello")
    
    app.run(port=5000)