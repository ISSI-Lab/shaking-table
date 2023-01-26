import sys
import os
import time

from flask import Flask, render_template, request, send_from_directory, flash, url_for, redirect
from werkzeug.utils import secure_filename

app = Flask(__name__, 
			static_url_path='', 
			static_folder='static',
			template_folder='templates')

app_root = os.path.join(os.path.dirname(os.path.abspath(__file__)))
UPLOAD_FOLDER = app_root + '/data/uploads'
ALLOWED_EXTENSIONS = {'csv', 'dat'}
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


def allowed_file(filename):
	return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def getFileTimeKey():
	return time.time()

def fileSave(filename, content):
	#f = open(os.path.join(app.config['UPLOAD_FOLDER'], filename), 'w')
	#f.write(content.encode("utf-8"))
	fout = open(os.path.join(app.config['UPLOAD_FOLDER'], filename), "wt")
	fout.write(content)
	fout.close()
	return 1

def getFileContent(filename):
	try:
		f = open(os.path.join(app.config['UPLOAD_FOLDER'], filename), 'rt')
	except OSError:
		print("No such file!!", file=sys.stderr)
		return ""
	
	content = f.read()
	f.close()
	return content

def removeFileTimeKeyFile():
	fp = os.path.join(app.config['UPLOAD_FOLDER'], "file_time_input.txt")
	if os.path.isfile(fp):
		os.remove(fp)

@app.route("/")
def home():
	removeFileTimeKeyFile()
	return render_template('index.html')

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

# #this is used for redirecting after uploading to avoid re-submission by page refresh 
# @app.route("/upload_success")
# def upload_success():
# 	message_txt = 'Uploaded Successfully!'
# 	return render_template('index.html', message=message_txt)
#
# #this is used for redirecting after uploading to avoid re-submission by page refresh 
# @app.route("/upload_error")
# def upload_error():
# 	message_txt = 'Wrong Type! Uploaded Unsuccessfully!'
# 	return render_template('index.html', message=message_txt)  	

@app.route('/uploader', methods=['GET', 'POST'])
def upload_file():
	if request.method == 'POST':
		file_time_key = getFileContent("file_time_input.txt")
		file_time_input = request.form.get("file_time_input")
		print("the file time input is: " + str(file_time_input) , file=sys.stderr)
		print("the file time key   is: " + str(file_time_key) , file=sys.stderr)
		#first time
		if file_time_input == "":
			if file_time_key == "":
				print("first time login and upload -- save a new file_time_key in the folder" , file=sys.stderr)
				file_time_input = getFileTimeKey()
				print("saving file time key: " +  str(file_time_input), file=sys.stderr)
				fileSave("file_time_input.txt", str(file_time_input))
			else:
				return redirect(url_for("home"))
		else:
			if file_time_key == file_time_input:
				return redirect(url_for("home"))

		fileSave("file_time_input.txt", str(file_time_input))
		 		
   		# check if the post request has the file part
		if 'file' not in request.files:
   			flash('No file part')
   			message_txt = 'No File Part'
   			return render_template('error.html', message=message_txt, file_time=getFileTimeKey())
		up_file = request.files['file']
		print(up_file, file=sys.stderr)
		if up_file:
   			if allowed_file(up_file.filename):
   				filename = secure_filename(up_file.filename)
   				up_file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
   				message_txt = 'Uploaded Successfully!'
   				return render_template('index.html', message=message_txt, file_time=getFileTimeKey())
   				#return redirect(url_for("upload_success"))
   			else:
   				print(str(up_file.filename), file=sys.stderr)
   				request.files['file'].filename = ""
   				print(str(request.files['file'].filename), file=sys.stderr)
   				message_txt = 'Wrong Type! Uploaded Unsuccessfully!'
   				return render_template('index.html', message=message_txt, file_time=getFileTimeKey())
   				#return redirect(url_for("upload_error"))  				
		return render_template('index.html')
	else:
		return render_template('index.html')
	message_txt = 'Something Wrong During Uploading!!'
	return render_template('index.html', message=message_txt, file_time=getFileTimeKey())

# We do not have to use app.run. Use of flask run is a newer preferred method
#if __name__ == "__main__":
#	app.run(host="0.0.0.0", debug=True, port=5000)