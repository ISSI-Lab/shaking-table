import sys
from flask import Flask
from flask import render_template

app = Flask(__name__, 
            static_url_path='', 
            static_folder='static',
            template_folder='templates')

@app.route("/")
def home():
    return render_template('index.html')






# if __name__ == "__main__":
#     app.run(host="0.0.0.0", debug=True, port=80)