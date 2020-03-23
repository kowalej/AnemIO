from rest_framework import status
from rest_framework.decorators import api_view
from rest_framework import viewsets
from rest_framework.response import Response
from rest_framework import permissions
from .models import *
from .serializers import *
from .filters import *

# Create your views here.
f = permissions.DjangoModelPermissions

class AccelerometerXyzViewSet(viewsets.ReadOnlyModelViewSet):
    queryset = AccelerometerXyz.objects.all()
    serializer_class = AccelerometerXyzSerializer
    filterset_class = AccelerometerXyzFilter
