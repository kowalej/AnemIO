#!/usr/bin/env python3

import logging
import os
from logging.handlers import RotatingFileHandler, SMTPHandler

from dotenv import load_dotenv
from ratelimitingfilter import RateLimitingFilter
from anemioradiodaemon.radiodaemon import RadioDaemon

# Load environment vars.
from dotenv import load_dotenv
env_path = Path('.') / '.test.env'
load_dotenv(env_path, verbose=True)
from .constants import *

load_dotenv()

if __name__ == '__main__':
    # Try to set OS high priority.
	try:
		os.nice(1)
	except:
		pass

	# Log info to file, limited to 5MB in size.
	log_formatter = logging.Formatter('%(asctime)s %(levelname)s %(funcName)s(%(lineno)d) %(message)s')
	logFile = 'anemio.log'
	file_handler = RotatingFileHandler(logFile, mode='a', maxBytes=5*1024*1024, 
									backupCount=2, encoding=None, delay=0)
	file_handler.setFormatter(log_formatter)
	file_handler.setLevel(logging.INFO)

	# Send email on critical errors.
	smtp_handler = SMTPHandler(mailhost=(EMAIL_HOST, EMAIL_PORT),
										fromaddr=EMAIL_FROM, 
										toaddrs=EMAIL_TO,
										credentials=(EMAIL_USER, EMAIL_PASSWORD),
										subject=u"Anemio station: a critical error has occured.")
	smtp_handler.setLevel(logging.CRITICAL)
	ratelimit = RateLimitingFilter(rate=EMAIL_RATE, per=EMAIL_RATE_SECONDS, burst=EMAIL_RATE_BURST)
	smtp_handler.addFilter(ratelimit)

	logger = logging.getLogger('anemio')
	logger.setLevel(logging.INFO)
	logger.addHandler(file_handler)
	logger.addHandler(smtp_handler)
	radio_daemon = RadioDaemon(logger=logger)
	radio_daemon.run()