import sys
import os
import time
from flask import Flask, render_template, request, send_from_directory, flash, url_for, redirect
from werkzeug.utils import secure_filename

from ulib.abstract_file import AbstractFile

app = Flask(__name__, 
			static_url_path='', 
			static_folder='static',
			template_folder='templates')

afile_handle = AbstractFile()

app_root = afile_handle.get_app_root()
upload_dir = afile_handle.get_upload_dir()

ALLOWED_EXTENSIONS = {'csv', 'dat'}
upload_dir = app_root + '/data/uploads'

def allowed_file(filename):
	return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def get_file_time_key():
	return time.time()

def file_save(filename, content, path=""):
	return afile_handle.file_save(filename, content, path)

def file_get_content(filename, path = ""):
	return afile_handle.file_get_content(filename, path)

def remove_time_key_file():
	fp = os.path.join(upload_dir, "file_time_input.txt")
	if os.path.isfile(fp):
		os.remove(fp)

@app.route("/")
def home():
	remove_time_key_file()
	return render_template('index.html')

@app.route('/js/<path:path>')
def send_js(path):
	#print(app_root + 'js')
	return send_from_directory(app_root + '/static/js', path)

@app.route('/css/<path:path>')
def send_css(path):
	#print(app_root + 'css')
	return send_from_directory(app_root + '/static/css', path)

@app.route('/img/<path:path>')
def send_img(path):
	return send_from_directory(app_root + '/static/img', path)

@app.route('/uploader', methods=['GET', 'POST'])
def upload_file():
	if request.method == 'POST':
		file_time_key = file_get_content("file_time_input.txt")
		file_time_input = request.form.get("file_time_input")
		#first time
		if file_time_input == "":
			if file_time_key == "":
				file_time_input = get_file_time_key()
				file_save("file_time_input.txt", str(file_time_input))
			else:
				return redirect(url_for("home"))
		else:
			if file_time_key == file_time_input:
				return redirect(url_for("home"))

		file_save("file_time_input.txt", str(file_time_input))
		 		
   		# check if the post request has the file part
		if 'file' not in request.files:
   			flash('No file part')
   			message_txt = 'No File Part'
   			return render_template('error.html', message=message_txt, file_time=get_file_time_key())
		up_file = request.files['file']
		#print(up_file, file=sys.stderr)
		if up_file:
   			if allowed_file(up_file.filename):
   				filename = secure_filename(up_file.filename)
   				up_file.save(os.path.join(upload_dir, filename))
   				message_txt = 'Uploaded Successfully!'
   				return render_template('index.html', message=message_txt, file_time=get_file_time_key())
   			else:
   				request.files['file'].filename = ""
   				message_txt = 'Wrong Type! Uploaded Unsuccessfully!'
   				return render_template('index.html', message=message_txt, file_time=get_file_time_key())
 				
		return render_template('index.html')
	else:
		return render_template('index.html')
	message_txt = 'Something Wrong During Uploading!!'
	return render_template('index.html', message=message_txt, file_time=get_file_time_key())

# We do not have to use app.run. Use of flask run is a newer preferred method
if __name__ == "__main__":
	app.run(host="0.0.0.0", debug=True, port=5000)