## Running Development Server

`# in /anemio folder`
`python manage.py runserver`
`celery -A anemio worker`
`celery -A anemio beat -l info`


## Environment Setup

The application requires Python, Redis, and Memcached. Python and the required packages should be installed using a Python virtualenv or conda environment.
For Windows - conda is useful, especially to get Memcached to install correctly (`conda install python-memcached` should just work). To run Redis easily for testing, Docker is recommended. You can simpy do `docker run --name anemio-redis -d redis -p "6379:6379"`.


## For Production on Pi

### Before you start:
You should always run 
`sudo apt update`
`sudo apt upgrade`

### Memcached setup:
Install with:
`sudo apt-get install memcached`
Configure:
`sudo nano /etc/memcached.conf`

Main configuration points:
-m 256 # Set memory to 256 megabytes.
-c 512 # Set number of max incoming connections to 512.

### Redis setup:
https://redis.io/topics/quickstart - see "Installing Redis more properly" guide.

