FROM balenalib/rpi-debian-python:3.7.4-buster-build AS base

# Allows this to run on other architectures (for testing).
RUN [ "cross-build-start" ]

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY run.py ./
COPY .prod.env ./
COPY anemioradiodaemon ./anemioradiodaemon
COPY ../../../RFM69/RFM69 ./RFM69

CMD [ "python", "./run.py" ]

# Allows this to run on other architectures (for testing).
RUN [ "cross-build-end" ]
