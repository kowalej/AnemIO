# Pull official base image.
FROM python:3.8.2-alpine

# Set working directory.
WORKDIR /usr/src/anemio

# Set environment variables.
ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1

# Install dependencies.
RUN pip install --upgrade pip
COPY ./requirements.txt /usr/src/anemio/requirements.txt
RUN pip install -r requirements.txt

# Copy project.
COPY ./anemio /usr/src/anemio/
