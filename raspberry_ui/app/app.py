import sys
import os
from flask import Flask
from flask import render_template
from flask import send_from_directory

app = Flask(__name__, 
			static_url_path='', 
			static_folder='static',
			template_folder='templates')

app_root = os.path.join(os.path.dirname(os.path.abspath(__file__)))


@app.route('/js/<path:path>')
def send_js(path):
	print(app_root + 'js')
	return send_from_directory(app_root + '/static/js', path)

@app.route('/css/<path:path>')
def send_css(path):
	print(app_root + 'css')
	return send_from_directory(app_root + '/static/css', path)

@app.route('/img/<path:path>')
def send_img(path):
	return send_from_directory(app_root + '/static/img', path)

@app.route("/")
def home():
	return render_template('index.html')


# We do not have to use app.run. Use of flask run is a newer preferred method
#if __name__ == "__main__":
#	app.run(host="0.0.0.0", debug=True, port=5000)