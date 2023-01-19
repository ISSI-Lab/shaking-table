FROM python:3.9-bullseye

# Install requirements
COPY requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt

#RUN mkdir -p /app
WORKDIR /app

COPY app/. /app/

CMD [ "./run_flask.sh"]