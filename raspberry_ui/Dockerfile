FROM python:3.9-bullseye

# Install requirements
COPY requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt

#RUN mkdir -p /app
WORKDIR /app

# use $PWD to link host and guest drives
COPY app/*.py /app/
COPY app/ulib /app/ulib
COPY app/run_flask.sh /app/

# copy everything
#COPY app/* /app/

CMD [ "./run_flask.sh"]