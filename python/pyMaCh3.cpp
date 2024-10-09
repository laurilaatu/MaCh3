#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void initPlotting(py::module &); // <- defined in python/plotting.cpp
void initFitter(py::module &); // <- defined in python/fitter.cpp
void initSamplePDF(py::module &); // <- defined in python/samplePDF.cpp
void initManager(py::module &); // <- defined in python/manager.cpp

PYBIND11_MODULE( pyMaCh3, m ) {
    initPlotting(m);
    initFitter(m);
    initSamplePDF(m);
    initManager(m);
}