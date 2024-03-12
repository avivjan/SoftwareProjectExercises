from setuptools import Extension, setup

module = Extension("kmeans", sources=['kmeansmodule.c'])
setup(name='kmeans',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])
