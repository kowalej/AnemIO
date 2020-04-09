#!/usr/bin/env python3

import logging
import argparse
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

from anemioradiodaemon.constants import *


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
        logFile = '/data/anemio.log'
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
    
    radio_daemon = RadioDaemon(logger=logger)
    radio_daemon.run()
