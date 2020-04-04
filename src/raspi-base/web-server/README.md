### Running Development Server

`# in /anemio folder`
`python manage.py runserver`
`celery -A anemio worker`
`celery -A anemio beat -l info`
