FROM alpine:latest

RUN apk add py3-flask py-gunicorn
COPY testserver.py /testserver.py
CMD gunicorn -b 0.0.0.0:8000 testserver:app
