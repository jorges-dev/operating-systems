FROM gcc:14

WORKDIR /app

COPY hello.c .

RUN gcc hello.c -o hello

CMD ["./hello"]