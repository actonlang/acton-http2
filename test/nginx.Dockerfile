FROM alpine:latest

RUN apk add nginx
COPY nginx.conf /nginx.conf
CMD nginx -g 'daemon off;' -c /nginx.conf
