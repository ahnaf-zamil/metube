# MeTube
A backend system inspired by YouTube’s upload flow. It allows users to upload videos through a REST API, which are then processed by distributed encoder workers. 

The video encoding process involves generating multiple video qualities using FFMPEG, with storage handled by S3/MinIO and task distribution done through RabbitMQ.

## Requirements
1. Python (for REST API)
2. C++ (for encoder worker code)
3. FFMPEG (video encoding)
4. RabbitMQ (task distribution)
5. S3/MinIO (for video storage)
6. Docker (for containerization)
7. PostgreSQL (for storing video information)
8. React+CRA (for the web frontend, will migrate to Vite soon)

## System Design
![image](https://github.com/user-attachments/assets/11e3bd72-51fb-4221-aca6-193a57419c88)


## Running the Project

### Encoder

This project uses Meson and Ninja to build the C++ program with the GCC compiler.

Use the following commands to build and run the encoder
```bash
cd encoder
meson setup build # setup the build files
ninja -C build # compile the program

# make sure to create the .env file and set up the credentials for first run
cp .env.example .env

# run the program
./build/encoder
```

### Web UI

Make sure you have `yarn` and Node.js installed.

```bash
cd web
yarn # to install dependencies
yarn start # start the react server
```

### REST API

Instructions WIP.

## Demo

https://github.com/user-attachments/assets/07682475-5c3d-4e50-837f-5a22ba210d7b


## Note
This project is just a proof of concept. It is not production ready.
