# MeTube
A backend system inspired by YouTube’s upload flow. It allows users to upload videos (chunked) through a REST API, which are then processed by distributed encoder workers. The video encoding process involves generating multiple video qualities using FFMPEG, with storage handled by S3/MinIO and message dispatching done via RabbitMQ.

## Requirements
1. Python (for REST API)
2. C++ (for encoder worker code)
3. FFMPEG (video encoding)
4. RabbitMQ (task distribution)
5. S3/MinIO (for video storage)
6. Docker (for containerization)
7. PostgreSQL (for storing video information)
8. React (for the web frontend)

## System Design
![image](https://github.com/user-attachments/assets/11e3bd72-51fb-4221-aca6-193a57419c88)


## Demo

https://github.com/user-attachments/assets/07682475-5c3d-4e50-837f-5a22ba210d7b


## Note
This project is just a proof of concept. It is not production ready.
