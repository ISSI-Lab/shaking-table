#!/bin/sh

BASEDIR=$(pwd)

PYTHONPATH=${BASEDIR}
export PYTHONPATH=${BASEDIR}/util:$PYTHONPATH
echo ${PYTHONPATH}

export FLASK_APP=${BASEDIR}/app.py
export FLASK_DEBUG=1

flask run --host=0.0.0.0
#python /app/app.py

