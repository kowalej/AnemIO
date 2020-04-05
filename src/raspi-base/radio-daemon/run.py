#!/usr/bin/env python3

from anemioradiodaemon.constants import *
import logging
import os
import argparse
import daemon
import lockfile
from pathlib import Path
from logging.handlers import RotatingFileHandler, SMTPHandler

from dotenv import load_dotenv
from ratelimitingfilter import RateLimitingFilter
from anemioradiodaemon.radiodaemon import RadioDaemon, connect_db

# Load environment vars.
from dotenv import load_dotenv
env_path = Path('.') / '.prod.env'
load_dotenv(env_path, verbose=True)

load_dotenv()

# Setup arguments.
parser = argparse.ArgumentParser("anemio-daemon")
parser.add_argument("--interactive-testing", dest='interactiveTesting',
                    help="Show the output in the terminal.", action='store_true', default=False, required=False)
args = parser.parse_args()

if __name__ == '__main__':
    # If we are interactive testing, we will show the output (in console) using a logger named 'anemio-debug'.
    if args.interactiveTesting:
        logging.basicConfig(level=logging.INFO, format='%(message)s')
        logger = logging.getLogger('anemio-debug')
        logger.setLevel(logging.INFO)

    else:
        # Log info to file, limited to 5MB in size.
        log_formatter = logging.Formatter(
            '%(asctime)s %(levelname)s %(funcName)s(%(lineno)d) %(message)s')
        logFile = 'anemio.log'
        # We will log to files with a max size of 5MB, of which there are up to 2 (i.e. backupCount).
        # This means there is a maximum log size of 10MB on disk.
        file_handler = RotatingFileHandler(logFile, mode='a', maxBytes=LOG_FILE_MAX_SIZE_MB*1024*1024,
                                            backupCount=LOG_FILE_BACKUP_COUNT, encoding=None, delay=0)
        file_handler.setFormatter(log_formatter)
        file_handler.setLevel(logging.INFO)

        # Send email on critical errors.
        smtp_handler = SMTPHandler(mailhost=(EMAIL_HOST, EMAIL_PORT),
                                    fromaddr=EMAIL_FROM,
                                    toaddrs=EMAIL_TO,
                                    credentials=(EMAIL_USER, EMAIL_PASSWORD),
                                    subject=u"Anemio station: a critical error has occured.")
        smtp_handler.setLevel(logging.CRITICAL)
        ratelimit = RateLimitingFilter(
            rate=EMAIL_RATE,
            per=EMAIL_RATE_SECONDS,
            burst=EMAIL_RATE_BURST)
        smtp_handler.addFilter(ratelimit)

        logger = logging.getLogger('anemio')
        logger.setLevel(logging.INFO)
        logger.addHandler(file_handler)
        logger.addHandler(smtp_handler)
    
    # Interactive testing, we will run the module normally.
    if args.interactiveTesting:
        radio_daemon = RadioDaemon(logger=logger)
        radio_daemon.run()

    else:
        stdlog = open('anemio-std.log', 'w+')
        log = open('anemio.log', 'a+')
        log1 = open('anemio.log.1', 'a+')
        db = open(DEFAULT_DB_NAME, 'a+')
        db_conn = connect_db(DEFAULT_DB_NAME)

        # For "production", we will run the program as a Daemon.
        with daemon.DaemonContext(
            files_preserve=['anemio.log', 'anemio.log.1', 'anemio.db'],
            working_directory='/home/pi/raspi-base/radio-daemon',
            stdin=None,
            stdout=stdlog,
            stderr=stdlog,
            umask=0o0022,
            pidfile=lockfile.FileLock('/var/run/anemio-radio-daemon.pid'),
        ):
            radio_daemon = RadioDaemon(db_conn=db_conn, logger=logger)
            radio_daemon.run()
