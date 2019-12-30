#!/usr/bin/env python3

import os
from anemioradiodaemon.radiodaemon import RadioDaemon

if __name__ == '__main__':
    # Try to set OS high priority.
	try:
		os.nice(1)
	except:
		pass

	radio_daemon = RadioDaemon()
	radio_daemon.run()
