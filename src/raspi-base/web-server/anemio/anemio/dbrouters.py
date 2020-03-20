class StationRouter:
    """
    A router to control all database operations on models in the
    station application.
    """
    route_app_labels = {'station'}

    def db_for_read(self, model, **hints):
        """
        Attempts to read station models from station db.
        """
        if model._meta.app_label in self.route_app_labels:
            return 'station'
        return None

    def db_for_write(self, model, **hints):
        """
        Attempts to write station models go to station db.
        """
        if model._meta.app_label in self.route_app_labels:
            return 'station'
        return None

    def allow_relation(self, obj1, obj2, **hints):
        """
        Allow relations if a model in the station app is
        involved.
        """
        if (
            obj1._meta.app_label in self.route_app_labels or
            obj2._meta.app_label in self.route_app_labels
        ):
           return True
        return None

    def allow_migrate(self, db, app_label, model_name=None, **hints):
        """
        Make sure the station app only appear in the
        'station' database.
        """
        if app_label in self.route_app_labels:
            return db == 'station'
        return None


class DefaultRouter:
    def db_for_read(self, model, **hints):
        return 'default'

    def db_for_write(self, model, **hints):
        return 'default'

    def allow_relation(self, obj1, obj2, **hints):
        return True

    def allow_migrate(self, db, app_label, model_name=None, **hints):
        return True
