import sys
from flask import Flask
from flask import render_template

app = Flask(__name__, 
            static_url_path='', 
            static_folder='static',
            template_folder='templates')


@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('js', path)

@app.route('/css/<path:path>')
def send_css(path):
    return send_from_directory('css', path)

@app.route('/img/<path:path>')
def send_js(path):
    return send_from_directory('img', path)

@app.route("/")
def home():
    return render_template('index.html')






# if __name__ == "__main__":
#     app.run(host="0.0.0.0", debug=True, port=80)