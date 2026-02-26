FROM alpine:3.19

RUN apk add --no-cache \
    nginx \
    php82 \
    php82-fpm \
    php82-mysqli

COPY nginx.conf /etc/nginx/nginx.conf
COPY html /usr/share/nginx/html

EXPOSE 8001
EXPOSE 8002

CMD sh -c "php-fpm82 && nginx -g 'daemon off;'"

