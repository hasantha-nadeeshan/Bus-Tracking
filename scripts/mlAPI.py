from flask import Flask, jsonify, request
import pyrebase
import os
import time

firebaseConfig = {
  "apiKey": "AIzaSyC4yrif5LbVTgFO8hdMnAfC9DBCkRVR2R0",
  "authDomain": "ml-venomous-snakes.firebaseapp.com",
  "databaseURL": "https://ml-venomous-snakes-default-rtdb.firebaseio.com",
  "projectId": "ml-venomous-snakes",
  "storageBucket": "ml-venomous-snakes.appspot.com",
  "messagingSenderId": "70450704587",
  "appId": "1:70450704587:web:1cfe038c37644f9047ab4e",
  "measurementId": "G-NKM66EJ98E"
}
firebase = pyrebase.initialize_app(firebaseConfig)
storage = firebase.storage()
PATH_GLOBAL = "input/"



app = Flask(__name__)

def downloadImage(url):
  storage.child(PATH_GLOBAL+url.strip()).download(url)

##def predictOutputImage(url):
  

@app.route('/predictText', methods=['POST'])
def predictText():
    
    incoming_data = request.json
    snake_lenght = incoming_data["snake_lenght"]
    snake_colour = incoming_data["snake_colour"]
    location = incoming_data["location"]
    scales_patter = incoming_data["scales_patter"]
    head_pattern = incoming_data["head_pattern"]
    time = incoming_data["time"]
  #  file = request.files.get('feature')
  #  feature  = np.load(file)
   # result = model.predict(feature)
  #  result = result.tolist()
   # return jsonify(prediction=result)
    if request.method == "POST":
        return jsonify({"res":"Hi " + "I saw" + snake_colour + " near " + location + " at " + time })
      

model2 = keras.models.load_model('Python_Scripts/attempt1.h5')
@app.route('/predictImage', methods=['POST'])
def predictImage():
 
    incoming_image = request.json
    image_name = incoming_image["name"]
    downloadImage(image_name)
  
    time.sleep(3)
    img_url = image_name
    test_image= tf.keras.utils.load_img(img_url,target_size=(480,640)) 
    test_image = tf.keras.utils.img_to_array(test_image)
    test_image = np.expand_dims(test_image, axis = 0)
    result = model2.predict(test_image)
    indx=np.where(result[0] ==max(result[0]))
    labels=['MuduKarawala', 'Naja', 'RusselViper', 'hump', 'thelKarawala', 'අරණි දත්කැටියා - Common banded kukri snake - Oligodon arnensis', 'අහරකුක්කා - Amphiesma stolatum', 'අළු රදනකයා - Common wolf snake - Lycodon aulicus', 'කටකලුවා - Coelognathus helena', 'ගැට රදනකයා - Common Bridal snake - Dryocalamus nympha', 'ගැරඩියා - Common rat snake - Ptyas mucosa', 'දැති ගෝමරයා - Black headed snake - sibynophis subpunctatus', 'දිය ගොයා මඩ පනුවා  රෙදි නයා - wart snake  little file snake - Acrochordus granulatus ', 'දිය වර්ණයා- Atretium schistosum', "පදුරැ හාල්දණ්ඩා - Dendrelaphis bifrenalis (Boulenger's Bronzeback)", 'මල් රදනකයා - Flowery wolf snake - Lycodon fasciolatus']
    print("Predicted class is:", labels[indx[0][0]])
    if request.method == "POST":
        return jsonify({"res":"Hi " + "I got " + labels[indx[0][0]] })


if __name__ == '__main__':
    app.run(debug=False, host='0.0.0.0')
