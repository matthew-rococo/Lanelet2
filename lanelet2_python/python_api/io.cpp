#include <lanelet2_io/Io.h>
#include <boost/python.hpp>
#include <iostream>

namespace py = boost::python;
using namespace lanelet;

std::shared_ptr<LaneletMap> loadWrapper(const std::string& filename, const Origin& origin) {
  return load(filename, origin);
}
std::shared_ptr<LaneletMap> loadDefaultProjectorWrapper(const std::string& filename) {
  return load(filename, DefaultProjector());
}
std::shared_ptr<LaneletMap> loadProjectorWrapper(const std::string& filename, const Projector& projector) {
  return load(filename, projector);
}
py::tuple loadWithErrorWrapper(const std::string& filename, const Projector& projector) {
  ErrorMessages errs;
  LaneletMapPtr map = load(filename, projector, &errs);
  return py::make_tuple(map, errs);
}
py::tuple loadDefaultWithErrorWrapper(const std::string& filename) {
  ErrorMessages errs;
  LaneletMapPtr map = load(filename, DefaultProjector(), &errs);
  return py::make_tuple(map, errs);
}

void writeWrapper(const std::string& filename, const LaneletMap& map, const Origin& origin) {
  write(filename, map, origin);
}

void writeProjectorWrapper(const std::string& filename, const LaneletMap& map, const Projector& projector) {
  write(filename, map, projector);
}
void writeDefaultProjectorWrapper(const std::string& filename, const LaneletMap& map) {
  write(filename, map, DefaultProjector());
}

ErrorMessages writeWithErrorWrapper(const std::string& filename, const LaneletMap& map, const Projector& projector) {
  ErrorMessages errs;
  write(filename, map, projector, &errs);
  return errs;
}

ErrorMessages writeDefaultWithErrorWrapper(const std::string& filename, const LaneletMap& map) {
  ErrorMessages errs;
  write(filename, map, DefaultProjector(), &errs);
  return errs;
}

BOOST_PYTHON_MODULE(PYTHON_API_MODULE_NAME) {  // NOLINT
  using namespace py;
  auto core = import("lanelet2.core");
  auto proj = import("lanelet2.projection");

  class_<Origin, std::shared_ptr<Origin>>("Origin", init<>())
      .def(init<GPSPoint>())
      .def("__init__", make_constructor(
                           +[](double lat, double lon, double alt) {
                             return std::make_shared<Origin>(GPSPoint{lat, lon, alt});
                           },
                           default_call_policies(), (arg("lat") = 0., arg("lon") = 0., arg("lon") = 0)))
      .add_property("position", &Origin::position);

  def("load", loadDefaultProjectorWrapper, (arg("filename")),
      "Loads a map using a simple spherical mercator projection");
  def("load", loadProjectorWrapper, (arg("filename"), arg("projector")),
      "Loads a map with a custom projection (recommended)");
  def("load", loadWrapper, (arg("filename"), arg("origin")),
      "Loads a map so that the origin is equal to (0,0) in the local coordinate system");
  def("loadRobust", loadWithErrorWrapper, (arg("filename"), arg("projector")),
      "Loads a map robustly. Parser errors are returned as second member of "
      "the tuple. If there are errors, the map will be incomplete somewhere.");
  def("loadRobust", loadDefaultWithErrorWrapper, arg("filename"),
      "Loads a map robustly. Parser errors are returned as second member of "
      "the tuple. If there are errors, the map will be incomplete somewhere.");

  def("write", writeDefaultProjectorWrapper, (arg("filename"), arg("map")),
      "Writes the map to a file. The extension determines which format will "
      "be used (usually .osm)");
  def("write", writeProjectorWrapper, (arg("filename"), arg("map"), arg("projector")),
      "Writes the map to a file. The extension determines which format will "
      "be used (usually .osm)");
  def("write", writeWrapper, (arg("filename"), arg("map"), arg("origin")),
      "Writes the map to a file. The extension determines which format will "
      "be used (usually .osm)");
  def("writeRobust", writeDefaultWithErrorWrapper, (arg("filename"), arg("map")),
      "Writes a map robustly and returns writer errors. If there are errors, "
      "the map will be incomplete somewhere.");
  def("writeRobust", writeWithErrorWrapper, (arg("filename"), arg("map"), arg("projector")),
      "Writes a map robustly and returns writer errors. If there are errors, "
      "the map will be incomplete somewhere.");
}
