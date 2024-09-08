from flask import Flask, jsonify, request
import firebase_admin
import google.cloud
from firebase_admin import credentials, firestore
from firebase_admin.firestore import GeoPoint
# import pyrebase
# import os
import time

TRIPS = 'trips'
OFFSETMULTIPLIER = -1
EMPTY_STRING  = ""

cred = credentials.Certificate("./bus-tracking-system-6713f-firebase-adminsdk-nd844-335738fd3a.json")
app = firebase_admin.initialize_app(cred)

db = firestore.client()


app = Flask(__name__)

def getCurLocation(busDocId):
  try:
    doc_ref = db.collection(TRIPS).document(busDocId)
    doc = doc_ref.get()
    return True, doc.to_dict()
  except google.cloud.exceptions.NotFound:
    return False, {}
  
def updatePrevLocation(busDocId, prevGeo):
  try:
    ref = db.collection(TRIPS).document(busDocId)
    ref.update({"prevLoc" : prevGeo})
    return True
  except google.cloud.exceptions.NotFound:
    return False  

def updateCurLocation(busDocId, curGeo):
  try:
    ref = db.collection(TRIPS).document(busDocId)
    ref.update({"curLoc" : curGeo})
    return True
  except google.cloud.exceptions.NotFound:
    return False    

def getDocumentID(busId):
  ids = []
  try:
      docs= (
      db.collection(TRIPS)
      .where(u"busId", u"==", str(busId))
      .stream()
      )
      for doc in docs:
        ids.append(str(doc.id))
      
      if len(ids) > 0:

        return str(ids[0])
      else: 
        return ""
      
  except google.cloud.exceptions.NotFound:
      return ""

def setStartField(busId):
  if busId != EMPTY_STRING:
    try:
      col_ref = db.collection(TRIPS)
      doc_ref_generator = col_ref.where(u"busId", u"==", str(busId)).stream()

      for doc_ref in doc_ref_generator:
        # Document Reference
        doc = col_ref.document(doc_ref.id)
        doc.update({u"isStarted": True})  
      
      return jsonify({"info" : "OK"
                      })
    except google.cloud.exceptions.NotFound:
      return jsonify({"info" : "Database Issue"
                      })
  else:
    return jsonify({"info" : "Database Issue"
                      })
    
def setEndField(busDocId):
  if busDocId != EMPTY_STRING:
    try:
      ref = db.collection(TRIPS).document(busDocId)
      ref.update({"isEnded" : True})
      return jsonify({"info" : "OK"
                      })
    except google.cloud.exceptions.NotFound:
      return jsonify({"info" : "Database Issue"
                      })
  else:
    return jsonify({"info" : "Database Issue"
                      })
      
@app.route('/updateStartField', methods=['POST'])
def updateStartField(): 
    incoming_req = request.json
    busId = incoming_req["busId"] 
    if request.method == "POST":
      return setStartField(busId)
    
@app.route('/updateEndField', methods=['POST'])
def updateEndField(): 
    incoming_req = request.json
    busId = incoming_req["busId"] 
    print(busId)
    docID = getDocumentID(busId) 
    print(docID)
    if request.method == "POST":
      return setEndField(docID)
    
@app.route('/updateLocation', methods=['POST'])
def updateLocation(): 
    incoming_req = request.json
    busDocId = incoming_req["busDocId"] 
    lat_offset = float(incoming_req["latOffset"])*(10**OFFSETMULTIPLIER)
    lon_offset = float(incoming_req["lonOffset"])*(10**OFFSETMULTIPLIER)
    state, data = getCurLocation(busDocId)
    if state == True:
      prev_upd_flag = updatePrevLocation(busDocId, data["curLoc"])
      cur_lon = float(data["curLoc"].longitude)
      cur_lat = float(data["curLoc"].latitude)
      new_lon = cur_lon + lon_offset
      new_lat = cur_lat + lat_offset
      cur_upd_flag = updateCurLocation(busDocId, GeoPoint(new_lat, new_lon))     
      if request.method == "POST":
        return jsonify({"info": prev_upd_flag and cur_upd_flag,
                         })
    return jsonify({"info": state,
                         })

if __name__ == '__main__':
    app.run(debug=False, host='0.0.0.0')
