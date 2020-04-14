###########
# BUILDER #
###########

# Pull official base image.
FROM balenalib/rpi-debian-python:3.8-buster-build AS builder

# Uncomment if you want to build for x86-x64.
# RUN ["cross-build-start"]

# Set work directory.
WORKDIR /usr/src/app

# Set environment variables.
ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1

# Lint.
RUN pip install --upgrade pip
RUN pip install flake8
COPY . .

# Run flake8 to ensure syntax is OK. 
# We will ingore line length and unused import errors.
RUN flake8 --ignore=E501,F401 .

# Install dependencies.
COPY ./requirements.txt .
RUN pip install wheel
RUN pip wheel --no-cache-dir --no-deps --wheel-dir /usr/src/app/wheels -r requirements.txt


#########
# FINAL #
#########

# Note: we have to run as root (default) user, since we have privileged hardward access.

# Pull official base image.
FROM balenalib/rpi-debian-python:3.8-buster-run

# Set home directory.
ENV HOME=/home

# Create directory for the app(s).
RUN mkdir -p $HOME/app

# Create the appropriate directories for Anemio.
ENV APP_HOME=$HOME/app/anemio
ENV APP_DATA=/data/anemio
RUN mkdir $APP_HOME
RUN mkdir -p $APP_DATA

# Working directory, everything below is relative to this.
WORKDIR $APP_HOME

# Install dependencies.
COPY --from=builder /usr/src/app/wheels /wheels
COPY --from=builder /usr/src/app/requirements.txt .
RUN pip install --upgrade pip
RUN pip install --no-cache /wheels/*

# Copy project.
COPY . .

# Uncomment if you want to build for x86-x64.
# RUN ["cross-build-end"]

# Run Python daemon script.
CMD [ "python", "./run.py" ]
